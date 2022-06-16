#pragma once

#include "Component.h"

#include "ResPtr.h"

#include "Mesh.h"
#include "Material.h"

class CMeshRender :
	public CComponent
{
private:
	CResPtr<CMesh>				m_pMesh;
	vector<CResPtr<CMaterial>>	m_vecMtrl;
	bool						m_bShadow; //그림자 여부

public:
	void SetMesh(CResPtr<CMesh>	_pMesh);
	void SetMaterial(CResPtr<CMaterial> _pMtrl, UINT _iSubset = 0) { m_vecMtrl[_iSubset] = _pMtrl; }
	void SetShadow(bool _bTrue) { m_bShadow = _bTrue; }

	CResPtr<CMesh> GetMesh() { return m_pMesh; }
	CResPtr<CMaterial> GetCloneMaterial(UINT _iSubset = 0);
	CResPtr<CMaterial> GetSharedMaterial(UINT _iSubset = 0) { return  m_vecMtrl[_iSubset]; }
	UINT GetMaterialCount() { return m_vecMtrl.size(); }
	bool IsShadowObj() { return m_bShadow; }
	ULONG64 GetInstID(UINT _iMtrlIdx);

public:
	virtual void update();
	void render();
	void render(UINT _iMtrl);
	void render_shadowmap();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

	CLONE(CMeshRender);

public:
	CMeshRender();
	virtual ~CMeshRender();
};

