#include "stdafx.h"
#include "Core.h"

#include "Device.h"
#include "PathMgr.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ResMgr.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "FontHelperMgr.h"
#include "instancingBuffer.h"

CCore::CCore()
	: m_hWnd(nullptr)
	, m_eState(SCENE_STATE::STOP)
	, render(nullptr)
	, scene_progress(nullptr)
{
}

CCore::~CCore()
{
}

int CCore::init(HWND _hWnd, bool _bWindowed)
{
	m_hWnd = _hWnd;

	RECT rt = {};
	GetClientRect(m_hWnd, &rt);

	m_tRes.fWidth = (float)(rt.right - rt.left);
	m_tRes.fHeight = (float)(rt.bottom - rt.top);

	// Manager 초기화
	if (FAILED(CDevice::GetInst()->init()))
		return E_FAIL;

	CRenderMgr::GetInst()->init(m_hWnd, m_tRes, _bWindowed);
	CRenderMgr::GetInst()->init2D();

	CPathMgr::init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();

	CResMgr::GetInst()->init();
	CSceneMgr::GetInst()->init();

	CinstancingBuffer::GetInst()->init();
	CFontHelperMgr::GetInst()->init();

	// 상수버퍼 등록
	CDevice::GetInst()->CreateConstBuffer(L"Transform", sizeof(tTransform), 0);
	CDevice::GetInst()->CreateConstBuffer(L"ShaderParam", sizeof(tShaderParam), 1);
	CDevice::GetInst()->CreateConstBuffer(L"Anim2D", sizeof(tAnim2D), 2);
	CDevice::GetInst()->CreateConstBuffer(L"Global", sizeof(tGlobalValue), 3);
	CDevice::GetInst()->CreateConstBuffer(L"Light3D", sizeof(tLight3DInfo) * 100 + 16, 4);

	SetRenderMode(true);
	SetState(SCENE_STATE::STOP);

	int iSize = sizeof(render);

	return S_OK;
}

void CCore::progress()
{
	CKeyMgr::GetInst()->update();
	CTimeMgr::GetInst()->update();

	// Scene Progress	
	(CSceneMgr::GetInst()->*scene_progress)();

	// Scene Render	
	(CRenderMgr::GetInst()->*render)();
}

void CCore::SetRenderMode(bool _bTool)
{
	if (_bTool)
		render = &CRenderMgr::render_tool;
	else
		render = &CRenderMgr::render;
}

void CCore::SetState(SCENE_STATE _eState)
{
	m_eState = _eState;

	if (m_eState == SCENE_STATE::PLAY)
	{
		scene_progress = &CSceneMgr::progress;
		SetRenderMode(false);
	}
	else
	{
		SetRenderMode(true);
		scene_progress = &CSceneMgr::progress_pause;
	}
}
