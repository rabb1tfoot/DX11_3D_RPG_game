#include "stdafx.h"
#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"

#include "RenderMgr.h"
#include "CollisionMgr.h"
#include "EventMgr.h"


CSceneMgr::CSceneMgr()
	: m_pCurScene(nullptr)
	, m_pNextScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
	SAFE_DELETE(m_pNextScene);
}

void CSceneMgr::init()
{
	// Resource Load
	//CreatePrefab();	

	//// 수업용 임시 Scene 생성
	m_pCurScene = new CScene;

	// Layer 추가하기
	m_pCurScene->AddLayer(L"Default", 0);
}

void CSceneMgr::progress()
{
	m_pCurScene->update();
	m_pCurScene->lateupdate();

	CRenderMgr::GetInst()->ClearRegisterObj();
	m_pCurScene->finalupdate();

	CCollisionMgr::GetInst()->update();
	CEventMgr::GetInst()->update();
}

void CSceneMgr::progress_pause()
{
	CRenderMgr::GetInst()->ClearRegisterObj();
	m_pCurScene->finalupdate();
	CEventMgr::GetInst()->update();
}

void CSceneMgr::FindGameObject(const wstring & _strTagName, vector<CGameObject*>& _vecOut)
{
	m_pCurScene->FindGameObject(_strTagName, _vecOut);
}

void CSceneMgr::ChangeScene(CScene * _pNextScene)
{
	SAFE_DELETE(m_pCurScene);
	m_pCurScene = _pNextScene;
}
