#include "stdafx.h"
#include "DepthStencilState.h"

#include "Device.h"

CDepthStencilState::CDepthStencilState()
	:m_pState(nullptr)
	, m_tDesc{}
	, m_iStencilRef(0)
{
}

CDepthStencilState::~CDepthStencilState()
{
	SAFE_RELEASE(m_pState);
}


void CDepthStencilState::UpdateData()
{
	CONTEXT->OMSetDepthStencilState(m_pState, m_iStencilRef);
}

void CDepthStencilState::UpdateDefaultData()
{
	ID3D11DepthStencilState* pState = nullptr;
	CONTEXT->OMSetDepthStencilState(pState, 0);
}

void CDepthStencilState::Create(D3D11_DEPTH_STENCIL_DESC & _tDesc)
{
	m_tDesc = _tDesc;
	DEVICE->CreateDepthStencilState(&m_tDesc, &m_pState);
}
