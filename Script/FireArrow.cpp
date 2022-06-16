#include "stdafx.h"
#include "FireArrow.h"
#include <Collider3D.h>
#include <Scene.h>
#include <SCeneMgr.h>

#include "PlayerScript.h"
#include "MonsterScript.h"


CFireArrow::CFireArrow()
	: CScript((UINT)SCRIPT_TYPE::FIREARROW)
	, fLiveTime(3.f)
	, bDead(false)
{
}


CFireArrow::~CFireArrow()
{
}

void CFireArrow::start()
{
}

void CFireArrow::update()
{
	UpdateMove();
	DeadTest();
}

void CFireArrow::OnCollision3D(CCollider3D* _pOther)
{
	int iColliderIdx = _pOther->Object()->GetLayerIdx();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	int iMonIdx = pCurScene->FindLayer(L"Monster")->GetLayerIdx();
	if (iColliderIdx == iMonIdx)
	{
		vector<CScript*> scripts = _pOther->Object()->GetScripts();
		CMonsterScript*  pMonScript = nullptr;
		for (size_t i = 0; i < scripts.size(); ++i)
		{
			if (scripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::MONSTERSCRIPT)
			{
				pMonScript = (CMonsterScript*)scripts[i];
				break;
			}
		}
		pMonScript->SubHp(m_pPlayerScript->GetMpp());
		DeleteObject(Object());

		//이펙트 부분 넣기
	}
}

void CFireArrow::UpdateMove()
{
	Vec3 pPos = Transform()->GetLocalPos();
	Vec3 pDir = Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT);
	pPos -= DT * 5000.f * pDir;

	Transform()->SetLocalPos(pPos);
}

void CFireArrow::DeadTest()
{
	if (!bDead)
	{
		fLiveTime -= DT;
		if (fLiveTime < 0.f)
		{
			DeleteObject(Object());
			bDead = true;
		}
	}
}

void CFireArrow::Setting(Vec3 _Dir, CPlayerScript* _script)
{
	Transform()->SetLookAt(_Dir);

	m_pPlayerScript = _script;
}
