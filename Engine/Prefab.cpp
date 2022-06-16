#include "stdafx.h"
#include "Prefab.h"

#include "GameObject.h"
#include "ResPtr.h"
#include "ResMgr.h"

#include "Transform.h"
#include "MeshRender.h"
#include "Collider2D.h"
#include "Animation2D.h"
#include "Script.h"

#include "Camera.h"
#include "Core.h"


CPrefab::CPrefab()
	: CResource(RES_TYPE::PREFAB)
	, m_pProto(nullptr)
{
}

CPrefab::CPrefab(CGameObject * _pProtoObj)
	: CResource(RES_TYPE::PREFAB)
	, m_pProto(_pProtoObj)
{
}

CPrefab::~CPrefab()
{
	SAFE_DELETE(m_pProto);
}

CGameObject * CPrefab::Instantiate()
{
	return m_pProto->Clone();
}

bool CPrefab::LoadFromScene(FILE * _pFile)
{
	CResource::LoadFromScene(_pFile);

	CResPtr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(GetName().c_str());
	if (nullptr != pPrefab)
		return false;

	wstring strPath = CPathMgr::GetResPath();
	strPath += GetPath();
	Load(strPath);

	return true;
}

void CPrefab::Save()
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += GetName();
	strFileName += L".pref";

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFileName.c_str(), L"wb");

	SaveProtoObject(m_pProto, pFile);

	fclose(pFile);
}


void CPrefab::SaveProtoObject(CGameObject * _pProto, FILE* _pFile)
{
	// Prefab 이 참조하고 있는 GameObject 정보를 저장한다.
	// 이름 저장
	SaveWString(_pProto->GetName().c_str(), _pFile);

	UINT i = 0;
	for (i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pCom = _pProto->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pCom)
			continue;

		// ComponentType 저장
		fwrite(&i, sizeof(UINT), 1, _pFile);

		pCom->SaveToScene(_pFile);
	}

	// Component End 마킹
	fwrite(&i, sizeof(UINT), 1, _pFile);

	// Script 저장
	const vector<CScript*>& vecScripts = _pProto->GetScripts();

	UINT iScriptCount = (UINT)vecScripts.size();
	fwrite(&iScriptCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iScriptCount; ++i)
	{
		// Script Type
		UINT iScriptType = vecScripts[i]->GetScriptType();
		fwrite(&iScriptType, sizeof(UINT), 1, _pFile);
	}

	vector<CGameObject*> vecChild = _pProto->GetChild();
	UINT iSize = vecChild.size();
	fwrite(&iSize, sizeof(UINT), 1, _pFile);
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveProtoObject(vecChild[i], _pFile);
	}
}

CGameObject* CPrefab::LoadProtoObject(FILE * _pFile)
{
	// Prefab 이 참조하고 있는 GameObject 정보를 저장한다.
	// 이름 저장
	CGameObject* pProto = new CGameObject;
	wstring strName = LoadWString(_pFile);
	pProto->SetName(strName);

	UINT i = 0;
	while (true)
	{
		// ComponentType 저장
		fread(&i, sizeof(UINT), 1, _pFile);

		if ((UINT)COMPONENT_TYPE::END == i)
			break;

		CComponent* pCom = nullptr;
		switch ((COMPONENT_TYPE)i)
		{
		case COMPONENT_TYPE::TRANSFORM:
			pCom = new CTransform;

			break;
		case COMPONENT_TYPE::MESHRENDER:
			pCom = new CMeshRender;

			break;
		case COMPONENT_TYPE::CAMERA:
			pCom = new CCamera;

			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pCom = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pCom = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			break;
		}

		pCom->LoadFromScene(_pFile);
		pProto->AddComponent(pCom);
	}

	// Script 읽기
	UINT iScriptCount = 0;
	fread(&iScriptCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iScriptCount; ++i)
	{
		// Script Type
		UINT iScriptType = 0;
		fread(&iScriptType, sizeof(UINT), 1, _pFile);
		m_vecScriptType.push_back(iScriptType);
	}

	UINT iSize = 0;
	fread(&iSize, sizeof(UINT), 1, _pFile);
	for (size_t i = 0; i < iSize; ++i)
	{
		CGameObject* pChild = LoadProtoObject(_pFile);
		pProto->AddChild(pChild);
	}

	return pProto;
}

void CPrefab::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	SCENE_STATE eCurType = CCore::GetInst()->GetState();
	CCore::GetInst()->SetState(SCENE_STATE::LOAD);

	m_pProto = LoadProtoObject(pFile);

	CCore::GetInst()->SetState(eCurType);

	fclose(pFile);
}
