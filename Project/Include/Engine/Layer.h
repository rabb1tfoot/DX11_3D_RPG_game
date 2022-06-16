#pragma once
#include "Entity.h"

class CGameObject;

class CLayer :
	public CEntity
{
private:
	vector<CGameObject*>	m_vecParentObj; // Layer �� ������ �ִ� �ֻ��� �θ�
	vector<CGameObject*>    m_vecObject;	// Layer �� ������ �ִ� ��� ������Ʈ
	int						m_iLayerIdx;	// Scene ���� ���° layer ���� 

public:
	void awake();
	void start();

	void update();
	void lateupdate();
	void finalupdate();

	void render();

public:
	void AddObject(CGameObject* _pObj, bool _bMoveAll);
	int GetLayerIdx() { return m_iLayerIdx; }

private:
	void SetLayerIdx(UINT _iIdx) { m_iLayerIdx = _iIdx; }
	void RegisterObj(CGameObject* _pObj) { m_vecObject.push_back(_pObj); }
	void ClearRegisterObj() { m_vecObject.clear(); }
	void RemoveParentObj(CGameObject* _pObj);

public:
	const vector<CGameObject*>& GetParentObject() { return m_vecParentObj; }
	const vector<CGameObject*>& GetAllObject() { return m_vecObject; }

	CLONE_DISABLE(CLayer);
public:
	CLayer();
	virtual ~CLayer();

	friend class CScene;
	friend class CGameObject;
	friend class CEventMgr;
};

