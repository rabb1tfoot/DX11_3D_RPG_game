#pragma once
#include <Script.h>

class CGameObject;
class CPlayerScript;

enum class MOVETYPE
{
	NORMAL,
	TRACKING,
	ATTACK,
	END,
};

class CMonsterScript :
	public CScript
{
public:
	CMonsterScript();
	virtual ~CMonsterScript();

private:
	MOVETYPE m_Movetype;
	int		 m_moveCommand;
	CGameObject* m_pTarget;
	CGameObject* m_UIMonsterHp;
	CGameObject* m_UIMonsterHpFont;

	CPlayerScript* m_pTargetScript;

	int m_iHp;
	int m_ibeforeHp;
	int m_iMaxHp;
	int m_iMp;
	int m_ibeforeMp;
	int m_iMaxMp;
	int m_iAttPoint;
	int m_iMpPoint;

	float m_fAccMovetick;
	float m_fAccShowHpBar;
	float m_fAccAttacktick;

	bool m_Hitted;
	bool m_bTouch;
	bool m_bDead;

public:
	int GetHp() { return m_iHp; }
	int GetMp() { return m_iMp; }
	int GetMaxHp() { return m_iMaxHp; }
	int GetMaxMp() { return m_iMaxMp; }
	int GetAtt() { return m_iAttPoint; }
	int GetMpp() { return m_iMpPoint; }

	void SetState(int _hp, int _mp, int _Att, int _Mpp);

	void SubHp(int _amount);
	void SubMp(int _amount) { m_iMp -= _amount;}
	void AddHp(int _amount) { m_iHp += _amount; }
	void AddMp(int _amount) { m_iMp += _amount; }

	void ChangeHp(int _amount) { m_iHp = _amount; };
	void ChangeMp(int _amount) { m_iMp = _amount; };
	void ChangeAtt(int _amount) { m_iAttPoint = _amount; }
	void ChangeMpp(int _amount) { m_iMpPoint = _amount; }


	void SetUnHitted() { m_Hitted = false; }
public:
	void CheckDistance();
	void CheckShowHp();
	void TrackingMove();
	void NormalMove();
	void CheckDead();
public:
	CLONE(CMonsterScript);

public:
	virtual void start();
	virtual void update();
	virtual void OnCollision3D(CCollider3D* _pOther);
	virtual void OnCollisionExit3D(CCollider3D* _pOther);
};

