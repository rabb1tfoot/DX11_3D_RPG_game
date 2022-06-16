#pragma once
#include <Script.h>
class CHotKeyScript :
	public CScript
{
public:
	CHotKeyScript();
	virtual ~CHotKeyScript();

public:
	virtual void start();
	virtual void update();

public:
	CLONE(CHotKeyScript);
};

