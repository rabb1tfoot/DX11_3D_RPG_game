﻿#pragma once


// CResDlg 대화 상자

class CResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResDlg)

private:
	bool m_bInit;

public:
	CResDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CResDlg();

public:
	void init();
	void Renew();

private:
	void LoadResource(CString _strFolderPath);

	//TreeControl
	HTREEITEM AddItem(CString _str, HTREEITEM _hParent, DWORD_PTR _dwData, HTREEITEM _hSortop);
	void MoveItem(HTREEITEM _hSrcItem, HTREEITEM _hDestItem);
	void GetChildItem(HTREEITEM _hItem, vector<HTREEITEM>& _vecChild);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CResDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrlTree;
	afx_msg void OnTvnBeginlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
