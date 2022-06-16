#pragma once
#include "Resource.h"

#include "ResPtr.h"
#include "Mesh.h"
#include "Material.h"

class CGameObject;

class CFilter23 :
	public CResource
{
private:
	CGameObject*	m_pFilterObj;

public:
	void Create(CResPtr<CMesh> _pMesh, CResPtr<CMaterial> _pMtrl);
	void render();

public:
	CFilter23();
	virtual ~CFilter23();
};

