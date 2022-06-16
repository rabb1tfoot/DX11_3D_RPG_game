#include "stdafx.h"
#include "tchar.h"
#include "corecrt_wstdio.h"
#include "PlayerAttackSpell.h"
#include "PlayerScript.h"
#include "MonsterScript.h"
#include "FireArrow.h"
#include <KeyMgr.h>
#include <GameObject.h>
#include <Font23.h>
#include <Animator3D.h>

CPlayerAttackSpell::CPlayerAttackSpell()
	: CScript((UINT)SCRIPT_TYPE::PLAYERATTACKSPELL)
	, m_fHillAmount(20.f)
	, m_fMedAmount(10.f)
	, m_SpellEffectHeal(nullptr)
	, m_SpellEffectMed(nullptr)
	, m_pPlayerScript(nullptr)
	, m_pCurScene(nullptr)
{
	for (UINT i = 0; i < (UINT)PLAYER_SKILL::END; ++i)
	{
		m_fSpellCooldownTimeAcc[i] = 0.f;
	}
	//쿨타임 절대수치
	m_fSpellCooldownTime[(UINT)PLAYER_SKILL::Attack] = 1.5f;
	m_fSpellCooldownTime[(UINT)PLAYER_SKILL::FIREARROW] = 3.f;
	m_fSpellCooldownTime[(UINT)PLAYER_SKILL::HEAL] = 5.f;
	m_fSpellCooldownTime[(UINT)PLAYER_SKILL::MEDITATION] = 5.f;
}


CPlayerAttackSpell::~CPlayerAttackSpell()
{
	SAFE_DELETE(m_SpellEffectHeal);
	SAFE_DELETE(m_SpellEffectMed);
}

void CPlayerAttackSpell::start()
{
	m_pCurScene = CSceneMgr::GetInst()->GetCurScene();

	 vector<CScript*> Scripts = Object()->GetScripts();
	 for (size_t i = 0; i < Scripts.size(); ++i)
	 {
		 if (Scripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::PLAYERSCRIPT)
		 {
			 m_pPlayerScript = (CPlayerScript*)Scripts[i];
			 break;
		 }
	 }

	for (UINT i = 0; i < (UINT)PLAYER_SKILL::END; ++i)
	{
		m_SpellCooldownIcon[i] = new CGameObject;

		wchar_t strNum[30];
		wsprintf(strNum, L"SkillUIIcon_%d", i);
		m_SpellCooldownIcon[i]->SetName(strNum);
		m_SpellCooldownIcon[i]->AddComponent(new CTransform);
		m_SpellCooldownIcon[i]->AddComponent(new CMeshRender);
		m_SpellCooldownIcon[i]->Transform()->SetLocalPos(Vec3(200.f + i * 100, -250.f, 100.f));
		m_SpellCooldownIcon[i]->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		m_SpellCooldownIcon[i]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		
		m_pCurScene->AddObject(L"UI", m_SpellCooldownIcon[i]);

		//공격쿨타임은 쿨타임 오브젝트를 만들지 않는다.
		if (0 == i)
		{
			continue;
		}

		m_SpellCooldownFont[i] = new CGameObject;
		wsprintf(strNum, L"SkillUITime_%d", i);
		m_SpellCooldownFont[i]->SetName(strNum);
		m_SpellCooldownFont[i]->AddComponent(new CFont23);
		m_SpellCooldownFont[i]->AddComponent(new CTransform);
		m_pCurScene->AddObject(L"UI", m_SpellCooldownFont[i]);

		m_SpellCooldownFont[i]->Font()->SetText(L"0");
		//m_SpellCooldownFont[i]->Font()->SetFontSize(25);
		m_SpellCooldownFont[i]->Font()->SetOffSet(Vec2(800.f + i * 100,-530.f));
	}
	m_SpellCooldownFont[0] = nullptr;
	int CoolDown = 1;
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\FireArrow_icon.png");
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::FIREARROW]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::FIREARROW]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::FIREARROW]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &CoolDown);


	pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Heal_icon.png");
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::HEAL]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::HEAL]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::HEAL]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &CoolDown);

	pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Med_icon.png");
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::MEDITATION]->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::MEDITATION]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_SpellCooldownIcon[(UINT)PLAYER_SKILL::MEDITATION]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &CoolDown);
}

void CPlayerAttackSpell::update()
{
	CheckSkill();
	UpdateCoolTime();
}

void CPlayerAttackSpell::OnCollision3D(CCollider3D* _pOther)
{
	float halfAnimFrm = 0.f;
	int iMonLayer = m_pCurScene->FindLayer(L"Monster")->GetLayerIdx();
	if (_pOther->Object()->GetLayerIdx() == iMonLayer)
	{
		const vector<CScript*>& vecScripts = _pOther->Object()->GetScripts();
		int ScriptIdx = 0;
		CMonsterScript* pMonsterScript = nullptr;
		for (size_t i = 0; i < vecScripts.size(); ++i)
		{
			if (vecScripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::MONSTERSCRIPT)
			{
				//현재 타깃 몬스터 가져오기
				pMonsterScript = (CMonsterScript*)vecScripts[i];
				break;
			}
		}

		wstring CurAnimName = Animator3D()->GetCurAnimCutClip()->strName;
		if (!wcscmp(CurAnimName.c_str(), L"Attack"))
		{
			halfAnimFrm = Animator3D()->GetCurEndFrm() / 2.f;
			if (halfAnimFrm <= Animator3D()->GetCurFrm() + 0.01f
				&& halfAnimFrm >= Animator3D()->GetCurFrm() - 0.01f)
			{
				pMonsterScript->SubHp(m_pPlayerScript->GetAtt());
				return;
			}
		}
	}
}


