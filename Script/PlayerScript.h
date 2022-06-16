#pragma once
#include <Script.h>
#include <ResPtr.h>
#include <Prefab.h>
#include <Material.h>

class CGameObject;

class CPlayerScript :
	public CScript
{
private:
	int m_iHp;
	int m_ibeforeHp;
	int m_iMaxHp;
	int m_iMp;
	int m_ibeforeMp;
	int m_iMaxMp;
	int m_iAttPoint;
	int m_iMpPoint;

	CGameObject* m_UIHp;
	CGameObject* m_UIMp;

	CGameObject* m_UIHpFont;
	CGameObject* m_UIMpFont;

	CGameObject* m_UIMonseterUI;
	CGameObject* m_UIMonseterUIFont;

	MOVE_STATE m_eState;

public:
	virtual void start();
	virtual void update();

	virtual void OnEnable() {};
	virtual void OnDisable() {};

private:
	CLONE(CPlayerScript);

public:
	int GetHp() { return m_iHp; }
	int GetMp() { return m_iMp; }
	int GetMaxHp() { return m_iMaxHp; }
	int GetMaxMp() { return m_iMaxMp; }
	int GetAtt() { return m_iAttPoint; }
	int GetMpp() { return m_iMpPoint; }

	void SubHp(int _amount) { m_iHp -= _amount; }
	void SubMp(int _amount) { m_iMp -= _amount; }
	void AddHp(int _amount) { m_iHp += _amount; }
	void AddMp(int _amount) { m_iMp += _amount; }

	void ChangeHp(int _amount) { m_iHp = _amount; };
	void ChangeMp(int _amount) { m_iMp = _amount; };
	void ChangeAtt(int _amount) { m_iAttPoint = _amount; }
	void ChangeMpp(int _amount) { m_iMpPoint = _amount; }

	MOVE_STATE GetMoveState() { return m_eState; }
	void SetMoveState(MOVE_STATE _eType) { m_eState = _eType; }

private:
	void CalCurState();
	void UpdateMoveDir(Vec3 _vPos, Vec3 _vRot);
	void DialogueBoxTest();
	void CheckInventory();


public:
	CPlayerScript();
	virtual ~CPlayerScript();
};

