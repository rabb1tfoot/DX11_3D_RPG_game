#include "stdafx.h"
#include "RenderMgr.h"

#include "SamplerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"

#include "ConstBuffer.h"
#include "Device.h"
#include "Core.h"
#include "SceneMgr.h"
#include "Scene.h"

#include "Light3D.h"
#include "Font23.h"
#include "ResMgr.h"
#include "Texture.h"

#include "RenderTarget23.h"
#include "MRT.h"
#include "Texture.h"

#include "Camera.h"


tGlobalValue g_global = {};

CRenderMgr::CRenderMgr()
	: m_arrSampler{}
	, m_arrBlendState{}
	, m_arrRSState{}
	, m_arrRT{}
	, m_arrLight3DInfo{}
	, m_pSwapChain(nullptr)
	, m_iLight3DCount(0)
	, m_bWindow(false)
{
}

CRenderMgr::~CRenderMgr()
{
	Safe_Delete_Array(m_arrSampler);
	Safe_Delete_Array(m_arrBlendState);
	Safe_Delete_Array(m_arrDSState);
	Safe_Release_Array(m_arrRSState);

	Safe_Delete_Array(m_arrMRT);
	Safe_Delete_Array(m_arrRT);

	SAFE_RELEASE(m_pSwapChain)

	SAFE_RELEASE(m_pBackBufferRT);
	SAFE_RELEASE(m_pWriteFactory);
}

void CRenderMgr::init(HWND _hWnd, tResolution _tres, bool _bWindow)
{
	m_hWnd = _hWnd;
	m_tRes = _tres;
	m_bWindow = _bWindow;

	CreateSamplerState();
	CreateBlendState();
	CreateDepthStencilState();
	CreateRSState();
	CreateRenderTarget();

	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	// ViewPort
	D3D11_VIEWPORT tVP = {};

	tVP.Width = m_tRes.fWidth;
	tVP.Height = m_tRes.fHeight;

	tVP.TopLeftX = 0;
	tVP.TopLeftY = 0;

	tVP.MinDepth = 0.f;
	tVP.MaxDepth = 1.f;

	CONTEXT->RSSetViewports(1, &tVP);
}

void CRenderMgr::init2D()
{
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pBackBuffer));

	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory2D);

	FLOAT dpiX;
	FLOAT dpiY;
	m_pFactory2D->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
		);


	hr = m_pFactory2D->CreateDxgiSurfaceRenderTarget(
		m_pBackBuffer,
		&props,
		&m_pBackBufferRT
	);

	SAFE_RELEASE(m_pFactory2D);
	SAFE_RELEASE(m_pBackBuffer);

	//write출력용 팩토리 만들기
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pWriteFactory)
	);
	

	return;
}

void CRenderMgr::render()
{
	// 장치 색상 초기화
	Clear();

	{
		static CConstBuffer* pGlobal = CDevice::GetInst()->FindConstBuffer(L"Global");
		g_global.tResolution = m_tRes;

		pGlobal->AddData(&g_global, sizeof(tGlobalValue));
		pGlobal->UpdateData();
		pGlobal->SetRegisterAll();

		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

		// 광원 정보 상수버퍼에 업데이트
		UpdateLight3D();

		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			m_vecCam[i]->SortGameObject();
			m_vecCam[i]->render_deferred();
		}

		render_shadowmap();

		render_lights();

		GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

		for (size_t i = 0; i < m_vecCam.size(); ++i)
		{
			m_vecCam[i]->render_forward();
		}
		render_posteffect();
		render_Font();
	}

	// 윈도우에 출력
	Present();
}

void CRenderMgr::render_tool()
{
	// 장치 색상 초기화
	Clear();

	static CConstBuffer* pGlobal = CDevice::GetInst()->FindConstBuffer(L"Global");
	g_global.tResolution = m_tRes;

	pGlobal->AddData(&g_global, sizeof(tGlobalValue));
	pGlobal->UpdateData();
	pGlobal->SetRegisterAll();

	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	// 상수버퍼에 광원 정보 업데이트
	UpdateLight3D();
}

void CRenderMgr::render_lights()
{
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->render();
	}

	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet();

	// filter	
	m_pMergeFilter->render();
}

void CRenderMgr::render_shadowmap()
{
	// 광원 시점으로 깊이를 그림
	for (UINT i = 0; i < m_vecLight3D.size(); ++i)
	{
		m_vecLight3D[i]->render_shadowmap();
	}

	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->OMSet(); // 메인카메라 깊이로 되돌리기 위해서
}

