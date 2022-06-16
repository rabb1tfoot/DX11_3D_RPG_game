#include "stdafx.h"
#include "Camera.h"

#include "Transform.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "RenderMgr.h"
#include "KeyMgr.h"
#include "ResMgr.h"

#include "GameObject.h"
#include "MeshRender.h"
#include "Material.h"
#include "Shader.h"
#include "Device.h"
#include "Animator2D.h"
#include "Animation2D.h"
#include "Animator3D.h"
#include "Collider3D.h"
#include "Font23.h"
#include "ParticleSystem.h"

#include "instancingBuffer.h"
#include "RenderMgr.h"
#include "MRT.h"

CCamera::CCamera()
	: m_eType(PROJ_TYPE::PERSPECTIVE)
	, m_fScale(1.f)
	, m_fFOV(XM_PI / 4.f)
	, m_fNear(1.f)
	, m_fFar(100000.f)
	, m_iLayerCheck(0)
	, m_iCamOrder(0)
	, m_fWidth(0.f)
	, m_fHeight(0.f)
	, m_bRegister(true)
	, CComponent(COMPONENT_TYPE::CAMERA)
{
	tResolution tRes = CRenderMgr::GetInst()->GetResolution();
	m_fWidth = tRes.fWidth;
	m_fHeight = tRes.fHeight;
}

CCamera::~CCamera()
{
}

void CCamera::update()
{
}

void CCamera::finalupdate()
{
	Vec3 vPos = Transform()->GetLocalPos();

	// View ��� �����
	m_matView = XMMatrixIdentity();
	m_matView._41 = -vPos.x;
	m_matView._42 = -vPos.y;
	m_matView._43 = -vPos.z;

	Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT);
	Vec3 vUp = Transform()->GetLocalDir(DIR_TYPE::DIR_UP);
	Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::DIR_FRONT);

	// ��, ��, �� ����� �������(��������) �� ����� �ִ� ȸ������� �ٷ� �佺���̽� ȸ�����
	// �������� ����� ��������̱� ������, �佺���̽� ȸ������� ��, ��, �� ����� ������̴�.
	// ��, ��, �� ���� ������� ����� ��������̱� ������ ��ġ����� �� �����

	Matrix matViewRot;
	matViewRot = Matrix::Identity;
	matViewRot._11 = vRight.x; matViewRot._12 = vUp.x; matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y; matViewRot._22 = vUp.y; matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z; matViewRot._32 = vUp.z; matViewRot._33 = vFront.z;

	m_matView *= matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr,m_matView);

	// Projection ��� �����	
	if (m_eType == PROJ_TYPE::PERSPECTIVE)
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar);
	else
		m_matProj = XMMatrixOrthographicLH(m_fWidth * m_fScale, m_fHeight * m_fScale, m_fNear, m_fFar);

	if (m_bRegister)
		CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::SortGameObject()
{
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_P)
		pair.second.clear();

	m_vecParticle.clear();

	uInstID uID = {};

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);

		//���̾ ���ų� ����°�� �Ѿ��.
		if (nullptr == pLayer || !(m_iLayerCheck & (1 << i)))
			continue;
		
		const vector<CGameObject*>& vAllObj = pLayer->GetAllObject();

		for (auto& pObj : vAllObj)
		{
			//��Ʈ�� �������� ��Ƶα⸸�Ѵ� MeshRender��� ����ȴ�.
			if (pObj->Font())
			{
				pObj->Font()->CollectRender();
			}

			//MeshRender�� ������ �Ѿ��. ��ƼŬ�� ���Ϳ� ��Ƶд�.
			if (pObj->MeshRender() == nullptr)
			{
				if (pObj->Particle())
					m_vecParticle.push_back(pObj);
				continue;
			}

			// �ش� ������Ʈ�� ����ü �ø��� �ϴ� ������Ʈ�̰�,
			// ����ü �׽�Ʈ�� �����ߴٸ�(�þ߿� �ȵ��´ٸ�) continue
			//if (pObj->MeshRender()->GetFrustumCull() && !m_frustum.CheckShpere(pObj))
			//{
			//	continue;
			//}

			UINT iMtrlCount = pObj->MeshRender()->GetMaterialCount();
			for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
			{
				CResPtr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(iMtrl);

				//Material�� ���ų� shader �� ���°�� �Ѿ��.
				if (nullptr == pMtrl || nullptr == pMtrl->GetShader())
					continue;

				//�ν��Ͻ� �׷�����
				map<ULONG64, vector<tInstObj>>* pMap = nullptr;
				if (pMtrl->GetShader()->GetEventTime() == SHADER_EVENT_TIME::DEFERRED)
					pMap = &m_mapInstGroup_D;
				else if (pMtrl->GetShader()->GetEventTime() == SHADER_EVENT_TIME::FORWARD)
					pMap = &m_mapInstGroup_F;
				else
					pMap = &m_mapInstGroup_P;

				uID.LID = pObj->MeshRender()->GetInstID(iMtrl);
				map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.LID);
				//�׷쿡 ������������� �׷쿡 �߰���Ų��.
				if (iter == pMap->end())
				{
					pMap->insert(make_pair(uID.LID, vector<tInstObj>{tInstObj{ pObj, iMtrl }}));
				}
				else
				{
					iter->second.push_back(tInstObj{ pObj, iMtrl });
				}
			}
			if (pObj->Collider3D())
			{
				//�ݶ��̴��ڽ��� ������ ���������� ����
				map<ULONG64, vector<tInstObj>>* pMap = nullptr;
				pMap = &m_mapInstGroup_F;

				uID.LID = pObj->MeshRender()->GetInstID(0);
				map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.LID);
				//�׷쿡 ������������� �׷쿡 �߰���Ų��.
				if (iter == pMap->end())
				{
					pMap->insert(make_pair(uID.LID, vector<tInstObj>{tInstObj{ pObj, 0 }}));
				}
				else
				{
					iter->second.push_back(tInstObj{ pObj, 0 });
				}
			}
		}
	}
}

