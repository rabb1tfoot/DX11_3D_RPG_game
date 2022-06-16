#include "stdafx.h"
#include "BlendState.h"

#include "Device.h"

CBlendState::CBlendState()
	: m_pState(nullptr)
	, m_tDesc{}
	, m_vFactor(Vec4(1.f, 1.f, 1.f, 1.f))
{
}

CBlendState::~CBlendState()
{
	SAFE_RELEASE(m_pState);
}

void CBlendState::UpdateData()
{
	CONTEXT->OMSetBlendState(m_pState, (float*)&m_vFactor, 0xffffffff);
}

void CBlendState::UpdateDefaultData()
{
	CONTEXT->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void CBlendState::Create(D3D11_BLEND_DESC * _pDesc)
{
	m_tDesc = *_pDesc;
	HRESULT hr = DEVICE->CreateBlendState(_pDesc, &m_pState);
}

