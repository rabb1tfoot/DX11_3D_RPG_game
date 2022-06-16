#pragma once

#include "Resource.h"
#include "FBXLoader.h"

#include "ResPtr.h"
#include "Material.h"
#include "Mesh.h"

class CGameObject;


class CMeshData :
	public CResource
{
private:
	CResPtr<CMesh>				m_pMesh;
	vector<CResPtr<CMaterial>>	m_vecMtrl;

	// Animation3D
public:
	static CMeshData* LoadFromFBX(const wstring& _strFilePath);

	virtual void Load(const wstring& _strFilePath);
	virtual void Save();
	CGameObject* Instantiate();
	void Input() { int a = 0; }

	CLONE_DISABLE(CMeshData);

public:
	CMeshData();
	virtual ~CMeshData();
};