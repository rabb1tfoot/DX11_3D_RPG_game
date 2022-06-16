#pragma once

#include "Component.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ResMgr.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Collider2D.h"
#include "Collider3D.h"
#include "Animation2D.h"
#include "Animator2D.h"
#include "Font23.h"

#include "Scene.h"
#include "Layer.h"
#include "SceneMgr.h"

class CScript :
	public CComponent
{
private:
	UINT			m_iScriptType;

public:
	virtual void update() {}

public:
	UINT GetScriptType() { return m_iScriptType; }
	virtual void SaveToScene(FILE* _pFile) {}
	virtual void LoadFromScene(FILE* _pFile) {}

protected:
	void Instantiate(CResPtr<CPrefab>& _pPrefab, const Vec3& _vPos, const wstring& _strLayerName);

	virtual void OnCollisionEnter(CCollider2D* _pOther) {};
	virtual void OnCollision(CCollider2D* _pOther) {};
	virtual void OnCollisionExit(CCollider2D* _pOther) {};
	virtual void OnCollisionClick() {};

	virtual void OnCollisionEnter3D(CCollider3D* _pOther) {};
	virtual void OnCollision3D(CCollider3D* _pOther) {};
	virtual void OnCollisionExit3D(CCollider3D* _pOther) {};
	virtual void OnCollisionClick3D() {};

	void AddChild(CGameObject* _pChildObject);
	void ClearChild(UINT _iChildIdx);
	void ClearParent();

	void CreateObject(CGameObject* _pTarget, int iLayerIdx);
	void DeleteObject(CGameObject* _pTarget);

	void SetDisable(CGameObject* _pTarget);
	void SetEnable(CGameObject* _pTarget);


public:
	CScript(UINT _iType);
	virtual ~CScript();

	friend class CCollider2D;
	friend class CCollider3D;
};

