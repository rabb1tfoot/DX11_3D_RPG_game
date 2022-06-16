// TerrainDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TerrainDlg.h"
#include "afxdialogex.h"

#include <GameObject.h>
#include <Terrain.h>
#include <PathMgr.h>


// CTerrainDlg 대화 상자

IMPLEMENT_DYNAMIC(CTerrainDlg, CDialogEx)

CTerrainDlg::CTerrainDlg(CWnd* pParent /*=nullptr*/)
	: CComponentDlg(IDD_CTerrainDlg, pParent)
	, m_bActive(FALSE)
{

}

CTerrainDlg::~CTerrainDlg()
{
}

void CTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editFaceX);
	DDX_Control(pDX, IDC_EDIT2, m_editFaceZ);
	DDX_Control(pDX, IDC_EDIT3, m_editBrushIdx);
	DDX_Control(pDX, IDC_EDIT4, m_editTileIdx);
	DDX_Control(pDX, IDC_EDIT5, m_editMode);
}

BEGIN_MESSAGE_MAP(CTerrainDlg, CDialogEx)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT4, &CComponentDlg::OnEnSetfocusEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT4, &CComponentDlg::OnEnKillfocusEdit)

	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT2, &CTerrainDlg::OnEnChangeXZ)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CTerrainDlg::init(CGameObject * _pTarget)
{
	Loadthumbnail();
}

void CTerrainDlg::update(CGameObject * _pTarget)
{
	int iMode = (int)_pTarget->Terrain()->GetTerrainMode();
	wstring strMode = {};
	if (iMode == 0)
	{
		strMode = L"높이변경";
	}
	else if (iMode == 1)
	{
		strMode = L"스플래팅";
	}
	else
	{
		strMode = L"대기";
	}

	int iBrushIdx = (int)_pTarget->Terrain()->GetBrushIdx();
	wchar_t strBrush[10];
	wsprintf(strBrush, L"%d", iBrushIdx);

	int iTileIdx = (int)_pTarget->Terrain()->GetTileIdx();
	wchar_t strTile[10];
	wsprintf(strTile, L"%d", iTileIdx);

	Vec2 Faces = _pTarget->Terrain()->GetFaceCountXZ();
	wchar_t strFaceX[10];
	wsprintf(strFaceX, L"%d", (int)Faces.x);
	wchar_t strfaceZ[10];
	wsprintf(strfaceZ, L"%d", (int)Faces.y);

	m_editFaceX.SetWindowTextW(strFaceX);
	m_editFaceZ.SetWindowTextW(strfaceZ);
	m_editMode.SetWindowTextW(strMode.c_str());
	m_editBrushIdx.SetWindowTextW(strBrush);
	m_editTileIdx.SetWindowTextW(strTile);
}

void CTerrainDlg::Loadthumbnail()
{
	CString resPath = CPathMgr::GetResPath();

	//브러쉬 목록 로딩하기
	resPath += "Texture\\Brush\\";
	wstring strFolderPath = resPath + L"*.*";

	WIN32_FIND_DATA tData = {};
	HANDLE h = (HANDLE)FindFirstFile(strFolderPath.c_str(), &tData);
	UINT err = GetLastError();

	int iIdx = 0;
	while (true)
	{
		if (FILE_ATTRIBUTE_DIRECTORY & tData.dwFileAttributes)
		{
			if (wcscmp(tData.cFileName, L".") && wcscmp(tData.cFileName, L".."))
			{
				continue;
			}
		}
		else
		{
			const wchar_t* pExt = CPathMgr::GetExt(tData.cFileName);
			wstring strPath = resPath + tData.cFileName;

			if (!wcscmp(pExt, L".png")
				|| !wcscmp(pExt, L".PNG")
				|| !wcscmp(pExt, L".bmp")
				|| !wcscmp(pExt, L".jpg")
				|| !wcscmp(pExt, L".jpeg")
				|| !wcscmp(pExt, L".tga"))
			{
				m_vImage[iIdx].Load(strPath.c_str());
				++iIdx;
			}
		}
		//로딩 실패
		if (false == FindNextFile(h, &tData))
		{
			err = GetLastError();
			break;
		}
	}

	//타일 목록 로딩하기
	resPath = CPathMgr::GetResPath();
	resPath += "Texture\\Tile\\";
	strFolderPath = resPath + L"*.*";

	tData = {};
	h = (HANDLE)FindFirstFile(strFolderPath.c_str(), &tData);
	err = GetLastError();
	while (true)
	{
		if (FILE_ATTRIBUTE_DIRECTORY & tData.dwFileAttributes)
		{
			if (wcscmp(tData.cFileName, L".") && wcscmp(tData.cFileName, L"..")
				&& wcscmp(tData.cFileName, L"Normal") && wcscmp(tData.cFileName, L"tga"))
			{
				continue;
			}
		}
		else
		{
			const wchar_t* pExt = CPathMgr::GetExt(tData.cFileName);
			wstring strPath = resPath + tData.cFileName;

			if (!wcscmp(pExt, L".png")
				|| !wcscmp(pExt, L".PNG")
				|| !wcscmp(pExt, L".bmp")
				|| !wcscmp(pExt, L".jpg")
				|| !wcscmp(pExt, L".jpeg")
				|| !wcscmp(pExt, L".tga"))
			{
				m_vImage[iIdx].Load(strPath.c_str());
				++iIdx;
			}
		}
		//로딩 실패
		if (false == FindNextFile(h, &tData))
		{
			err = GetLastError();
			break;
		}
	}
}

void CTerrainDlg::OnEnChangeXZ(UINT _id)
{
	//if (!IsEditActive())
	//	return;
	//
	//CString str;
	//
	//if (_id == IDC_EDIT1)
	//{
	//	m_editFaceX.GetWindowTextW(str);
	//	float fData = (float)_wtof(str);
	//	GetTarget()->Terrain()->SetFaceCountX(fData);
	//}
	//else
	//{
	//	m_editFaceZ.SetWindowTextW(str);
	//	float fData = (float)_wtof(str);
	//	GetTarget()->Terrain()->SetFaceCountZ(fData);
	//}
}


void CTerrainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CComponentDlg::OnPaint()을(를) 호출하지 마십시오.

	//브러쉬 타일 그리기
	SetStretchBltMode(dc.m_hDC, HALFTONE);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CRect rect(
				0 + j * 50
				, 200 + i * 50
				, 50 + j * 50
				, 250 + i * 50);
			if (m_vImage[i * 4 + j] == nullptr)
				continue;
			m_vImage[i * 4 + j].Draw(dc.m_hDC, rect);
		}
	}
}