void CPlayerAttackSpell::CheckSkill()
{
	wstring animName = L"";

	if (KEYHOLD(KEY_TYPE::KEY_H))
	{
		if (m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::Attack] < 0.01f)
		{
			animName = L"Attack";
			Animator3D()->SetCurAnimCutClip(animName.c_str());
			m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::Attack] = m_fSpellCooldownTime[(UINT)PLAYER_SKILL::Attack];
			m_pPlayerScript->SetMoveState(MOVE_STATE::STOPMOVING);
		}
	}

	if (KEYHOLD(KEY_TYPE::KEY_J))
	{
		if (m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::FIREARROW] < 0.01f)
		{
			animName = L"Magic";
			Animator3D()->SetCurAnimCutClip(animName.c_str());
			m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::FIREARROW] = m_fSpellCooldownTime[(UINT)PLAYER_SKILL::FIREARROW];
			m_pPlayerScript->SetMoveState(MOVE_STATE::STOPMOVING);

			CreateFireArrow();
		}
	}

	if (KEYHOLD(KEY_TYPE::KEY_K))
	{
		if (m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::HEAL] < 0.01f)
		{
			animName = L"Magic";
			Animator3D()->SetCurAnimCutClip(animName.c_str());
			m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::HEAL] = m_fSpellCooldownTime[(UINT)PLAYER_SKILL::HEAL];
			m_pPlayerScript->SetMoveState(MOVE_STATE::STOPMOVING);
			if (m_pPlayerScript->GetMaxHp() - m_fHillAmount < m_pPlayerScript->GetHp())
			{
				m_pPlayerScript->ChangeHp(m_pPlayerScript->GetMaxHp());
			}
			else
			{
				m_pPlayerScript->AddHp((int)m_fHillAmount);
			}

			//이펙트부분 나중에 추가할것
		}
	}

	if (KEYHOLD(KEY_TYPE::KEY_L))
	{
		if (m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::MEDITATION] < 0.01f)
		{
			animName = L"Magic";
			Animator3D()->SetCurAnimCutClip(animName.c_str());
			m_fSpellCooldownTimeAcc[(UINT)PLAYER_SKILL::MEDITATION] = m_fSpellCooldownTime[(UINT)PLAYER_SKILL::MEDITATION];
			m_pPlayerScript->SetMoveState(MOVE_STATE::STOPMOVING);
			if (m_pPlayerScript->GetMaxMp() - m_fMedAmount < m_pPlayerScript->GetMp())
			{
				m_pPlayerScript->ChangeMp(m_pPlayerScript->GetMaxMp());
			}
			else
			{
				m_pPlayerScript->AddMp((int)m_fMedAmount);
			}

			//이펙트부분 나중에 추가할것
		}
	}
	CAnimator3D* pCurAnim = Animator3D();
	float frm = pCurAnim->GetCurFrm();
	float endFrm = Animator3D()->GetCurEndFrm();
	if (!(m_pPlayerScript->GetMoveState() == MOVE_STATE::MOVING)
		&& Animator3D()->GetCurFrm()+0.1f >= Animator3D()->GetCurEndFrm())
	{
		m_pPlayerScript->SetMoveState(MOVE_STATE::READYTOMOVE);
		animName = L"Stand";
		Animator3D()->SetCurAnimCutClip(animName.c_str());
	}
}

void CPlayerAttackSpell::UpdateCoolTime()
{
	for (UINT i = 0; i < (UINT)PLAYER_SKILL::END; ++i)
	{
		if (m_fSpellCooldownTimeAcc[i] > 0.f)
		{
			m_fSpellCooldownTimeAcc[i] -= 1.f * DT;
			if (m_SpellCooldownFont[i] != nullptr)
			{
				wchar_t strTime[10];
				_stprintf_s(strTime, 10, L"%.1f", m_fSpellCooldownTimeAcc[i]);
				m_SpellCooldownFont[i]->Font()->SetText(strTime);
				m_SpellCooldownIcon[(UINT)PLAYER_SKILL::FIREARROW]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::FLOAT_0, &m_fSpellCooldownTimeAcc);

				if (m_fSpellCooldownTimeAcc[i] <= 0.01f)
				{
					float zero = 0.f;
					m_SpellCooldownFont[i]->Font()->SetText(L"");
					m_SpellCooldownIcon[(UINT)PLAYER_SKILL::FIREARROW]->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::FLOAT_0, &zero);
				}
			}
		}
	}
}

void CPlayerAttackSpell::CreateFireArrow()
{
	Vec3 vPlayerPos = Transform()->GetLocalPos();
	Vec3 vPlayerDir = Transform()->GetLocalDir(DIR_TYPE::DIR_FRONT);

	CGameObject* pObject = new CGameObject();
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider3D);
	CFireArrow* pScript = new CFireArrow();
	pObject->AddComponent(pScript);
	pScript->Setting(vPlayerDir, m_pPlayerScript);
	pObject->SetName(L"FireArrow");
	pObject->Transform()->SetLocalPos(vPlayerPos);
	pObject->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pObject->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\Std3DMtrl.mtrl"));

	int iLayer = m_pCurScene->FindLayer(L"Bullet")->GetLayerIdx();
	CreateObject(pObject, iLayer);

}
