#pragma once
#include "Entity.h"
class CStructuredBuffer :
	public CEntity
{
private:
	ID3D11Buffer*				m_pBuffer;
	ID3D11ShaderResourceView *	m_pSRV;
	ID3D11UnorderedAccessView*	m_pUAV;

	UINT						m_iElementSize;	// 요소 크기
	UINT						m_iElementCount;// 요소 개수

public:
	void Create(UINT _iElementSize, UINT _iElementCount, void* _pSysMem);

	void UpdateData(UINT _iRegister);
	void UpdateRWData(UINT _iRegister);

	void ClearData(UINT _iRegister);
	void ClearRWData(UINT _iRegister);

	CLONE_DISABLE(CStructuredBuffer);

public:
	CStructuredBuffer();
	virtual ~CStructuredBuffer();
};

