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
	HWND			m_hWnd;	// ��� ������ �ڵ�
	tResolution		m_tRes; // ������ �ػ� ����
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

