#include "stdafx.h"
#include "DialogueScript.h"
#include <PathMgr.h>

CDialogueScript::CDialogueScript()
	:CScript((UINT)SCRIPT_TYPE::DIALOGUESCRIPT)
	, m_bIsOn(false)
	, m_bIsWriting(false)
	, m_bEnterText(false)
	, m_iTextIdx(0)
	, m_iSentenceIdx(0)
	, m_fTextSpeed(0.f)
	, m_FirstFont(Vec2(150.f, -375.f))
{
}


CDialogueScript::~CDialogueScript()
{
}

void CDialogueScript::LoadingText(const wstring & _FileName)
{
	m_vecText.clear();

	wstring Path = CPathMgr::GetResPath();
	Path += L"Dialogue\\";
	Path += _FileName;

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, Path.c_str(), L"r");
	wstring wTextString;
	char ctextbuffer[1000] = {};
	wchar_t wText[1000] = {};
	if (pFile == nullptr)
		return;

	while (!feof(pFile))
	{
		fscanf(pFile, "%s", ctextbuffer);
		memset(wText, 0, 1000);
		MultiByteToWideChar(0, 0, ctextbuffer, 1000, wText, 1000);
		wTextString += wText;
		for (int i = 0; i < 1000; ++i)
		{
			//줄바꿈
			if (ctextbuffer[i] == '.')
			{
				m_vecText.push_back(wTextString);
				wTextString.clear();
				break;
			}
			else if (ctextbuffer[i] == '\0')
			{
				wTextString += L" ";
				break;
			}
		}
	}

	fclose(pFile);
}

void CDialogueScript::start()
{
	//다이얼로그 백그라운드
	m_pBackground = new CGameObject();
	m_pBackground->SetName(L"Dialogue_Background");
	m_pBackground->AddComponent(new CTransform);
	m_pBackground->AddComponent(new CMeshRender);
	m_pBackground->Transform()->SetLocalPos(Vec3(0.f, -100.f, 0.f));
	m_pBackground->Transform()->SetLocalScale(Vec3(900.f, 200.f, 0.f));
	m_pBackground->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Background.png");
	m_pBackground->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_pBackground->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->AddObject(L"UI", m_pBackground);
	SetDisable(m_pBackground);

	m_pTextObject = new CGameObject;
	m_pTextObject->SetName(L"Dialogue_Text");
	m_pTextObject->AddComponent(new CTransform);
	m_pTextObject->AddComponent(new CFont23);
	m_pTextObject->Font()->SetOffSet(Vec2(150.f, -375.f));
	m_pTextObject->Font()->SetFontStyle(L"Dialogue");
	m_pTextObject->Font()->SetText(L".");
	pCurScene->AddObject(L"UI", m_pTextObject);
	SetDisable(m_pTextObject);
}

void CDialogueScript::update()
{
	if (m_bIsOn)
	{
		//대충 캐릭터 못움직이게 하는 코드추가해야됨
		if (!m_pBackground->IsActive() && !m_pTextObject->IsActive())
		{
			SetEnable(m_pBackground);
			SetEnable(m_pTextObject);
		}

		Writing();

		if (KEYTAB(KEY_TYPE::KEY_ENTER))
		{
			if (m_bIsWriting)
			{
				//한 번에 다 출력
				m_iTextIdx = m_vecText[m_iSentenceIdx].size() -1;
			}
			else
			{
				//다음문장으로 넘어감
				m_iTextIdx = 0;
				++m_iSentenceIdx;

				//모든 문장 종료시
				if (m_vecText.size() <= m_iSentenceIdx)
				{
					m_bIsOn = false;
					SetDisable(m_pTextObject);
					SetDisable(m_pBackground);
				}
			}
		}
	}
}


void CDialogueScript::Writing()
{
	if (m_iTextIdx == m_vecText[m_iSentenceIdx].size())
	{
		m_bIsWriting = false;
		return;
	}
	else
		m_bIsWriting = true;

	if (m_iTextIdx % 50 == 0 && m_iTextIdx != 0 && !m_bEnterText)
	{
		int iEnterNum = 0;
		for (int i = 1; i < 4; ++i)
		{
			if (m_iTextIdx / 50 == i)
			{
				iEnterNum = 50 * i;
			}
		}
		m_vecText[m_iSentenceIdx].insert(iEnterNum, L"\n");
		m_bEnterText = true;
	}

	m_fTextSpeed += DT * 10.f;

	if (m_fTextSpeed > 1.f)
	{
		wstring localStr = m_vecText[m_iSentenceIdx].substr(0, m_iTextIdx);
		m_pTextObject->Font()->SetText(localStr);
		Vec2 vPos = m_pTextObject->Font()->GetOffSet();

		m_fTextSpeed = 0;
		++m_iTextIdx;
		vPos.x = m_FirstFont.x + localStr.size() * 10;
		m_bEnterText = false;

		m_pTextObject->Font()->SetOffSet(vPos);
	}


}