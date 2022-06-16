// CTransformDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TransformDlg.h"
#include "afxdialogex.h"

#include "MainFrm.h"

#include <GameObject.h>
#include <Transform.h>
#include <EventMgr.h>
#include <SCeneMgr.h>
#include <Scene.h>
#include <Layer.h>

#include <func.h>

#include "resource.h"


// CTransformDlg 대화 상자

IMPLEMENT_DYNAMIC(CTransformDlg, CDialogEx)

CTransformDlg::CTransformDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CTransformDlg, pParent)
	, m_bActive(FALSE)
{

}

CTransformDlg::~CTransformDlg()
{
}

void CTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (UINT i = 0; i < 9; ++i)
	{
		DDX_Control(pDX, 2000 + i, m_edit[i]);
	}
	DDX_Control(pDX, IDC_COMBO1, m_cbLayer);
}


BEGIN_MESSAGE_MAP(CTransformDlg, CDialogEx)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT9, &CComponentDlg::OnEnSetfocusEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT9, &CComponentDlg::OnEnKillfocusEdit)

	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbSetfocusEdit)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_COMBO1, IDC_COMBO1, &CComponentDlg::OnCbKillfocusEdit)

	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT9, &CTransformDlg::OnEnChangeEdit)

	ON_CBN_SELCHANGE(IDC_COMBO1, &CTransformDlg::OnCbnSelchangeCombo)

	ON_BN_CLICKED(IDC_CHECK1, &CTransformDlg::OnBnClickedActive)
END_MESSAGE_MAP()

void CTransformDlg::init(CGameObject * _pTarget)
{
	// 현재 레이어 목록 추가
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0, j = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);
		if (nullptr == pLayer)
			continue;

		m_cbLayer.InsertString(j, pLayer->GetName().c_str());
		m_cbLayer.SetItemData(j, i);

		++j;
	}

	m_bActive = false;
}

void CTransformDlg::update(CGameObject * _pTarget)
{
	SetTarget(_pTarget);

	bool a = IsEditActive();
	bool b = IsCbActive();

	if (IsEditActive() || IsCbActive())
		return;

	CString str;
	CTransform* pTrans = GetTarget()->Transform();
	Vec3 vRot = pTrans->GetLocalRot();

	vRot.x = GetAngle(vRot.x);
	vRot.y = GetAngle(vRot.y);
	vRot.z = GetAngle(vRot.z);

	Vec3 arrData[3] = { pTrans->GetLocalPos() , pTrans->GetLocalScale() , vRot };

	for (UINT i = 0; i < 9; ++i)
	{
		str.Format(L"%f", arrData[i / 3][i % 3]);
		m_edit[i].SetWindowTextW(str);
	}

	// Layer 확인
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->GetLayer(GetTarget()->GetLayerIdx());
	UINT iSel = m_cbLayer.FindString(0, pLayer->GetName().c_str());
	m_cbLayer.SetCurSel(iSel);

	m_bActive = GetTarget()->IsActive();
}

// CTransformDlg 메시지 처리기


void CTransformDlg::OnCbnSelchangeCombo()
{
	// Layer 변경
	int iSel = m_cbLayer.GetCurSel();
	DWORD_PTR iLayerIdx = m_cbLayer.GetItemData(iSel);
	int iCurIdx = GetTarget()->GetLayerIdx();

	// 변경 사항이 없다.
	if (iCurIdx == iLayerIdx)
		return;

	// 자식까지 전부 이동할 것인지 확인
	bool bMoveAll = true;
	if (!GetTarget()->GetChild().empty())
	{
		int id = MessageBox(L"자식 오브젝트도 Layer 를 변경하시겠습니까?", L"Layer 변경 확인", MB_OKCANCEL);
		if (id == IDCANCEL)
			bMoveAll = false;
	}

	tEvent event = {};
	event.eType = EVENT_TYPE::CHANGE_LAYER;
	event.lParam = (DWORD_PTR)GetTarget();
	event.wParam = (DWORD_PTR)MAKELONG(bMoveAll, iLayerIdx);

	CEventMgr::GetInst()->AddEvent(event);
}


void CTransformDlg::OnEnChangeEdit(UINT _id)
{
	// 직접 변경할 경우에만 오브젝트에 적용됨
	// 평소 오브젝트의 값을 가져오는 상황(EnChange) 으로 인해서 다시 오브젝트가 변경되는일을 막는다.
	if (!IsEditActive())
		return;

	CTransform* pTrans = GetTarget()->Transform();

	CString str;

	Vec3 arrData[3] = {};

	for (UINT i = 0; i < 9; ++i)
	{
		m_edit[i].GetWindowTextW(str);

		float fData = (float)_wtof(str);

		arrData[i / 3][i % 3] = fData;
	}

	pTrans->SetLocalPos(arrData[0]);
	pTrans->SetLocalScale(arrData[1]);

	arrData[2].x = GetRadian(arrData[2].x);
	arrData[2].y = GetRadian(arrData[2].y);
	arrData[2].z = GetRadian(arrData[2].z);

	pTrans->SetLocalRot(arrData[2]);
}


void CTransformDlg::OnBnClickedActive()
{
	m_bActive = !m_bActive;
	GetTarget()->Active(m_bActive);
}
