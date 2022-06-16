#pragma once


class CGameObject;
class CMainFrame;

class CGameObjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGameObjectDlg)
private:
	HTREEITEM m_hDragItem;
	HTREEITEM m_hTargetItem;

	bool m_bInit;

public:
	void init();

private:
	void AddGameObject(CGameObject* _pObject, HTREEITEM _hParent);

private:
	HTREEITEM AddItem(CString _str, HTREEITEM _hParent, DWORD_PTR _dwData);
	void MoveItem(HTREEITEM _hSrcItem, HTREEITEM _hDestItem);
	void GetChildItem(HTREEITEM _hItem, vector<HTREEITEM>& _vecChild);

public:
	CGameObjectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CGameObjectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CGameObjectDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CTreeCtrl m_ctrlTree;
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	friend class CMainFrame;
};
