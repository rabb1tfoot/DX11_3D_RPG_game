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
	// 삭제예정 오브젝트 삭제
	for (UINT i = 0; i < m_vecDeleteObj.size(); ++i)
	{
		m_vecDeleteObj[i]->Disconnect();
		SAFE_DELETE(m_vecDeleteObj[i]);
	}

	// 생성 예정 오브젝트 생성
	for (UINT i = 0; i < m_vecNewObj.size(); ++i)
	{
		m_vecNewObj[i];
	}

	m_vecDeleteObj.clear();
	m_vecNewObj.clear();

	// 이벤트 처리
	for (UINT i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJECT:
		{
			// lParam : GameObject 주소
			// wParam : LayerIdx			
			CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(m_vecEvent[i].wParam);
			pLayer->AddObject((CGameObject*)m_vecEvent[i].lParam, true);
		}
		break;
		case EVENT_TYPE::DELETE_OBJECT:
			// lParam : GameObject 주소
			((CGameObject*)m_vecEvent[i].lParam)->Kill();
			m_vecDeleteObj.push_back(((CGameObject*)m_vecEvent[i].lParam));
			break;
		case EVENT_TYPE::ADD_CHILD:
			// lParam : ParentObject 주소
			// wParam : ChildObject 주소
			((CGameObject*)m_vecEvent[i].lParam)->AddChild(((CGameObject*)m_vecEvent[i].wParam));
			break;
		case EVENT_TYPE::CLEAR_PARANT:
			// lParam : Child Object 주소
			((CGameObject*)m_vecEvent[i].lParam)->ClearParent();
			break;
		case EVENT_TYPE::CLEAR_CHILD:
			// lParam : Parent Object 주소
			// wParam : Child Index
			((CGameObject*)m_vecEvent[i].lParam)->ClearChild((UINT)m_vecEvent[i].wParam);
			break;
		case EVENT_TYPE::ENABLE:
			// lParam : Object 주소			
			((CGameObject*)m_vecEvent[i].lParam)->OnEnable();
			break;
		case EVENT_TYPE::DISABLE:
			// lParam : Object 주소			
			((CGameObject*)m_vecEvent[i].lParam)->OnDisable();
			break;
		case EVENT_TYPE::CHANGE_LAYER:
		{
			// lParam : Object 주소
			// wParam : (HIGH)Layer Index
			// wParam : (LOW) MoveAll(레이어 변경 시 자식까지 전부 이동할 것인지)
			WORD iLayerIdx = HIWORD(m_vecEvent[i].wParam);
			WORD bMoveAll = LOWORD(m_vecEvent[i].wParam);

			// 기존 레이어에서 제거
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
