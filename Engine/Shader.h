#pragma once
#include "Resource.h"

class CBlendState;
class CDepthStencilState;

struct tPramInfo
{
	SHADER_PARAM eType;
	wchar_t		 szName[50];
	bool		 m_bAccess;
};

enum class SHADER_EVENT_TIME
{
	DEFERRED,
	FORWARD,
	PARTICLE,
	POSTEFFECT,
};

class CShader :
	public CResource
{
	ID3DBlob*				m_pVSBlob;
	ID3DBlob*				m_pVSInstBlob;
	ID3DBlob*				m_pHSBlob;
	ID3DBlob*				m_pDSBlob;
	ID3DBlob*				m_pGSBlob;
	ID3DBlob*			    m_pCSBlob;
	ID3DBlob*				m_pPSBlob;
	ID3DBlob*				m_pErrBlob;

	ID3D11VertexShader *	m_pVS;
	ID3D11VertexShader *	m_pVSInst;
	ID3D11HullShader *		m_pHS;
	ID3D11DomainShader *	m_pDS;
	ID3D11GeometryShader *	m_pGS;
	ID3D11ComputeShader*    m_pCS;
	ID3D11PixelShader *		m_pPS;

	CBlendState*			m_pBlendState;
	CDepthStencilState*		m_pDepthStencilState;
	RS_TYPE					m_eRSType;
	vector<tPramInfo>		m_vecParam;
	D3D11_PRIMITIVE_TOPOLOGY m_eTopology;

	SHADER_EVENT_TIME		m_eEventTime;

public:
	ID3DBlob* GetVSBlob() { return  m_pVSBlob; }
	ID3DBlob* GetVSInstBlob() { return  m_pVSInstBlob; }
	ID3DBlob* GetHSBlob() { return m_pHSBlob; }
	ID3DBlob* GetDSBlob() { return m_pDSBlob; }
	ID3DBlob* GetGSBlob() { return m_pGSBlob; }
	ID3DBlob* GetPSBlob() { return m_pPSBlob; }

	ID3D11VertexShader *	GetVS() { return m_pVS; }
	ID3D11HullShader *		GetHS() { return m_pHS; }
	ID3D11DomainShader *	GetDS() { return m_pDS; }
	ID3D11GeometryShader *	GetGS() { return m_pGS; }
	ID3D11PixelShader *		GetPS() { return m_pPS; }

	void SetBlendState(CBlendState* _pBlendState) { m_pBlendState = _pBlendState; }
	void SetDepthStencilState(CDepthStencilState* _pDSState) { m_pDepthStencilState = _pDSState; }
	void SetRSType(RS_TYPE _eType) { m_eRSType = _eType; }

	void SetEventTime(SHADER_EVENT_TIME _eTime) { m_eEventTime = _eTime; }
	SHADER_EVENT_TIME GetEventTime() { return m_eEventTime; }

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _eTopology) { m_eTopology = _eTopology; }

public:
	void CreateVertexShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);
	void CreateVertexInstShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);
	void CreateHullShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);
	void CreateDomainShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);
	void CreateGeometryShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);
	void CreatePixelShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);

	void CreateComputeShader(const wstring& _strFilePath, const string& _strFuncName, UINT _iHigh, UINT _iLow);

	void UpdateData();
	void UpdateDataInstancing();

	void AddParam(SHADER_PARAM _eType, const wstring& _strMean, bool _bAccess = true)
	{
		tPramInfo param;
		param.eType = _eType;
		param.m_bAccess = _bAccess;
		wcscpy_s(param.szName, 50, _strMean.c_str());
		m_vecParam.push_back(param);
	}

	const vector<tPramInfo>& GetParamInfo() { return 	m_vecParam; }

public:
	CShader();
	virtual ~CShader();
};

