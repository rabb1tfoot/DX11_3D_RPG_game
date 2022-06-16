#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Mesh.h"
#include "Material.h"

//큐브, 구

class CCollider3D :
	public CComponent
{
private:
	static vector<CResPtr<CMaterial>> g_vecCol3DMtrl;
	static CResPtr<CMaterial>		  g_pOriginMtrl;

public:
	static void CreateMaterial();
	static CResPtr<CMaterial> GetCloneMtrl();
	static CResPtr<CMaterial> GetSharedMaterial() { return g_pOriginMtrl; }
	static void ClearMaterial();

private:
	Vec3 m_vOffset;
	Vec3 m_vFinalPos;

	Vec3 m_vScale;
	Vec3 m_vFinalScale;

	COLLIDER3D_TYPE m_eType;
	Matrix			m_matColWorld; //월드행렬

	UINT m_iColCount;

	CResPtr<CMesh> m_pMesh;
	CResPtr<CMaterial> m_pCurMtrl;

public:
	virtual void update();
	virtual void finalupdate();

public:
	void SetOffset(const Vec3& _vOffset) { m_vOffset = _vOffset;}
	const Vec3& GetOffset() { return m_vOffset; }

	void SetScale(const Vec3& _vScale) { m_vScale = _vScale;}
	const Vec3& GetScale() { return m_vScale; }

	void SetColliderType(COLLIDER3D_TYPE _eType);
	COLLIDER3D_TYPE GetCollider3DType() { return m_eType; }

	const Matrix& GetWorldMat() { return m_matColWorld; }

	void OnCollisionEnter(CCollider3D* _pOther);
	void OnCollision(CCollider3D* _pOther);
	void OnCollisionExit(CCollider3D* _pOther);
	void OnCollisionClick3D();

	void UpdateData();
	void render_inst();
	void render();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);


	CLONE(CCollider3D);

public:
	CCollider3D();
	virtual ~CCollider3D();
};

