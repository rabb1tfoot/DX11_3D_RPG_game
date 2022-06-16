#include "stdafx.h"
#include "GameObject.h"

#include "Transform.h"
#include "MeshRender.h"
#include "script.h"
#include "EventMgr.h"
#include "Core.h"

CGameObject::CGameObject()
	: m_pParent(nullptr)
	, m_arrCom{}
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_bActive(true)
{
}

CGameObject::CGameObject(const CGameObject & _origin)
	: CEntity(_origin)
	, m_pParent(nullptr)
	, m_arrCom{}
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_bActive(true)
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == _origin.m_arrCom[i])
			continue;

		AddComponent(_origin.m_arrCom[i]->Clone());
	}

	for (UINT i = 0; i < _origin.m_vecScript.size(); ++i)
	{
		AddComponent(_origin.m_vecScript[i]->Clone());
	}

	for (UINT i = 0; i < _origin.m_vecChild.size(); ++i)
	{
		AddChild(_origin.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Delete_Array(m_arrCom);
	Safe_Delete_Vec(m_vecScript);
	Safe_Delete_Vec(m_vecChild);
}

void CGameObject::awake()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->awake();
	}

	for (UINT i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->awake();
	}

	for (UINT i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->awake();
	}
}

void CGameObject::start()
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->start();
	}

	for (UINT i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->start();
	}

	for (UINT i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->start();
	}
}

void CGameObject::update()
{
	if (m_bActive)
	{
		for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
		{
			if (nullptr != m_arrCom[i])
				m_arrCom[i]->update();
		}

		for (UINT i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->update();
		}

		for (UINT i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->update();
		}
	}
}

void CGameObject::lateupdate()
{
	if (m_bActive)
	{
		for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
		{
			if (nullptr != m_arrCom[i])
				m_arrCom[i]->lateupdate();
		}

		for (UINT i = 0; i < m_vecScript.size(); ++i)
		{
			m_vecScript[i]->lateupdate();
		}

		for (UINT i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->lateupdate();
		}
	}
}

void CGameObject::finalupdate()
{
	if (m_bActive)
	{
		for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
		{
			if (nullptr != m_arrCom[i])
				m_arrCom[i]->finalupdate();
		}

		for (UINT i = 0; i < m_vecChild.size(); ++i)
		{
			m_vecChild[i]->finalupdate();
		}

		// Layer �� Object ���
		if (!m_bDead)
		{
			CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(m_iLayerIdx);
			if (-1 != m_iLayerIdx && pCurLayer != nullptr)
				pCurLayer->RegisterObj(this);
		}
	}
}

void CGameObject::OnEnable()
{

	m_bActive = true;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->OnEnable();
	}

	for (UINT i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->OnEnable();
	}

	for (UINT i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->OnEnable();
	}
}

void CGameObject::OnDisable()
{

	m_bActive = false;

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->OnDisable();
	}

	for (UINT i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->OnDisable();
	}

	for (UINT i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->OnDisable();
	}
}

void CGameObject::render()
{
	if (m_bActive)
	{
		if (!MeshRender())
			return;

		((CMeshRender*)m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER])->render();
	}
}

void CGameObject::AddComponent(CComponent * _pCom)
{
	COMPONENT_TYPE eType = _pCom->GetCompnentType();

	if (COMPONENT_TYPE::SCRIPT == eType)
	{
		m_vecScript.push_back((CScript*)_pCom);
	}
	else
	{
		// GameObject �� �ش� ������Ʈ�� �̹� ������ �ִ� ���
		assert(!m_arrCom[(UINT)eType]);
		m_arrCom[(UINT)eType] = _pCom;
	}

	_pCom->SetGameObject(this);
}

void CGameObject::AddChild(CGameObject * _pChildObj)
{
	// 1. Child Object �� �θ� �ִ� ���  
	//	 - ���� �θ� ��Ͽ��� ���� �Ǿ�� ��
	if (_pChildObj->GetParent())
	{
		// ���� �θ� �ٽ� �ڽ����� ������ ���(�ߺ� �̺�Ʈ ó�� �߻�)
		assert(_pChildObj->GetParent() != this);

		_pChildObj->ClearParent();
	}
	else
	{
		// �ڽ����� �������� ������Ʈ�� �ֻ��� �θ� ������Ʈ�ε�, Ư�� Layer �� �����ִ°��
		// Layer �� �����ϵ�, �θ������Ʈ�μ��� ����� ����
		if (_pChildObj->GetLayerIdx() != -1)
		{
			CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(_pChildObj->GetLayerIdx());

			// Scene Load �߿��� ����ó�� �˻� ����
			if (SCENE_STATE::LOAD != CCore::GetInst()->GetState())
				pCurLayer->RemoveParentObj(_pChildObj);
		}

	}

	m_vecChild.push_back(_pChildObj);
	_pChildObj->SetParent(this);
}

bool CGameObject::IsAncestor(CGameObject* _pTarget)
{
	CGameObject* pAncestor = m_pParent;

	while (pAncestor)
	{
		if (_pTarget == pAncestor)
			return true;

		pAncestor = pAncestor->m_pParent;
	}

	return false;
}

void CGameObject::Active(bool _bTrue)
{
	if (m_bActive == _bTrue)
		return;

	m_bActive = _bTrue;

	tEvent event{};

	if (m_bActive)
		event.eType = EVENT_TYPE::ENABLE;
	else
		event.eType = EVENT_TYPE::DISABLE;

	event.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(event);
}

void CGameObject::SaveToScene(FILE * _pFile)
{
	fwrite(&m_iLayerIdx, sizeof(int), 1, _pFile);
}

void CGameObject::LoadFromScene(FILE * _pFile)
{
	fread(&m_iLayerIdx, sizeof(int), 1, _pFile);
}

void CGameObject::SetParent(CGameObject * _pParent)
{
	m_pParent = _pParent;
}

void CGameObject::ClearParent()
{
	if (!m_pParent)
		return;

	vector<CGameObject*>::iterator iter = m_pParent->m_vecChild.begin();
	for (; iter != m_pParent->m_vecChild.end(); ++iter)
	{
		if (this == *iter)
		{
			m_pParent->m_vecChild.erase(iter);
			break;
		}
	}

	m_pParent = nullptr;

	// �ֻ��� ������Ʈ�� �Ǿ��� ������, �ش� ���̾ �ֻ��� ������Ʈ�μ� ��� �ȴ�.
	if (!m_bDead)
	{
		CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(GetLayerIdx());
		pCurLayer->AddObject(this, true);
	}
}

void CGameObject::ClearLayer()
{
	// 1. Layer �� �������� �ʴ´�.
	if (-1 == m_iLayerIdx)
		return;

	// 2. �θ� ������Ʈ�� ������.	
	if (!m_pParent)
	{
		CLayer* pCurLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(m_iLayerIdx);
		pCurLayer->RemoveParentObj(this);
	}

	m_iLayerIdx = -1;
}

void CGameObject::Disconnect()
{
	if (m_pParent)
	{
		ClearParent();
	}
	else
	{
		ClearLayer();
	}
}

void CGameObject::ClearChild(UINT _iChildIdx)
{
	assert(!(m_vecChild.size() <= _iChildIdx));

	CGameObject* pChild = m_vecChild[_iChildIdx];

	pChild->ClearParent();
}