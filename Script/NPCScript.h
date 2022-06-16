#pragma once
#include <Script.h>
#include "DialogueScript.h"

enum class DialogueProcess
{
	HELLO,
	PORTION,
	MONSTERHUNT,
	BOSSHUNT,
	END,
};

class CNPCScript :
	public CScript
{
public:
	CNPCScript();
	virtual ~CNPCScript();

private:
	DialogueProcess m_eProcess;
	CDialogueScript* m_pDialogueScript;

public:
	void LoadDialogue();

public:
	virtual void start();
	virtual void update();
	virtual void OnCollisionClick3D();
public:
	CLONE(CNPCScript);
};

