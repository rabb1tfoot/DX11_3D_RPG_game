#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Texture.h"
#include "Material.h"

class CGameObject;

enum class TERRAIN_MOD
{
	HEIGHTMAP,
	SPLATTING,
	END,
};

class CTerrain :
	public CComponent
{
private:
	UINT						m_iXFaceCount;
	UINT						m_iZFaceCount;
	TERRAIN_MOD					m_eMod;
	Vec2						m_vBrushScale;

	CResPtr<CTexture>			m_pHeightMap;
	CResPtr<CMaterial>			m_pHeightMapMtrl;

	CResPtr<CTexture>			m_pWeightTex;
	CResPtr<CMaterial>			m_pWeightMtrl;
	UINT						m_iTileIdx;

	CResPtr<CTexture>			m_pOutput;
	CResPtr<CMaterial>			m_pPickMtrl;

	vector<CResPtr<CTexture>>	m_vecBrush;
	UINT						m_iBrushIdx;

	CCamera*					m_pToolCam;
	CGameObject*				m_pTessCam;

	int							m_HeightScale;

public:
	void init(UINT _iXFace, UINT _iZFace);
	void SetToolCamera(CCamera* _pCamera) { m_pToolCam = _pCamera; }
	void SetTessCamera(CGameObject* _pCamera) { m_pTessCam = _pCamera; }
	void ChangeXZFaceCount(UINT _iXFace, UINT _iZFace);

private:
	void ModCheck();
	void KeyCheck();
	int Picking(Vec2& _vPos);
	float PickingFeet(Vec3 _vPos);
	void SetFaceCount(UINT _iXFace, UINT _iZFace, CGameObject* _pTarget = nullptr);
	void CreateComputeShader();
	void LoadResource();

public:
	const TERRAIN_MOD& GetTerrainMode() { return m_eMod; }
	const UINT& GetTileIdx() {return m_iTileIdx;}
	const UINT& GetBrushIdx() { return m_iBrushIdx; }
	const Vec2& GetFaceCountXZ() { return Vec2((float)m_iXFaceCount, (float)m_iZFaceCount); }

	void SetFaceCountX(UINT _faceX) {m_iXFaceCount = _faceX;}
	void SetFaceCountZ(UINT _faceZ) { m_iZFaceCount = _faceZ; }

	float GetCollisionHeight(Vec3 _vPos);

public:
	virtual void update() {};
	virtual void finalupdate();

public:
	CLONE(CTerrain);
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile) {};
	virtual void LoadFromScene(FILE* _pFile, CGameObject* _pTarget);

public:
	CTerrain();
	virtual ~CTerrain();
};

