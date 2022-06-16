#pragma once

#include "ComponentDlg.h"

// CCollider3DDlg 대화 상자
class CGameObject;

class CCollider3DDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CCollider3DDlg)

public:
	virtual void update(CGameObject* _Target);
	CCollider3DDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCollider3DDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCollider3DDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit[6];
	afx_msg void OnEnChangeEdit(UINT _id);
	afx_msg void OnCbnSelchangeCombo();
	// m_TypeCombo
	CComboBox m_cbType;
	virtual BOOL OnInitDialog();
};
