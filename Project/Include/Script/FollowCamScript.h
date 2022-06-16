#pragma once
#include <Script.h>

class CGameObject;

class CFollowCamScript :
	public CScript
{
public:
	CFollowCamScript();
	virtual ~CFollowCamScript();

private:
	CGameObject *m_pPlayer;
	float m_fAngle;

public:
	CLONE(CFollowCamScript);

public:
	virtual void start();
	virtual void update();

};

