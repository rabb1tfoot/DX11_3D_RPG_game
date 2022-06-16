#include "stdafx.h"

#include "CreateScene.h"
#include "GameObjectEx.h"
#include "CameraEx.h"
#include "MaterialEx.h"

#include <Scene.h>
#include <SCeneMgr.h>
#include <Transform.h>
#include <Layer.h>
#include <MeshRender.h>
#include <Mesh.h>
#include <Material.h>
#include <Terrain.h>
#include <Light3D.h>
#include <ResMgr.h>
#include <Core.h>
#include <RenderMgr.h>
#include <MRT.h>

#include "CamScript.h"
#include "GridScript.h"


CCreateScene::CCreateScene()
	: m_pToolCam(nullptr)
	, m_pComputeRender(nullptr)
{
}


CCreateScene::~CCreateScene()
{
	SAFE_DELETE(m_pToolCam);
	SAFE_DELETE(m_pToolUICam);
	SAFE_DELETE(m_pGridObject);
	Safe_Delete_Vec(m_vecToolUI);

	SAFE_DELETE(m_pGridMtrl);
	SAFE_DELETE(m_pGridShader);
	SAFE_DELETE(m_pComputeRender);
}

void CCreateScene::Create()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	pCurScene->AddLayer(L"Player", 1);
	pCurScene->AddLayer(L"Bullet", 2);
	pCurScene->AddLayer(L"Monster", 3);

	// Camera Object 만들기
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");
	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);

	pCamObj->Camera()->SetCamOrder(0);
	//pCamObj->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pCamObj->Camera()->SetFOV(XM_PI / 4.f);
	pCamObj->Camera()->SetScale(1.f);
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Player")->GetLayerIdx());

	pCurScene->AddObject(L"Default", pCamObj);

	// Light3D 추가하기
	CGameObject* pLightObj = new CGameObject;
	pLightObj->SetName(L"Directional light");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight3D);

	pLightObj->Transform()->SetLocalPos(Vec3(1000.f, 500.f, 1000.f));
	pLightObj->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light3D()->SetLightDir(Vec3(0.f, -1.f, 0.f));
	pLightObj->Light3D()->SetLightDiffuse(Vec3(1.f, 1.f, 1.f));
	pLightObj->Light3D()->SetLightSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pLightObj->Light3D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pLightObj->Light3D()->SetLightAngle(40.f);
	pLightObj->Light3D()->SetLightRange(1000.f);

	pCurScene->AddObject(L"Default", pLightObj);

	// SkyBox 추가하기
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"Skybox");
	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CMeshRender);

	pSkyBox->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pSkyBox->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\Skybox.mtrl"));
	CResPtr<CTexture> pSkyBoxTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Skybox\\Sky01.png");
	pSkyBox->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pSkyBoxTex);

	pCurScene->AddObject(L"Default", pSkyBox);

	//테레인 생성
	CGameObject* pNewObj = new CGameObject;
	pNewObj->SetName(L"Terrain");
	pNewObj->AddComponent(new CTransform);
	pNewObj->AddComponent(new CMeshRender);
	pNewObj->AddComponent(new CTerrain);
	
	pNewObj->Terrain()->SetToolCamera(m_pToolCam->Camera());
	pNewObj->Terrain()->SetTessCamera(pCamObj);
	
	pNewObj->Terrain()->init(64, 64);

	CSceneMgr::GetInst()->GetCurScene()->AddObject(L"Default", pNewObj);


	//테스트 모형 생성
	CGameObject* pParent = new CGameObject;
	pParent->SetName(L"Player");
	
	CTransform* pTransform = new CTransform;
	CMeshRender* pMeshRender = new CMeshRender;
	
	pTransform->SetLocalPos(Vec3(0.f, 100.f, 500.f));
	pTransform->SetLocalScale(Vec3(500.f, 500.f, 500.f));
	//pTransform->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));
	
	pMeshRender->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh"));
	pMeshRender->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\Std3DMtrl.mtrl"));
	
	pParent->AddComponent(pTransform);
	pParent->AddComponent(pMeshRender);
	
	pCurScene->AddObject(L"Player", pParent);
}

void CCreateScene::init()
{
	CreateToolResource();
	CreateToolCamera();
	CreateToolObject();

	wstring strContent = CPathMgr::GetResPath();
	LoadResource(strContent);
}

void CCreateScene::update()
{
	SCENE_STATE eState = CCore::GetInst()->GetState();

	if (SCENE_STATE::PAUSE != eState && SCENE_STATE::STOP != eState)
		return;

	update_tool();

	render_tool();
}

void CCreateScene::LoadResource(wstring _strFolderPath)
{
	wstring strFolderPath = _strFolderPath + L"\\*.*";

	WIN32_FIND_DATA tData = {};
	HANDLE h = (HANDLE)FindFirstFile(strFolderPath.c_str(), &tData);
	UINT err = GetLastError();

	while (true)
	{
		if ((FILE_ATTRIBUTE_DIRECTORY & tData.dwFileAttributes))
		{
			if (wcscmp(tData.cFileName, L".") && wcscmp(tData.cFileName, L".."))
			{
				wstring temp = _strFolderPath + tData.cFileName;
				temp += L'\\';
				LoadResource(temp.c_str());
			}
		}
		else
		{
			const wchar_t* pExt = CPathMgr::GetExt(tData.cFileName);
			wstring strPath = _strFolderPath + tData.cFileName;
			wstring strKey = CPathMgr::GetRelativePath(strPath.c_str());

			if (!wcscmp(pExt, L".png")
				|| !wcscmp(pExt, L".bmp")
				|| !wcscmp(pExt, L".jpg")
				|| !wcscmp(pExt, L".jpeg")
				|| !wcscmp(pExt, L".tga")
				|| !wcscmp(pExt, L".dds"))
			{
				CResMgr::GetInst()->Load<CTexture>(strKey, strKey);
			}
			else if (!wcscmp(pExt, L".mp3")
				|| !wcscmp(pExt, L".ogg")
				|| !wcscmp(pExt, L".wav"))
			{
				CResMgr::GetInst()->Load<CSound>(strKey, strKey);
			}
			else if (!wcscmp(pExt, L".mtrl"))
			{
				CResMgr::GetInst()->Load<CMaterial>(strKey, strKey);
			}
			else if (!wcscmp(pExt, L".mdat"))
			{
				//CResMgr::GetInst()->Load<CMeshData>(strKey, strPath);
			}
			else if (!wcscmp(pExt, L".prf"))
			{
				CResMgr::GetInst()->Load<CPrefab>(strKey, strKey);
			}
		}

		if (false == FindNextFile(h, &tData))
		{
			err = GetLastError();
			break;
		}
	}
}

