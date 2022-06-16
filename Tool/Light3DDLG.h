#pragma once


// CLight3D 대화 상자

#include "ComponentDlg.h"

class CGameObject;

class CLight3DDLG : public CComponentDlg
{
	DECLARE_DYNAMIC(CLight3DDLG)

public:
	virtual void init(CGameObject* _pTarget);
	virtual void update(CGameObject* _pTarget);

private:
	void DisableDirectionalData();
	void DisablePointData();
	void DisableSpotData();

public:
	CLight3DDLG(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLight3DDLG();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLight3D };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bActive;
	CComboBox m_cbType;
	CEdit m_edit[14];
	afx_msg void OnCbnSelchangeType();
	afx_msg void OnEnChangeEdit(UINT _id);
};
