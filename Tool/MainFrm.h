﻿
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "CUserSplitterWnd.h"
#include "GameObjectDlg.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:
	void init();
	void update();

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CUserSplitterWnd m_mainSplitter;
	CUserSplitterWnd m_subSplitter;

public:
	CWnd* GetHierachyView() { return m_mainSplitter.GetPane(0, 1); }
	CWnd* GetComView() { return m_mainSplitter.GetPane(0, 2); }
	CWnd* GetBtnView() { return m_subSplitter.GetPane(0, 0); }
	CWnd* GetGameView() { return m_subSplitter.GetPane(1, 0); }
	CWnd* GetDbgView() { return m_subSplitter.GetPane(2, 0); }

	void TestCode();

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void SaveSceneMenu();
	afx_msg void LoadSceneMenu();
	afx_msg void CreateObject();
};


