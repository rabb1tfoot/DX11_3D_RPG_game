// GameObjectDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "GameObjectDlg.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "ComponentView.h"

#include <EventMgr.h>
#include <SCeneMgr.h>
#include <Scene.h>
#include <Layer.h>
#include <GameObject.h>


// CGameObjectDlg 대화 상자

IMPLEMENT_DYNAMIC(CGameObjectDlg, CDialogEx)


CGameObjectDlg::CGameObjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CGameObjectDlg, pParent)
	, m_hDragItem(nullptr)
	, m_bInit(false)
{

}

CGameObjectDlg::~CGameObjectDlg()
{
}

void CGameObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ctrlTree);

	m_bInit = true;
}


void CGameObjectDlg::init()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);

		if (nullptr == pLayer)
			continue;

		const vector<CGameObject*>& vecParentObj = pLayer->GetParentObject();

		for (UINT i = 0; i < vecParentObj.size(); ++i)
		{
			AddGameObject(vecParentObj[i], nullptr);
		}
	}
}

void CGameObjectDlg::AddGameObject(CGameObject * _pObject, HTREEITEM _hParent)
{
	HTREEITEM hItem = AddItem(_pObject->GetName().c_str(), _hParent, (DWORD_PTR)_pObject);

	const vector<CGameObject*>& vecChild = _pObject->GetChild();

	for (UINT i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(vecChild[i], hItem);
	}
}

HTREEITEM CGameObjectDlg::AddItem(CString _str, HTREEITEM _hParent, DWORD_PTR _dwData)
{
	TVINSERTSTRUCT tInsert = {};

	tInsert.hParent = _hParent;
	tInsert.hInsertAfter = TVI_SORT;
	tInsert.item.pszText = _str.GetBuffer();
	tInsert.item.mask = TVIF_TEXT;

	HTREEITEM hItem = m_ctrlTree.InsertItem(&tInsert);
	m_ctrlTree.SetItemData(hItem, _dwData);

	return hItem;
}

void CGameObjectDlg::MoveItem(HTREEITEM _hSrcItem, HTREEITEM _hDestItem)
{
	wchar_t szName[255] = {};

	TV_ITEMW item = {};

	item.hItem = _hSrcItem;
	item.pszText = szName;
	item.cchTextMax = 255;
	item.mask = TVIF_TEXT | TVIF_HANDLE;

	m_ctrlTree.GetItem(&item);;

	DWORD_PTR dwData = m_ctrlTree.GetItemData(_hSrcItem);

	HTREEITEM hCopyItem = AddItem(szName, _hDestItem, dwData);

	vector<HTREEITEM> vecChild;
	GetChildItem(_hSrcItem, vecChild);

	for (UINT i = 0; i < vecChild.size(); ++i)
	{
		MoveItem(vecChild[i], hCopyItem);
	}

	m_ctrlTree.DeleteItem(_hSrcItem);

	m_ctrlTree.Expand(_hDestItem, TVE_EXPAND);
}

void CGameObjectDlg::GetChildItem(HTREEITEM _hItem, vector<HTREEITEM>& _vecChild)
{
	if (!m_ctrlTree.ItemHasChildren(_hItem))
		return;

	HTREEITEM hChildItem = m_ctrlTree.GetChildItem(_hItem);
	_vecChild.push_back(hChildItem);

	while (true)
	{
		hChildItem = m_ctrlTree.GetNextSiblingItem(hChildItem);

		if (nullptr == hChildItem)
			break;

		_vecChild.push_back(hChildItem);
	}
}


BEGIN_MESSAGE_MAP(CGameObjectDlg, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CGameObjectDlg::OnTvnSelchangedTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, &CGameObjectDlg::OnTvnBegindragTree)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, &CGameObjectDlg::OnTvnBeginlabeleditTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE1, &CGameObjectDlg::OnTvnEndlabeleditTree)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CGameObjectDlg 메시지 처리기


void CGameObjectDlg::PostNcDestroy()
{
	delete this;

	CDialogEx::PostNcDestroy();
}


void CGameObjectDlg::OnOK()
{

}


void CGameObjectDlg::OnCancel()
{

}

void CGameObjectDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_bInit)
	{
		// Tree Control Size 를 Dialog Size 로 맞춘다.
		m_ctrlTree.SetWindowPos(nullptr, 0, 0, cx, cy, 0);
	}
}

void CGameObjectDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	CGameObject* pTargetObj = (CGameObject*)m_ctrlTree.GetItemData(pNMTreeView->itemNew.hItem);

	CComponentView* pComView = (CComponentView*)((CMainFrame*)AfxGetMainWnd())->GetComView();

	pComView->SetTarget(pTargetObj);

	*pResult = 0;
}


void CGameObjectDlg::OnTvnBegindragTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	SetCapture();

	m_hDragItem = pNMTreeView->itemNew.hItem;

	*pResult = 0;
}

void CGameObjectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nullptr != m_hDragItem)
	{
		::ClientToScreen(m_hWnd, &point);
		m_ctrlTree.ScreenToClient(&point);

		//현재 마우스 좌표에 있는 아이템을 타깃으로 한다.
		m_hTargetItem = m_ctrlTree.HitTest(point);

		if (nullptr != m_hTargetItem)
		{
			m_ctrlTree.SelectDropTarget(m_hTargetItem);
		}
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CGameObjectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	if (nullptr == m_hDragItem)
	{
		CDialogEx::OnLButtonUp(nFlags, point);
		return;
	}

	// 변경되는 아이템의 관계에 대응하는 GameObject 들의 실제 관계도 변경되어야 한다.

	// 예외처리
	// 1. 자기 자신의 자식으로 들어가는 경우
	// 2. 부모가 자식 아이템의 자식으로 들어가는 경우

	CGameObject* pSrcObj = (CGameObject*)m_ctrlTree.GetItemData(m_hDragItem);
	CGameObject* pDestObj = nullptr;

	if (nullptr != m_hTargetItem)
		pDestObj = (CGameObject*)m_ctrlTree.GetItemData(m_hTargetItem);

	//최상위 부모 오브젝트인경우
	if (nullptr == pDestObj)
	{
		if (nullptr != pSrcObj->GetParent())
		{
			tEvent event{};
			event.eType = EVENT_TYPE::CLEAR_PARANT;
			event.lParam = (DWORD_PTR)pSrcObj;
			CEventMgr::GetInst()->AddEvent(event);

			MoveItem(m_hDragItem, m_hTargetItem);
		}
	}
	//자식으로 들어가는경우
	else
	{
		if (m_hDragItem != m_hTargetItem && !pDestObj->IsAncestor(pSrcObj))
		{
			MoveItem(m_hDragItem, m_hTargetItem);
			pDestObj->AddChild(pSrcObj);
		}
	}

	m_hTargetItem = nullptr;
	m_hDragItem = nullptr;
	m_ctrlTree.SelectDropTarget(nullptr);

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CGameObjectDlg::OnTvnBeginlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CGameObjectDlg::OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	
	CEdit* pEdit = m_ctrlTree.GetEditControl();

	if (pEdit)
	{
		CString strEdit;
		pEdit->GetWindowTextW(strEdit);

		if (strEdit.GetLength() > 0)
		{
			m_ctrlTree.SetItemText(pTVDispInfo->item.hItem, strEdit);
			CGameObject* pObject = (CGameObject*)m_ctrlTree.GetItemData(pTVDispInfo->item.hItem);
			pObject->SetName(strEdit.GetBuffer());
		}
	}

	*pResult = 0;
}
