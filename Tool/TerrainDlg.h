#pragma once


// CTerrainDlg 대화 상자

#include "ComponentDlg.h"
#include <atlimage.h>

class CTerrainDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTerrainDlg)

public:
	virtual void init(CGameObject* _pTarget);
	virtual void update(CGameObject* _pTarget);

private:
	void Loadthumbnail();

public:
	CTerrainDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTerrainDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTerrainDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CImage m_vImage[8];
	BOOL m_bActive;
	CEdit m_editFaceX;
	CEdit m_editFaceZ;
	CEdit m_editBrushIdx;
	CEdit m_editTileIdx;
	CEdit m_editMode;
	afx_msg void OnEnChangeXZ(UINT _id);
	afx_msg void OnPaint();
};
