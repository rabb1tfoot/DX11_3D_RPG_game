#pragma once
#include <Script.h>

class CPlayerScript;

class CFireArrow :
	public CScript
{
private:
	CGameObject* m_ParticleObj;
	CPlayerScript* m_pPlayerScript;
	float  fLiveTime;
	bool   bDead;

public:
	void Setting(Vec3 _Dir, CPlayerScript* _script);

public:
	virtual void start();
	virtual void update();
	virtual void OnCollision3D(CCollider3D* _pOther);

private:
	void UpdateMove();
	void DeadTest();
public:
	CFireArrow();
	virtual ~CFireArrow();

public:
	CLONE(CFireArrow);
};

