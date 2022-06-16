#pragma once
#include "Component.h"
#include "FontHelperMgr.h"
class CFont23 :
	public CComponent
{
public:
	CFont23();
	virtual ~CFont23();

private:
	tFontBox*	 m_tFontBox;
	wstring		 m_wStr;
	float		 m_fstrlan;
	Vec2		 m_vOffset;
	Vec4		 m_vPosBox;

public:
	void	SetText(wstring _wStr);
	void	SetFontStyle(wstring _FontName);
	void	SetOffSet(Vec2 _vec) { m_vOffset = _vec; };
	void	SetFontSize(int _size);
	tFontBox* GetBox() { return m_tFontBox; }
	wstring GetText() { return m_wStr; };
	int		GetLangth() { return (int)m_fstrlan; }
	Vec4	GetFinalPosBox() {return m_vPosBox;};
	Vec2    GetOffSet() { return m_vOffset; }
	void	CollectRender();

private:
	void CalTransformBox();

public:
	virtual void update();
	virtual void finalupdate();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLONE_DISABLE(CFont23);
};

