#include "stdafx.h"
#include "Collider3D.h"
#include "ResMgr.h"
#include "ConstBuffer.h"
#include "Device.h"
#include "Transform.h"
#include "Script.h"

CResPtr<CMaterial> CCollider3D::g_pOriginMtrl = nullptr;
vector<CResPtr<CMaterial>> CCollider3D::g_vecCol3DMtrl;

CCollider3D::CCollider3D()
	: CComponent(COMPONENT_TYPE::COLLIDER3D)
	, m_vOffset(Vec3(0.f, 0.f, 0.f))
	, m_vScale(Vec3(1.f, 1.f, 1.f))
	, m_eType(COLLIDER3D_TYPE::END)
	, m_pCurMtrl(nullptr)
	, m_iColCount(0)
{
	SetColliderType(COLLIDER3D_TYPE::CUBE);
	m_pCurMtrl = g_pOriginMtrl;
}


CCollider3D::~CCollider3D()
{
}


void CCollider3D::CreateMaterial()
{
	g_pOriginMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\Collider3DMtrl.mtrl");

	for (UINT i = 0; i < 10; ++i)
	{
		CResPtr<CMaterial> pCloneMtrl = g_pOriginMtrl->Clone();
		CResMgr::GetInst()->AddCloneRes<CMaterial>(pCloneMtrl);
		g_vecCol3DMtrl.push_back(pCloneMtrl);
	}
}

CResPtr<CMaterial> CCollider3D::GetCloneMtrl()
{
	if (g_vecCol3DMtrl.empty())
	{
		CResPtr<CMaterial> pNewClone = g_pOriginMtrl->Clone();
		CResMgr::GetInst()->AddCloneRes<CMaterial>(pNewClone);
		return pNewClone;
	}

	CResPtr<CMaterial> pMtrl = g_vecCol3DMtrl.back();
	g_vecCol3DMtrl.pop_back();

	return pMtrl;
}

void CCollider3D::ClearMaterial()
{
	g_pOriginMtrl = nullptr;
	for (UINT i = 0; i < g_vecCol3DMtrl.size(); ++i)
	{
		g_vecCol3DMtrl[i] = nullptr;
	}
}

void CCollider3D::update()
{
}

void CCollider3D::finalupdate()
{
	//일단 콜라이더가 회전하지 않는방식으로

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vScale = Transform()->GetLocalScale();
	//Vec3 vRot = Transform()->GetLocalRot();

	m_vFinalPos = vPos + m_vOffset;
	m_vFinalScale = vScale *m_vScale;

	Matrix matScale = XMMatrixScaling(m_vFinalScale.x, m_vFinalScale.y, m_vFinalScale.z);
	Matrix matRotationX = XMMatrixRotationX(0.f); //vRot.x
	Matrix matRotationY = XMMatrixRotationY(0.f); //vRot.y
	Matrix matRotationZ = XMMatrixRotationZ(0.f); //vRot.z
	Matrix matTrans = XMMatrixTranslation(m_vFinalPos.x, m_vFinalPos.y, m_vFinalPos.z);

	m_matColWorld = matScale * matRotationX * matRotationY * matRotationZ * matTrans;

	if (Object()->GetParent())
		m_matColWorld *= Object()->GetParent()->Transform()->GetWorldMat();

	//빛영향x 인자값
	int a = 1;
	m_pCurMtrl->SetData(SHADER_PARAM::INT_1, &a);
}

void CCollider3D::SetColliderType(COLLIDER3D_TYPE _eType)
{
	m_eType = _eType;
	if (COLLIDER3D_TYPE::CUBE == m_eType)
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeColliderMesh");
	}
	else if (COLLIDER3D_TYPE::SPHERE == m_eType)
	{
		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
	}
	else
		assert(nullptr);
}

void CCollider3D::OnCollisionEnter(CCollider3D * _pOther)
{
	// 현재 참조하고 있는 CurMtrl 이 이미 Clone 이라면 다시 풀에서 꺼내오지 않는다.
	if (g_pOriginMtrl == m_pCurMtrl)
		m_pCurMtrl = GetCloneMtrl();

	int a = 1;
	m_pCurMtrl->SetData(SHADER_PARAM::INT_0, &a);

	++m_iColCount;


	const vector<CScript*>& vecScript = Object()->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionEnter3D(_pOther);
	}
}

void CCollider3D::OnCollision(CCollider3D * _pOther)
{
	const vector<CScript*>& vecScript = Object()->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollision3D(_pOther);
	}
}

void CCollider3D::OnCollisionExit(CCollider3D * _pOther)
{
	--m_iColCount;

	int a = 0;
	m_pCurMtrl->SetData(SHADER_PARAM::INT_0, &a);

	if (0 == m_iColCount)
	{
		g_vecCol3DMtrl.push_back(m_pCurMtrl);
		m_pCurMtrl = g_pOriginMtrl;
	}

	const vector<CScript*>& vecScript = Object()->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionExit3D(_pOther);
	}
}

void CCollider3D::OnCollisionClick3D()
{
	const vector<CScript*>& vecScript = Object()->GetScripts();
	for (UINT i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnCollisionClick3D();
	}
}

void CCollider3D::UpdateData()
{
	g_transform.matWorld = m_matColWorld;

	static CConstBuffer* pCB = CDevice::GetInst()->FindConstBuffer(L"Transform");

	pCB->AddData(&g_transform, sizeof(tTransform));
	pCB->UpdateData();
	pCB->SetRegister((UINT)SHADER_TYPE::VERTEX_SHADER);
}

void CCollider3D::render_inst()
{
	UpdateData();

	m_pMesh->SetLayout(m_pCurMtrl->GetShader());
	m_pCurMtrl->UpdateDataInstancing();
	m_pMesh->render_instancing(0);
}

void CCollider3D::render()
{
	UpdateData();

	m_pMesh->SetLayout(m_pCurMtrl->GetShader());
	m_pCurMtrl->UpdateData();
	m_pMesh->render();
}

void CCollider3D::SaveToScene(FILE * _pFile)
{
	fwrite(&m_vOffset, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_eType, sizeof(UINT), 1, _pFile);
}

void CCollider3D::LoadFromScene(FILE * _pFile)
{
	fread(&m_vOffset, sizeof(Vec3), 1, _pFile);
	fread(&m_vScale, sizeof(Vec3), 1, _pFile);
	fread(&m_eType, sizeof(UINT), 1, _pFile);

	SetColliderType(m_eType);
}