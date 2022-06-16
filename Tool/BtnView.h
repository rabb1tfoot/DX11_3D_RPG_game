#pragma once


// CBtnView 보기

class CBtnView : public CFormView
{
	DECLARE_DYNCREATE(CBtnView)

protected:
	CBtnView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CBtnView();


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CBtnView};
#endif

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void DoDataExchange(CDataExchange* pDX);
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif 
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnPlay;
	CButton m_btnPause;
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnPause();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


