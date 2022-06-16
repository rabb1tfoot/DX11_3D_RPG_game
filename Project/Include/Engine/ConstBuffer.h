#pragma once
#include "Entity.h"

class CConstBuffer :
	public CEntity
{
private:
	ID3D11Buffer* m_pBuffer;
	UINT m_iSize;
	UINT m_iRegister;
	
	void* m_pSysMem;
	UINT m_iCurSize;

public:
	void Create(UINT _iSize, UINT _iRegisterNum);
	void AddData(const void* _pSrc, UINT _iSize);
	void UpdateData();
	void SetRegister(UINT _iShaderType);
	void SetRegisterAll();

public:
	CLONE_DISABLE(CConstBuffer);
public:
	CConstBuffer();
	virtual ~CConstBuffer();
};

