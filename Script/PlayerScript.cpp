#include "stdafx.h"
#include "PlayerScript.h"
#include <sound.h>
#include <ResMgr.h>
#include <Font23.h>
#include <Animator3D.h>
#include "tchar.h"
#include "corecrt_wstdio.h"
#include "TestMgrScript.h"
#include "DialogueBoxScript.h"
#include "Inventory.h"

CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_iHp(50)
	, m_ibeforeHp(0)
	, m_iMaxHp(100)
	, m_iMp(10)
	, m_ibeforeMp(0)
	, m_iMaxMp(50)
	, m_iAttPoint(10)
	, m_iMpPoint(20)
	, m_UIHp(nullptr)
	, m_UIMp(nullptr)
	, m_UIHpFont(nullptr)
	, m_UIMpFont(nullptr)
	, m_UIMonseterUI(nullptr)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::start()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	int ShaderSwitch = 2;
	m_UIHp = new CGameObject;
	m_UIHp->SetName(L"UI_hp");
	m_UIHp->AddComponent(new CTransform);
	m_UIHp->AddComponent(new CMeshRender);
	m_UIHp->Transform()->SetLocalPos(Vec3(-470.f, -250.f, 100.f));
	m_UIHp->Transform()->SetLocalScale(Vec3(200.f, 30.f, 100.f));
	m_UIHp->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Hp_bar.png");
	m_UIHp->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_UIHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_UIHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &ShaderSwitch);
	pCurScene->AddObject(L"UI", m_UIHp);

	m_UIMp = new CGameObject;
	m_UIMp->SetName(L"UI_Mp");
	m_UIMp->AddComponent(new CTransform);
	m_UIMp->AddComponent(new CMeshRender);
	m_UIMp->Transform()->SetLocalPos(Vec3(-470.f, -300.f, 100.f));
	m_UIMp->Transform()->SetLocalScale(Vec3(200.f, 30.f, 100.f));
	m_UIMp->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Mp_bar.png");
	m_UIMp->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_UIMp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_UIMp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &ShaderSwitch);
	pCurScene->AddObject(L"UI", m_UIMp);

	m_UIHpFont = new CGameObject;
	m_UIHpFont->SetName(L"UI_hpFont");
	m_UIHpFont->AddComponent(new CTransform);
	m_UIHpFont->AddComponent(new CFont23);
	m_UIHpFont->Font()->SetOffSet(Vec2(100.f, -560.f));
	pCurScene->AddObject(L"UI", m_UIHpFont);

	m_UIMpFont = new CGameObject;
	m_UIMpFont->SetName(L"UI_mpFont");
	m_UIMpFont->AddComponent(new CTransform);
	m_UIMpFont->AddComponent(new CFont23);
	m_UIMpFont->Font()->SetOffSet(Vec2(100.f, -610.f));
	pCurScene->AddObject(L"UI", m_UIMpFont);

	Vec3 vRot = Transform()->GetLocalRot();
	vRot.y = (XM_PI / 2) * 3.f;
	Transform()->SetLocalRot(vRot);
}

void CPlayerScript::update()
{
	CalCurState();

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	if (MOVE_STATE::STOPMOVING != m_eState)
		UpdateMoveDir(vPos, vRot);

	//테스트용 완성시 지울것
	DialogueBoxTest();
	CheckInventory();
}

void CPlayerScript::CalCurState()
{
	wchar_t textcurHp[20];
	wchar_t textMaxHp[20];
	wstring textHp = L"";
	_stprintf_s(textcurHp, 20, L"%d", m_iHp);
	_stprintf_s(textMaxHp, 20, L"%d", m_iMaxHp);
	float HpRaito = (float)m_iHp / (float)m_iMaxHp;
	textHp += textcurHp;
	textHp += L" / ";
	textHp += textMaxHp;
	if (m_iHp != m_ibeforeHp)
	{
		m_ibeforeHp = m_iHp;
		m_UIHpFont->Font()->SetText(textHp.c_str());
		m_UIHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::FLOAT_0, &HpRaito);
	}

	wchar_t textcurMp[20];
	wchar_t textMaxMp[20];
	wstring textMp = L"";
	_stprintf_s(textcurMp, 20, L"%d", m_iMp);
	_stprintf_s(textMaxMp, 20, L"%d", m_iMaxMp);
	float MpRaito = (float)m_iMp / (float)m_iMaxMp;
	textMp += textcurMp;
	textMp += L" / ";
	textMp += textMaxMp;
	if (m_iMp != m_ibeforeMp)
	{
		m_ibeforeMp = m_iMp;
		m_UIMpFont->Font()->SetText(textMp.c_str());
		m_UIMp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::FLOAT_0, &MpRaito);
	}
}

