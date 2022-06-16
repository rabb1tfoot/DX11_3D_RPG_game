#include "stdafx.h"
#include "FollowCamScript.h"
#include <GameObject.h>
#include <SCeneMgr.h>
#include <Scene.h>
#include <Transform.h>
#include <Camera.h>
#include <Script.h>
#include "WalkingGravityScript.h"

CFollowCamScript::CFollowCamScript()
	: CScript((UINT)SCRIPT_TYPE::FOLLOWCAMSCRIPT)
	, m_pPlayer(nullptr)
	, m_fAngle(0.f)
{
	SetName(L"FollowCamScript");
}

CFollowCamScript::~CFollowCamScript()
{
}

void CFollowCamScript::start()
{
	vector<CGameObject*> vecObj = {};
	CSceneMgr::GetInst()->GetCurScene()->FindGameObject(L"Player", vecObj);
	if (nullptr != vecObj[0])
		m_pPlayer = vecObj[0];
	else
		assert(nullptr);

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vplayerPos = m_pPlayer->Transform()->GetLocalPos();

	vPos = Vec3(vplayerPos.x, vplayerPos.y + 1000.f, vplayerPos.z - 1000.f);
	Transform()->SetLocalPos(vPos);
}

void CFollowCamScript::update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vplayerPos = m_pPlayer->Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();
	vPos = Vec3(vplayerPos.x, vplayerPos.y + 1000.f, vplayerPos.z - 1000.f);

	if (KEYHOLD(KEY_TYPE::KEY_RBTN))
	{
		Vec2 vDragDir = CKeyMgr::GetInst()->GetDragDir();

		vRot.y += DT * XM_PI * 0.04f * vDragDir.x;
		vRot.z = 0.f;
		Transform()->SetLocalRot(vRot);

		// 우벡터 회전
		Matrix matAxis = XMMatrixRotationAxis(Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT), DT * XM_PI * 0.1f * -vDragDir.y);
		Transform()->AddLocalRot(matAxis);

		//vector<CScript*> vPScripts = m_pPlayer->GetScripts();
		//int iIdx = -1;
		//for (size_t i = 0; i < vPScripts.size(); ++i)
		//{
		//	if (vPScripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::WALKINGGRAVITYSCRIPT)
		//	{
		//		iIdx = i;
		//		break;
		//	}
		//}
		//if (((CWalkingGravityScript*)(vPScripts[iIdx]))->GetMoveState() == MOVE_STATE::MOVING)
		//{
		//	Vec3 vRot = Transform()->GetLocalRot();
		//	vRot.y += DT * XM_PI * 0.1f * vDragDir.x;
		//
		//	// 우벡터 회전
		//	Matrix matAxis = XMMatrixRotationAxis(Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT), DT * XM_PI * 0.1f * -vDragDir.y);
		//	Transform()->AddLocalRot(matAxis);
		//}
		//
		//else
		//{
		//	//좌로 회전
		//	if (vDragDir.x > 1)
		//	{
		//		Vec3 vDir = vplayerPos - vPos;
		//		float vDirlan = vDir.Length();
		//		m_fAngle += XM_PI / 180.f * vDragDir.x;
		//		vPos.x += cosf(m_fAngle) * vDirlan * DT;
		//		vPos.z += sinf(m_fAngle) * vDirlan * DT;
		//	}
		//	//우로 회전
		//	else if (vDragDir.x < -1)
		//	{
		//		Vec3 vDir = vplayerPos - vPos;
		//		float vDirlan = vDir.Length();
		//		m_fAngle -= XM_PI / 180.f * vDragDir.x;
		//		vPos.x += cosf(m_fAngle) * vDirlan * DT;
		//		vPos.z += sinf(m_fAngle) * vDirlan * DT;
		//	}
		//}
	}

	if (KEYHOLD(KEY_TYPE::KEY_PgUP))
	{
		float FOV = Camera()->GetFOV();
		if(FOV >= XM_PI / 4.f)
			FOV -= XM_PI / 4.f * DT;
		Camera()->SetFOV(FOV);
	}
	if (KEYHOLD(KEY_TYPE::KEY_PgDOWN))
	{
		float FOV = Camera()->GetFOV();
		if (FOV <= XM_PI / 2.f)
			FOV += XM_PI / 4.f * DT;
		Camera()->SetFOV(FOV);
	}

	Transform()->SetLocalPos(vPos);
	//Transform()->SetLocalRot(vRot);
}
