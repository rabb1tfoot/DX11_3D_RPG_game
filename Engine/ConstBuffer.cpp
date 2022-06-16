#include "stdafx.h"
#include "ConstBuffer.h"

#include "Device.h"

CConstBuffer::CConstBuffer()
	: m_pBuffer(nullptr)
	, m_iSize(0)
	, m_iRegister(0)
	, m_pSysMem(nullptr)
	, m_iCurSize(0)
{
}


CConstBuffer::~CConstBuffer()
{
	SAFE_RELEASE(m_pBuffer);
	SAFE_DELETE(m_pSysMem);
}


void CConstBuffer::Create(UINT _iSize, UINT _iRegisterNum)
{
	m_iSize = _iSize;
	m_iRegister = _iRegisterNum;

	//������۸����
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = _iSize;
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DEVICE->CreateBuffer(&tBufferDesc, nullptr, &m_pBuffer);

	assert(m_pBuffer);

	m_pSysMem = malloc(m_iSize);
}

void CConstBuffer::AddData(const void * _pSrc, UINT _iSize)
{
	assert(!(m_iSize < m_iCurSize + _iSize));

	memcpy((BYTE*)m_pSysMem + m_iCurSize, _pSrc, _iSize);
}

void CConstBuffer::UpdateData()
{
	m_iCurSize = 0;

	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, m_pSysMem, m_iSize);
	CONTEXT->Unmap(m_pBuffer, 0);
}

void CConstBuffer::SetRegister(UINT _iShaderType)
{
	if (_iShaderType & (UINT)SHADER_TYPE::VERTEX_SHADER)
		CONTEXT->VSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	if (_iShaderType & (UINT)SHADER_TYPE::HULL_SHADER)
		CONTEXT->HSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	if (_iShaderType & (UINT)SHADER_TYPE::DOMAIN_SHADER)
		CONTEXT->DSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	if (_iShaderType & (UINT)SHADER_TYPE::GEOMETRY_SHADER)
		CONTEXT->GSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	if (_iShaderType & (UINT)SHADER_TYPE::COMPUTE_SHADER)
		CONTEXT->CSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	if (_iShaderType & (UINT)SHADER_TYPE::PIXEL_SHADER)
		CONTEXT->PSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
}

void CConstBuffer::SetRegisterAll()
{
	CONTEXT->VSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	CONTEXT->HSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	CONTEXT->DSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	CONTEXT->GSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	CONTEXT->CSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
	CONTEXT->PSSetConstantBuffers(m_iRegister, 1, &m_pBuffer);
}
