#include "stdafx.h"
#include "Light3D.h"

#include "ResMgr.h"
#include "RenderMgr.h"
#include "Transform.h"
#include "Camera.h"
#include "MRT.h"
#include "PathMgr.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_pMtrl(nullptr)
	, m_iIdx(-1)
{
	//광원시점으로 카메라 관리
	m_pCamObj = new CGameObject;
	m_pCamObj->AddComponent(new CTransform);
	m_pCamObj->AddComponent(new CCamera);

	//광원이 관리하는 카메라는 랜더 매니저에 등록되면 안된다.
	m_pCamObj->Camera()->SetRegisterOnOff(false);
	m_pCamObj->Camera()->CheckLayerALL();
}

CLight3D::CLight3D(const CLight3D & _light)
	: CComponent(_light)
	, m_tInfo(_light.m_tInfo)
	, m_pMtrl(_light.m_pMtrl)
	, m_pMesh(_light.m_pMesh)
	, m_iIdx(-1)
	, m_pCamObj(nullptr)
{
	m_pCamObj = _light.m_pCamObj->Clone();
}

CLight3D::~CLight3D()
{
	SAFE_DELETE(m_pCamObj);
}

void CLight3D::finalupdate()
{
	m_tInfo.vPos = Transform()->GetWorldPos();
	m_iIdx = CRenderMgr::GetInst()->RegisterLight3D(this);

	//광원 카메라도 같은 Transform 정보를 가진다.
	*m_pCamObj->Transform() = *Transform();
	m_pCamObj->finalupdate();
}

void CLight3D::render()
{
	m_pMtrl->SetData(SHADER_PARAM::INT_0, &m_iIdx);

	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	g_transform.matView = pMainCam->GetViewMat();
	g_transform.matViewInv = pMainCam->GetViewInvMat();
	g_transform.matProj = pMainCam->GetProjMat();

	Transform()->UpdateData();

	//Directional Light인 경우
	if (m_tInfo.iType == (UINT)LIGHT_TYPE::DIRECTIONAL)
	{
		//광원 시점 ShadowMap 깊이 정보 택스처
		CResPtr<CTexture> pShadowMapTex = CResMgr::GetInst()->FindRes<CTexture>(L"ShadowmapTargetTex");
		m_pMtrl->SetData(SHADER_PARAM::TEX_2, &pShadowMapTex);

		//광원으로 투영시키기위해 View Proj 매트릭스
		Matrix matVP = m_pCamObj->Camera()->GetViewMat() * m_pCamObj->Camera()->GetProjMat();
		m_pMtrl->SetData(SHADER_PARAM::MAT_0, &matVP);

		//매인카메라의 뷰역행렬
		Matrix matInvView = CRenderMgr::GetInst()->GetMainCam()->GetViewInvMat();
		m_pMtrl->SetData(SHADER_PARAM::MAT_1, &matInvView);
	}

	m_pMtrl->UpdateData();
	m_pMesh->SetLayout(m_pMtrl->GetShader());
	m_pMesh->render(0);
}

void CLight3D::render_shadowmap()
{
	if (m_tInfo.iType != (UINT)LIGHT_TYPE::DIRECTIONAL)
		return;

	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SHADOWMAP)->OMSet();

	m_pCamObj->Camera()->SortShadowGameObject();
	m_pCamObj->Camera()->render_shadowmap();
}

void CLight3D::SetLightType(LIGHT_TYPE _eType)
{
	m_tInfo.iType = (UINT)_eType;

	if ((UINT)LIGHT_TYPE::DIRECTIONAL == m_tInfo.iType)
	{
		m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\DirLightMtrl.mtrl");
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");

		m_pCamObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_pCamObj->Camera()->SetWidth(7680.f);
		m_pCamObj->Camera()->SetHeight(7680.f);
	}
	else if ((UINT)LIGHT_TYPE::POINT == m_tInfo.iType)
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\PointLightMtrl.mtrl");
	}
	else
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
		m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\SpotLightMtrl.mtrl");
	}
}

void CLight3D::SetLightDir(const Vec3 & _vLightDir)
{
	m_tInfo.vDir = _vLightDir;
	m_tInfo.vDir.Normalize();

	Transform()->SetLookAt(Vec3(m_tInfo.vDir.x, m_tInfo.vDir.y, m_tInfo.vDir.z));
}

void CLight3D::SetLightRange(float _fRange)
{
	m_tInfo.fRange = _fRange;
	Transform()->SetLocalScale(Vec3(_fRange, _fRange, _fRange));
}

void CLight3D::SaveToScene(FILE * _pFile)
{
	fwrite(&m_tInfo, sizeof(tLight3DInfo), 1, _pFile);
	
	SaveWString(m_pMtrl->GetName().c_str(), _pFile);
	SaveWString(m_pMtrl->GetPath().c_str(), _pFile);
	SaveWString(m_pMesh->GetName().c_str(), _pFile);
	SaveWString(m_pMesh->GetPath().c_str(), _pFile);

	fwrite(&m_iIdx, sizeof(int), 1, _pFile);
}

void CLight3D::LoadFromScene(FILE * _pFile)
{
	fread(&m_tInfo, sizeof(tLight3DInfo), 1, _pFile);

	wstring strKey = LoadWString(_pFile);
	wstring strPath = LoadWString(_pFile);

	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strKey.c_str());

	if (nullptr != m_pMtrl)
		m_pMtrl = CResMgr::GetInst()->Load<CMaterial>(strKey.c_str(), strPath.c_str());

	strKey = LoadWString(_pFile);
	strPath = LoadWString(_pFile);

	m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(strKey.c_str());

	if (nullptr != m_pMesh)
		m_pMesh = CResMgr::GetInst()->Load<CMesh>(strKey.c_str(), strPath.c_str());

	fread(&m_iIdx, sizeof(int), 1, _pFile);
}
