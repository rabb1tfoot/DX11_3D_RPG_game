#pragma once
#include <Script.h>

enum class EANSWER
{
	NO,
	YES,
	NONE,
};

class CDialogueBoxScript :
	public CScript
{


public:
	CDialogueBoxScript();
	virtual ~CDialogueBoxScript();

private:
	CGameObject* m_pInfo;
	CGameObject* m_pYesOrNoObj[2];
	EANSWER		 m_eAnswer;

public:
	void DialogueOn();
	void SetBoxText(wstring _text);
	UINT GetAnswer();

private:
	void DialogueOff();
public:
	virtual void start();
	virtual void update();

public:
	CLONE(CDialogueBoxScript);
};

