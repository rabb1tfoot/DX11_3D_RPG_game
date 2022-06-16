#pragma once
#include "Entity.h"

#include "GameObject.h"

class CComponent :
	public CEntity
{
private:
	COMPONENT_TYPE	m_eComType;
	CGameObject*    m_pObject;

public:
	virtual void awake() {};
	virtual void start() {};

	virtual void update() = 0;
	virtual void lateupdate() {};
	virtual void finalupdate() {};

	virtual void OnEnable() {};
	virtual void OnDisable() {};

	COMPONENT_TYPE GetCompnentType() { return m_eComType; }
	CGameObject* Object() { return m_pObject; }
	CGameObject* ParentObject() { return m_pObject->GetParent(); }
	bool HasParent() { return m_pObject->GetParent(); };

	virtual void SaveToScene(FILE* _pFile) = 0;
	virtual void LoadFromScene(FILE* _pFile) = 0;
	virtual void LoadFromScene(FILE* _pFile, CGameObject* _pTarget) {};

protected:
	CTransform* Transform() { return m_pObject->Transform(); }
	CMeshRender* MeshRender() { return m_pObject->MeshRender(); }
	CCollider2D* Collider2D() { return m_pObject->Collider2D(); }
	CCollider3D* Collider3D() { return m_pObject->Collider3D(); }
	CCamera* Camera() { return m_pObject->Camera(); }
	CAnimator2D* Animator2D() { return m_pObject->Animator2D(); }
	CAnimator3D* Animator3D() { return m_pObject->Animator3D(); }
	CTerrain* Terrain() { return m_pObject->Terrain(); }
	CLight3D* Light() { return m_pObject->Light3D(); }
	CFont23* Font() { return m_pObject->Font(); }

private:
	void SetGameObject(CGameObject* _pObject) { m_pObject = _pObject; }

	CLONE_DISABLE(CComponent);
public:
	CComponent(const CComponent& _com);

public:
	CComponent(COMPONENT_TYPE _eType);
	virtual ~CComponent();

	friend class CGameObject;
};

