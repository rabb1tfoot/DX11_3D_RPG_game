#include "stdafx.h"
#include "Inventory.h"

#include "ItemScript.h"
#include <GameObject.h>

CInventory::CInventory()
	: CScript((UINT)SCRIPT_TYPE::INVENTORY)
	, m_arrItem{nullptr, }
	, m_bEnable(false)
{
}


CInventory::~CInventory()
{
}

void CInventory::InventoryOn()
{
	m_pBackTexture->OnEnable();
	m_bEnable = true;
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (m_arrItem[i] == nullptr)
			continue;
		m_arrItem[i]->OnEnable();
	}
}

void CInventory::InventoryOff()
{
	m_pBackTexture->OnDisable();
	m_bEnable = false;
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (m_arrItem[i] == nullptr)
			continue;
		m_arrItem[i]->OnDisable();
	}
}

void CInventory::AddItem(wstring _name, int _num)
{
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (m_arrItem[i] == nullptr)
		{
			continue;
		}

		//���� �̸��� ������� ������ �߰��Ѵ�.
		vector<CScript*> scripts = m_arrItem[i]->GetScripts();
		for (size_t j = 0; j < scripts.size(); ++j)
		{
			if (scripts[j]->GetScriptType() == (UINT)SCRIPT_TYPE::ITEMSCRIPT)
			{
				if (!wcscmp(((CItemScript*)scripts[j])->GetName().c_str(), _name.c_str()))
				{
					((CItemScript*)scripts[j])->AddItemNum(_num);
					break;
				}
			}
		}
	}
	//�ش� �������� ���°�� ������ ����
	for (int i = 0; i < MAX_INVENTORY; ++i)
	{
		if (m_arrItem[i] == nullptr)
		{
			m_arrItem[i] = CraeteItem(_name, _num, i);
			break;
		}
	}
}

CGameObject* CInventory::CraeteItem(wstring _name, int _num, int _location)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	CGameObject* pItem = new CGameObject;
	pItem->AddComponent(new CTransform);
	CItemScript* pScript = new CItemScript;
	pItem->AddComponent(pScript);

	tItem tempItemlist;
	for (size_t i = 0; i < m_vItemList.size(); ++i)
	{
		if (!wcscmp(m_vItemList[i].strName.c_str(), _name.c_str()))
		{
			tempItemlist = m_vItemList[i];
			break;
		}
	}
	pScript->SetItemInfo(_name
		, tempItemlist.strInfo, tempItemlist.iIdx, tempItemlist.iEffCount, _location, _num);
	pCurScene->AddObject(L"UI", pItem);

	return pItem;

}


void CInventory::start()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	m_pBackTexture = new CGameObject;
	m_pBackTexture->SetName(L"Inventory");
	m_pBackTexture->AddComponent(new CTransform);
	m_pBackTexture->Transform()->SetLocalScale(Vec3(471.f, 549.f, 0.f));
	m_pBackTexture->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	m_pBackTexture->AddComponent(new CMeshRender);
	m_pBackTexture->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Inventory.png");
	m_pBackTexture->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	m_pBackTexture->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	pCurScene->AddObject(L"UI", m_pBackTexture);
	m_pBackTexture->OnDisable();


	CreateItemList();

	AddItem(L"ü������", 3);
	AddItem(L"��������", 1);
	AddItem(L"�ռҵ�", 1);
}

void CInventory::update()
{
}

void CInventory::CreateItemList()
{
	tItem t;
	t.strName = L"ü������";
	t.strInfo = L"ü���� 10 ü���ݴϴ�.";
	t.iIdx = Vec2(0.f, 12.f);
	t.iEffCount = 10;
	m_vItemList.push_back(t);

	t.strName = L"��������";
	t.strInfo = L"������ 10 ü���ݴϴ�.";
	t.iIdx = Vec2(1.f, 12.f);
	t.iEffCount = 10;
	m_vItemList.push_back(t);

	t.strName = L"�ռҵ�";
	t.strInfo = L"�ʺ��ڰ� �ٷ�� ���� ���Դϴ�.";
	t.iIdx = Vec2(3.f, 9.f);
	t.iEffCount = 10;
	m_vItemList.push_back(t);

	t.strName = L"���̾� �Ƹ�";
	t.strInfo = L"�ʺ��ڰ� ����ϱ� ���� �����Դϴ�.";
	t.iIdx = Vec2(9.f, 10.f);
	t.iEffCount = 10;
	m_vItemList.push_back(t);
}