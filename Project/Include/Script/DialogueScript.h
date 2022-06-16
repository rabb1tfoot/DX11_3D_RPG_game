#pragma once
#include <Script.h>


class CDialogueScript :
	public CScript
{
public:
	CDialogueScript();
	virtual ~CDialogueScript();

private:
	CGameObject*	m_pBackground;
	CGameObject*	m_pTextObject;
	vector<wstring> m_vecText;

	int				m_iTextIdx;
	int				m_iSentenceIdx;
	float			m_fTextSpeed;
	Vec2			m_FirstFont;
	bool			m_bIsWriting;
	bool			m_bIsOn;
	bool			m_bEnterText;

public:
	void LoadingText(const wstring& _FileName);
	bool IsDialgue() { return m_bIsOn; }
	void OnDialogue() { m_bIsOn = true; };
	void ExitDialogue() { m_bIsOn = false; };

private:
	void Writing();

public:
	virtual void start();
	virtual void update();

public:
	CLONE(CDialogueScript);
};

