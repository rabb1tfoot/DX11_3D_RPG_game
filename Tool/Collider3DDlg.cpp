// Collider3DDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Collider3DDlg.h"
#include "afxdialogex.h"

#include <GameObject.h>
#include <Collider3D.h>


// CCollider3DDlg 대화 상자

IMPLEMENT_DYNAMIC(CCollider3DDlg, CDialogEx)

void CCollider3DDlg::update(CGameObject * _Target)
{
	SetTarget(_Target);

	if (IsEditActive() || IsCbActive())
		return;

	CCollider3D* pCol = GetTarget()->Collider3D();
	COLLIDER3D_TYPE ePType = pCol->GetCollider3DType();

	Vec3 vOffset = pCol->GetOffset();
	Vec3 vScale = pCol->GetScale();

	Vec3 vArrData[2] = { vOffset, vScale };
	COLLIDER3D_TYPE eType = pCol->GetCollider3DType();

	CString str;
	for (UINT i = 0; i < 6; ++i)
	{
		float fInputData = vArrData[i / 3][i % 3];
		str.Format(L"%f", fInputData);
		m_edit[i].SetWindowTextW(str);
	}
	m_cbType.SetCurSel((UINT)ePType);
}

CCollider3DDlg::CCollider3DDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CCollider3DDlg, pParent)
{

}

CCollider3DDlg::~CCollider3DDlg()
{
}

void CCollider3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbType);
	for (UINT i = 0; i < 6; ++i)
	{
		DDX_Control(pDX, 2000 + i, m_edit[i]);
	}
}


BEGIN_MESSAGE_MAP(CCollider3DDlg, CDialogEx)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT6, &CComponentDlg::OnEnSetfocusEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT6, &CComponentDlg::OnEnKillfocusEdit)

	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbSetfocusEdit)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbKillfocusEdit)

	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT6, &CCollider3DDlg::OnEnChangeEdit)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCollider3DDlg::OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// CCollider3DDlg 메시지 처리기


void CCollider3DDlg::OnEnChangeEdit(UINT _id)
{
	if (!IsEditActive())
		return;

	CCollider3D* pCol = GetTarget()->Collider3D();
	CString str;
	Vec3 arrData[2] = {};

	for (UINT i = 0; i < 6; ++i)
	{
		m_edit[i].GetWindowTextW(str);

		float fData = (float)_wtof(str);

		arrData[i / 3][i % 3] = fData;
	}
	pCol->SetOffset(arrData[0]);
	pCol->SetScale(arrData[1]);
}


void CCollider3DDlg::OnCbnSelchangeCombo()
{
	CComponentDlg::OnOK();
	CCollider3D* pCol = GetTarget()->Collider3D();
	int iIdx = m_cbType.GetCurSel();

	pCol->SetColliderType(COLLIDER3D_TYPE(iIdx));
}


BOOL CCollider3DDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_cbType.AddString(L"CUBE");
	m_cbType.AddString(L"SPHERE");

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
