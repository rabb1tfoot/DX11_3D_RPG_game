// ComponentView.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "ComponentView.h"

#include <SCeneMgr.h>
#include <GameObject.h>
#include <Scene.h>
#include <Layer.h>

#include "resource.h"
#include "TransformDlg.h"
#include "MeshRenderDlg.h"
#include "CameraDlg.h"
#include "ScriptDlg.h"
#include "Light3DDlg.h"
#include "TerrainDlg.h"
#include "Collider3DDlg.h"
#include "Animator3DDlg.h"
#include "AddComDlg.h"

#include <TimeMgr.h>
#include <Resource.h>


// CComponentView

IMPLEMENT_DYNCREATE(CComponentView, CView)

CComponentView::CComponentView()
	: m_pTarget(nullptr)
	, m_arrComDlg{}
	, m_pResource(nullptr)
	, m_AddCom(nullptr)
{

}

CComponentView::~CComponentView()
{
}


int CComponentView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_arrComDlg[(UINT)DLG_TYPE::TRANSFORM] = new CTransformDlg;
	m_arrComDlg[(UINT)DLG_TYPE::TRANSFORM]->Create(IDD_CTransformDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::TRANSFORM]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::MESHRENDER] = new CMeshRenderDlg;
	m_arrComDlg[(UINT)DLG_TYPE::MESHRENDER]->Create(IDD_CMeshRenderDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::MESHRENDER]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::CAMERA] = new CCameraDlg;
	m_arrComDlg[(UINT)DLG_TYPE::CAMERA]->Create(IDD_CCameraDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::CAMERA]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::LIGHT3D] = new CLight3DDLG;
	m_arrComDlg[(UINT)DLG_TYPE::LIGHT3D]->Create(IDD_CLight3D, this);
	m_arrComDlg[(UINT)DLG_TYPE::LIGHT3D]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::TERRAIN] = new CTerrainDlg;
	m_arrComDlg[(UINT)DLG_TYPE::TERRAIN]->Create(IDD_CTerrainDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::TERRAIN]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::Collider3D] = new CCollider3DDlg;
	m_arrComDlg[(UINT)DLG_TYPE::Collider3D]->Create(IDD_CCollider3DDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::Collider3D]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::Collider3D] = new CCollider3DDlg;
	m_arrComDlg[(UINT)DLG_TYPE::Collider3D]->Create(IDD_CCollider3DDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::Collider3D]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::ANIMATOR3D] = new CAnimator3DDlg;
	m_arrComDlg[(UINT)DLG_TYPE::ANIMATOR3D]->Create(IDD_CAnimator3DDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::ANIMATOR3D]->ShowWindow(false);

	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT] = new CScriptDlg;
	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->Create(IDD_CScriptDlg, this);
	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->ShowWindow(false);

	m_AddCom = new CAddComDlg;
	m_AddCom->Create(IDD_CAddComDlg, this);
	m_AddCom->ShowWindow(false);

	return 0;
}


BEGIN_MESSAGE_MAP(CComponentView, CView)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CComponentView 그리기

void CComponentView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
}


// CComponentView 진단

#ifdef _DEBUG
void CComponentView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CComponentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
void CComponentView::init()
{
	// 타겟 오브젝트 찾기
	vector<CGameObject*> vecOut;
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"Player");
	vecOut = pLayer->GetParentObject();
	if (!vecOut.empty())
	{
		m_pTarget = vecOut[0];
	}

	SetDlgPos();

	if (nullptr == m_pTarget)
		return;

	for (UINT i = 0; i < (UINT)DLG_TYPE::END; ++i)
	{
		if (nullptr == m_arrComDlg[i])
			continue;

		m_arrComDlg[i]->init(m_pTarget);
	}

	m_AddCom->init(m_pTarget);
}
void CComponentView::update_always()
{
	if (nullptr == m_pTarget)
		return;

	//애니메이션은 소수초 단위 프레임이라서 always에서 update한다.
	if (nullptr != m_pTarget->GetComponent(COMPONENT_TYPE::ANIMATOR3D))
	{
		m_arrComDlg[(UINT)DLG_TYPE::ANIMATOR3D]->update_always(m_pTarget);
	}
}
void CComponentView::update()
{
	update_always();

	// 1 초에 한번
	static float fAccTime = 0.f;
	fAccTime += DT;
	if (fAccTime < 1.f)
		return;

	fAccTime = 0.f;

	// CompnentView Update
	SetDlgPos();

	if (nullptr == m_pTarget)
		return;

	for (UINT i = 0; i < (UINT)DLG_TYPE::END; ++i)
	{
		if (nullptr == m_pTarget->GetComponent((COMPONENT_TYPE)i)
			|| nullptr == m_arrComDlg[i])
			continue;

		m_arrComDlg[i]->update(m_pTarget);
	}

	if (nullptr != m_pTarget->GetComponent(COMPONENT_TYPE::SCRIPT))
	{
		m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->update(m_pTarget);
	}

	m_AddCom->update(m_pTarget);
}
void CComponentView::SetTarget(CGameObject * _pTarget)
{
	m_pTarget = _pTarget;
	SetDlgPos();
}
void CComponentView::SetDlgPos()
{
	if (nullptr == m_pTarget)
	{
		for (UINT i = 0; i < (UINT)DLG_TYPE::END; ++i)
		{
			if (nullptr == m_arrComDlg[i])
				continue;

			m_arrComDlg[i]->ShowWindow(false);
		}

		return;
	}

	// Dlg Size 조절
	int iHeight = 0;

	CRect rtView;
	GetClientRect(rtView);

	CRect rtDlg;

	for (UINT i = 0; i < (UINT)DLG_TYPE::END; ++i)
	{
		if (nullptr == m_arrComDlg[i] || i == (UINT)DLG_TYPE::SCRIPT)
			continue;

		if (m_pTarget->GetComponent((COMPONENT_TYPE)i))
		{
			m_arrComDlg[i]->GetWindowRect(rtDlg);
			m_arrComDlg[i]->SetWindowPos(nullptr, 0, iHeight, rtView.Width(), rtDlg.Height(), 0);
			m_arrComDlg[i]->ShowWindow(true);
			iHeight += rtDlg.Height() - 1;
		}
		else
		{
			m_arrComDlg[i]->ShowWindow(false);
		}
	}
	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->GetWindowRect(rtDlg);
	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->SetWindowPos(nullptr, 0, iHeight, rtView.Width(), rtDlg.Height(), 0);
	m_arrComDlg[(UINT)DLG_TYPE::SCRIPT]->ShowWindow(true);

	iHeight += rtDlg.Height() - 1;

	m_AddCom->GetWindowRect(rtDlg);
	m_AddCom->SetWindowPos(nullptr, 0, iHeight, rtView.Width(), rtDlg.Height(), 0);
	m_AddCom->ShowWindow(true);
}

#endif
#endif //_DEBUG


// CComponentView 메시지 처리기