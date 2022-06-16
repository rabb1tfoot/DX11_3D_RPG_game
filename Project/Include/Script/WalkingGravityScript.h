#pragma once
#include <Script.h>
class CGameObject;
class CWalkingGravityScript :
	public CScript
{

private:
	float m_fGravity;
	CGameObject* m_pTerrain;

public:
	virtual void start();
	virtual void update();

private:
	CLONE(CWalkingGravityScript);

public:
	CWalkingGravityScript();
	virtual ~CWalkingGravityScript();
};


