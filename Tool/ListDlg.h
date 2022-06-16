#pragma once


// CListDlg 대화 상자

class CListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListDlg)

private:
	CString		m_strCaption;
	CString		m_strSelect;
	CString		m_strKey;

	vector<CString> m_vecFileName;
	vector<CString> m_vecResName;

public:
	const CString& GetSelectItem() { return m_strSelect; }
	const CString& GetSelectSecondItem() { return m_strKey; }

public:
	//CListDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	CListDlg(CString _strCaption, vector<CString>& _vecFileName, vector<CString>& _vecResName, CWnd* pParent = nullptr);
	virtual ~CListDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CListDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrl;
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
