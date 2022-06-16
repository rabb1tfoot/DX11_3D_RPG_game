// BtnView.cpp: 구현 파일
//

#include "stdafx.h"
#include "BtnView.h"

#include "resource.h"
#include "MainFrm.h"

#include <Core.h>
#include <SceneMgr.h>
#include <Scene.h>

// CBtnView
IMPLEMENT_DYNCREATE(CBtnView, CFormView)

CBtnView::CBtnView()
	: CFormView(IDD_CBtnView)
{
}

CBtnView::~CBtnView()
{
}

BEGIN_MESSAGE_MAP(CBtnView, CFormView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECK1, &CBtnView::OnBtnPlay)
	ON_BN_CLICKED(IDC_BUTTON4, &CBtnView::OnBtnPause)
END_MESSAGE_MAP()


// CBtnView 그리기

void CBtnView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}

void CBtnView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK1, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON4, m_btnPause);


	// 버튼 세개 중앙에 배치
	CRect rt1, rt2;
	m_btnPlay.GetWindowRect(rt1);
	m_btnPause.GetWindowRect(rt2);

	float fAccWidth = (float)(rt1.Width() + rt2.Width());

	CRect rtBtnView;
	GetClientRect(rtBtnView);

	float fStartX = (rtBtnView.Width() / 2.f) - (fAccWidth / 2.f);

	m_btnPlay.SetWindowPos(nullptr, (int)fStartX, 10, rt1.Width(), rt1.Height(), 0);

	fStartX += rt1.Width();

	m_btnPause.SetWindowPos(nullptr, (int)fStartX, 10, rt2.Width(), rt1.Height(), 0);
}


// CBtnView 진단

#ifdef _DEBUG
void CBtnView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBtnView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBtnView 메시지 처리기
void CBtnView::OnBtnPlay()
{
	// 1. 정지상태에서 재생버튼이 눌린 경우
	// -> awake(), start() 및 update(), lateupdate() 활성화
	if (!!m_btnPlay.GetCheck())
	{
		// 임시 저장파일 생성 

		m_btnPlay.SetWindowTextW(L"■");

		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

		pCurScene->awake();
		pCurScene->start();

		if (m_btnPause.GetCheck())
			CCore::GetInst()->SetState(SCENE_STATE::PAUSE);
		else
			CCore::GetInst()->SetState(SCENE_STATE::PLAY);
	}
	// 재생 중인 상태에서 다시 재생버튼 눌렀을 경우
	else
	{
		m_btnPlay.SetWindowTextW(L"▶");
		CCore::GetInst()->SetState(SCENE_STATE::STOP);

		// 임시 저장상태로 되돌아감
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	::SetFocus(pMainFrm->GetGameView()->m_hWnd);
}

void CBtnView::OnBtnPause()
{
	// 1. 재생모드 x, 일시정지 누른 경우
	// -> 아무일도 없다.
	if (!m_btnPlay.GetCheck())
		return;

	// 2. 재생 상태에서 일시정지 버튼이 눌린 경우
	// -> update() lateupdate() 비활성화

	// 일시정지가 이미 눌려있던 경우
	if (!m_btnPause.GetCheck())
	{
		CCore::GetInst()->SetState(SCENE_STATE::PLAY);
	}
	else
	{
		CCore::GetInst()->SetState(SCENE_STATE::PAUSE);
	}

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	::SetFocus(pMainFrm->GetGameView()->m_hWnd);
}

int CBtnView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}