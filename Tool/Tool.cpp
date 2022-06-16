
// Tool.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"
#include "HierachyView.h"
#include "ComponentView.h"
#include "CGameView.h"


#include "GameObjectEx.h"
#include "CameraEx.h"
#include "MaterialEx.h"

#include <Scene.h>
#include <SCeneMgr.h>
#include <CollisionMgr.h>
#include <Transform.h>
#include <Layer.h>
#include <MeshRender.h>
#include <Mesh.h>
#include <Material.h>
#include <Terrain.h>
#include <Font23.h>
#include <ParticleSystem.h>
#include <Animator3D.h>
#include <Light3D.h>
#include <ResMgr.h>
#include <Core.h>
#include <RenderMgr.h>
#include <MRT.h>

#include "CamScript.h"
#include "GridScript.h"
#include <PlayerScript.h>
#include <PlayerAttackSpell.h>
#include <WalkingGravityScript.h>
#include <FollowCamScript.h>
#include <MonsterScript.h>
#include <DialogueScript.h>
#include <NPCScript.h>
#include <DialogueBoxScript.h>
#include <Inventory.h>


#include <Core.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolApp

BEGIN_MESSAGE_MAP(CToolApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CToolApp::OnAppAbout)
END_MESSAGE_MAP()


// CToolApp 생성

CToolApp::CToolApp() noexcept
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Tool.AppID.NoVersion"));

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CToolApp 개체입니다.

CToolApp theApp;


// CToolApp 초기화

BOOL CToolApp::InitInstance()
{
	//_CrtSetBreakAlloc(2421);

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));


	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CFrameWnd* pFrame = new CMainFrame;
	m_pFrame = pFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 프레임을 만들어 리소스와 함께 로드합니다.
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	//Game뷰 가져오기
	HWND hGameViewHwnd = ((CMainFrame*)pFrame)->GetGameView()->m_hWnd;
	
	//엔진 초기화
	CCore::GetInst()->init(hGameViewHwnd, true);

	//툴 프레임 초기화
	((CMainFrame*)pFrame)->init();

	//Scene코딩
	CreateTestScene();
	
	//Hierachy뷰 초기화
	((CComponentView*)((CMainFrame*)pFrame)->GetComView())->init();
	CHierachyView* pView = (CHierachyView*)((CMainFrame*)pFrame)->GetHierachyView();
	pView->init_object();

	return TRUE;
}

int CToolApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

// CToolApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CToolApp::CreateTestScene()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	pCurScene->AddLayer(L"Player", 1);
	pCurScene->AddLayer(L"Bullet", 2);
	pCurScene->AddLayer(L"Monster", 3);
	pCurScene->AddLayer(L"Wall", 4);
	pCurScene->AddLayer(L"UI", 5);

	//씬 로딩
	//((CMainFrame*)m_pFrame)->LoadSceneMenu();

	////// FBX Loading
	//CResPtr<CMeshData> pMeshData = CResMgr::GetInst()->LoadFBX(L"FBX\\NPC1.fbx");
	//pMeshData->Save();

	CResPtr<CMeshData> pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"MeshData\\Hero1.mdat");
	////CResPtr<CMeshData> pMeshData2 = CResMgr::GetInst()->FindRes<CMeshData>(L"MeshData\\Warehouse01.mdat");
	//CResPtr<CMeshData> pMeshData3 = CResMgr::GetInst()->FindRes<CMeshData>(L"MeshData\\AGRON.mdat");
	//
	CGameObject* pMeshObject = pMeshData->Instantiate();
	pMeshObject->SetName(L"Player");
	////CGameObject* pMeshObject2 = pMeshData2->Instantiate();
	//CGameObject* pMeshObject3 = pMeshData3->Instantiate();
	//
	pMeshObject->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pMeshObject->Transform()->SetLocalPos(Vec3(500.f, 0.f, 500.f));
	pMeshObject->Transform()->SetLocalRot(Vec3((XM_PI / 2.f) * 3.f, 0.f, 0.f));
	pMeshObject->AddComponent(new CCollider3D);
	pMeshObject->Collider3D()->SetScale(Vec3(100.f, 100.f, 100.f));
	//pMeshObject->AddComponent(new CWalkingGravityScript);
	pMeshObject->AddComponent(new CPlayerScript);
	pMeshObject->AddComponent(new CPlayerAttackSpell);
	pMeshObject->Animator3D()->SetFromFileAnim(L"Player.Anim");

	CResPtr<CMeshData> pMeshData2 = CResMgr::GetInst()->FindRes<CMeshData>(L"MeshData\\Monster1.mdat");
	CGameObject* pMeshObject2 = pMeshData2->Instantiate();
	pMeshObject2->SetName(L"Monster1");
	pMeshObject2->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pMeshObject2->Transform()->SetLocalPos(Vec3(1500.f, 0.f, 1500.f));
	pMeshObject2->Transform()->SetLocalRot(Vec3((XM_PI / 2.f) * 3.f, 0.f, 0.f));
	pMeshObject2->AddComponent(new CCollider3D);
	pMeshObject2->Collider3D()->SetScale(Vec3(100.f, 100.f, 100.f));
	CMonsterScript* pMonScript = new CMonsterScript();
	pMonScript->SetState(100, 100, 10, 20);
	pMeshObject2->AddComponent(pMonScript);

	CResPtr<CMeshData> pMeshData3 = CResMgr::GetInst()->FindRes<CMeshData>(L"MeshData\\NPC1.mdat");
	CGameObject* pMeshObject3 = pMeshData3->Instantiate();
	pMeshObject3->SetName(L"NPC1");
	pMeshObject3->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pMeshObject3->Transform()->SetLocalPos(Vec3(1500.f, 0.f, 1500.f));
	pMeshObject3->Transform()->SetLocalRot(Vec3((XM_PI / 2.f) * 3.f, 0.f, 0.f));
	pMeshObject3->AddComponent(new CCollider3D);
	pMeshObject3->Collider3D()->SetScale(Vec3(100.f, 100.f, 100.f));
	pMeshObject3->AddComponent(new CNPCScript);
	pMeshObject3->AddComponent(new CDialogueScript);


	pCurScene->AddObject(L"Player", pMeshObject);
	pCurScene->AddObject(L"Monster", pMeshObject2);
	pCurScene->AddObject(L"Monster", pMeshObject3);

	//pCurScene->AddObject(L"Player", pMeshObject2);
	//pCurScene->AddObject(L"Default", pMeshObject3);

	// ====================
	// Particle Object 생성
	// ====================
	//CGameObject* pParticle = new CGameObject;
	//pParticle->SetName(L"Particle1");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);
	//
	//pParticle->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//
	//pCurScene->AddObject(L"Default", pParticle);

	// Camera Object 만들기
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");
	CTransform* pTrans = new CTransform;
	pTrans->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pCamObj->AddComponent(pTrans);
	pCamObj->AddComponent(new CCamera);
	pCamObj->AddComponent(new CFollowCamScript);
	
	pCamObj->Camera()->SetCamOrder(0);
	pCamObj->Camera()->SetFOV(XM_PI / 4.f);
	pCamObj->Camera()->SetScale(1.f);
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Default")->GetLayerIdx());
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Player")->GetLayerIdx());
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Monster")->GetLayerIdx());
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Wall")->GetLayerIdx());
	pCamObj->Camera()->CheckLayer(pCurScene->FindLayer(L"Bullet")->GetLayerIdx());
	
	pCurScene->AddObject(L"Default", pCamObj);
	CCollisionMgr::GetInst()->SetCamera(pCamObj);

	// Camera Object 만들기
	CGameObject* pCamObj2 = new CGameObject;
	pCamObj2->SetName(L"UICamera");
	pCamObj2->AddComponent(new CTransform);
	pCamObj2->Transform()->SetLocalPos(Vec3(0.f, 0.f, -1.f));
	pCamObj2->AddComponent(new CCamera);

	pCamObj2->Camera()->SetCamOrder(1);
	pCamObj2->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHIC);
	pCamObj2->Camera()->SetScale(1.f);
	pCamObj2->Camera()->CheckLayer(pCurScene->FindLayer(L"UI")->GetLayerIdx());

	pCurScene->AddObject(L"Default", pCamObj2);

	//// Font Object 만들기
	//CGameObject* pFontObj = new CGameObject;
	//pFontObj->SetName(L"Font1");
	//CTransform* pTransFont = new CTransform;
	//pTransFont->SetLocalPos(Vec3(100.f, 300.f, 0.f));
	//pFontObj->AddComponent(pTransFont);
	//CFont23* font = new CFont23;
	//font->SetOffSet(Vec2(200.f, -250.f));
	//font->SetText(L"GameObject로 폰트 출력입니다");
	//pFontObj->AddComponent(font);
	//
	//pCurScene->AddObject(L"Default", pFontObj);

	//// UI Default Object 만들기
	//CGameObject* pUIObj = new CGameObject;
	//pUIObj->SetName(L"UI1");
	//pUIObj->AddComponent(new CTransform);
	//pUIObj->AddComponent(new CFont23);
	//pUIObj->Transform()->SetLocalScale(Vec3(410.f, 200.f, 0.f));
	//pUIObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//pUIObj->AddComponent(new CMeshRender);
	//pUIObj->AddComponent(new CCollider2D);
	//pUIObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Dialogue.png");
	//pUIObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	//pUIObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	//pUIObj->Font()->SetOffSet(Vec2(440.f, -300.f));
	//pUIObj->Font()->SetText(L"샘플용 텍스트입니다");
	//pCurScene->AddObject(L"UI", pUIObj);
	//
	//CGameObject* m_pYesOrNoObj0 = new CGameObject;
	//m_pYesOrNoObj0->SetName(L"Font1");
	//m_pYesOrNoObj0->AddComponent(new CTransform);
	//m_pYesOrNoObj0->AddComponent(new CFont23);
	//m_pYesOrNoObj0->Font()->SetOffSet(Vec2(470.f, -400.f));
	//m_pYesOrNoObj0->Font()->SetText(L"네");
	//pCurScene->AddObject(L"UI", m_pYesOrNoObj0);
	//
	//CGameObject* m_pYesOrNoObj1 = new CGameObject;
	//m_pYesOrNoObj1->SetName(L"Font2");
	//m_pYesOrNoObj1->AddComponent(new CTransform);
	//m_pYesOrNoObj1->AddComponent(new CFont23);
	//m_pYesOrNoObj1->Font()->SetOffSet(Vec2(625.f, -400.f));
	//m_pYesOrNoObj1->Font()->SetText(L"아니오");
	//pCurScene->AddObject(L"UI", m_pYesOrNoObj1);

	////Inventory만들기
	//CGameObject* m_pBackTexture = new CGameObject;
	//m_pBackTexture->SetName(L"Inventory");
	//m_pBackTexture->AddComponent(new CTransform);
	//m_pBackTexture->Transform()->SetLocalScale(Vec3(471.f, 549.f, 0.f));
	//m_pBackTexture->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//m_pBackTexture->AddComponent(new CMeshRender);
	//m_pBackTexture->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\Inventory.png");
	//m_pBackTexture->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	//m_pBackTexture->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	//pCurScene->AddObject(L"UI", m_pBackTexture);

	//int iswitch = 3;
	//Vec2 vIdx = Vec2(0.0625f, 0.02564f);
	//Vec2 vIdx2 = Vec2(24.f / 384.f, 24.f / 936.f);
	//CGameObject* m_pIconObj = new CGameObject();
	//m_pIconObj->SetName(L"Item");
	//m_pIconObj->AddComponent(new CTransform);
	//m_pIconObj->AddComponent(new CMeshRender);
	//m_pIconObj->Transform()->SetLocalPos(Vec3(0.f,0.f,0.f));
	//m_pIconObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//m_pIconObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//m_pIconObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TextureMtrl.mtrl"));
	//CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\UI\\ItemSet.png");
	//m_pIconObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::TEX_0, &pTex);
	//m_pIconObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::VEC2_0, &vIdx);
	//m_pIconObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::VEC2_1, &vIdx2);
	//m_pIconObj->MeshRender()->GetCloneMaterial()->SetData(SHADER_PARAM::INT_0, &iswitch);
	//pCurScene->AddObject(L"UI", m_pIconObj);

	//TestObj만들기
	CGameObject* pObj = new CGameObject;
	pObj->SetName(L"DialogueBox");
	pObj->AddComponent(new CTransform);
	pObj->AddComponent(new CDialogueBoxScript);
	pCurScene->AddObject(L"Default", pObj);

	//TestObj2 만들기
	CGameObject* pObj2 = new CGameObject;
	pObj2->SetName(L"Inventory");
	pObj2->AddComponent(new CTransform);
	pObj2->AddComponent(new CInventory);
	pCurScene->AddObject(L"Default", pObj2);

	// Light3D 추가하기
	CGameObject* pLightObj = new CGameObject;
	pLightObj->SetName(L"Directional light");
	pLightObj->AddComponent(new CTransform);
	pLightObj->AddComponent(new CLight3D);
	
	pLightObj->Transform()->SetLocalPos(Vec3(-500.f, 5000.f, -500.f));
	pLightObj->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pLightObj->Light3D()->SetLightDir(Vec3(-1.f, -1.f, -1.f));
	pLightObj->Light3D()->SetLightDiffuse(Vec3(1.f, 1.f, 1.f));
	pLightObj->Light3D()->SetLightSpecular(Vec3(0.3f, 0.3f, 0.3f));
	pLightObj->Light3D()->SetLightAmbient(Vec3(0.1f, 0.1f, 0.1f));
	pLightObj->Light3D()->SetLightAngle(40.f);
	pLightObj->Light3D()->SetLightRange(100.f);
	
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
	
	CGameView* pView = (CGameView*)(((CMainFrame*)m_pMainWnd)->GetGameView());
	pNewObj->Terrain()->SetToolCamera(pView->GetToolCamera()->Camera());
	pNewObj->Terrain()->SetTessCamera(pCamObj);
	
	pNewObj->Terrain()->init(64, 64);
	
	CSceneMgr::GetInst()->GetCurScene()->AddObject(L"Default", pNewObj);

	//테스트 모형 생성3
	CGameObject* pParent3 = new CGameObject;
	pParent3->SetName(L"Wall");

	CTransform* pTransform3 = new CTransform;
	CMeshRender* pMeshRender3 = new CMeshRender;

	pTransform3->SetLocalPos(Vec3(1000.f, 0.f, 1000.f));
	pTransform3->SetLocalScale(Vec3(500.f, 1000.f, 100.f));

	pMeshRender3->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pMeshRender3->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\Std3DMtrl.mtrl"));
	pParent3->AddComponent(pTransform3);
	pParent3->AddComponent(pMeshRender3);
	pParent3->AddComponent(new CCollider3D);

	pCurScene->AddObject(L"Wall", pParent3);

	//// Post Effect Object
	//CGameObject* pObject = new CGameObject;
	//pObject->SetName(L"PostEffect");
	//
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//
	//pObject->MeshRender()->SetShadow(false);
	//
	//// Material 값 셋팅
	//CResPtr<CMaterial> pMtrl = new CMaterial;
	//pMtrl->SaveDisable();
	//pMtrl->SetShader(CResMgr::GetInst()->FindRes<CShader>(L"DistortionShader"));
	//CResPtr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(L"PosteffectTargetTex");
	//pMtrl->SetData(SHADER_PARAM::TEX_0, &pTex);
	//
	//CResMgr::GetInst()->AddRes<CMaterial>(L"DistortionMtrl", pMtrl);
	//
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetMaterial(pMtrl, 0);
	//
	//pObject->Transform()->SetLocalScale(Vec3(1500.f, 1500.f, 1500.f));
	//pObject->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//
	//pCurScene->AddObject(L"Default", pObject);
	//

	//충돌체크
	//CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Player");
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Wall");
	CCollisionMgr::GetInst()->CollisionCheck(L"Player", L"Monster");
	CCollisionMgr::GetInst()->CollisionCheck(L"Monster", L"Bullet");
}

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CToolApp 메시지 처리기





BOOL CToolApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CCore::GetInst()->progress();

	((CMainFrame*)m_pMainWnd)->update();

	return CWinApp::OnIdle(0);
}
