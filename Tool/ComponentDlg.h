#pragma once


// CComponentDlg 대화 상자

class CGameObject;

class CComponentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CComponentDlg)

public:
	CComponentDlg(UINT nIDTemplate, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CComponentDlg();

private:
	CGameObject* m_pTarget;
	bool m_bEditActive;
	bool m_bCbActive;

public:
	virtual void init(CGameObject* _pTarget) {}
	virtual void update(CGameObject* _pTarget) = 0;
	virtual void update_always(CGameObject* _pTarget) {};

public:
	CGameObject* GetTarget() { return m_pTarget; }
	void SetTarget(CGameObject* _pTarget) { m_pTarget = _pTarget; }

	bool IsEditActive() { return m_bEditActive; }
	bool IsCbActive() { return m_bCbActive; }

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CComponentDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg void OnEnSetfocusEdit(UINT _id);
	afx_msg void OnEnKillfocusEdit(UINT _id);

	afx_msg void OnCbSetfocusEdit(UINT _id);
	afx_msg void OnCbKillfocusEdit(UINT _id);


	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
};
