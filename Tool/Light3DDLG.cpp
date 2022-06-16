// Light3D.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Light3DDLG.h"
#include "afxdialogex.h"

#include "MainFrm.h"

#include <SCeneMgr.h>
#include <Scene.h>
#include <Layer.h>

#include <EventMgr.h>
#include <GameObject.h>
#include <Light3D.h>


// CLight3D 대화 상자

IMPLEMENT_DYNAMIC(CLight3DDLG, CDialogEx)

CLight3DDLG::CLight3DDLG(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CLight3D, pParent)
	, m_bActive(FALSE)
{

}

CLight3DDLG::~CLight3DDLG()
{
}

void CLight3DDLG::init(CGameObject * _pTarget)
{
	m_cbType.ResetContent();

	CString LightType[3] = { L"DIRECTIONAL", L"POINT", L"SPOT" };

	for (UINT i = 0; i < (UINT)LIGHT_TYPE::END; ++i)
	{
		m_cbType.InsertString(i, LightType[i]);
	}
}

void CLight3DDLG::update(CGameObject * _pTarget)
{
	if (IsEditActive() || IsCbActive())
		return;
	SetTarget(_pTarget);

	CString str;

	CLight3D* pLight = GetTarget()->Light3D();
	const tLight3DInfo& pLightInfo = pLight->GetLight3DInfo();

	//라이트 타입 설정
	CString LightType[3] = { L"DIRECTIONAL", L"POINT", L"SPOT" };
	UINT iSel = m_cbType.FindString(0, LightType[pLightInfo.iType]);
	m_cbType.SetCurSel(iSel);

	//속성 설정
	Vec3 arrData[4] = { pLightInfo.vDir, pLightInfo.vDiff, pLightInfo.vSpec
	, pLightInfo.vAmb};

	float fLRange = pLightInfo.fRange;
	float fLAngle = (pLightInfo.fAngle * 180.f) / XM_PI;

	for (UINT i = 0; i < 12; ++i)
	{
		str.Format(L"%f", arrData[i / 3][i % 3]);
		m_edit[i].SetWindowTextW(str);
	}
	str.Format(L"%f", fLRange);
	m_edit[12].SetWindowTextW(str);
	str.Format(L"%f", fLAngle);
	m_edit[13].SetWindowTextW(str);

	if (m_cbType.GetCurSel() == (int)LIGHT_TYPE::DIRECTIONAL)
	{
		DisableDirectionalData();
	}
	else if (m_cbType.GetCurSel() == (int)LIGHT_TYPE::POINT)
	{
		DisablePointData();
	}
	else if(m_cbType.GetCurSel() == (int)LIGHT_TYPE::SPOT)
	{
		DisableSpotData();
	}

}

void CLight3DDLG::DisableDirectionalData()
{
	//전광원은 범위와 각도가 무시된다
	m_edit[12].EnableWindow(false);
	m_edit[13].EnableWindow(false);

	for (int i = 0; i < 12; ++i)
	{
		m_edit[i].EnableWindow(true);
	}
}

void CLight3DDLG::DisablePointData()
{
	//점광원은 방향, 환경광, 각도가 무시된다.
	m_edit[0].EnableWindow(false);
	m_edit[1].EnableWindow(false);
	m_edit[2].EnableWindow(false);

	m_edit[9].EnableWindow(false);
	m_edit[10].EnableWindow(false);
	m_edit[11].EnableWindow(false);

	m_edit[13].EnableWindow(false);

	for (int i = 4; i < 9; ++i)
	{
		m_edit[i].EnableWindow(true);
	}
	m_edit[12].EnableWindow(true);
}

void CLight3DDLG::DisableSpotData()
{
	//스포트라이트는 환경광이 무시된다.
	m_edit[9].EnableWindow(false);
	m_edit[10].EnableWindow(false);
	m_edit[11].EnableWindow(false);

	for (int i = 0; i < 9; ++i)
	{
		m_edit[i].EnableWindow(true);
	}

	for (int i = 12; i < 14; ++i)
	{
		m_edit[i].EnableWindow(true);
	}
}

void CLight3DDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbType);
	for (UINT i = 0; i < 12; ++i)
	{
		DDX_Control(pDX, 2000 + i, m_edit[i]);
	}
	DDX_Control(pDX, 2000 + 12, m_edit[12]);
	DDX_Control(pDX, 2000 + 13, m_edit[13]);
}


BEGIN_MESSAGE_MAP(CLight3DDLG, CDialogEx)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT14, &CComponentDlg::OnEnSetfocusEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT14, &CComponentDlg::OnEnKillfocusEdit)

	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbSetfocusEdit)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbKillfocusEdit)

	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT14, &CLight3DDLG::OnEnChangeEdit)

	ON_CBN_SELCHANGE(IDC_COMBO1, &CLight3DDLG::OnCbnSelchangeType)
END_MESSAGE_MAP()


// CLight3D 메시지 처리기


void CLight3DDLG::OnCbnSelchangeType()
{
	//변경된 타입가져오기
	int iSel = m_cbType.GetCurSel();
	CLight3D* pLight = GetTarget()->Light3D();
	const tLight3DInfo& tInfo = pLight->GetLight3DInfo();
	int iCurType = (int)tInfo.iType;

	//동일한 타입로 변경시 변화없음
	if (iSel == iCurType)
		return;

	pLight->SetLightType((LIGHT_TYPE)iSel);
}


void CLight3DDLG::OnEnChangeEdit(UINT _id)
{
	if (!IsEditActive())
		return;

	CLight3D* pLight = GetTarget()->Light3D();
	CString str;
	Vec3 arrData[4] = {};

	for (UINT i = 0; i < 12; ++i)
	{
		m_edit[i].GetWindowTextW(str);

		float fData = (float)_wtof(str);
		arrData[i / 3][i % 3] = fData;
	}

	pLight->SetLightDir(arrData[0]);
	pLight->SetLightDiffuse(arrData[1]);
	pLight->SetLightSpecular(arrData[2]);
	pLight->SetLightAmbient(arrData[3]);

	m_edit[12].GetWindowTextW(str);
	float fData = (float)_wtof(str);
	pLight->SetLightRange(fData);

	m_edit[13].GetWindowTextW(str);
	fData = (float)_wtof(str);
	pLight->SetLightAngle(fData);
	
}
