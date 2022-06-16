#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Mesh.h"
#include "Material.h"

// 1. 직사각형 
// 2. 원

class CCollider2D :
	public CComponent
{
private:
	static vector<CResPtr<CMaterial>>	g_vecColMtrl;	// 충돌체들이 사용 할 Material Pool
	static CResPtr<CMaterial>			g_pOriginMtrl;  // 충돌체가 사용할 원본 Material

public:
	static void CreateMaterial();
	static CResPtr<CMaterial> GetCloneMtrl();
	static void ClearMaterial();


private:
	Vec3				m_vOffset;	// Object 로 부터 상대좌표
	Vec3				m_vFinalPos;// Transform 위치 + Offset

	Vec3				m_vScale;	// 추가로 곱해지는 크기 배율값
	Vec3				m_vFinalScale; // Transform 크기 * 충돌체 크기

	COLLIDER2D_TYPE		m_eType;
	Matrix				m_matColWorld; // 충돌체 월드 행렬

	UINT				m_iColCount;

	CResPtr<CMesh>		m_pMesh;
	CResPtr<CMaterial>  m_pCurMtrl;

public:
	virtual void update();
	virtual void finalupdate();

public:
	void SetOffset(const Vec3& _vOffset) { m_vOffset = _vOffset; m_vOffset.z = 0.f; }
	const Vec3& GetOffset() { return m_vOffset; }

	void SetScale(const Vec3& _vScale) { m_vScale = _vScale; m_vScale.z = 0.f; }
	const Vec3& GetScale() { return m_vScale; }
	const Vec3& GetFinalPos() { return m_vFinalPos; }

	void SetColliderType(COLLIDER2D_TYPE _eType);
	COLLIDER2D_TYPE GetCollider2DType() { return m_eType; }

	const Matrix& GetWorldMat() { return m_matColWorld; }

	void OnCollisionEnter(CCollider2D* _pOther);
	void OnCollision(CCollider2D* _pOther);
	void OnCollisionExit(CCollider2D* _pOther);
	void OnCollisionClick();

	void UpdateData();
	void render();

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);


	CLONE(CCollider2D);

public:
	CCollider2D();
	virtual ~CCollider2D();
};

