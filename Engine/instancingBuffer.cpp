#include "stdafx.h"
#include "instancingBuffer.h"

#include "Device.h"

CinstancingBuffer::CinstancingBuffer()
	: m_iMaxCount(10)
	, m_iAnimInstCount(0)
{
}


CinstancingBuffer::~CinstancingBuffer()
{
	SAFE_RELEASE(m_pInstancingBuffer);
}

void CinstancingBuffer::init()
{
	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer)))
		assert(nullptr);
}

void CinstancingBuffer::AddInstancingBoneMat(vector<Matrix>& _vecBoneMat)
{
	++m_iAnimInstCount;
	m_vecBoneMat.insert(m_vecBoneMat.end(), _vecBoneMat.begin(), _vecBoneMat.end());
}

void CinstancingBuffer::SetData(CResPtr<CTexture> _pBoneTex)
{
	if (m_vecData.size() > m_iMaxCount)
	{
		Resize(m_vecData.size());
	}

	D3D11_MAPPED_SUBRESOURCE tMap = { };

	CONTEXT->Map(m_pInstancingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);
	memcpy(tMap.pData, &m_vecData[0], sizeof(tInstancingData) * m_vecData.size());
	CONTEXT->Unmap(m_pInstancingBuffer, 0);

	if (0 == m_iAnimInstCount)
		return;

	if (m_vecBoneMat.size() * 4 > _pBoneTex->GetWidth() * _pBoneTex->GetHeight())
	{
		int iBoneCount = m_vecBoneMat.size() / m_iAnimInstCount;
		_pBoneTex->Resize(iBoneCount * 4, m_iAnimInstCount);
	}

	_pBoneTex->SetData(&m_vecBoneMat[0], sizeof(Matrix) * m_vecBoneMat.size());

}

void CinstancingBuffer::Resize(UINT _iCount)
{
	SAFE_RELEASE(m_pInstancingBuffer);

	m_iMaxCount = _iCount;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = sizeof(tInstancingData) * m_iMaxCount;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if(FAILED(DEVICE->CreateBuffer(&tDesc, NULL, &m_pInstancingBuffer)))
		assert(nullptr);
}
