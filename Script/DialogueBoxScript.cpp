#include "stdafx.h"
#include "DialogueBoxScript.h"
#include <Font23.h>


CDialogueBoxScript::CDialogueBoxScript()
	: CScript((UINT)SCRIPT_TYPE::DIALOGUEBOXSCRIPT)
	, m_eAnswer(EANSWER::NONE)
{
}


CDialogueBoxScript::~CDialogueBoxScript()
{
}

void CDialogueBoxScript::start()
{	
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	m_pInfo = new CGameObject;
	m_pInfo->SetName(L"UI1");
	m_pInfo->AddComponent(new CTransform);
	m_pInfo->AddComponent(new CFont23);
	m_pInfo->Transform()->SetLocalScale(Vec3(410.f, 200.f, 0.f));
	m_pInfo->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	m_pInfo->AddComponent(new CMeshRender);
	m_pInfo->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Dialogue.png");
	m_pInfo->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_pInfo->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_pInfo->Font()->SetOffSet(Vec2(370.f, -300.f));
	m_pInfo->Font()->SetText(L"아");
	pCurScene->AddObject(L"UI", m_pInfo);

	m_pYesOrNoObj[0] = new CGameObject;
	m_pYesOrNoObj[0]->SetName(L"Font1");
	m_pYesOrNoObj[0]->AddComponent(new CTransform);
	m_pYesOrNoObj[0]->AddComponent(new CFont23);
	m_pYesOrNoObj[0]->Font()->SetOffSet(Vec2(470.f, -400.f));
	m_pYesOrNoObj[0]->Font()->SetText(L"네");
	pCurScene->AddObject(L"UI", m_pYesOrNoObj[0]);

	m_pYesOrNoObj[1] = new CGameObject;
	m_pYesOrNoObj[1]->SetName(L"Font2");
	m_pYesOrNoObj[1]->AddComponent(new CTransform);
	m_pYesOrNoObj[1]->AddComponent(new CFont23);
	m_pYesOrNoObj[1]->Font()->SetOffSet(Vec2(625.f, -400.f));
	m_pYesOrNoObj[1]->Font()->SetText(L"아니오");
	pCurScene->AddObject(L"UI", m_pYesOrNoObj[1]);

	m_pInfo->OnDisable();
	m_pYesOrNoObj[0]->OnDisable();
	m_pYesOrNoObj[1]->OnDisable();
}

void CDialogueBoxScript::DialogueOn()
{
	m_pInfo->OnEnable();
	m_pYesOrNoObj[0]->OnEnable();
	m_pYesOrNoObj[1]->OnEnable();
}

void CDialogueBoxScript::SetBoxText(wstring _text)
{
	m_pInfo->Font()->SetText(_text);
	int iSize = _text.size();
	Vec2 vCurPos = m_pInfo->Font()->GetOffSet();
	vCurPos.x = 370.f + iSize * 7;
	m_pInfo->Font()->SetOffSet(vCurPos);

	m_eAnswer = EANSWER::NONE;

}

UINT CDialogueBoxScript::GetAnswer()
{
	//받는 부분에서 NONE이 리턴되는경우 받는 쪽에서 처리해야된다.
	// 호출타이밍도 호출하는 쪽에서 신경써야된다.
	//(NONE이 리턴되면 애초에 호출조차 되지 않았다.)
	return (UINT)m_eAnswer;
}

void CDialogueBoxScript::DialogueOff()
{
	m_pInfo->OnDisable();
	m_pYesOrNoObj[0]->OnDisable();
	m_pYesOrNoObj[1]->OnDisable();
}

void CDialogueBoxScript::update()
{
	if (KEYTAB(KEY_TYPE::KEY_LBTN))
	{
		POINT CurmPos = CKeyMgr::GetInst()->GetMousePos();

		//네 범위
		if (CurmPos.x >= 440.f && CurmPos.x <= 500.f
			&& CurmPos.y >= 387.f && CurmPos.y <= 402.f)
		{
			m_eAnswer = EANSWER::YES;
		}

		//아니오 범위
		if (CurmPos.x >= 600.f && CurmPos.x <= 661.f
			&& CurmPos.y >= 387.f && CurmPos.y <= 402.f)
		{
			m_eAnswer = EANSWER::NO;
		}
		DialogueOff();
	}
}
