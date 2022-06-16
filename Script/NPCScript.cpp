#include "stdafx.h"
#include "NPCScript.h"


CNPCScript::CNPCScript()
	:CScript((UINT)SCRIPT_TYPE::NPCSCRIPT)
	, m_eProcess(DialogueProcess::HELLO)
	, m_pDialogueScript(nullptr)
{
}


CNPCScript::~CNPCScript()
{
}

void CNPCScript::LoadDialogue()
{
}

void CNPCScript::start()
{
	vector<CScript*> pScripts =  Object()->GetScripts();
	for (size_t i = 0; i < pScripts.size(); ++i)
	{
		if (pScripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::DIALOGUESCRIPT)
		{
			m_pDialogueScript = (CDialogueScript*)pScripts[i];
			break;
		}
	}
}

void CNPCScript::update()
{
}

void CNPCScript::OnCollisionClick3D()
{
	if (m_pDialogueScript->IsDialgue())
		return;

	switch (m_eProcess)
	{
	case DialogueProcess::HELLO:
		m_pDialogueScript->LoadingText(L"Sample.txt");
		m_pDialogueScript->OnDialogue();
		m_eProcess = DialogueProcess::PORTION;
		break;
	case DialogueProcess::PORTION:
		break;
	case DialogueProcess::MONSTERHUNT:
		break;
	case DialogueProcess::BOSSHUNT:
		break;
	}
	m_pDialogueScript->OnDialogue();
}
