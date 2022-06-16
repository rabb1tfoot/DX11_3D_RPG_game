#include "stdafx.h"
#include "MRT.h"

#include "Device.h"
#include "RenderTarget23.h"
#include "Texture.h"

CMRT::CMRT()
	: m_arrRT{}
	, m_pDepthTex(nullptr)
	, m_iRTCount(0)
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(CRenderTarget23 *(&_arrRT)[8], CResPtr<CTexture> _pDepthTex)
{
	memcpy(m_arrRT, _arrRT, sizeof(void*) * 8);
	m_pDepthTex = _pDepthTex;

	for (m_iRTCount = 0; m_iRTCount < 8; ++m_iRTCount)
	{
		if (nullptr == _arrRT[m_iRTCount])
			break;
	}
	m_tViewPortDesc.Width = (float)_arrRT[0]->GetRTTex()->GetWidth();
	m_tViewPortDesc.Height = (float)_arrRT[0]->GetRTTex()->GetHeight();
	m_tViewPortDesc.MinDepth = 0.f;
	m_tViewPortDesc.MaxDepth = 1.f;
	m_tViewPortDesc.TopLeftX = 0;
	m_tViewPortDesc.TopLeftY = 0;
}

void CMRT::OMSet()
{
	ID3D11RenderTargetView* arrView[8] = {};

	for (int i = 0; i < m_iRTCount; ++i)
	{
		arrView[i] = m_arrRT[i]->GetRTTex()->GetRTV();
	}

	if (m_pDepthTex != nullptr)
	{
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, m_pDepthTex->GetDSV());
	}
	else
	{
		// MRT 에 깊이 텍스쳐가 없는 경우
		// 장치에 전달된 이전 상태를 이어 받는다.
		ID3D11DepthStencilView* pDepthView = nullptr;
		CONTEXT->OMGetRenderTargets(0, nullptr, &pDepthView);

		if (nullptr != pDepthView)
			SAFE_RELEASE(pDepthView);

		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, pDepthView);
	}

	CONTEXT->RSSetViewports(1, &m_tViewPortDesc);
}

void CMRT::clear()
{
	for (UINT i = 0; i < (UINT)m_iRTCount; ++i)
	{
		m_arrRT[i]->clear();
	}

	if (m_pDepthTex != nullptr)
		CONTEXT->ClearDepthStencilView(m_pDepthTex->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}