void CPlayerScript::UpdateMoveDir(Vec3 _vPos, Vec3 _vRot)
{
	wstring Animstr = L"Moving";
	wstring CurAnimName = Animator3D()->GetCurAnimCutClip()->strName;

	if (wcscmp(CurAnimName.c_str(), Animstr.c_str()) && m_eState == MOVE_STATE::MOVING)
		Animator3D()->SetCurAnimCutClip(Animstr.c_str());

	if (KEYHOLD(KEY_TYPE::KEY_W))
	{
		_vPos += Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT) * 400.f * DT;
		m_eState = MOVE_STATE::MOVING;
	}
	if (KEYHOLD(KEY_TYPE::KEY_S))
	{
		_vPos -= Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT) * 400.f * DT;
		m_eState = MOVE_STATE::MOVING;
	}
	if (KEYHOLD(KEY_TYPE::KEY_A))
	{
		_vPos -= Transform()->GetLocalDir(DIR_TYPE::DIR_UP) * 400.f * DT;
		m_eState = MOVE_STATE::MOVING;
	}
	if (KEYHOLD(KEY_TYPE::KEY_D))
	{
		_vPos += Transform()->GetLocalDir(DIR_TYPE::DIR_UP) * 400.f * DT;
		m_eState = MOVE_STATE::MOVING;
	}

	if (KEYHOLD(KEY_TYPE::KEY_RBTN) && m_eState == MOVE_STATE::MOVING)
	{
		Vec2 vDragDir = CKeyMgr::GetInst()->GetDragDir();

		_vRot.y += DT * XM_PI * 0.5f* vDragDir.x;
		_vRot.z = 0.f;
	}

	if (KEYAWAY(KEY_TYPE::KEY_W) || KEYAWAY(KEY_TYPE::KEY_A) || KEYAWAY(KEY_TYPE::KEY_S) ||
		KEYAWAY(KEY_TYPE::KEY_D))
	{
		m_eState = MOVE_STATE::READYTOMOVE;
		Animator3D()->SetCurAnimCutClip(L"Stand");
	}
	Transform()->SetLocalPos(_vPos);
	Transform()->SetLocalRot(_vRot);
}

void CPlayerScript::DialogueBoxTest()
{
	//if (KEYTAB(KEY_TYPE::KEY_A))
	//{
	//	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	//	vector<CGameObject*> vObj = pCurScene->FindLayer(L"Default")->GetParentObject();
	//	CDialogueBoxScript* pBoxScript = nullptr;
	//	for (size_t i = 0; i < vObj.size(); ++i)
	//	{
	//		if (!wcscmp(vObj[i]->GetName().c_str(), L"DialogueBox"))
	//		{
	//			vector<CScript*> Scripts = vObj[i]->GetScripts();
	//			for (size_t j = 0; j < Scripts.size(); ++j)
	//			{
	//				if ((UINT)SCRIPT_TYPE::DIALOGUEBOXSCRIPT == Scripts[j]->GetScriptType())
	//				{
	//					pBoxScript = (CDialogueBoxScript*)Scripts[j];
	//					break;
	//				}
	//
	//			}
	//		}
	//	}
	//	if (pBoxScript->GetAnswer() == UINT(2))
	//	{
	//		pBoxScript->SetBoxText(L"에에에에에에");
	//		pBoxScript->DialogueOn();
	//	}
	//}

}

void CPlayerScript::CheckInventory()
{
	if (KEYTAB(KEY_TYPE::KEY_I))
	{
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		vector<CGameObject*> vObj = pCurScene->FindLayer(L"Default")->GetParentObject();
		CInventory* pInvenScript = nullptr;
		for (size_t i = 0; i < vObj.size(); ++i)
		{
			if (!wcscmp(vObj[i]->GetName().c_str(), L"Inventory"))
			{
				vector<CScript*> Scripts = vObj[i]->GetScripts();
				for (size_t j = 0; j < Scripts.size(); ++j)
				{
					if ((UINT)SCRIPT_TYPE::INVENTORY == Scripts[j]->GetScriptType())
					{
						pInvenScript = (CInventory*)Scripts[j];
						break;
					}
		
				}
			}
		}

		if (!pInvenScript->GetEnable())
		{
			pInvenScript->InventoryOn();
		}
		else
		{
			pInvenScript->InventoryOff();
		}
	}
}

