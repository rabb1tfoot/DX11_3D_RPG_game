#pragma once
#include <Material.h>

class CMaterialEx :
	public CMaterial
{
public:
	virtual void SetData(SHADER_PARAM _eType, void* _pSrc);

public:
	CMaterialEx();
	virtual ~CMaterialEx();
};

