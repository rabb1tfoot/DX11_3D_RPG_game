#pragma once
#include "Component.h"

class CTransform :
	public CComponent
{
private:
	Vec3		m_vLocalPos;
	Vec3		m_vLocalScale;
	Vec3		m_vLocalRot;

	Vec3		m_vLocalDir[(UINT)DIR_TYPE::END];
	Vec3		m_vWorldDir[(UINT)DIR_TYPE::END];

	Matrix		m_matWorld;
	Matrix		m_matAddRot;

public:
	void SetLocalPos(const Vec3& _vPos) { m_vLocalPos = _vPos; }
	void SetLocalScale(const Vec3& _vScale) { m_vLocalScale = _vScale; }
	void SetLocalRot(const Vec3& _vRot) { m_vLocalRot = _vRot; }

	void AddLocalRot(const Matrix& _matRot);

	const Vec3& GetLocalPos() { return m_vLocalPos; }
	Vec3 GetWorldPos();

	const Vec3& GetLocalScale() { return m_vLocalScale; }
	const Vec3& GetLocalRot() { return m_vLocalRot; }

	const Vec3& GetLocalDir(DIR_TYPE _eType) { return m_vLocalDir[(UINT)_eType]; }
	const Matrix& GetWorldMat() { return m_matWorld; }

	void SetLookAt(const Vec3& _vDir);
	const Vec3& GetLookAt(DIR_TYPE _eType) { return m_vLocalDir[(UINT)_eType]; }

public:
	virtual void update();
	virtual void finalupdate();
	void UpdateData();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLONE(CTransform);

public:
	CTransform();
	virtual ~CTransform();
};

