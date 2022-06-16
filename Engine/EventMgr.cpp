#include "stdafx.h"
#include "EventMgr.h"

#include "GameObject.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

CEventMgr::CEventMgr()
{
}

CEventMgr::~CEventMgr()
{
	Safe_Delete_Vec(m_vecDeleteObj);
	Safe_Delete_Vec(m_vecNewObj);
}

void CEventMgr::update()
{
	// �������� ������Ʈ ����
	for (UINT i = 0; i < m_vecDeleteObj.size(); ++i)
	{
		m_vecDeleteObj[i]->Disconnect();
		SAFE_DELETE(m_vecDeleteObj[i]);
	}

	// ���� ���� ������Ʈ ����
	for (UINT i = 0; i < m_vecNewObj.size(); ++i)
	{
		m_vecNewObj[i];
	}

	m_vecDeleteObj.clear();
	m_vecNewObj.clear();

	// �̺�Ʈ ó��
	for (UINT i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJECT:
		{
			// lParam : GameObject �ּ�
			// wParam : LayerIdx			
			CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(m_vecEvent[i].wParam);
			pLayer->AddObject((CGameObject*)m_vecEvent[i].lParam, true);
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
			// lParam : GameObject �ּ�
			((CGameObject*)m_vecEvent[i].lParam)->Kill();
			m_vecDeleteObj.push_back(((CGameObject*)m_vecEvent[i].lParam));
			break;
		case EVENT_TYPE::ADD_CHILD:
			// lParam : ParentObject �ּ�
			// wParam : ChildObject �ּ�
			((CGameObject*)m_vecEvent[i].lParam)->AddChild(((CGameObject*)m_vecEvent[i].wParam));
			break;
		case EVENT_TYPE::CLEAR_PARANT:
			// lParam : Child Object �ּ�
			((CGameObject*)m_vecEvent[i].lParam)->ClearParent();
			break;
		case EVENT_TYPE::CLEAR_CHILD:
			// lParam : Parent Object �ּ�
			// wParam : Child Index
			((CGameObject*)m_vecEvent[i].lParam)->ClearChild((UINT)m_vecEvent[i].wParam);
			break;
		case EVENT_TYPE::ENABLE:
			// lParam : Object �ּ�			
			((CGameObject*)m_vecEvent[i].lParam)->OnEnable();
			break;
		case EVENT_TYPE::DISABLE:
			// lParam : Object �ּ�			
			((CGameObject*)m_vecEvent[i].lParam)->OnDisable();
			break;
		case EVENT_TYPE::CHANGE_LAYER:
		{
			// lParam : Object �ּ�
			// wParam : (HIGH)Layer Index
			// wParam : (LOW) MoveAll(���̾� ���� �� �ڽı��� ���� �̵��� ������)
			WORD iLayerIdx = HIWORD(m_vecEvent[i].wParam);
			WORD bMoveAll = LOWORD(m_vecEvent[i].wParam);

			// ���� ���̾�� ����
			int iPrevLayerIdx = ((CGameObject*)m_vecEvent[i].lParam)->GetLayerIdx();
			CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(iPrevLayerIdx);
			if (nullptr == ((CGameObject*)m_vecEvent[i].lParam)->GetParent())
				pLayer->RemoveParentObj((CGameObject*)m_vecEvent[i].lParam);

			pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(iLayerIdx);
			pLayer->AddObject(((CGameObject*)m_vecEvent[i].lParam), (bool)bMoveAll);
		}
		break;
		default:
			break;
		}
	}

	m_vecEvent.clear();
}
