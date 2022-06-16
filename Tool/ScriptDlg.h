#pragma once


// CScriptDlg 대화 상자

#include "ComponentDlg.h"

class CScriptDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CScriptDlg)

public:
	virtual void init(CGameObject* _pTarget);
	virtual void update(CGameObject* _pTarget);

public:
	CScriptDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CScriptDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CScriptDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlScript;
	CComboBox m_cbScript;
	afx_msg void OnBnClickedAddScript();
};
