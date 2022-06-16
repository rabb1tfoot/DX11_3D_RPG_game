// MeshRenderDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "MeshRenderDlg.h"
#include "afxdialogex.h"

#include "ListDlg.h"

#include <GameObject.h>
#include <ResMgr.h>
#include <MeshRender.h>


// CMeshRenderDlg 대화 상자

IMPLEMENT_DYNAMIC(CMeshRenderDlg, CDialogEx)

CMeshRenderDlg::CMeshRenderDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CMeshRenderDlg, pParent)
	, m_bShadow(true)
{

}

CMeshRenderDlg::~CMeshRenderDlg()
{
}

void CMeshRenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_BtnMesh);
	DDX_Control(pDX, IDC_BUTTON2, m_BtnMtrl);
	DDX_Control(pDX, IDC_EDIT1, m_editMesh);
	DDX_Control(pDX, IDC_EDIT2, m_editMtrl);
}

void CMeshRenderDlg::update(CGameObject * _pTarget)
{
	SetTarget(_pTarget);

	CString strName;

	CMeshRender* pMeshRender = GetTarget()->MeshRender();

	CResPtr<CMesh> pMesh = pMeshRender->GetMesh();
	CResPtr<CMaterial> pMtrl = pMeshRender->GetSharedMaterial();

	if (nullptr == pMesh)
		m_editMesh.SetWindowTextW(L"");
	else
	{
		strName = CPathMgr::GetFileName(pMesh->GetName().c_str());
		m_editMesh.SetWindowTextW(strName);
	}

	if (nullptr == pMtrl)
		m_editMtrl.SetWindowTextW(L"");
	else
	{
		strName = CPathMgr::GetFileName(pMtrl->GetName().c_str());
		m_editMtrl.SetWindowTextW(strName);
	}
}

BEGIN_MESSAGE_MAP(CMeshRenderDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CMeshRenderDlg::OnBnClickedMesh)
	ON_BN_CLICKED(IDC_BUTTON2, &CMeshRenderDlg::OnBnClickedMtrl)
	ON_BN_CLICKED(IDC_CHECK1, &CMeshRenderDlg::OnBnClickedShadow)
END_MESSAGE_MAP()


// CMeshRenderDlg 메시지 처리기


void CMeshRenderDlg::OnBnClickedMesh()
{
	const map<wstring, CResource*>& mapRes = CResMgr::GetInst()->GetResources(RES_TYPE::MESH);

	vector<CString> vecFileName;
	vector<CString> vecResName;

	for (const auto& pair : mapRes)
	{
		vecFileName.push_back(CPathMgr::GetFileName(pair.second->GetName().c_str()));
		vecResName.push_back(pair.second->GetName().c_str());
	}

	CListDlg dlg(L"Mesh", vecFileName, vecResName);
	int iRet = dlg.DoModal();

	if (IDOK == iRet)
	{
		CString strSelect = dlg.GetSelectSecondItem();

		CResPtr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(strSelect.GetBuffer());

		GetTarget()->MeshRender()->SetMesh(pMesh);
	}
}


void CMeshRenderDlg::OnBnClickedMtrl()
{
	const map<wstring, CResource*>& mapRes = CResMgr::GetInst()->GetResources(RES_TYPE::MATERIAL);

	vector<CString> vecFileName;
	vector<CString> vecResName;

	for (const auto& pair : mapRes)
	{
		vecFileName.push_back(CPathMgr::GetFileName(pair.second->GetName().c_str()));
		vecResName.push_back(pair.second->GetName().c_str());
	}

	CListDlg dlg(L"Material", vecFileName, vecResName);
	int iRet = dlg.DoModal();

	if (IDOK == iRet)
	{
		CString strSelect = dlg.GetSelectSecondItem();

		CResPtr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(strSelect.GetBuffer());

		GetTarget()->MeshRender()->SetMaterial(pMtrl);
	}
}


void CMeshRenderDlg::OnBnClickedShadow()
{
	m_bShadow = !m_bShadow;
	GetTarget()->MeshRender()->SetShadow(m_bShadow);
}
