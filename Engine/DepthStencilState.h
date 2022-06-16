#pragma once
#include "State.h"

class CDepthStencilState :
	public CState
{
private:
	ID3D11DepthStencilState* m_pState;
	D3D11_DEPTH_STENCIL_DESC m_tDesc;
	UINT					 m_iStencilRef;

public:
	void SetStencilRef(UINT _iRef) { m_iStencilRef = _iRef; }

	virtual void UpdateData();
	static void UpdateDefaultData();

public:
	void Create(D3D11_DEPTH_STENCIL_DESC& _tDesc);
	CLONE_DISABLE(CDepthStencilState);

public:
	CDepthStencilState();
	virtual ~CDepthStencilState();
};

