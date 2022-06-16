#pragma once
#include "Component.h"
#include "FontHelperMgr.h"
class CFont :
	public CComponent
{
public:
	CFont();
	virtual ~CFont();

private:
	tFontBox*	 m_tFontBox;
	wstring		 m_wStr;
	float		 m_fstrlan;
	float		 m_fLtopX;
	float		 m_fLtopY;
	float		 m_fRBottomX;
	float		 m_fRBottomY;

public:
	void CreateFontStyle();
	void SetFontSize(float _size);
	void FindFontStyle(const wstring _FontName);

private:
	void CalTransformBox();

public:
	virtual void update();
	virtual void finalupdate();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLONE_DISABLE(CFont);
};

