#include "stdafx.h"
#include "SaveLoadMgr.h"

#include <ResMgr.h>
#include <SceneMgr.h>
#include <Scene.h>
#include <Layer.h>
#include <GameObject.h>
#include <func.h>

#include <Component.h>
#include <Transform.h>
#include <MeshRender.h>
#include <Camera.h>
#include <Collider2D.h>
#include <Animator2D.h>
#include <Animator3D.h>
#include <Filter23.h>
#include <Light3D.h>
#include <Terrain.h>
#include <Font23.h>



#include <Script.h>
#include "ScriptMgr.h"


CSaveLoadMgr::CSaveLoadMgr() {}
CSaveLoadMgr::~CSaveLoadMgr() {}


void CSaveLoadMgr::SaveResource(FILE* _pFile)
{
	INT i = 0;
	for (; i < (UINT)RES_TYPE::END; ++i)
	{
		// 쉐이더는 설계상 문제로 저장 및 로드하지 않는다.(Engine 코드에서 전부 생성해둠)
		if (i == (UINT)RES_TYPE::SHADER 
			|| i == (UINT)RES_TYPE::FILTER
			|| i == (UINT)RES_TYPE::MESHDATA)
			continue;

		const map<wstring, CResource*>& mapRes = CResMgr::GetInst()->GetResources((RES_TYPE)i);

		// 리소스 타입
		fwrite(&i, sizeof(UINT), 1, _pFile);

		// 해당 리소스 개수
		UINT iResCount = mapRes.size();
		fwrite(&iResCount, sizeof(UINT), 1, _pFile);

		// 리소스 정보
		for (const auto& pair : mapRes)
		{
			pair.second->SaveToScene(_pFile);
		}
	}

	// 리스소 End 정보 (끝을 알림)
	fwrite(&i, sizeof(UINT), 1, _pFile);
}


void CSaveLoadMgr::SaveLayer(CLayer * _pLayer, FILE * _pFile)
{
	bool bExist = _pLayer;
	fwrite(&bExist, sizeof(bool), 1, _pFile);

	if (nullptr == _pLayer)
		return;

	// Layer 이름
	SaveWString(_pLayer->GetName().c_str(), _pFile);

	// GameObject 저장
	const vector<CGameObject*>& vecObject = _pLayer->GetParentObject();
	UINT iSize = (UINT)vecObject.size();
	fwrite(&iSize, sizeof(UINT), 1, _pFile);

	for (size_t i = 0; i < vecObject.size(); ++i)
	{
		SaveGameObject(vecObject[i], _pFile);
	}
}

void CSaveLoadMgr::SaveGameObject(CGameObject * _pObject, FILE * _pFile)
{
	// 이름 저장
	SaveWString(_pObject->GetName().c_str(), _pFile);

	// LayerIdx 저장
	// Layer 에 속해있지 않는 오브젝트는 존재하면 안된다.
	_pObject->SaveToScene(_pFile);

	UINT i = 0;

	for (i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pCom = _pObject->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pCom)
			continue;

		// ComponentType 저장
		fwrite(&i, sizeof(UINT), 1, _pFile);

		pCom->SaveToScene(_pFile);
	}

	// Component End 마킹
	fwrite(&i, sizeof(UINT), 1, _pFile);

	// Script 저장
	const vector<CScript*>& vecScripts = _pObject->GetScripts();

	UINT iScriptCount = (UINT)vecScripts.size();
	fwrite(&iScriptCount, sizeof(UINT), 1, _pFile);

	wstring strScriptName;
	for (UINT i = 0; i < iScriptCount; ++i)
	{
		// Script Name
		strScriptName = CScriptMgr::GetScriptName(vecScripts[i]);
		SaveWString(strScriptName.c_str(), _pFile);

		vecScripts[i]->SaveToScene(_pFile);
	}


	vector<CGameObject*> vecChild = _pObject->GetChild();
	UINT iSize = vecChild.size();
	fwrite(&iSize, sizeof(UINT), 1, _pFile);
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveGameObject(vecChild[i], _pFile);
	}
}

void CSaveLoadMgr::SaveScene(const wstring& _strPath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strPath.c_str(), L"wb");

	// 리소스 저장
	//SaveResource(pFile);

	// Scene 저장
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurScene->GetLayer(i);
		SaveLayer(pLayer, pFile);
	}

	fclose(pFile);
}