void CRenderMgr::render_posteffect()
{
	// 후처리 물체 렌더링		
	if (CCore::GetInst()->GetState() == SCENE_STATE::STOP)
	{
		m_pToolCam->render_posteffect();
	}
	else
	{
		for (UINT i = 0; i < m_vecCam.size(); ++i)
		{
			m_vecCam[i]->render_posteffect();
		}
	}
}

void CRenderMgr::render_Font()
{
	m_pBackBufferRT->BeginDraw();

	for (size_t i = 0; i < m_vecFontBox.size(); ++i)
	{
		m_pBackBufferRT->DrawTextW(

			m_vecFontBox[i]->GetText().c_str(),

			m_vecFontBox[i]->GetLangth(),

			m_vecFontBox[i]->GetBox()->pTextFormat,

			D2D1::RectF(m_vecFontBox[i]->GetFinalPosBox().x
				, m_vecFontBox[i]->GetFinalPosBox().y
				, m_vecFontBox[i]->GetFinalPosBox().z
				, m_vecFontBox[i]->GetFinalPosBox().w),//m_vecFontBox[i]->GetFinalPosBox().w),

			m_vecFontBox[i]->GetBox()->pBrush

		);
	}

	m_pBackBufferRT->EndDraw();
}

void CRenderMgr::CopySwapToPosteffect()
{
	CONTEXT->CopyResource(m_arrRT[(UINT)RT_TYPE::POSTEFFECT]->GetRTTex()->GetTex2D()
		, m_arrRT[(UINT)RT_TYPE::SWAPCHAIN]->GetRTTex()->GetTex2D());
}

int CRenderMgr::RegisterLight3D(CLight3D * _pLight3D)
{
	int iIdx = m_iLight3DCount;
	m_arrLight3DInfo[m_iLight3DCount++] = _pLight3D->GetLight3DInfo();
	m_vecLight3D.push_back(_pLight3D);
	return iIdx;
}

void CRenderMgr::SetRSState(RS_TYPE _eType)
{
	CONTEXT->RSSetState(m_arrRSState[(UINT)_eType]);
}

void CRenderMgr::Clear()
{
	for (UINT i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr != m_arrMRT[i])
			m_arrMRT[i]->clear();
	}

	// 리소스 클리어
	CTexture::ClearAllRegister();
}

#include "Core.h"
CCamera * CRenderMgr::GetMainCam()
{
	if (CCore::GetInst()->GetState() != SCENE_STATE::PLAY)
		return m_pToolCam;

	if (m_vecCam.empty())
		return nullptr;

	return m_vecCam[0];
}

void CRenderMgr::CreateSamplerState()
{
	D3D11_SAMPLER_DESC tDesc = {};

	m_arrSampler[(UINT)SAMPLER_TYPE::DEFAULT] = new CSamplerState;

	tDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

	m_arrSampler[(UINT)SAMPLER_TYPE::DEFAULT]->Create(&tDesc, 0);
	m_arrSampler[(UINT)SAMPLER_TYPE::DEFAULT]->UpdateData();

	m_arrSampler[(UINT)SAMPLER_TYPE::LINEAR] = new CSamplerState;

	tDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;

	m_arrSampler[(UINT)SAMPLER_TYPE::LINEAR]->Create(&tDesc, 1);
	m_arrSampler[(UINT)SAMPLER_TYPE::LINEAR]->UpdateData();
}

void CRenderMgr::CreateBlendState()
{
	D3D11_BLEND_DESC tDesc = {};

	// ====================
	// Default Blend State
	// ====================
	m_arrBlendState[(UINT)BLEND_TYPE::DEFAULT] = new CBlendState;

	// =================
	// Alpha Blend State
	// =================
	m_arrBlendState[(UINT)BLEND_TYPE::ALPHABLEND] = new CBlendState;

	tDesc.AlphaToCoverageEnable = true;   // 투명물체 깊의에 의해서 가려지는 현상 제거(x4 멀티샘플이 지원될 경우)
	tDesc.IndependentBlendEnable = false; // 렌더타켓 별로 독립적인 블렌드 공식 사용

	tDesc.RenderTarget[0].BlendEnable = true;

	// 색상 혼합 공식
	tDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	tDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;

	// Alpha 값 끼리 혼합공식
	tDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	tDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_arrBlendState[(UINT)BLEND_TYPE::ALPHABLEND]->Create(&tDesc);

	// =================
	// One - One Blend
	// =================
	m_arrBlendState[(UINT)BLEND_TYPE::ONE_ONE] = new CBlendState;

	tDesc.AlphaToCoverageEnable = false;   // 투명물체 깊의에 의해서 가려지는 현상 제거(x4 멀티샘플이 지원될 경우)
	tDesc.IndependentBlendEnable = false; // 렌더타켓 별로 독립적인 블렌드 공식 사용

	tDesc.RenderTarget[0].BlendEnable = true;

	// 색상 혼합 공식
	tDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;

	// Alpha 값 끼리 혼합공식
	tDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	tDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	tDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;

	tDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_arrBlendState[(UINT)BLEND_TYPE::ONE_ONE]->Create(&tDesc);
}