void CCreateScene::update_tool()
{
	m_pToolCam->update();
	m_pToolCam->lateupdate();
	m_pToolCam->finalupdate();

	m_pToolUICam->update();
	m_pToolUICam->lateupdate();
	m_pToolUICam->finalupdate();

	// Tool 전용 Object update
	m_pGridObject->update();
	m_pGridObject->lateupdate();
	m_pGridObject->finalupdate();

	for (size_t i = 0; i < m_vecToolUI.size(); ++i)
	{
		m_vecToolUI[i]->update();
		m_vecToolUI[i]->lateupdate();
		m_vecToolUI[i]->finalupdate();
	}
}

void CCreateScene::render_tool()
{
	// Tool Camera Render	
	m_pToolCam->Camera()->render_deferred(); // deferred MRT 에 출력

	CRenderMgr::GetInst()->render_lights();

	m_pToolCam->Camera()->render_forward(); // SwapChain Mrt 에 출력

	CMRT** pMRT = CRenderMgr::GetInst()->GetMRTs();

	// SwapChain MRT 로 복구
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// Grid render
	m_pGridObject->render();

	// Tool UI render
	m_pToolUICam->Camera()->render_forward();

	// Tool 전용 Object render
	for (size_t i = 0; i < m_vecToolUI.size(); ++i)
	{
		m_vecToolUI[i]->render();
	}

	CRenderMgr::GetInst()->Present();
}

void CCreateScene::CreateToolResource()
{
	// Tool Resource
	m_pGridShader = new CShader;

	m_pGridShader->CreateVertexShader(L"Shader\\tool.fx", "VS_Grid", 5, 0);
	m_pGridShader->CreatePixelShader(L"Shader\\tool.fx", "PS_Grid", 5, 0);
	m_pGridShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ALPHABLEND));
	m_pGridShader->SetRSType(RS_TYPE::CULL_NONE);
	m_pGridShader->SetName(L"GridShader");

	m_pGridMtrl = new CMaterialEx;
	m_pGridMtrl->SetName(L"Material\\GridMtrl.mtrl");
	m_pGridMtrl->SetShader(m_pGridShader);
}

void CCreateScene::CreateToolCamera()
{
	// Tool Camera
	m_pToolCam = new CGameObjectEx;

	m_pToolCam->AddComponent(new CTransform);
	m_pToolCam->AddComponent(new CCameraEx);
	m_pToolCam->AddComponent(new CCamScript);

	m_pToolCam->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_pToolCam->Camera()->CheckLayer(i);
	}

	// Tool UI Camera
	m_pToolUICam = new CGameObjectEx;
	m_pToolUICam->AddComponent(new CTransform);
	m_pToolUICam->AddComponent(new CCameraEx);

	m_pToolUICam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	m_pToolUICam->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
}

void CCreateScene::CreateToolObject()
{
	tResolution res = CRenderMgr::GetInst()->GetResolution();

	// Grid Object
	m_pGridObject = new CGameObjectEx;

	m_pGridObject->AddComponent(new CTransform);
	m_pGridObject->AddComponent(new CMeshRender);
	m_pGridObject->AddComponent(new CGridScript);
	m_pGridObject->GetScript<CGridScript>()->SetToolCamera(m_pToolCam);

	m_pGridObject->Transform()->SetLocalScale(Vec3(100000.f, 100000.f, 1.f));
	m_pGridObject->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));
	m_pGridObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pGridObject->MeshRender()->SetMaterial(m_pGridMtrl);

	// Tool UI Object
	CGameObject* arrUI[5] = {};

	float fScale = 120.f;
	CResPtr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl");
	CResPtr<CTexture> pTargetTex = nullptr;

	for (int i = 0; i < 5; ++i)
	{
		arrUI[i] = new CGameObjectEx;
		arrUI[i]->AddComponent(new CTransform);
		arrUI[i]->AddComponent(new CMeshRender);

		arrUI[i]->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		arrUI[i]->MeshRender()->SetMaterial(pMtrl);

		arrUI[i]->Transform()->SetLocalScale(Vec3(fScale, fScale, 1.f));
		arrUI[i]->Transform()->SetLocalPos(Vec3((-res.fWidth / 2.f) + (fScale / 2.f) + (fScale * i)
			, (res.fHeight / 2.f) - (fScale / 2.f), 1.f));

		arrUI[i]->MeshRender()->GetCloneMaterial();

		m_vecToolUI.push_back(arrUI[i]);
	}

	pTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex");
	arrUI[0]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTargetTex);

	pTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex");
	arrUI[1]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTargetTex);

	pTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");
	arrUI[2]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTargetTex);

	pTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"LightTargetTex");
	arrUI[3]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTargetTex);

	pTargetTex = CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex");
	arrUI[4]->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &pTargetTex);

}
