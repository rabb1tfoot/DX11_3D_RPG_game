// CameraDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "CameraDlg.h"
#include "afxdialogex.h"

#include <GameObject.h>
#include <Camera.h>


// CCameraDlg 대화 상자

IMPLEMENT_DYNAMIC(CCameraDlg, CDialogEx)


CCameraDlg::CCameraDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CCameraDlg, pParent)
{

}

CCameraDlg::~CCameraDlg()
{
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbProj);
	DDX_Control(pDX, IDC_EDIT1, m_editFar);
	DDX_Control(pDX, IDC_EDIT2, m_editScale);
	DDX_Control(pDX, IDC_EDIT3, m_editFOV);
}

void CCameraDlg::update(CGameObject * _pTarget)
{
	SetTarget(_pTarget);

	CCamera* pCam = GetTarget()->Camera();

	if (IsEditActive() || IsCbActive())
		return;

	//직교 원근투영
	PROJ_TYPE ePType = pCam->GetProjType();

	if(PROJ_TYPE::ORTHOGRAPHIC == ePType)
	{
		if (m_cbProj.GetCurSel() != 0)
		{
			m_cbProj.SetCurSel(0);
			m_cbProj.GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(m_cbProj.GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM)m_cbProj.GetSafeHwnd());
		}
	}
	else
	{
		if (m_cbProj.GetCurSel() == 0)
		{
			m_cbProj.SetCurSel(1);
			m_cbProj.GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(m_cbProj.GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM)m_cbProj.GetSafeHwnd());
		}
	}

	CString str;
	float fData = 0.f;

	//Far
	fData = pCam->GetFar();
	str.Format(L"%f", fData);
	m_editFar.SetWindowTextW(str);

	//Scale
	fData = pCam->GetScale();
	str.Format(L"%f", fData);
	m_editScale.SetWindowTextW(str);

	//FOV
	fData = pCam->GetFOV();
	fData = (fData / XM_PI) * 180.f;
	str.Format(L"%f", fData);
	m_editFOV.SetWindowTextW(str);
}


BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT3, &CComponentDlg::OnEnSetfocusEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT3, &CComponentDlg::OnEnKillfocusEdit)

	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbSetfocusEdit)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbKillfocusEdit)

	ON_EN_CHANGE(IDC_EDIT1, &CCameraDlg::OnEnChangeEditFar)
	ON_EN_CHANGE(IDC_EDIT2, &CCameraDlg::OnEnChangeEditScale)
	ON_EN_CHANGE(IDC_EDIT3, &CCameraDlg::OnEnChangeEditFOV)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCameraDlg::OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// CCameraDlg 메시지 처리기



void CCameraDlg::OnEnChangeEditFar()
{
	CString str;
	m_editFar.GetWindowTextW(str);
	float fFar = (float)_wtof(str);
	GetTarget()->Camera()->SetFar(fFar);
}


void CCameraDlg::OnEnChangeEditScale()
{
	CString str;
	m_editScale.GetWindowTextW(str);
	float fScale = (float)_wtof(str);
	GetTarget()->Camera()->SetScale(fScale);
}


void CCameraDlg::OnEnChangeEditFOV()
{
	CString str;
	m_editFOV.GetWindowTextW(str);
	float fFOV = (float)_wtof(str);
	fFOV = (fFOV / 180.f) * XM_PI;
	GetTarget()->Camera()->SetFOV(fFOV);
}


void CCameraDlg::OnCbnSelchangeCombo()
{
	int iIdx = m_cbProj.GetCurSel();

	if (0 == iIdx)
	{
		GetTarget()->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
		m_editFOV.EnableWindow(false);
		m_editScale.EnableWindow(true);
	}
	else
	{
		GetTarget()->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		m_editFOV.EnableWindow(true);
		m_editScale.EnableWindow(false);
	}

	OnOK();
}


BOOL CCameraDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cbProj.AddString(L"Orthographic");
	m_cbProj.AddString(L"Perspective");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}