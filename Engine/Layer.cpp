#include "stdafx.h"
#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Delete_Vec(m_vecParentObj);
}

void CLayer::awake()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->awake();
	}
}

void CLayer::start()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->start();
	}
}

void CLayer::update()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->update();
	}
}

void CLayer::lateupdate()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->lateupdate();
	}
}

void CLayer::finalupdate()
{
	for (UINT i = 0; i < m_vecParentObj.size(); ++i)
	{
		if (m_vecParentObj[i]->IsActive())
			m_vecParentObj[i]->finalupdate();
	}
}

void CLayer::render()
{
	for (UINT i = 0; i < m_vecObject.size(); ++i)
	{
		if (m_vecObject[i]->IsActive())
			m_vecObject[i]->render();
	}
}

void CLayer::AddObject(CGameObject * _pObj, bool _bMoveAll)
{
	// 1. Object 가 최상위 부모 오브젝트인가?
	if (!_pObj->GetParent())
	{
		m_vecParentObj.push_back(_pObj);
	}
	_pObj->SetLayerIdx(m_iLayerIdx);

	// 2. 자식 오브젝트도 전부 옮길것인지
	if (_bMoveAll)
	{
		const vector<CGameObject*>& vecChild = _pObj->GetChild();
		for (UINT i = 0; i < vecChild.size(); ++i)
		{
			AddObject(vecChild[i], _bMoveAll);
		}
	}
}

void CLayer::RemoveParentObj(CGameObject * _pObj)
{
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if (_pObj == *iter)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}

	// 부모오브젝트, Layer 에서 제거
	// Layer 안에 없었다.
	assert(nullptr);
}
