#include "stdafx.h"
#include "FontHelperMgr.h"
#include "RenderMgr.h"


CFontHelperMgr::CFontHelperMgr()
{
}


CFontHelperMgr::~CFontHelperMgr()
{
	for (size_t i = 0; i < m_vecFontBoxs.size(); ++i)
	{
		SAFE_RELEASE(m_vecFontBoxs[i]->pTextFormat);
		SAFE_RELEASE(m_vecFontBoxs[i]->pBrush);

		SAFE_DELETE(m_vecFontBoxs[i]);
	}
}

void CFontHelperMgr::init()
{
	CreateFontBox(L"Default", L"±Ã¼­", 15, BrushColor::BLACK);
	CreateFontBox(L"Default2", L"¹ÙÅÁ", 20, BrushColor::BLACK);
	CreateFontBox(L"Default3", L"±Ã¼­", 10, BrushColor::BLACK);
	CreateFontBox(L"Dialogue", L"¹ÙÅÁ", 20, BrushColor::WHITE);
}

void CFontHelperMgr::CreateFontBox(wstring _wFontName, wstring _wFontType, int _iFontSize, BrushColor _Color)
{
	IDWriteTextFormat*			pTextFormat = nullptr;
	ID2D1SolidColorBrush*		pBrush = nullptr;
	tFontBox* FontBox = new tFontBox();
	CreateTextFormat(_wFontType.c_str(), _iFontSize, pTextFormat);
	CreateBrush(_Color, pBrush);

	FontBox->pTextFormat = pTextFormat;
	FontBox->pBrush = pBrush;
	FontBox->wFontName = _wFontName.c_str();
	FontBox->wFontFamilyName = _wFontType.c_str();
	FontBox->iFamilyNameLen = _iFontSize;
	m_vecFontBoxs.push_back(FontBox);
}

void CFontHelperMgr::CreateFontBox(wstring _wFontName, HRESULT _fontStyle, int _iFontSize, D2D1_COLOR_F _Color)
{
	tFontBox* FontBoxInst = new tFontBox();
	int iIdx = FindFont(_wFontName, FontBoxInst);
	DelFont(iIdx);
	SAFE_DELETE(FontBoxInst);

	IDWriteTextFormat*			pTextFormat = nullptr;
	ID2D1SolidColorBrush*		pBrush = nullptr;
	tFontBox* FontBox = new tFontBox();
	CreateTextFormat((const wchar_t*)_fontStyle, _iFontSize, pTextFormat);
	CreateBrush(_Color, pBrush);

	FontBox->pTextFormat = pTextFormat;
	FontBox->pBrush = pBrush;
	FontBox->wFontName = _wFontName;
	m_vecFontBoxs.push_back(FontBox);
}

void CFontHelperMgr::CreateTextFormat(const wchar_t* _wFontType, int _iFontSize, IDWriteTextFormat* &_inout)
{
	CRenderMgr::GetInst()->GetWriteFactory()->CreateTextFormat(
		_wFontType,
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		(float)_iFontSize,
		L"ko",
		&_inout
	);

	return;
}

void CFontHelperMgr::CreateBrush(BrushColor _Color, ID2D1SolidColorBrush* &_inout)
{
	D2D1_COLOR_F Color;
	switch (_Color)
	{
	case BrushColor::WHITE:
		Color = D2D1::ColorF(D2D1::ColorF::White, 1.0f);
		break;
	case BrushColor::BLACK:
		Color = D2D1::ColorF(D2D1::ColorF::Black, 1.0f);
		break;
	}

	CRenderMgr::GetInst()->Get2DBackBuffer()->CreateSolidColorBrush(Color, &_inout);

	return;
}

void CFontHelperMgr::CreateBrush(D2D1_COLOR_F _Color, ID2D1SolidColorBrush * &_inout)
{
	CRenderMgr::GetInst()->Get2DBackBuffer()->CreateSolidColorBrush(_Color, &_inout);
	return;
}

int CFontHelperMgr::FindFont(wstring _wName, tFontBox* & _inout)
{
	for (size_t i = 0; i < m_vecFontBoxs.size(); ++i)
	{
		if (!wcscmp(m_vecFontBoxs[i]->wFontName.c_str(), _wName.c_str()))
		{
			_inout = m_vecFontBoxs[i];
			return i;
		}
	}
}


void CFontHelperMgr::DelFont(int _iIdx)
{
	vector<tFontBox*>::iterator iter;
	iter += _iIdx;
	m_vecFontBoxs.erase(iter);
}