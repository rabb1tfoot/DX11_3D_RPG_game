#pragma once

#include "RenderMgr.h"
#include "SceneMgr.h"

enum class SCENE_STATE
{
	PLAY,
	PAUSE,
	STOP,
	SAVE,
	LOAD,
};

class CCore
{
	SINGLE(CCore);
private:
	HWND			m_hWnd;	// 출력 윈도우 핸들
	tResolution		m_tRes; // 윈도우 해상도 정보
	SCENE_STATE		m_eState;

	void(CRenderMgr::*render)(void);
	void(CSceneMgr::*scene_progress)(void);

	void SetRenderMode(bool _bTool);

public:
	int init(HWND _hWnd, bool _bWindowed);
	void progress();
	void SetState(SCENE_STATE _eState);
	SCENE_STATE GetState() { return m_eState; }

public:
	HWND GetWindowHwnd() { return m_hWnd; }
};