void CRenderMgr::CreateDepthStencilState()
{
	// Default State
	m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::DEFAULT] = nullptr;

	CDepthStencilState* pDSState = nullptr;
	D3D11_DEPTH_STENCIL_DESC tDesc = {};

	// Less Equal
	pDSState = new CDepthStencilState;
	tDesc.DepthEnable = true;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // D3D11_DEPTH_WRITE_MASK_ZERO;
	tDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	pDSState->Create(tDesc);
	m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::LESS_EQUAL] = pDSState;

	// No Depth Test
	pDSState = new CDepthStencilState;
	tDesc.DepthEnable = false;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // D3D11_DEPTH_WRITE_MASK_ZERO;
	tDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	pDSState->Create(tDesc);
	m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::NO_DEPTH_TEST] = pDSState;

	// No Depth Write
	pDSState = new CDepthStencilState;
	tDesc.DepthEnable = true;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	tDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	pDSState->Create(tDesc);
	m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::NO_DEPTH_WRITE] = pDSState;

	// No Depth Test, Write
	pDSState = new CDepthStencilState;
	tDesc.DepthEnable = false;
	tDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	tDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	pDSState->Create(tDesc);
	m_arrDSState[(UINT)DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE] = pDSState;
}

void CRenderMgr::CreateRSState()
{
	// Rasterizer State
	m_arrRSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	ID3D11RasterizerState* pRSState = nullptr;
	D3D11_RASTERIZER_DESC tRSDesc = {};
	tRSDesc.CullMode = D3D11_CULL_FRONT;
	tRSDesc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&tRSDesc, &pRSState);
	m_arrRSState[(UINT)RS_TYPE::CULL_FRONT] = pRSState;

	tRSDesc.CullMode = D3D11_CULL_NONE;
	tRSDesc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&tRSDesc, &pRSState);
	m_arrRSState[(UINT)RS_TYPE::CULL_NONE] = pRSState;

	tRSDesc.CullMode = D3D11_CULL_NONE;
	tRSDesc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&tRSDesc, &pRSState);
	m_arrRSState[(UINT)RS_TYPE::WIREFRAME] = pRSState;
}