void CSaveLoadMgr::LoadResource(FILE * _pFile)
{
	UINT iResType = (UINT)RES_TYPE::END;
	UINT iResCount = 0;
	while (true)
	{
		// 리소스 타입
		fread(&iResType, sizeof(UINT), 1, _pFile);

		// 리소스를 다 읽었다.
		if ((UINT)RES_TYPE::END == iResType)
			break;

		// 리소스 개수		
		fread(&iResCount, sizeof(UINT), 1, _pFile);

		CResource* pResource = nullptr;
		bool bRet = true;

		for (UINT i = 0; i < iResCount; ++i)
		{
			switch ((RES_TYPE)iResType)
			{
			case RES_TYPE::MATERIAL:
				pResource = new CMaterial;
				break;
			case RES_TYPE::PREFAB:
				pResource = new CPrefab;
				break;
			case RES_TYPE::MESH:
				pResource = new CMesh;
				break;
			case RES_TYPE::TEXTURE:
				pResource = new CTexture;
				break;
			case RES_TYPE::SOUND:
				pResource = new CSound;
				break;
			}

			bRet = pResource->LoadFromScene(_pFile);
			if (false == bRet)
			{
				delete pResource;
				continue;
			}

			CResMgr::GetInst()->AddRes((RES_TYPE)iResType, pResource);
		}
	}
}

CGameObject * CSaveLoadMgr::LoadGameObject(FILE * _pFile)
{
	// 이름 읽기
	CGameObject* pObject = new CGameObject;
	wstring strObjName = LoadWString(_pFile);
	pObject->SetName(strObjName);

	// LayerIdx 읽기	
	pObject->LoadFromScene(_pFile);

	UINT iComType = 0;
	while (true)
	{
		fread(&iComType, sizeof(UINT), 1, _pFile);

		if ((UINT)COMPONENT_TYPE::END == iComType)
			break;

		CComponent* pCom = nullptr;
		switch ((COMPONENT_TYPE)iComType)
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
		case COMPONENT_TYPE::LIGHT3D:
			pCom = new CLight3D;
			break;
		case COMPONENT_TYPE::TERRAIN:
			pCom = new CTerrain;
			break;
		case COMPONENT_TYPE::COLLIDER2D:
			pCom = new CCollider2D;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			pCom = new CCollider3D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			pCom = new CAnimator2D;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			pCom = new CAnimator3D;
			break;
		case COMPONENT_TYPE::Font:
			pCom = new CFont23;
			break;
		}
		if ((COMPONENT_TYPE)iComType == COMPONENT_TYPE::TERRAIN)
			pCom->LoadFromScene(_pFile, pObject);
		else
		{
			pCom->LoadFromScene(_pFile);
			pObject->AddComponent(pCom);
		}
	}

	// Script 불러오기	
	UINT iScriptCount = 0;
	fread(&iScriptCount, sizeof(UINT), 1, _pFile);

	wstring strScriptName;
	for (UINT i = 0; i < iScriptCount; ++i)
	{
		// Script Name		
		strScriptName = LoadWString(_pFile);
		CScript* pScript = CScriptMgr::GetScript(strScriptName);
		pScript->LoadFromScene(_pFile);
		pObject->AddComponent(pScript);
	}

	UINT iSize = 0;
	fread(&iSize, sizeof(UINT), 1, _pFile);
	for (size_t i = 0; i < iSize; ++i)
	{
		CGameObject* pChild = LoadGameObject(_pFile);
		pObject->AddChild(pChild);
	}

	return pObject;
}

CLayer * CSaveLoadMgr::LoadLayer(FILE * _pFile)
{
	bool bExist = true;
	fread(&bExist, 1, 1, _pFile);

	if (false == bExist)
		return nullptr;

	CLayer* pLayer = new CLayer;

	// Layer 이름
	wstring strLayerName = LoadWString(_pFile);
	pLayer->SetName(strLayerName);

	// GameObject 로딩	
	UINT iSize = 0;
	fread(&iSize, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iSize; ++i)
	{
		pLayer->AddObject(LoadGameObject(_pFile), false);
	}

	return pLayer;
}


void CSaveLoadMgr::LoadScene(const wstring & _strPath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strPath.c_str(), L"rb");

	// 리소스 불러오기
	//LoadResource(pFile);

	// Scene 불러오기
	CScene* pScene = new CScene;
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = LoadLayer(pFile);
		if (nullptr != pLayer)
			pScene->AddLayer(pLayer, i);
	}

	// SceneMgr 에 현재 Scene 으로 지정
	CSceneMgr::GetInst()->ChangeScene(pScene);

	fclose(pFile);
}
