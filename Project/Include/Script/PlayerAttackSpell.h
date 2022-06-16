#pragma once
#include <Script.h>

class CGameObject;
class CPlayerScript;

class CPlayerAttackSpell :
	public CScript
{
private:
	float			m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::END];
	float			m_fSpellCooldownTime[(UINT)PLAYER_SKILL::END];

	//폰트용 게임오브젝트
	CGameObject*	m_SpellCooldownFont[(UINT)PLAYER_SKILL::END];
	CGameObject*	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::END];
	//스킬 이팩트용 게임오브젝트
	CGameObject*	m_SpellEffectHeal;
	CGameObject*	m_SpellEffectMed;
	CPlayerScript*	m_pPlayerScript;

	CScene*			m_pCurScene;

	float			m_fHillAmount;
	float			m_fMedAmount;

public:
	virtual void start();
	virtual void update();
	virtual void OnCollision3D(CCollider3D* _pOther);

private:
	CLONE(CPlayerAttackSpell);

private:
	void CheckSkill();
	void UpdateCoolTime();
	void CreateFireArrow();

public:
	CPlayerAttackSpell();
	virtual ~CPlayerAttackSpell();
};