void CRenderMgr::CreateRenderTarget()
{
	// Create SwapChain
	DXGI_SWAP_CHAIN_DESC tSwapDesc = {};

	tSwapDesc.BufferCount = 1;
	tSwapDesc.BufferDesc.Width = (UINT)m_tRes.fWidth;
	tSwapDesc.BufferDesc.Height = (UINT)m_tRes.fHeight;
	tSwapDesc.BufferDesc.RefreshRate.Numerator = 60;
	tSwapDesc.BufferDesc.RefreshRate.Denominator = 1;
	tSwapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	tSwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	tSwapDesc.SampleDesc.Count = 1;
	tSwapDesc.SampleDesc.Quality = 0;

	tSwapDesc.OutputWindow = m_hWnd;	// 출력 윈도우
	tSwapDesc.Windowed = m_bWindow;

	tSwapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	tSwapDesc.Flags = 0;

	IDXGIDevice* pIDXDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	DEVICE->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIDXDevice);
	pIDXDevice->GetAdapter(&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	HRESULT hr = pFactory->CreateSwapChain(DEVICE, &tSwapDesc, &m_pSwapChain);

	// 참조 Count 줄여주기
	SAFE_RELEASE(pIDXDevice);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	// RenderTarget View
	// SwapChain 이 가지고 있는 RenderTarget 용 Texture 를 얻어온다.
	ID3D11Texture2D* pRenderTargetTex = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pRenderTargetTex);
	CResPtr<CTexture> pSwapChainTex = CResMgr::GetInst()->CreateTexture(L"SwapChainTex", pRenderTargetTex);

	// DepthStencil Texture
	CResPtr<CTexture> pDepthTex = CResMgr::GetInst()->CreateTexture(L"DepthStencilTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, DXGI_FORMAT_D24_UNORM_S8_UINT);

	// ============
	// RenderTarget
	// ============
	// SwapChain RenderTarget
	m_arrRT[(UINT)RT_TYPE::SWAPCHAIN] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::SWAPCHAIN]->Create(L"SwapChainTarget", pSwapChainTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// Diffuse RenderTarget	
	CResPtr<CTexture> pTargetTex = CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_arrRT[(UINT)RT_TYPE::DIFFUSE] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::DIFFUSE]->Create(L"DiffuseTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 1.f));

	// Normal RenderTarget
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"NormalTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_arrRT[(UINT)RT_TYPE::NORMAL] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::NORMAL]->Create(L"NormalTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// Position RenderTarget
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"PositionTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	m_arrRT[(UINT)RT_TYPE::POSITION] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::POSITION]->Create(L"PositionTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// Light RenderTarget
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"LightTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_arrRT[(UINT)RT_TYPE::LIGHT] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::LIGHT]->Create(L"LightTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// Specular RenderTarget
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_arrRT[(UINT)RT_TYPE::SPECULAR] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::SPECULAR]->Create(L"SpecularTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// Shadow RenderTarget	
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"ShadowmapTargetTex"
		, 4096, 4096
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		, D3D11_USAGE_DEFAULT
		, DXGI_FORMAT_R32_FLOAT);

	m_arrRT[(UINT)RT_TYPE::SHADOWMAP] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::SHADOWMAP]->Create(L"ShadowmapTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));

	// PostEffect RenderTarget
	pTargetTex = CResMgr::GetInst()->CreateTexture(L"PosteffectTargetTex", (UINT)m_tRes.fWidth, (UINT)m_tRes.fHeight
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_arrRT[(UINT)RT_TYPE::POSTEFFECT] = new CRenderTarget23;
	m_arrRT[(UINT)RT_TYPE::POSTEFFECT]->Create(L"PosteffectTarget", pTargetTex, Vec4(0.f, 0.f, 0.f, 0.f));


	// =============
	// Swapchain MRT
	// =============
	CRenderTarget23* rt[8] = { m_arrRT[(UINT)RT_TYPE::SWAPCHAIN] , };
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(rt, pDepthTex);

	// ============
	// Deferred MRT
	// ============
	memset(rt, 0, sizeof(void*) * 8);
	rt[0] = m_arrRT[(UINT)RT_TYPE::DIFFUSE];
	rt[1] = m_arrRT[(UINT)RT_TYPE::NORMAL];
	rt[2] = m_arrRT[(UINT)RT_TYPE::POSITION];

	m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(rt, nullptr);

	// ============
	// Light MRT
	// ============
	memset(rt, 0, sizeof(void*) * 8);
	rt[0] = m_arrRT[(UINT)RT_TYPE::LIGHT];
	rt[1] = m_arrRT[(UINT)RT_TYPE::SPECULAR];

	m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(rt, nullptr);

	// ============
	// Shadowmap MRT
	// ============
	memset(rt, 0, sizeof(void*) * 8);
	rt[0] = m_arrRT[(UINT)RT_TYPE::SHADOWMAP];

	// Shadowmap Depth Texture
	CResPtr<CTexture> pShadowmapDepthTex = CResMgr::GetInst()->CreateTexture(L"ShadowmapDepthTex", 4096, 4096
		, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, DXGI_FORMAT_D32_FLOAT);

	m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP] = new CMRT;
	m_arrMRT[(UINT)MRT_TYPE::SHADOWMAP]->Create(rt, pShadowmapDepthTex);
}

void CRenderMgr::UpdateLight3D()
{
	static CConstBuffer* pLightBuffer = CDevice::GetInst()->FindConstBuffer(L"Light3D");

	pLightBuffer->AddData(m_arrLight3DInfo, sizeof(tLight3DInfo) * 100);
	pLightBuffer->AddData(&m_iLight3DCount, sizeof(int));
	pLightBuffer->UpdateData();
	pLightBuffer->SetRegisterAll();

	m_iLight3DCount = 0;
}
