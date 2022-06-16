#pragma once

#include <DirectXTex.h>
#include <DirectXTex.inl>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug")
#else
#pragma comment(lib, "DirectXTex")
#endif

#include "Resource.h"
#include "ResPtr.h"
class CMaterial;

class CTexture :
	public CResource
{
public:
	static CMaterial*		g_pClearMtrl;
private:
	ID3D11ShaderResourceView*	m_pSRV;
	ID3D11UnorderedAccessView*  m_pUAV;
	ID3D11RenderTargetView*		m_pRTV;
	ID3D11DepthStencilView*		m_pDSV;

	ID3D11Texture2D*			m_pTex2D;
	ScratchImage				m_Image;
	D3D11_TEXTURE2D_DESC		m_tDesc;


public:
	virtual void Load(const wstring& _strFilePath);
	void Save(const wstring& _strPath);
	virtual void Create(UINT _iWidth, UINT _iHeight, UINT _iBindFlag, D3D11_USAGE _eUsage, DXGI_FORMAT _eFormat);
	virtual void Create(ID3D11Texture2D* _pTex2D);
	void CreateArrayTexture(vector<CResPtr<CTexture>>& _vecTex);

public:
	UINT GetWidth() { return m_tDesc.Width; }
	UINT GetHeight() { return m_tDesc.Height; }

	size_t GetRowPitch() { return m_Image.GetImages()->rowPitch; }
	size_t GetSlicePitch() { return m_Image.GetImages()->slicePitch; }

	ID3D11Texture2D* GetTex2D() { return m_pTex2D; }
	const D3D11_TEXTURE2D_DESC& GetDesc() { return m_tDesc; }

	ID3D11ShaderResourceView* GetSRV() { return m_pSRV; }
	ID3D11RenderTargetView* GetRTV() { return m_pRTV; }
	ID3D11DepthStencilView* GetDSV() { return m_pDSV; }
	ID3D11UnorderedAccessView* GetUAV() { return m_pUAV; }

	void Capture();
	void* GetSysMem();
	void ClearTex();
	void ClearRWTex();

public:
	void SetRegister(UINT _iRegister, UINT _iShaderType);
	void SetRegisterAll(UINT _iRegister);
	static void ClearRegister(UINT _iRegister, UINT _iShaderType);
	static void ClearAllRegister();

	void SetData(void * _pSrc, size_t _size);
	void Resize(UINT _iWidth, UINT _iHeight);

	void SetRWRegister(UINT _iRegister);
	static void ClearRWRegister(UINT _iRegister);

	virtual bool LoadFromScene(FILE* _pFile);

public:
	CTexture();
	virtual ~CTexture();
};

