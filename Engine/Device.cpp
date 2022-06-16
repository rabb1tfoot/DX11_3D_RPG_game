#include "stdafx.h"
#include "Device.h"

#include "ConstBuffer.h"
#include "ResMgr.h"

CDevice::CDevice()
	: m_pDevice(nullptr)
	, m_pContext(nullptr)
	, m_iQuality(0)
{
}


CDevice::~CDevice()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

	Safe_Delete_Map(m_mapConstBuffer);
}


int CDevice::init()
{
	int iFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	iFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL m_featureLevel;

	D3D11CreateDevice(
		nullptr
		, D3D_DRIVER_TYPE_HARDWARE
		, nullptr
		, iFlag
		, featureLevels
		, ARRAYSIZE(featureLevels)
		, D3D11_SDK_VERSION
		, &m_pDevice
		, &m_featureLevel
		, &m_pContext);

	// Multi Sample Level Check
	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_iQuality);

	return S_OK;
}

void CDevice::CreateConstBuffer(const wstring & _strKey, UINT _iSize, UINT _iRegister)
{
	map<wstring, CConstBuffer*>::iterator iter = m_mapConstBuffer.find(_strKey);
	assert(iter == m_mapConstBuffer.end());

	CConstBuffer* pConstBuffer = new CConstBuffer;

	pConstBuffer->Create(_iSize, _iRegister);
	pConstBuffer->SetName(_strKey);

	m_mapConstBuffer.insert(make_pair(_strKey, pConstBuffer));
}

CConstBuffer * CDevice::FindConstBuffer(const wstring & _strKey)
{
	return m_mapConstBuffer.find(_strKey)->second;
}
