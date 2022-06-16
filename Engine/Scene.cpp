#include "stdafx.h"
#include "Scene.h"

#include "Layer.h"
#include "Camera.h"

CScene::CScene()
	: m_arrLayer{}
	, m_arrCam{}
	, m_iCamCount(0)
{
}

CScene::~CScene()
{
	Safe_Delete_Array(m_arrLayer);
}


void CScene::awake()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		m_arrLayer[i]->awake();
	}
}

void CScene::start()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		m_arrLayer[i]->start();
	}
}

void CScene::update()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		m_arrLayer[i]->update();
	}
}

void CScene::lateupdate()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		m_arrLayer[i]->lateupdate();
	}
}

void CScene::finalupdate()
{
	// 각 레이어에 속한 오브젝트 해제
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr != m_arrLayer[i])
		{
			m_arrLayer[i]->ClearRegisterObj();
		}
	}

	ResetRegisterCam();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		m_arrLayer[i]->finalupdate();
	}
}

void CScene::AddLayer(const wstring & _strLayerName, UINT _iLayerIdx)
{
	if (nullptr != m_arrLayer[_iLayerIdx])
		return;

	m_arrLayer[_iLayerIdx] = new CLayer;
	m_arrLayer[_iLayerIdx]->SetName(_strLayerName);
	m_arrLayer[_iLayerIdx]->SetLayerIdx(_iLayerIdx);
}

void CScene::AddLayer(CLayer * _pLayer, UINT _iLayerIdx)
{
	assert(!m_arrLayer[_iLayerIdx]);

	m_arrLayer[_iLayerIdx] = _pLayer;
	m_arrLayer[_iLayerIdx]->SetLayerIdx(_iLayerIdx);
}

void CScene::AddObject(const wstring & _strLayerName, CGameObject * _pObj, bool _bMoveAll)
{
	CLayer* pFindLayer = FindLayer(_strLayerName);
	assert(pFindLayer);
	pFindLayer->AddObject(_pObj, _bMoveAll);
}

void CScene::AddObject(UINT _iLayerIdx, CGameObject * _pObj, bool _bAllMove)
{
	assert(m_arrLayer[_iLayerIdx]);
	m_arrLayer[_iLayerIdx]->AddObject(_pObj, _bAllMove);
}

CLayer * CScene::FindLayer(const wstring & _strLayer)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		if (m_arrLayer[i]->GetName() == _strLayer)
		{
			return m_arrLayer[i];
		}
	}

	return nullptr;
}

void FindObject(CGameObject* _pParent, const wstring& _strName, vector<CGameObject*>& _vecOut)
{
	vector<CGameObject*> stack;

	stack.push_back(_pParent);

	while (!stack.empty())
	{
		CGameObject* pObject = stack.back();
		stack.pop_back();

		if (pObject->GetName() == _strName)
		{
			_vecOut.push_back(pObject);
		}

		const vector<CGameObject*>& vecChild = pObject->GetChild();

		for (UINT i = 0; i < vecChild.size(); ++i)
		{
			stack.push_back(vecChild[i]);
		}
	}
}

CGameObject * CScene::FindGameObject(const wstring & _strName, vector<CGameObject*>& _vecOut)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (nullptr == m_arrLayer[i])
			continue;

		for (UINT j = 0; j < m_arrLayer[i]->m_vecParentObj.size(); ++j)
		{
			FindObject(m_arrLayer[i]->m_vecParentObj[j], _strName, _vecOut);
		}
	}

	return nullptr;
}

void CScene::AddCamera(CCamera* _pCam)
{
	m_iCamCount += 1;
	UINT iIdx = _pCam->GetCamOrder();
	assert(!m_arrCam[iIdx]); // 카메라 순서가 겹치는 경우
	m_arrCam[iIdx] = _pCam;
}