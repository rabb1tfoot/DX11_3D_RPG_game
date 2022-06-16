#include "stdafx.h"
#include "WalkingGravityScript.h"
#include <ResMgr.h>
#include <Texture.h>
#include <Animator3D.h>
#include <Terrain.h>
#include <SceneMgr.h>
#include <Collider3D.h>
#include <Scene.h>

CWalkingGravityScript::CWalkingGravityScript()
	: CScript((UINT)SCRIPT_TYPE::WALKINGGRAVITYSCRIPT)
	, m_fGravity(1000.f)
	, m_pTerrain(nullptr)
{
}


CWalkingGravityScript::~CWalkingGravityScript()
{
}


void CWalkingGravityScript::start()
{
	vector<CGameObject*> _vecOut;
	CSceneMgr::GetInst()->GetCurScene()->FindGameObject(L"Terrain", _vecOut);
	m_pTerrain = _vecOut[0];
}

void CWalkingGravityScript::update()
{
	CGameObject* pObj = Object();
	Vec3 vPos = pObj->Transform()->GetLocalPos();
	vPos.y -= m_fGravity * DT;

	float TerrainHeight = m_pTerrain->Terrain()->GetCollisionHeight(vPos);

	if (TerrainHeight > vPos.y)
		vPos.y = TerrainHeight;

	pObj->Transform()->SetLocalPos(vPos);
}