void CCamera::render_deferred()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	g_transform.matView = m_matView;
	g_transform.matViewInv = XMMatrixInverse(nullptr, m_matView);
	g_transform.matProj = m_matProj;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		if (pair.second.empty())
			continue;
		else if(pair.second.size() <= 10)
		{
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));

			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		CResPtr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		CResPtr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);


		if (nullptr == pMtrl->GetShader())
			continue;

		CinstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			if (pair.second[i].pObj->Animator2D())
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second.at(0).pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[i]}));
				continue;
			}
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				tInstData.iRowIdx = iRowIdx++;
				CinstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			CinstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}
		CAnimation2D::ClearRegister();

		CinstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());
		CResPtr<CTexture> pBoneTex = pObj->MeshRender()->GetMesh()->GetBoneTex();
		pMtrl->SetData(SHADER_PARAM::TEX_7, &pBoneTex);

		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader(), true);
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		if (pair.second[0].pObj->Animator3D())
			pair.second[0].pObj->Animator3D()->UpdateData();

		// �ϳ��� ������Ʈ ���� �������� �����(���׸���) �� �����Ǿ����� �� �ִ�.
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			pair.second[i].pObj->Transform()->UpdateData();
			pair.second[i].pObj->MeshRender()->render(pair.second[i].iMtrlIdx);
		}
	}
}

void CCamera::render_forward()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;
	g_transform.matViewInv = m_matViewInv;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		if (pair.second.empty())
			continue;

		else if (pair.second.size() == 1)
		{
			map<INT_PTR, vector<tInstObj>>::iterator iter
				= m_mapSingleObj.find((INT_PTR)pair.second[0].pObj);

			if (iter != m_mapSingleObj.end())
				iter->second.push_back(pair.second[0]);
			else
				m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[0]}));

			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		CResPtr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		CResPtr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		if (nullptr == pMtrl->GetShader())
			continue;

		CinstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			if (pair.second[i].pObj->Animator2D())
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[0].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[i]}));

				continue;
			}
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				tInstData.iRowIdx = iRowIdx++;
				CinstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			CinstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}
		CinstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());
		CAnimation2D::ClearRegister();

		if (pObj->Collider3D())
		{
			pObj->Collider3D()->render_inst();
			continue;
		}

		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader(), true);
		pMesh->render_instancing(pair.second[0].iMtrlIdx);
	}

	//����������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		if (pair.second[0].pObj->Collider3D())
		{
			pair.second[0].pObj->Collider3D()->render();
			continue;
		}

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			pair.second[i].pObj->Transform()->UpdateData();
			pair.second[i].pObj->MeshRender()->render(pair.second[i].iMtrlIdx);
		}
	}

	for (auto& obj : m_vecParticle)
	{
		obj->Particle()->render();
	}
}

void CCamera::render_posteffect()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;
	g_transform.matViewInv = m_matViewInv;

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_P)
	{
		//������Ʈ�� ���̴��� ���°��
		if (pair.second.empty())
			continue;
		else if (pair.second.size() == 1)
		{
			map<INT_PTR, vector<tInstObj>>::iterator iter =
				m_mapSingleObj.find((INT_PTR)pair.second[0].pObj);

			if (iter != m_mapSingleObj.end())
				iter->second.push_back(pair.second[0]);
			else
			{
				m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[0]}));
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		CResPtr<CMesh> pMesh = pObj->MeshRender()->GetMesh();
		CResPtr<CMaterial> pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second[0].iMtrlIdx);

		if (NULL == pMtrl->GetShader())
			continue;

		CinstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			if (pair.second[i].pObj->Animator2D())
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[0].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[0].pObj, vector<tInstObj>{pair.second[i]}));
				}
				continue;
			}

			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				tInstData.iRowIdx = iRowIdx++;
				CinstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
			}
			else
			{
				tInstData.iRowIdx = -1;
			}

			CinstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// Copy Swapchain To Posteffect
		CRenderMgr::GetInst()->CopySwapToPosteffect();

		CinstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());
		CAnimation2D::ClearRegister();

		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader(), true);
		pMesh->render_instancing(pair.second[0].iMtrlIdx);
	}

	// ���� ������
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			// Copy Swapchain To Posteffect
			CRenderMgr::GetInst()->CopySwapToPosteffect();

			pair.second[i].pObj->Transform()->UpdateData();
			pair.second[i].pObj->MeshRender()->render(pair.second[i].iMtrlIdx);
		}
	}
}

