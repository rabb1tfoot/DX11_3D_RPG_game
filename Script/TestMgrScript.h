#pragma once

#include "SingleScript.h"

class CTestMgrScript :
	public CSingleScript<CTestMgrScript>
{
private:
	virtual void update();

public:
	void TestFunc();

public:
	CTestMgrScript();
	virtual ~CTestMgrScript();
};

