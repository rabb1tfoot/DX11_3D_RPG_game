#include "stdafx.h"
#include "ItemScript.h"
#include "PlayerScript.h"


CItemScript::CItemScript()
	:CScript((UINT)SCRIPT_TYPE::ITEMSCRIPT)
	, m_bDrag(false)
	, m_iNum(1)
	, m_fClickTime(0.f)
	, m_vBackgroundSize(Vec2(384.f, 936))
	, m_vIconSize(Vec2(24.f, 24.f))
	, m_pPlayer(nullptr)
{
}


CItemScript::~CItemScript()
{
}


void CItemScript::start()
{
}

void CItemScript::update()
{
	if (m_bDrag)
	{
		m_fClickTime += DT;
	}

	if (m_iNum <= 0)
	{
		//삭제
		DeleteObject(Object());
	}
}

void CItemScript::OnCollisionClick()
{
	if (m_bDrag)
	{
		m_bDrag = false;
		//1초안에 한 경우 사용
		if (m_fClickTime < 1.f)
		{
			UseItem();
		}
		m_fClickTime = 0;
	}
	else
	{
		m_bDrag = true;

		//인벤토리 스크립트를 통해서 위치를 바꾼다.
		//단축키설정을 한다.
	}
}

void CItemScript::SetItemInfo(wstring& _name, wstring& _info, Vec2 _iIdx, int _count, int _location, int _iNum)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	m_iIconIdx = _iIdx;
	Info = _info;
	m_TexName = _name;
	m_iNum = _iNum;
	m_iItemEffectCount = _count;
	CalIconIdx();
	//아이콘 작성
	int iswitch = 3;
	m_pIconObj = new CGameObject();
	m_pIconObj->AddComponent(new CTransform);
	m_pIconObj->AddComponent(new CMeshRender);
	m_pIconObj->AddComponent(new CFont23);
	m_pIconObj->AddComponent(new CCollider2D);
	Vec3 vPos = Vec3(-202.f + int(_location % 10) * 44.4f, 207.f + int(_location / 10.f) * 42.2f, 0.f);
	m_pIconObj->Transform()->SetLocalPos(vPos);
	m_pIconObj->Transform()->SetLocalScale(Vec3(42.f, 38.f, 0.f));
	m_pIconObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pIconObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\ItemSet.png");
	m_pIconObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &iswitch);
	m_pIconObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	m_pIconObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC2_0, &m_vecMtrlPosRaito);
	m_pIconObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC2_1, &m_vecMtrlSizeRaito);
	pCurScene->AddObject(L"UI", m_pIconObj);
	vector<CGameObject*> vObj = pCurScene->FindLayer(L"Player")->GetParentObject();
	vector<CScript*> vScripts = vObj[0]->GetScripts();
	for (int i = 0; i < vScripts.size(); ++i)
	{
		if (vScripts[i]->GetScriptType() == (UINT)SCRIPT_TYPE::PLAYERSCRIPT)
		{
			m_pPlayer = (CPlayerScript*)vScripts[i];
			break;
		}

	}
	wchar_t strNum[20] = {};
	wsprintf(strNum, L"%d", m_iNum);
	m_pIconObj->Font()->SetText(strNum);
	m_pIconObj->Font()->SetOffSet(Vec2(550.f, 107.f));
	m_pIconObj->OnDisable();
}

void CItemScript::ChangeItemNum(int _num)
{
	m_iNum = _num;
	wchar_t strNum[20] = {};
	wsprintf(strNum, L"%d", m_iNum);

	Object()->Font()->SetText(strNum);
}

void CItemScript::AddItemNum(int _num)
{
	m_iNum += _num;
	wchar_t strNum[20] = {};
	wsprintf(strNum, L"%d", m_iNum);

	Object()->Font()->SetText(strNum);
}

void CItemScript::SubItemNum(int _num)
{
	m_iNum -= _num;
	wchar_t strNum[20] = {};
	wsprintf(strNum, L"%d", m_iNum);

	Object()->Font()->SetText(strNum);
}

void CItemScript::UseItem()
{
	switch (m_eType)
	{
	case ITEM_TYPE::ITEM_HPPORTION:
		UseHpPortion();
		break;
	case ITEM_TYPE::ITEM_MPPORTION:
		UseMpPortion();
		break;
	case ITEM_TYPE::ITEM_SWORD:
		UseSword();
		break;
	case ITEM_TYPE::ITEM_SHIELD:
		UseShield();
		break;
	}
}

void CItemScript::OnDisable()
{
	m_pIconObj->OnDisable();
}

void CItemScript::OnEnable()
{
	m_pIconObj->OnEnable();
}

void CItemScript::UseHpPortion()
{
	SubItemNum(1);
	m_pPlayer->AddHp(m_iItemEffectCount);
}

void CItemScript::UseMpPortion()
{
	SubItemNum(1);
	m_pPlayer->AddMp(m_iItemEffectCount);
}

void CItemScript::UseSword()
{
	SubItemNum(1);
	//장비스크립트에서 상호작용해야함
}

void CItemScript::UseShield()
{
	SubItemNum(1);
	//장비스크립트에서 상호작용해야함
}

void CItemScript::CalIconIdx()
{
	m_vecMtrlPosRaito = Vec2(m_iIconIdx.x * m_vIconSize.x, m_iIconIdx.y * m_vIconSize.y);
	m_vecMtrlPosRaito.x /= m_vBackgroundSize.x;
	m_vecMtrlPosRaito.y /= m_vBackgroundSize.y;

	m_vecMtrlSizeRaito.x = m_vIconSize.x / m_vBackgroundSize.x;
	m_vecMtrlSizeRaito.y = m_vIconSize.y / m_vBackgroundSize.y;
}
