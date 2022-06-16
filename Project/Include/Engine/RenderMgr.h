#pragma once

class CSamplerState;
class CBlendState;
class CDepthStencilState;
class CLight3D;
class CRenderTarget23;
class CMRT;
class CCamera;
class CFont23;

#include "FontHelperMgr.h"
#include "ResPtr.h"
#include "Filter23.h"

class CRenderMgr
{
	SINGLE(CRenderMgr);
private:
	HWND					m_hWnd;		// 출력 윈도우 핸들
	tResolution				m_tRes;		// 윈도우 해상도
	IDXGISwapChain*			m_pSwapChain;			// SwapChain 기능 담당
	bool					m_bWindow;
	//Font용 인자들
	IDXGISurface*			m_pBackBuffer;
	ID2D1Factory*			m_pFactory2D;
	ID2D1RenderTarget*		m_pBackBufferRT;
	IDWriteFactory*			m_pWriteFactory;

	CSamplerState*			m_arrSampler[(UINT)SAMPLER_TYPE::END];
	CBlendState*			m_arrBlendState[(UINT)BLEND_TYPE::END];
	CDepthStencilState*		m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::END];
	ID3D11RasterizerState*  m_arrRSState[(UINT)RS_TYPE::END];
	CRenderTarget23*		m_arrRT[(UINT)RT_TYPE::END];
	CMRT*					m_arrMRT[(UINT)MRT_TYPE::END];

	tLight3DInfo			m_arrLight3DInfo[100];	// Scene 에 있는 광원
	int						m_iLight3DCount;
	vector<CLight3D*>		m_vecLight3D;
	vector<CCamera*>		m_vecCam;				// Scene 에 있는 카메라
	vector<CFont23*>		m_vecFontBox;
	CCamera*				m_pToolCam;

	CResPtr<CFilter23>      m_pMergeFilter;

public:
	void init(HWND _hWnd, tResolution _tres, bool _bWindow);
	void init2D();
	void render();
	void render_tool();
	void render_lights();
	void render_shadowmap();
	void render_posteffect();
	void render_Font();

	void CopySwapToPosteffect();

public:
	CBlendState* GetBlendState(BLEND_TYPE _eType) { return m_arrBlendState[(UINT)_eType]; }
	CDepthStencilState* GetDepthStencilState(DEPTH_STENCIL_TYPE _eType) { return m_arrDSState[(UINT)_eType]; }
	CMRT* GetMRT(MRT_TYPE _eType) { return m_arrMRT[(UINT)_eType]; }
	CMRT** GetMRTs() { return m_arrMRT; }
	IDWriteFactory* GetWriteFactory() { return m_pWriteFactory; }
	ID2D1RenderTarget* Get2DBackBuffer() { return m_pBackBufferRT; }
	void SetMergeFilter(CResPtr<CFilter23> _pFilter) { m_pMergeFilter = _pFilter; }

public:
	int RegisterLight3D(CLight3D* _pLight3D);
	void RegisterCamera(CCamera* _pCam) { m_vecCam.push_back(_pCam); }
	void AddFont(CFont23* _pFont) { m_vecFontBox.push_back(_pFont); }
	void ClearFont() { m_vecFontBox.clear(); }

	void ClearRegisterObj() { m_vecFontBox.clear(); m_vecCam.clear(); m_vecLight3D.clear(); m_iLight3DCount = 0; }
	void SetRSState(RS_TYPE _eType);
	void SetToolCam(CCamera* _pCam) { m_pToolCam = _pCam; }
	void Present() { m_pSwapChain->Present(0, 0); }
	void Clear();
	tResolution GetResolution() { return m_tRes; }
	CCamera* GetMainCam();


private:
	void CreateSamplerState();
	void CreateBlendState();
	void CreateDepthStencilState();
	void CreateRSState();
	void CreateRenderTarget();
	void UpdateLight3D();

};

