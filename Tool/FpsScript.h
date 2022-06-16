#pragma once
#include <Script.h>
class CFpsScript :
	public CScript
{
private:
	float m_fFPS;

public:
	virtual void update();

public:
	CLONE(CFpsScript);

public:
	CFpsScript();
	virtual ~CFpsScript();
};

