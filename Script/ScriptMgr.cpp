#include "stdafx.h"
#include "ScriptMgr.h"

#include "DialogueBoxScript.h"
#include "DialogueScript.h"
#include "FireArrow.h"
#include "FollowCamScript.h"
#include "HotKeyScript.h"
#include "Inventory.h"
#include "ItemScript.h"
#include "MonsterScript.h"
#include "NPCScript.h"
#include "PlayerAttackSpell.h"
#include "PlayerScript.h"
#include "TestMgrScript.h"
#include "WalkingGravityScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"DialogueBoxScript");
	_vec.push_back(L"DialogueScript");
	_vec.push_back(L"FireArrow");
	_vec.push_back(L"FollowCamScript");
	_vec.push_back(L"HotKeyScript");
	_vec.push_back(L"Inventory");
	_vec.push_back(L"ItemScript");
	_vec.push_back(L"MonsterScript");
	_vec.push_back(L"NPCScript");
	_vec.push_back(L"PlayerAttackSpell");
	_vec.push_back(L"PlayerScript");
	_vec.push_back(L"TestMgrScript");
	_vec.push_back(L"WalkingGravityScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"DialogueBoxScript" == _strScriptName)
		return new CDialogueBoxScript;
	if (L"DialogueScript" == _strScriptName)
		return new CDialogueScript;
	if (L"FireArrow" == _strScriptName)
		return new CFireArrow;
	if (L"FollowCamScript" == _strScriptName)
		return new CFollowCamScript;
	if (L"HotKeyScript" == _strScriptName)
		return new CHotKeyScript;
	if (L"Inventory" == _strScriptName)
		return new CInventory;
	if (L"ItemScript" == _strScriptName)
		return new CItemScript;
	if (L"MonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"NPCScript" == _strScriptName)
		return new CNPCScript;
	if (L"PlayerAttackSpell" == _strScriptName)
		return new CPlayerAttackSpell;
	if (L"PlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"TestMgrScript" == _strScriptName)
		return new CTestMgrScript;
	if (L"WalkingGravityScript" == _strScriptName)
		return new CWalkingGravityScript;
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::DIALOGUEBOXSCRIPT:
		return L"DialogueBoxScript";
		break;

	case SCRIPT_TYPE::DIALOGUESCRIPT:
		return L"DialogueScript";
		break;

	case SCRIPT_TYPE::FIREARROW:
		return L"FireArrow";
		break;

	case SCRIPT_TYPE::FOLLOWCAMSCRIPT:
		return L"FollowCamScript";
		break;

	case SCRIPT_TYPE::HOTKEYSCRIPT:
		return L"HotKeyScript";
		break;

	case SCRIPT_TYPE::INVENTORY:
		return L"Inventory";
		break;

	case SCRIPT_TYPE::ITEMSCRIPT:
		return L"ItemScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"MonsterScript";
		break;

	case SCRIPT_TYPE::NPCSCRIPT:
		return L"NPCScript";
		break;

	case SCRIPT_TYPE::PLAYERATTACKSPELL:
		return L"PlayerAttackSpell";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"PlayerScript";
		break;

	case SCRIPT_TYPE::TESTMGRSCRIPT:
		return L"TestMgrScript";
		break;

	case SCRIPT_TYPE::WALKINGGRAVITYSCRIPT:
		return L"WalkingGravityScript";
		break;

	}
	return nullptr;
}