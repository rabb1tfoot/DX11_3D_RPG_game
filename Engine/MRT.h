#pragma once
#include "Entity.h"

#include "ResPtr.h"
#include "Texture.h"

class CRenderTarget23;

class CMRT :
	public CEntity
{
private:
	CRenderTarget23*	m_arrRT[8];
	CResPtr<CTexture>	m_pDepthTex;
	int					m_iRTCount;
	D3D11_VIEWPORT		m_tViewPortDesc; //����Ÿ�� ������� ����Ʈ ����� �����.

public:
	void Create(CRenderTarget23* (&_arrRT)[8], CResPtr<CTexture> _pDepthTex);
	void OMSet();
	void clear();

public:
	CLONE_DISABLE(CMRT);

public:
	CMRT();
	virtual ~CMRT();
};

