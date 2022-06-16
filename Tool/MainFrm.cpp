
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "HierachyView.h"
#include "ComponentView.h"
#include "BtnView.h"
#include "DebugView.h"
#include "CGameView.h"

#include <Core.h>
#include <PathMgr.h>
#include <CollisionMgr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_32771, &CMainFrame::SaveSceneMenu)
	ON_COMMAND(ID_32772, &CMainFrame::LoadSceneMenu)
	ON_COMMAND(ID_32773, &CMainFrame::CreateObject)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT rt = {};
	GetWindowRect(&rt);
	SetWindowPos(nullptr, 50, 25, rt.right - rt.left, rt.bottom - rt.top, 0);

	// SplitterWnd 를 이용해서 작업영역을 분할
	m_mainSplitter.SetBorder(POINT{ 0, 0 }, POINT{ 1, 1 });
	m_mainSplitter.CreateStatic(this, 1, 3); // (0, 0)   (0, 1)   (0, 2)	

	m_mainSplitter.SetColumnInfo(0, GAMEVIEW_X - 4, GAMEVIEW_X - 4);
	m_mainSplitter.SetColumnInfo(1, HIERACHYVIEW_X, HIERACHYVIEW_X);

	m_mainSplitter.CreateView(0, 1, RUNTIME_CLASS(CHierachyView), CSize(HIERACHYVIEW_X, FRAME_Y), nullptr);
	m_mainSplitter.CreateView(0, 2, RUNTIME_CLASS(CComponentView), CSize(COMVIEW_X, FRAME_Y), nullptr);

	// MainSplitter 의 1열을 다시 3행으로 분할한다.
	m_subSplitter.CreateStatic(&m_mainSplitter, 3, 1, WS_CHILD | WS_VISIBLE, m_mainSplitter.IdFromRowCol(0, 0));
	m_subSplitter.SetBorder(POINT{ 0, 0 }, POINT{ 1, 1 });
	m_subSplitter.CreateView(0, 0, RUNTIME_CLASS(CBtnView), CSize(BTNVIEW_X, BTNVIEW_Y), nullptr);
	m_subSplitter.CreateView(1, 0, RUNTIME_CLASS(CGameView), CSize(GAMEVIEW_X, GAMEVIEW_Y), nullptr);
	m_subSplitter.CreateView(2, 0, RUNTIME_CLASS(CDebugView), CSize(DBGVIEW_X, DBGVIEW_Y), nullptr);

	return 0;
}

void CMainFrame::init()
{
	// Init
	((CGameView*)GetGameView())->init();
	((CHierachyView*)GetHierachyView())->init();

	// GameView Forcus
	::SetFocus(GetGameView()->m_hWnd);
}

void CMainFrame::update()
{
	// CompnentView update
	((CComponentView*)GetComView())->update();
	((CGameView*)GetGameView())->update();

	TestCode();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	RECT rt = { 0, 0, FRAME_X, FRAME_Y };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, true);

	cs.cx = rt.right - rt.left;
	cs.cy = rt.bottom - rt.top;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::TestCode()
{
}

#include <SaveLoadMgr.h>

void CMainFrame::SaveSceneMenu()
{
	static wchar_t sz_Filter[] = L"Scene 파일(*.scene)|*.scene|모든파일(*.*)|*.*||";

	CFileDialog dlg(FALSE, nullptr, nullptr, OFN_HIDEREADONLY, sz_Filter);

	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Scene\\";
	dlg.m_ofn.lpstrInitialDir = strInitPath;

	CString pathName;
	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();
	}
	CSaveLoadMgr::SaveScene(pathName.GetBuffer());
}


void CMainFrame::LoadSceneMenu()
{
	static wchar_t sz_Filter[] = L"Scene 파일(*.scene)|*.scene|모든파일(*.*)|*.*||";

	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, sz_Filter);

	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Scene\\";
	dlg.m_ofn.lpstrInitialDir = strInitPath;

	CString pathName;
	if (IDOK == dlg.DoModal())
	{
		pathName = dlg.GetPathName();
	}
	CCore::GetInst()->SetState(SCENE_STATE::LOAD);

	CSaveLoadMgr::LoadScene(pathName.GetBuffer());
	CCore::GetInst()->SetState(SCENE_STATE::STOP);
}

#include <Transform.h>
#include <GameObject.h>
#include <SCeneMgr.h>
#include <Scene.h>

#include "GameObjectDlg.h"

void CMainFrame::CreateObject()
{
	//빈오브젝트 만들기
	CGameObject* pObj = new CGameObject;
	static UINT iNumName = 0;
	CString cName = L"";
	cName.Format(L"Empty Object %d", iNumName);
	++iNumName;
	wstring wName = cName.GetBuffer();
	pObj->SetName(wName);

	CTransform* pTransform = new CTransform;
	pTransform->SetLocalPos(Vec3(0.f, 0.f, 500.f));
	pTransform->SetLocalScale(Vec3(100.f, 100.f, 1.f));
	pTransform->SetLocalRot(Vec3(0.f, 0.f, 0.f));

	pObj->AddComponent(pTransform);
	CSceneMgr::GetInst()->GetCurScene()->AddObject(L"Default", pObj);

	((CHierachyView*)GetHierachyView())->GetGameObjectDlg()->AddGameObject(pObj, nullptr);
}
