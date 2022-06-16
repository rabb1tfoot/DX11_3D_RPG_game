// Animator3DDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "Animator3DDlg.h"
#include "afxdialogex.h"

#include <Animator3D.h>
#include <PathMgr.h>


// CAnimator3DDlg 대화 상자

IMPLEMENT_DYNAMIC(CAnimator3DDlg, CDialogEx)

CAnimator3DDlg::CAnimator3DDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CAnimator3DDlg, pParent)
	, m_bListActive(false)
{

}

CAnimator3DDlg::~CAnimator3DDlg()
{
}

void CAnimator3DDlg::update(CGameObject * _pTarget)
{
	SetTarget(_pTarget);
	CAnimator3D* pAnim = _pTarget->Animator3D();

	if (IsEditActive() || IsCbActive() || IsListActive())
		return;

	//리스트 아이템 추가
	m_LCutAnim.DeleteAllItems();
	vector<tAnim3D*> pVecAnim = pAnim->GetAnimClips();
	for (UINT i = 0; i < pVecAnim.size(); ++i)
	{
		CString str = pVecAnim[i]->strName.c_str();
		float fStart = pVecAnim[i]->fStartFrm;
		float fEnd = pVecAnim[i]->fEndFrm;

		m_LCutAnim.InsertItem(i, str);
		str.Format(L"%f", fStart);
		m_LCutAnim.SetItemText(i, 1, str);
		str.Format(L"%f", fEnd);
		m_LCutAnim.SetItemText(i, 2, str);
	}

	tAnim3D* tAnim = tAnim = pAnim->GetCurAnimCutClip();
	m_SliderCur.SetRange((int)(tAnim->fStartFrm * 100.f), (int)(tAnim->fEndFrm * 100.f), TRUE);
	m_SliderCur.Invalidate(TRUE);

	tAnim = pAnim->FindAnimCutClip(L"TotalAnimClip");
	m_SliderStart.SetRange((int)(tAnim->fStartFrm * 100.f), (int)(tAnim->fEndFrm * 100.f), TRUE);
	m_SliderStart.Invalidate(TRUE);
	m_SliderEnd.SetRange((int)(tAnim->fStartFrm * 100.f), (int)(tAnim->fEndFrm * 100.f), TRUE);
	m_SliderEnd.Invalidate(TRUE);
}

void CAnimator3DDlg::update_always(CGameObject * _pTarget)
{
	SetTarget(_pTarget);
	CAnimator3D* pAnim = _pTarget->Animator3D();

	float fCurFrm = pAnim->GetCurFrm();
	CString str;
	str.Format(L"%.2f", fCurFrm);
	m_editFrm[0].SetWindowTextW(str);

	m_SliderCur.SetPos(int(fCurFrm * 100.f));
}


void CAnimator3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_LCutAnim);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderStart);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderEnd);
	DDX_Control(pDX, IDC_SLIDER3, m_SliderCur);
	for (UINT i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, 2000 + i, m_editFrm[i]);
	}
	DDX_Control(pDX, IDC_EDIT4, m_editAnimName);
}


BEGIN_MESSAGE_MAP(CAnimator3DDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAnimator3DDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON6, &CAnimator3DDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON2, &CAnimator3DDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON3, &CAnimator3DDlg::OnBnClickedButtonAddCutAnim)
	ON_BN_CLICKED(IDC_BUTTON4, &CAnimator3DDlg::OnBnClickedChangeCurCutAnim)
	ON_BN_CLICKED(IDC_BUTTON5, &CAnimator3DDlg::OnBnClickedDeleteCutAnim)
	ON_BN_CLICKED(IDC_BUTTON7, &CAnimator3DDlg::OnBnClickedSaveFile)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST2, &CAnimator3DDlg::OnNMSetfocusList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST2, &CAnimator3DDlg::OnNMKillfocusList)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT3, &CAnimator3DDlg::OnEnChangeEdit)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


void CAnimator3DDlg::init(CGameObject * _pTarget)
{
	//리스트 초기화 부분
	m_LCutAnim.InsertColumn(0, _T("이름"), LVCFMT_LEFT, 150);
	m_LCutAnim.InsertColumn(1, _T("시작Frm"), LVCFMT_LEFT, 60);
	m_LCutAnim.InsertColumn(2, _T("종료Frm"), LVCFMT_LEFT, 60);

	m_SliderCur.SetRange(1, 10, TRUE);
}

// CAnimator3DDlg 메시지 처리기

void CAnimator3DDlg::OnBnClickedButtonStop()
{
	GetTarget()->Animator3D()->StopAnim();
	float fStartFrm = GetTarget()->Animator3D()->GetCurAnimCutClip()->fStartFrm;
	GetTarget()->Animator3D()->SetCurFrm(fStartFrm);
}


void CAnimator3DDlg::OnBnClickedButtonAddCutAnim()
{
	CString str;
	m_editFrm[1].GetWindowTextW(str);
	float fStart = (float)_ttof(str);
	m_editFrm[2].GetWindowTextW(str);
	float fEnd = (float)_ttof(str);
	m_editAnimName.GetWindowTextW(str);

	GetTarget()->Animator3D()->AddAnimCutClip(fStart, fEnd, str.GetBuffer());
}


