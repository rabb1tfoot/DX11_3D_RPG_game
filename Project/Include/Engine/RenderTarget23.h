#pragma once
#include "Entity.h"

#include "ResPtr.h"
#include "Texture.h"

class CRenderTarget23 :
	public CEntity
{
private:
	CResPtr<CTexture>	m_pRTTex;
	Vec4				m_vClearColor;

public:
	void Create(const wstring& _strName, CResPtr<CTexture> _pRTTex, const Vec4& _vClearColor);
	CResPtr<CTexture> GetRTTex() { return m_pRTTex; }
	void clear();

public:
	CLONE_DISABLE(CRenderTarget23);

public:
	CRenderTarget23();
	virtual ~CRenderTarget23();
};

