#include "stdafx.h"
#include "MonsterScript.h"
#include "tchar.h"
#include <Animator3D.h>
#include "PlayerScript.h"


CMonsterScript::CMonsterScript()
	: CScript((UINT)SCRIPT_TYPE::MONSTERSCRIPT)
	, m_Movetype(MOVETYPE::NORMAL)
	, m_fAccMovetick(0.f)
	, m_fAccShowHpBar(5.f)
	, m_fAccAttacktick(3.f)
	, m_Hitted(false)
	, m_bDead(false)
	, m_bTouch(false)
{
}


CMonsterScript::~CMonsterScript()
{
}


void CMonsterScript::start()
{
	srand((UINT)time(NULL));
	vector<CGameObject*> vecObj = {};
	CSceneMgr::GetInst()->GetCurScene()->FindGameObject(L"Player", vecObj);
	if (nullptr != vecObj[0])
		m_pTarget = vecObj[0];

	vector<CScript*> Script = m_pTarget->GetScripts();
	for (size_t i = 0; i < Script.size(); ++i)
	{
		if (Script[i]->GetScriptType() == (UINT)SCRIPT_TYPE::PLAYERSCRIPT)
		{
			m_pTargetScript = (CPlayerScript*)Script[i];
			break;
		}
	}

	//몬스터 hp정보 UI
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	m_UIMonsterHp = new CGameObject;
	int ShaderSwitch = 2;

	m_UIMonsterHp->SetName(L"UI_MonsterHp");
	m_UIMonsterHp->AddComponent(new CTransform);
	m_UIMonsterHp->AddComponent(new CMeshRender);
	m_UIMonsterHp->Transform()->SetLocalPos(Vec3(-350.f, 300.f, 100.f));
	m_UIMonsterHp->Transform()->SetLocalScale(Vec3(200.f, 30.f, 100.f));
	m_UIMonsterHp->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Hp_bar.png");
	m_UIMonsterHp->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_UIMonsterHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_UIMonsterHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &ShaderSwitch);
	pCurScene->AddObject(L"UI", m_UIMonsterHp);
	SetDisable(m_UIMonsterHp);
	

	m_UIMonsterHpFont = new CGameObject;
	m_UIMonsterHpFont->SetName(L"UI_MonsterHpFont");
	m_UIMonsterHpFont->AddComponent(new CTransform);
	m_UIMonsterHpFont->AddComponent(new CFont23);
	m_UIMonsterHpFont->Font()->SetOffSet(Vec2(160.f, -25.f));
	pCurScene->AddObject(L"UI", m_UIMonsterHpFont);
	SetDisable(m_UIMonsterHpFont);

	Animator3D()->SetCurAnimCutClip(L"Stand");
}

void CMonsterScript::update()
{
	CheckDistance();

	if (m_Movetype == MOVETYPE::TRACKING)
		TrackingMove();
	else if (m_Movetype == MOVETYPE::NORMAL)
		NormalMove();

	CheckShowHp();
	CheckDead();
}

void CMonsterScript::OnCollision3D(CCollider3D * _pOther)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	int iIdx = pCurScene->FindLayer(L"Player")->GetLayerIdx();

	if(_pOther->Object()->GetLayerIdx() == iIdx)
		m_bTouch = true;
}

void CMonsterScript::OnCollisionExit3D(CCollider3D * _pOther)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	int iIdx = pCurScene->FindLayer(L"Player")->GetLayerIdx();

	if (_pOther->Object()->GetLayerIdx() == iIdx)
		m_bTouch = false;
}


void CMonsterScript::SetState(int _hp, int _mp, int _Att, int _Mpp)
{
	m_iHp = _hp;
	m_ibeforeHp = _hp;
	m_iMaxHp = _hp;
	m_iMp = _mp;
	m_ibeforeMp = _mp;
	m_iMaxMp = _mp;
	m_iAttPoint = _Att;
	m_iMpPoint = _Mpp;
}

void CMonsterScript::SubHp(int _amount)
{
	//이전 몬스터의 UI는 보이지 않는다.
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	vector<CGameObject*> vObj = pCurScene->FindLayer(L"Monster")->GetParentObject();
	CMonsterScript* pMonScript = nullptr;

	for (size_t i = 0; i < vObj.size(); ++i)
	{
		vector<CScript*> vScripts = vObj[i]->GetScripts();
		for (size_t j = 0; j < vScripts.size(); ++j)
		{
			if (vScripts[j]->GetScriptType() == (UINT)SCRIPT_TYPE::MONSTERSCRIPT)
			{
				pMonScript = (CMonsterScript*)vScripts[j];
				pMonScript->SetUnHitted();
				break;
			}
		}
	}

	m_iHp -= _amount;
	m_Hitted = true;
	m_fAccShowHpBar = 5.f;

	wchar_t textcurHp[20];
	wchar_t textMaxHp[20];
	wstring textHp = L"MonsterHp : ";
	_stprintf_s(textcurHp, 20, L"%d", m_iHp);
	_stprintf_s(textMaxHp, 20, L"%d", m_iMaxHp);
	float HpRaito = (float)m_iHp / (float)m_iMaxHp;
	textHp += textcurHp;
	textHp += L" / ";
	textHp += textMaxHp;
	if (m_iHp != m_ibeforeHp)
	{
		m_ibeforeHp = m_iHp;
		m_UIMonsterHpFont->Font()->SetText(textHp.c_str());
		m_UIMonsterHp->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::FLOAT_0, &HpRaito);
	}
}

