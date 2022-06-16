#pragma once
#include <Script.h>
class CCamScript :
	public CScript
{
private:
	float		m_fSpeed;
	float		m_fMul;

public:
	virtual void update();

public:
	CLONE(CCamScript)

public:
	CCamScript();
	virtual ~CCamScript();
};

