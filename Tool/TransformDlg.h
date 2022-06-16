#pragma once

#include "ComponentDlg.h"

// CTransformDlg 대화 상자
class CGameObject;

class CTransformDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTransformDlg)

public:
	virtual void init(CGameObject* _pTarget);

	void update(CGameObject* _pTarget);

public:
	CTransformDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTransformDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTransformDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbLayer;
	afx_msg void OnCbnSelchangeCombo();
	CEdit m_edit[9];
	afx_msg void OnEnChangeEdit(UINT _id);
	bool m_bActive;

	afx_msg void OnBnClickedActive();
};
