#pragma once

#include "ResPtr.h"
#include "Texture.h"

class CConstBuffer;

class CDevice
{

	SINGLE(CDevice);

private:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

	UINT					    m_iQuality;
	map<wstring, CConstBuffer*> m_mapConstBuffer;

public:
	int init();
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pContext; }

	void CreateConstBuffer(const wstring& _strKey, UINT _iSize, UINT _iRegister);
	CConstBuffer* FindConstBuffer(const wstring& _strKey);


};