void CAnimator3DDlg::OnBnClickedChangeCurCutAnim()
{
	CAnimator3D* pAnim = GetTarget()->Animator3D();
	CString str;
	int count = m_LCutAnim.GetItemCount(), index = 0;
	for (int i = 0; i < count; i++)
	{
		// 개별 항목의 현재 상태가 선택되어진 상태일 경우
		if (LVIS_SELECTED == m_LCutAnim.GetItemState(i, LVIS_SELECTED))
		{

			str = m_LCutAnim.GetItemText(i, 0);

		}
	}

	pAnim->SetCurAnimCutClip(str.GetBuffer());

}


void CAnimator3DDlg::OnBnClickedDeleteCutAnim()
{
	CAnimator3D* pAnim = GetTarget()->Animator3D();
	CString str;
	int count = m_LCutAnim.GetItemCount(), index = 0;
	for (int i = 0; i < count; i++)
	{
		if (LVIS_SELECTED == m_LCutAnim.GetItemState(i, LVIS_SELECTED))
		{
			str = m_LCutAnim.GetItemText(i, 0);
		}
	}

	//삭제할 클립이 현재 애니메이션인경우 임시로 바꿔준다.
	if (!wcscmp(str.GetBuffer(), pAnim->GetCurAnimCutClip()->strName.c_str()))
		pAnim->SetCurAnimCutClip(L"TotalAnimClip");

	//TotalAnimClip은 삭제 불가능
	if(!wcscmp(str.GetBuffer(), L"TotalAnimClip"))
		return;

	pAnim->DeleteAnimCurClip(str.GetBuffer());
}

void CAnimator3DDlg::OnBnClickedSaveFile()
{
	FILE* pFile = nullptr;
	wstring strFileName =CPathMgr::GetResPath();
	strFileName += L"Animation3D\\";
	strFileName += GetTarget()->GetName();
	strFileName += L".Anim";
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	const vector<tAnim3D*> vecAnim = GetTarget()->Animator3D()->GetAnimClips();
	int vecAnimlength = vecAnim.size();
	fwrite(&vecAnimlength, sizeof(int), 1, pFile);

	for (int i = 0; i < vecAnimlength; ++i)
	{
		fwrite(&vecAnim[i]->fStartFrm, sizeof(float), 1, pFile);
		fwrite(&vecAnim[i]->fEndFrm, sizeof(float), 1, pFile);
		SaveWString(vecAnim[i]->strName.c_str(), pFile);
	}

	fclose(pFile);
}

void CAnimator3DDlg::OnEnChangeEdit(UINT _id)
{
	CAnimator3D* pAnim = GetTarget()->Animator3D();
	//값이 바뀌면 슬라이더를 변화시킨다.
	if (_id == IDC_EDIT1)
	{
		//애니메이션이 실행중에는 현재프레임을 변경할 수 없다.
		if (pAnim->IsPlay())
			return;
		CString str;
		m_editFrm[0].GetWindowTextW(str);
		int Frm = (int)(_ttof(str) * 100.f);
		m_SliderCur.SetPos(Frm);
	}

	if (_id == IDC_EDIT2)
	{
		CString str;
		m_editFrm[1].GetWindowTextW(str);
		int Frm = (int)(_ttof(str) * 100.f);
		m_SliderStart.SetPos(Frm);
	}

	if (_id == IDC_EDIT3)
	{
		CString str;
		m_editFrm[2].GetWindowTextW(str);
		int Frm = (int)(_ttof(str) * 100.f);
		m_SliderEnd.SetPos(Frm);
	}
}

void CAnimator3DDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CAnimator3D* pAnim = GetTarget()->Animator3D();
	if ((CSliderCtrl*)pScrollBar == &m_SliderCur)
	{
		//애니메이션이 실행중에는 현재프레임을 변경할 수 없다.
		if (pAnim->IsPlay())
			return;

		CString str;
		float Frm = m_SliderCur.GetPos() / 100.f;
		str.Format(L"%f", Frm);
		SetDlgItemTextW(IDC_EDIT1, str);

		pAnim->SetCurFrm(Frm);
	}
	else if ((CSliderCtrl*)pScrollBar == &m_SliderStart)
	{
		CString str;
		float Frm = m_SliderStart.GetPos() / 100.f;
		str.Format(L"%f", Frm);
		SetDlgItemTextW(IDC_EDIT2, str);
	}
	else if ((CSliderCtrl*)pScrollBar == &m_SliderEnd)
	{
		CString str;
		float Frm = m_SliderEnd.GetPos() / 100.f;
		str.Format(L"%f", Frm);
		SetDlgItemTextW(IDC_EDIT3, str);
	}

	CComponentDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAnimator3DDlg::OnBnClickedButtonPlay()
{
	GetTarget()->Animator3D()->PlayAnim();
}

void CAnimator3DDlg::OnBnClickedButtonPause()
{
	GetTarget()->Animator3D()->StopAnim();
}

void CAnimator3DDlg::OnNMSetfocusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_bListActive = true;
	*pResult = 0;
}


void CAnimator3DDlg::OnNMKillfocusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_bListActive = false;
	*pResult = 0;
}

