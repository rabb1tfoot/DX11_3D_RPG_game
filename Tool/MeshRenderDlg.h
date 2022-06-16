#pragma once

#include "ComponentDlg.h"

class CGameObject;

class CMeshRenderDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CMeshRenderDlg)

public:
	CMeshRenderDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMeshRenderDlg();

public:
	void update(CGameObject* _pTarget);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMeshRenderDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMesh();
	afx_msg void OnBnClickedMtrl();
	CButton m_BtnMesh;
	CButton m_BtnMtrl;
	CEdit m_editMesh;
	CEdit m_editMtrl;
	bool m_bShadow;
	afx_msg void OnBnClickedShadow();
};