void CCamera::render_shadowmap()
{
	//����İ� ��������� ������������ �����ش�.
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;
	g_transform.matViewInv = m_matViewInv;

	for (UINT i = 0; i < m_vecShadowObj.size(); ++i)
	{
		CResPtr<CTexture> pBoneTex = nullptr;
		CResPtr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");
		if (m_vecShadowObj[i]->Animator3D())
		{
			m_vecShadowObj[i]->Animator3D()->UpdateData();
			CResPtr<CTexture> pBoneTex = m_vecShadowObj[i]->Animator3D()->GetBoneTex();
			pMtrl->SetData(SHADER_PARAM::TEX_7, &pBoneTex);
		}
		else
		{
			pMtrl->SetData(SHADER_PARAM::TEX_7, &pBoneTex);
		}
		m_vecShadowObj[i]->MeshRender()->render_shadowmap();
	}
}

void CCamera::SortShadowGameObject()
{
	m_vecShadowObj.clear();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = nullptr;

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		pLayer = pCurScene->GetLayer(i);
		if (nullptr == pLayer || !(m_iLayerCheck & (1 << i)))
			continue;

		const vector<CGameObject*>& vecObj = pLayer->GetAllObject();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			//MeshRender�� ���ų� Material�� �������� �ʴ°��
			if (vecObj[j]->MeshRender() == nullptr
				|| vecObj[j]->MeshRender()->GetSharedMaterial() == nullptr
				|| vecObj[j]->MeshRender()->GetSharedMaterial()->GetShader() == nullptr)
			{
				continue;
			}

			// Frustum Check
			//if (!m_frustum.CheckShpere(vecObj[j]))
			//{
			//	continue;
			//}

			if (vecObj[j]->MeshRender()->IsShadowObj())
			{
				m_vecShadowObj.push_back(vecObj[j]);
			}
		}
	}
}


const tRay & CCamera::GetRay()
{
	CalRay();

	return m_tRay;
}

void CCamera::CheckLayer(UINT _iLayerIdx)
{
	UINT iCheck = 1 << _iLayerIdx;
	if (m_iLayerCheck & iCheck)
		m_iLayerCheck &= ~iCheck;
	else
		m_iLayerCheck |= iCheck;
}

void CCamera::SaveToScene(FILE * _pFile)
{
	fwrite(&m_eType, sizeof(UINT), 1, _pFile);
	fwrite(&m_fScale, sizeof(float), 1, _pFile);
	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_fNear, sizeof(float), 1, _pFile);
	fwrite(&m_fFar, sizeof(float), 1, _pFile);
	fwrite(&m_iLayerCheck, sizeof(UINT), 1, _pFile);
	fwrite(&m_iCamOrder, sizeof(UINT), 1, _pFile);
	fwrite(&m_fWidth, sizeof(float), 1, _pFile);
	fwrite(&m_fHeight, sizeof(float), 1, _pFile);
	fwrite(&m_bRegister, sizeof(bool), 1, _pFile);
}

void CCamera::LoadFromScene(FILE * _pFile)
{
	fread(&m_eType, sizeof(UINT), 1, _pFile);
	fread(&m_fScale, sizeof(float), 1, _pFile);
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_fNear, sizeof(float), 1, _pFile);
	fread(&m_fFar, sizeof(float), 1, _pFile);
	fread(&m_iLayerCheck, sizeof(UINT), 1, _pFile);
	fread(&m_iCamOrder, sizeof(UINT), 1, _pFile);
	fread(&m_fWidth, sizeof(float), 1, _pFile);
	fread(&m_fHeight, sizeof(float), 1, _pFile);
	fread(&m_bRegister, sizeof(bool), 1, _pFile);
}

void CCamera::CalRay()
{
	POINT ptMousePos = CKeyMgr::GetInst()->GetMousePos();

	D3D11_VIEWPORT tVP = {};
	UINT iSize = 1;
	CONTEXT->RSGetViewports(&iSize, &tVP);

	if (iSize == 0)
	{
		tResolution res = CRenderMgr::GetInst()->GetResolution();
		tVP.Width = res.fWidth;
		tVP.Height = res.fHeight;
		tVP.MinDepth = 1.f;
		tVP.MaxDepth = 1.f;
		tVP.TopLeftX = 0.f;
		tVP.TopLeftY = 0.f;
	}

	m_tRay.vStart = Transform()->GetWorldPos();
	m_tRay.vDir.x = ((((ptMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_tRay.vDir.y = (-(((ptMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_tRay.vDir.z = 1.f;

	m_tRay.vDir = XMVector3TransformNormal(m_tRay.vDir, m_matViewInv);
	m_tRay.vDir.Normalize();
}