void CMonsterScript::CheckDistance()
{
	Vec3 vTargetPos = m_pTarget->Transform()->GetLocalPos();
	Vec3 vMonPos = Transform()->GetLocalPos();

	float fDistance = sqrt(pow((vTargetPos.x - vMonPos.x), 2) + pow((vTargetPos.z - vMonPos.z), 2));

	if (fDistance < 1000.f)
		m_Movetype = MOVETYPE::TRACKING;
	else
		m_Movetype = MOVETYPE::NORMAL;
}

void CMonsterScript::CheckShowHp()
{
	if (m_Hitted)
	{
		m_fAccShowHpBar -= 1.f *DT;
		SetEnable(m_UIMonsterHp);
		SetEnable(m_UIMonsterHpFont);
		//시간이 지나면 보이지않는다.
		if (m_fAccShowHpBar <= 0.f)
		{
			SetDisable(m_UIMonsterHp);
			SetDisable(m_UIMonsterHpFont);
			m_Hitted = false;
		}
	}
}

void CMonsterScript::TrackingMove()
{
	if (!m_bTouch)
	{
		if (wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Moving"))
			Animator3D()->SetCurAnimCutClip(L"Moving");

		Vec3 vTargetPos = m_pTarget->Transform()->GetLocalPos();
		Vec3 vMonPos = Transform()->GetLocalPos();
		Vec3 vMonRot = Transform()->GetLocalRot();

		Vec3 movePos = Vec3(vTargetPos.x - vMonPos.x, 0, vTargetPos.z - vMonPos.z);
		movePos.Normalize();
		Transform()->SetLookAt(movePos);
		vMonRot.x = (XM_PI /2.f)*3.f; vMonRot.z = 0.f;
		vMonRot.y = Transform()->GetLocalRot().y - XM_PI /2.f;
		vMonPos += Vec3(movePos) * 300 * DT;

		Transform()->SetLocalPos(vMonPos);
		Transform()->SetLocalRot(vMonRot);
	}
	if (m_bTouch)
	{
		m_fAccAttacktick -= DT;

		if (m_fAccAttacktick < 0.f)
		{
			m_fAccAttacktick = 3.f;
			if (wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Attack"))
				Animator3D()->SetCurAnimCutClip(L"Attack");

		}
		float fEndFrm = Animator3D()->GetCurEndFrm();
		float fCurFrm = Animator3D()->GetCurFrm();

		//애니메이션이 끝나면 데미지피격후 Stand애니메이션 으로 바꾼다
		if (fEndFrm - 0.1f <= fCurFrm && 
			!wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Attack"))
		{
			Animator3D()->SetCurAnimCutClip(L"Stand");
			m_pTargetScript->SubHp(GetAtt());
		}
	}
}

void CMonsterScript::NormalMove()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	if (m_fAccMovetick < 0.f)
	{
		m_fAccMovetick = 10.f;
		m_moveCommand = rand() % 4 + 1;
	}
	else
	{
		m_fAccMovetick -= 1 * DT;
	}

	if (m_fAccMovetick > 5.f)
	{
		if(wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Moving"))
			Animator3D()->SetCurAnimCutClip(L"Moving");
		switch (m_moveCommand)
		{
		case 1:
			vPos.x += 300.f * DT;
			vRot.y = 0.f;
			break;
		case 2:
			vPos.x -= 300.f * DT;
			vRot.y = XM_PI;
			break;
		case 3:
			vPos.z += 300.f * DT;
			vRot.y  = (XM_PI / 2) * 3.f;
			break;
		case 4:
			vPos.z -= 300.f * DT;
			vRot.y = (XM_PI / 2);
			break;
		}


	}

	if (m_fAccMovetick < 5.f)
	{
		if (wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Stand"))
			Animator3D()->SetCurAnimCutClip(L"Stand");
	}

	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);
}

void CMonsterScript::CheckDead()
{
	if (GetHp() <= 0.f)
	{
		if (wcscmp(Animator3D()->GetCurAnimCutClip()->strName.c_str(), L"Dead"))
			Animator3D()->SetCurAnimCutClip(L"Dead");

		float fEndFrm = Animator3D()->GetCurEndFrm();
		float fCurFrm = Animator3D()->GetCurFrm();

		//데드 애니메이션이 끝나면 사망처리한다.
		if (fEndFrm - 0.1f <= fCurFrm && !m_bDead)
		{
			m_bDead = true;
			DeleteObject(Object());
			DeleteObject(m_UIMonsterHp);
			DeleteObject(m_UIMonsterHpFont);
			Object()->OnDisable();
			m_UIMonsterHp->OnDisable();
			m_UIMonsterHpFont->OnDisable();
		}
	}
}

