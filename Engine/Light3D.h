#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Material.h"
#include "Mesh.h"

class CLight3D :
	public CComponent
{
private:
	tLight3DInfo		m_tInfo;
	CResPtr<CMaterial>	m_pMtrl;
	CResPtr<CMesh>		m_pMesh;
	int					m_iIdx;  //RenderMgr 에 등록시킬때 추가된 배열의 인덱스

	CGameObject*		m_pCamObj;

public:
	virtual void update() {}
	virtual void finalupdate();
	void render();
	void render_shadowmap();

public:
	void SetLightType(LIGHT_TYPE _eType);
	void SetLightDir(const Vec3& _vLightDir);
	void SetLightDiffuse(const Vec3& _vLightColor) { m_tInfo.vDiff = _vLightColor; }
	void SetLightSpecular(const Vec3& _vLightSpec) { m_tInfo.vSpec = _vLightSpec; }
	void SetLightAmbient(const Vec3& _vLightAmb) { m_tInfo.vAmb = _vLightAmb; }
	void SetLightRange(float _fRange);
	void SetLightAngle(float _fAngle) { m_tInfo.fAngle = GetRadian(_fAngle); }

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

	const tLight3DInfo& GetLight3DInfo() { return m_tInfo; }


public:
	CLONE(CLight3D);

public:
	CLight3D();
	CLight3D(const CLight3D& _light); // 광원 복사 시, 별도의 카메라를 가져야 하므로, 깊은복사
	virtual ~CLight3D();
};

