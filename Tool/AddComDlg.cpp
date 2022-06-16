// AddComDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "AddComDlg.h"
#include "afxdialogex.h"

#include <GameObject.h>
#include <MeshRender.h>
#include <Camera.h>
#include <Collider3D.h>
#include <Material.h>
#include <Light3D.h>
#include <ResMgr.h>

// CAddComDlg 대화 상자

IMPLEMENT_DYNAMIC(CAddComDlg, CDialogEx)

CAddComDlg::CAddComDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CAddComDlg, pParent)
{

}

void CAddComDlg::init(CGameObject * _pTarget)
{
	wstring ComName = L"";
	for (UINT i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0:
			ComName = L"MESHRENDER";
			break;
		case 1:
			ComName = L"CAMERA";
			break;
		case 2:
			ComName = L"LIGHT3D";
			break;
		case 3:
			ComName = L"COLLIDER3D";
			break;
		}
		m_cbComponent.AddString(ComName.c_str());
	}
	m_cbComponent.SetItemData(0, (DWORD_PTR)m_Comtype0);
	m_cbComponent.SetItemData(1, (DWORD_PTR)m_Comtype1);
	m_cbComponent.SetItemData(2, (DWORD_PTR)m_Comtype2);
	m_cbComponent.SetItemData(3, (DWORD_PTR)m_Comtype3);
}

void CAddComDlg::update(CGameObject * _pTarget)
{
	SetTarget(_pTarget);
}

CAddComDlg::~CAddComDlg()
{
}

void CAddComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbComponent);

	m_Comtype0 = COMPONENT_TYPE::CAMERA;
	m_Comtype1 = COMPONENT_TYPE::COLLIDER3D;
	m_Comtype2 = COMPONENT_TYPE::LIGHT3D;
	m_Comtype3 = COMPONENT_TYPE::MESHRENDER;
}


BEGIN_MESSAGE_MAP(CAddComDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAddComDlg::OnBnClickedAddComponent)
END_MESSAGE_MAP()


// CAddComDlg 메시지 처리기


void CAddComDlg::OnBnClickedAddComponent()
{
	int iIdx = m_cbComponent.GetCurSel();
	CGameObject* pTarget = GetTarget();
	COMPONENT_TYPE eType = (COMPONENT_TYPE)m_cbComponent.GetItemData(iIdx);

	switch (eType)
	{
		if (nullptr == pTarget->GetComponent(eType))
			return;

		case COMPONENT_TYPE::CAMERA:
		{
			CCamera* pCam = new CCamera;
			pCam->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
			pCam->SetFOV(XM_PI / 4.f);
			pCam->SetScale(1.f);

			pTarget->AddComponent(pCam);
		}
		break;

		case COMPONENT_TYPE::COLLIDER3D:
		{
			CCollider3D* pCol = new CCollider3D;
			pCol->SetColliderType(COLLIDER3D_TYPE::CUBE);
			pTarget->AddComponent(pCol);
		}
		break;

		case COMPONENT_TYPE::LIGHT3D:
		{
			CLight3D* pLight = new CLight3D;
			pLight->SetLightType(LIGHT_TYPE::DIRECTIONAL);
			pLight->SetLightDiffuse(Vec3(1.f, 1.f, 1.f));
			pLight->SetLightDir(Vec3(1.f, -1.f, 1.f));
			pLight->SetLightSpecular(Vec3(0.1f, 0.1f, 0.1f));
			pLight->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
			pTarget->AddComponent(pLight);
		}
		break;
		case COMPONENT_TYPE::MESHRENDER:
		{
			CMeshRender* pRender = new CMeshRender;
			pRender->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"));
			pRender->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
			pTarget->AddComponent(pRender);
		}

		break;
	}
}
