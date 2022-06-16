#pragma once

struct tFontBox
{
	wstring						wFontName;
	IDWriteTextFormat*			pTextFormat;
	ID2D1SolidColorBrush*		pBrush;

	//폰트 재설정할때 필요한 값들 저장
	wstring						wFontFamilyName;
	UINT						iFamilyNameLen;

	tFontBox()
	:  pTextFormat(nullptr)
	,  pBrush(nullptr)
	,  wFontName{}
	{};
};

class CFontHelperMgr
{
public:
	SINGLE(CFontHelperMgr);

private:
	vector<tFontBox*> m_vecFontBoxs;

public:
	void init();
	void CreateFontBox(wstring _wFontName, wstring _wFontType, int _iFontSize, BrushColor _Color);
	void CreateFontBox(wstring _wFontName, HRESULT _fontStyle, int _iFontSize, D2D1_COLOR_F _Color);
	int FindFont(wstring _wName, tFontBox* & _inout);

private:
	void DelFont(int _iIdx);
	void CreateTextFormat(const wchar_t* _wFontType, int _iFontSize, IDWriteTextFormat* &_inout);
	void CreateBrush(BrushColor _Color, ID2D1SolidColorBrush* &_inout);
	void CreateBrush(D2D1_COLOR_F _Color, ID2D1SolidColorBrush* &_inout);
};

