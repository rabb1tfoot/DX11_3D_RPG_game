#include "stdafx.h"
#include "Shader.h"

#include "PathMgr.h"

#include "Device.h"

#include "BlendState.h"
#include "DepthStencilState.h"
#include "RenderMgr.h"

CShader::CShader()
	: CResource(RES_TYPE::SHADER)
	, m_pVSBlob(nullptr)
	, m_pVSInstBlob(nullptr)
	, m_pHSBlob(nullptr)
	, m_pDSBlob(nullptr)
	, m_pGSBlob(nullptr)
	, m_pCSBlob(nullptr)
	, m_pPSBlob(nullptr)
	, m_pErrBlob(nullptr)
	, m_pVS(nullptr)
	, m_pVSInst(nullptr)
	, m_pHS(nullptr)
	, m_pDS(nullptr)
	, m_pGS(nullptr)
	, m_pCS(nullptr)
	, m_pPS(nullptr)
	, m_pBlendState(nullptr)
	, m_pDepthStencilState(nullptr)
	, m_eRSType(RS_TYPE::CULL_BACK)
	, m_eEventTime(SHADER_EVENT_TIME::FORWARD)
	, m_eTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

CShader::~CShader()
{
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pVSInstBlob);
	SAFE_RELEASE(m_pHSBlob);
	SAFE_RELEASE(m_pDSBlob);
	SAFE_RELEASE(m_pGSBlob);
	SAFE_RELEASE(m_pPSBlob);
	SAFE_RELEASE(m_pCSBlob);
	SAFE_RELEASE(m_pErrBlob);

	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pVSInst);
	SAFE_RELEASE(m_pHS);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pCS);
	SAFE_RELEASE(m_pPS);
}

void CShader::CreateVertexShader(const wstring & _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow)
{
	// Shader 만들기
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "vs_%d_%d", _iHigh, _iLow);

	// VtxShader Compile
	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pVSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Vertex Shader Error", MB_OK);
		assert(nullptr);
	}

	// Create Vertex Shader
	DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), nullptr, &m_pVS);
}

void CShader::CreateVertexInstShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	//Shader 만들기
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "vs_%d_%d", _iHigh, _iLow);

	//vtxshader compile
	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pVSInstBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "VertexInst Shader Error", MB_OK);
		assert(nullptr);
	}

	//Create VertexInstShader
	DEVICE->CreateVertexShader(m_pVSInstBlob->GetBufferPointer(), m_pVSInstBlob->GetBufferSize(), nullptr, &m_pVSInst);
}

void CShader::CreateHullShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "hs_%d_%d", _iHigh, _iLow);

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pHSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Hull Shader Error", MB_OK);
		assert(nullptr);
	}

	DEVICE->CreateHullShader(m_pHSBlob->GetBufferPointer(), m_pHSBlob->GetBufferSize(), nullptr, &m_pHS);
}

void CShader::CreateDomainShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "ds_%d_%d", _iHigh, _iLow);

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pDSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Domain Shader Error", MB_OK);
		assert(nullptr);
	}

	DEVICE->CreateDomainShader(m_pDSBlob->GetBufferPointer(), m_pDSBlob->GetBufferSize(), nullptr, &m_pDS);
}

void CShader::CreateGeometryShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "gs_%d_%d", _iHigh, _iLow);

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pGSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Geometry Shader Error", MB_OK);
		assert(nullptr);
	}

	DEVICE->CreateGeometryShader(m_pGSBlob->GetBufferPointer(), m_pGSBlob->GetBufferSize(), nullptr, &m_pGS);
}

void CShader::CreatePixelShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	// Shader 만들기
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "ps_%d_%d", _iHigh, _iLow);

	// VtxShader Compile
	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pPSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Pixel Shader Error", MB_OK);
		assert(nullptr);
	}

	// Create Vertex Shader
	DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize(), nullptr, &m_pPS);
}

void CShader::CreateComputeShader(const wstring & _strFilePath, const string & _strFuncName, UINT _iHigh, UINT _iLow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG;
#endif

	wstring strPath = CPathMgr::GetResPath();
	strPath += _strFilePath;

	char strTarget[20] = {};
	sprintf_s(strTarget, "cs_%d_%d", _iHigh, _iLow);

	if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _strFuncName.c_str()
		, strTarget, iFlag, 0, &m_pCSBlob, &m_pErrBlob)))
	{
		char* pErr = (char*)m_pErrBlob->GetBufferPointer();
		MessageBoxA(nullptr, pErr, "Compute Shader Error", MB_OK);
		assert(nullptr);
	}

	DEVICE->CreateComputeShader(m_pCSBlob->GetBufferPointer(), m_pCSBlob->GetBufferSize(), nullptr, &m_pCS);
}


void CShader::UpdateData()
{
	// Topology 
	CONTEXT->IASetPrimitiveTopology(m_eTopology);

	// 물체 렌더링
	CONTEXT->VSSetShader(m_pVS, nullptr, 0);
	CONTEXT->HSSetShader(m_pHS, nullptr, 0);
	CONTEXT->DSSetShader(m_pDS, nullptr, 0);
	CONTEXT->GSSetShader(m_pGS, nullptr, 0);
	CONTEXT->CSSetShader(m_pCS, nullptr, 0);
	CONTEXT->PSSetShader(m_pPS, nullptr, 0);

	if (m_pBlendState)
		m_pBlendState->UpdateData();
	else
		CBlendState::UpdateDefaultData();

	if (m_pDepthStencilState)
		m_pDepthStencilState->UpdateData();
	else
		CDepthStencilState::UpdateDefaultData();

	CRenderMgr::GetInst()->SetRSState(m_eRSType);
}

void CShader::UpdateDataInstancing()
{
	CONTEXT->IASetPrimitiveTopology(m_eTopology);

	//물체 렌더링
	CONTEXT->VSSetShader(m_pVSInst, nullptr, 0);
	CONTEXT->HSSetShader(m_pHS, nullptr, 0);
	CONTEXT->DSSetShader(m_pDS, nullptr, 0);
	CONTEXT->GSSetShader(m_pGS, nullptr, 0);
	CONTEXT->CSSetShader(m_pCS, nullptr, 0);
	CONTEXT->PSSetShader(m_pPS, nullptr, 0);

	if (m_pBlendState)
		m_pBlendState->UpdateData();
	else
		CBlendState::UpdateDefaultData();

	if (m_pDepthStencilState)
		m_pDepthStencilState->UpdateData();
	else
		CDepthStencilState::UpdateDefaultData();

	CRenderMgr::GetInst()->SetRSState(m_eRSType);
}
