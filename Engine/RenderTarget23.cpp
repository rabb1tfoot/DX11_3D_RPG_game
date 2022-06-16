#include "stdafx.h"
#include "RenderTarget23.h"

#include "Device.h"

CRenderTarget23::CRenderTarget23()
	: m_pRTTex(nullptr)
{
}

CRenderTarget23::~CRenderTarget23()
{
}

void CRenderTarget23::Create(const wstring & _strName, CResPtr<CTexture> _pRTTex, const Vec4& _vClearColor)
{
	SetName(_strName);
	m_pRTTex = _pRTTex;
	m_vClearColor = _vClearColor;
}

void CRenderTarget23::clear()
{
	FLOAT arr[4]{ m_vClearColor.x, m_vClearColor.y, m_vClearColor.z, m_vClearColor.w };
	CONTEXT->ClearRenderTargetView(m_pRTTex->GetRTV(), arr);
}