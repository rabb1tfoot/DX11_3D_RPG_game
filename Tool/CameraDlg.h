#pragma once


// CCameraDlg 대화 상자

#include "ComponentDlg.h"

class CCameraDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CCameraDlg)

public:
	virtual void update(CGameObject* _pTarget);

public:
	CCameraDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCameraDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCameraDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbProj;
	CEdit m_editFar;
	CEdit m_editScale;
	CEdit m_editFOV;

	afx_msg void OnEnChangeEditFar();
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnEnChangeEditFOV();
	afx_msg void OnCbnSelchangeCombo();

	virtual BOOL OnInitDialog();
};
