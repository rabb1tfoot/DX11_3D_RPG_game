#include "stdafx.h"
#include "Font23.h"
#include "Transform.h"
#include "GameObject.h"
#include "FontHelperMgr.h"
#include "RenderMgr.h"


CFont23::CFont23()
	: CComponent(COMPONENT_TYPE::Font)
	, m_tFontBox{}
	, m_vOffset(Vec2(0.f,0.f))
{
	CFontHelperMgr::GetInst()->FindFont(L"Default", m_tFontBox);
	SetText(L"");
}


CFont23::~CFont23()
{
}


void CFont23::update()
{
}

void CFont23::finalupdate()
{
	CalTransformBox();
}

void CFont23::CalTransformBox()
{
	Vec3 vPos = Object()->Transform()->GetLocalPos();
	float fFontSize = m_tFontBox->pTextFormat->GetFontSize();
	m_vPosBox.x = vPos.x - m_fstrlan / 2.f * fFontSize + m_vOffset.x;
	m_vPosBox.y = vPos.y - fFontSize - m_vOffset.y;
	m_vPosBox.z = vPos.x + m_fstrlan / 2.f * fFontSize + m_vOffset.x;
	m_vPosBox.w = vPos.y + fFontSize - m_vOffset.y;
}

void CFont23::SetText(wstring _wStr)
{
	m_wStr = _wStr;
	m_fstrlan = (float)wcslen(_wStr.c_str());
}

void CFont23::SetFontStyle(wstring _FontName)
{
	CFontHelperMgr::GetInst()->FindFont(_FontName.c_str(), m_tFontBox);
}

void CFont23::SetFontSize(int _size)
{
	//폰트 자체를 다시만들어야한다.(사이즈만 못바꿈) 일단 보류
	CFontHelperMgr::GetInst()->CreateFontBox(
		m_tFontBox->wFontName,
		m_tFontBox->pTextFormat->GetFontFamilyName((WCHAR*)m_tFontBox->wFontFamilyName.c_str(), m_tFontBox->iFamilyNameLen),
		_size,
		m_tFontBox->pBrush->GetColor()
	);
}

void CFont23::CollectRender()
{
	CRenderMgr::GetInst()->AddFont(this);
}


void CFont23::SaveToScene(FILE * _pFile)
{ 
	SaveWString(m_tFontBox->wFontName.c_str(), _pFile);
	SaveWString(m_wStr.c_str(), _pFile);
	fwrite(&m_vOffset, sizeof(Vec2), 1, _pFile);

}

void CFont23::LoadFromScene(FILE * _pFile)
{
	wstring wFontName  = LoadWString(_pFile);
	wstring wStr = LoadWString(_pFile);
	CFontHelperMgr::GetInst()->FindFont(wFontName.c_str(), m_tFontBox);
	SetText(wStr);
	fread(&m_vOffset, sizeof(Vec2), 1, _pFile);
}
