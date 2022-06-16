#pragma once


// CAnimator3DDlg 대화 상자

#include "ComponentDlg.h"

class CAnimator3DDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CAnimator3DDlg)

private:
	bool m_bListActive;

public:
	virtual void init(CGameObject* _pTarget);
	virtual void update(CGameObject* _pTarget);
	virtual void update_always(CGameObject* _pTarget);

public:
	CAnimator3DDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAnimator3DDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAnimator3DDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	bool IsListActive() { return m_bListActive; }
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonAddCutAnim();
	afx_msg void OnBnClickedChangeCurCutAnim();
	afx_msg void OnBnClickedDeleteCutAnim();
	CListCtrl m_LCutAnim;
	CEdit m_editFrm[3];
	CEdit m_editAnimName;
	CSliderCtrl m_SliderStart;
	CSliderCtrl m_SliderEnd;
	CSliderCtrl m_SliderCur;
	afx_msg void OnNMSetfocusList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit(UINT _id);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSaveFile();
};
