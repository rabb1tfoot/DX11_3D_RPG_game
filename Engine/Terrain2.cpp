#include "stdafx.h"
#include "Terrain.h"

#include "ResMgr.h"
#include "Mesh.h"

#include "MeshRender.h"

#include "Shader.h"
#include "Material.h"
#include "Camera.h"
#include "Transform.h"
#include "KeyMgr.h"
#include "SCeneMgr.h"


void CTerrain::ModCheck()
{
	if (KEYTAB(KEY_TYPE::KEY_Z))
	{
		m_eMod = TERRAIN_MOD::HEIGHTMAP;
	}
	else if (KEYTAB(KEY_TYPE::KEY_X))
	{
		m_eMod = TERRAIN_MOD::SPLATTING;
	}
	else if (KEYTAB(KEY_TYPE::KEY_ESC))
	{
		m_eMod = TERRAIN_MOD::END;
	}
	else if (KEYTAB(KEY_TYPE::KEY_C))
	{
		m_iTileIdx += 1;
		if (m_iTileIdx > 3)
			m_iTileIdx = 0;
	}
	else if (KEYTAB(KEY_TYPE::KEY_V))
	{
		m_iBrushIdx += 1;
		if (m_iBrushIdx > 1)
			m_iBrushIdx = 0;
	}
}

void CTerrain::KeyCheck()
{
	Vec2 vPos = Vec2(0.f, 0.f);
	int iPicking = 0;

	if (m_eMod == TERRAIN_MOD::END || !Picking(vPos))
	{
		//모드 end 이거나 마우스 위치가 터레인에 픽킹되지않는경우
		CResPtr<CTexture> pTex = nullptr;
		MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_3, &pTex);
		return;
	}

	CResPtr<CMaterial> pMtrl = MeshRender()->GetSharedMaterial();
	pMtrl->SetData(SHADER_PARAM::TEX_3, &m_vecBrush[m_iBrushIdx]);
	pMtrl->SetData(SHADER_PARAM::VEC2_0, &vPos);
	pMtrl->SetData(SHADER_PARAM::VEC2_1, &m_vBrushScale);

	if (KEYHOLD(KEY_TYPE::KEY_LBTN))
	{
		if (m_eMod == TERRAIN_MOD::HEIGHTMAP)
		{
			m_pHeightMapMtrl->SetData(SHADER_PARAM::TEX_0, &m_vecBrush[m_iBrushIdx]);
			m_pHeightMapMtrl->SetData(SHADER_PARAM::VEC2_0, &vPos);
			m_pHeightMapMtrl->SetData(SHADER_PARAM::VEC2_1, &m_vBrushScale);
			m_pHeightMapMtrl->ExcuteComputeShader(1, 1024, 1);
		}
		else if(m_eMod == TERRAIN_MOD::SPLATTING)
		{
			m_pWeightMtrl->SetData(SHADER_PARAM::TEX_0, &m_vecBrush[m_iBrushIdx]);
			m_pWeightMtrl->SetData(SHADER_PARAM::VEC2_0, &vPos);
			m_pWeightMtrl->SetData(SHADER_PARAM::VEC2_1, &m_vBrushScale);
			m_pWeightMtrl->SetData(SHADER_PARAM::INT_2, &m_iTileIdx);
			m_pWeightMtrl->ExcuteComputeShader(1, 1024, 1);
		}
	}

}

int CTerrain::Picking(Vec2 & _vPos)
{
	//광선을 쏴서 터레인 충돌여부 확인
	tRay ray = m_pToolCam->GetRay();
	Matrix matWorldInv = Transform()->GetWorldMat();
	matWorldInv = XMMatrixInverse(nullptr, matWorldInv);

	Vec4 vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	Vec4 vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	vDir.Normalize();

	m_pPickMtrl->SetData(SHADER_PARAM::INT_0, &m_iXFaceCount);
	m_pPickMtrl->SetData(SHADER_PARAM::INT_1, &m_iZFaceCount);
	m_pPickMtrl->SetData(SHADER_PARAM::VEC4_0, &vStart);
	m_pPickMtrl->SetData(SHADER_PARAM::VEC4_1, &vDir);

	int iXGroup = (m_iXFaceCount * 2) / 32;
	int iYGroup = (m_iZFaceCount);

	if ((m_iXFaceCount * 2) % 32 != 0)
		iXGroup += 1;
	if (m_iZFaceCount % 32)
		iYGroup += 1;

	//RWTexture 초기화(clear)
	m_pOutput->ClearRWTex();

	m_pPickMtrl->ExcuteComputeShader(iXGroup, iYGroup, 1);


	//택스처에서 픽셀 뽑아오기
	m_pOutput->Capture();
	Vec4* pData = (Vec4*)m_pOutput->GetSysMem();

	//픽킹 실패
	if (pData->w == 0.f)
		return 0;

	//픽킹위치(월드값)를 비율로 전환
	_vPos = Vec2(pData->x / (float)m_iXFaceCount, 1.f - (pData->z) / (float)m_iZFaceCount);

	return 1;

}

float CTerrain::PickingFeet(Vec3 _vPos)
{
	//택스처에서 픽셀 뽑아오기
	m_pHeightMap->Capture();
	
	int WidthIdx = m_pHeightMap->GetWidth();
	int HeightIdx = m_pHeightMap->GetHeight();
	
	//터레인 좌하단 우상단좌표
	Vec2 TerrainPosLB = Vec2(Transform()->GetLocalPos().x, Transform()->GetLocalPos().z);
	Vec2 TerrainPosRT = Vec2(TerrainPosLB.x + m_iXFaceCount * Transform()->GetLocalScale().x
		, TerrainPosLB.y + m_iZFaceCount * Transform()->GetLocalScale().z);

	Vec2 ratio = Vec2(_vPos.x / TerrainPosRT.x *1024.f,(_vPos.z / TerrainPosRT.y -1) * -1024.f - 1); 
	Vec4* pData = (Vec4*)m_pHeightMap->GetSysMem();

	//픽킹 실패
	if (pData[(UINT)ratio.y * 1024 + (UINT)ratio.x].y == 0.f)
		return -1.f;
	
	return pData[(UINT)ratio.y * 1024 + (UINT)ratio.x].y * 1000.f;
}

void CTerrain::SetFaceCount(UINT _iXFace, UINT _iZFace, CGameObject* _pTarget)
{
	m_iXFaceCount = _iXFace;
	m_iZFaceCount = _iZFace;

	vector<VTX> vecVtx;	VTX v;
	vector<UINT> vecIdx;

	for (UINT z = 0; z < m_iZFaceCount + 1; ++z)
	{
		for (UINT x = 0; x < m_iXFaceCount + 1; ++x)
		{
			v.vPos = Vec3((float)x, 0.f, (float)z);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, 1.f);

			v.vUV = Vec2((float)x, (float)(m_iZFaceCount - z));
			vecVtx.push_back(v);
		}
	}

	// 0	 1---2
	// | \	  \  |
	// 2---1    0
	for (UINT z = 0; z < m_iZFaceCount; ++z)
	{
		for (UINT x = 0; x < m_iXFaceCount; ++x)
		{
			vecIdx.push_back((x + (m_iXFaceCount + 1)  * z) + (m_iXFaceCount + 1));
			vecIdx.push_back((x + (m_iXFaceCount + 1) * z) + 1);
			vecIdx.push_back((x + (m_iXFaceCount + 1) * z));

			vecIdx.push_back((x + (m_iXFaceCount + 1) * z) + 1);
			vecIdx.push_back((x + (m_iXFaceCount + 1) * z) + (m_iXFaceCount + 1));
			vecIdx.push_back((x + (m_iXFaceCount + 1) * z) + (m_iXFaceCount + 1) + 1);
		}
	}

	CResPtr<CMesh> pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	CResMgr::GetInst()->AddRes(L"TerrainRect", pMesh);

	if(_pTarget == nullptr)
		MeshRender()->SetMesh(pMesh);
	else
	{
		_pTarget->MeshRender()->SetMesh(pMesh);
	}
}

void CTerrain::CreateComputeShader()
{
	// ===================
	// HeightMap Compute Shader
	// ===================
	CShader* pShader = nullptr;

	pShader = new CShader;
	pShader->CreateComputeShader(L"Shader\\compute.fx", "CS_HeightMap", 5, 0);
	CResMgr::GetInst()->AddRes<CShader>(L"CS_HeightMap", pShader);


	m_pHeightMapMtrl = new CMaterial;
	m_pHeightMapMtrl->SaveDisable();
	m_pHeightMapMtrl->SetShader(pShader);
	CResMgr::GetInst()->AddRes<CMaterial>(L"HeightMapMtrl", m_pHeightMapMtrl);

	m_pHeightMapMtrl->SetData(SHADER_PARAM::RWTEX_0, &m_pHeightMap);
	m_pHeightMapMtrl->SetData(SHADER_PARAM::TEX_0, &m_vecBrush[0]);
	m_pHeightMapMtrl->SetData(SHADER_PARAM::TEX_1, &m_vecBrush[1]);

	int width = m_pHeightMap->GetWidth();
	int height = m_pHeightMap->GetHeight();

	m_pHeightMapMtrl->SetData(SHADER_PARAM::INT_0, &width);
	m_pHeightMapMtrl->SetData(SHADER_PARAM::INT_1, &height);

	// ===================
	// Weight Compute Shader
	// ===================

	pShader = new CShader;
	pShader->CreateComputeShader(L"Shader\\compute.fx", "CS_Weight", 5, 0);
	CResMgr::GetInst()->AddRes<CShader>(L"CS_Weight", pShader);


	m_pWeightMtrl = new CMaterial;
	m_pWeightMtrl->SaveDisable();
	m_pWeightMtrl->SetShader(pShader);
	CResMgr::GetInst()->AddRes<CMaterial>(L"WeightMtrl", m_pWeightMtrl);

	m_pWeightMtrl->SetData(SHADER_PARAM::RWTEX_0, &m_pWeightTex);
	m_pWeightMtrl->SetData(SHADER_PARAM::TEX_0, &m_vecBrush[0]);
	m_pWeightMtrl->SetData(SHADER_PARAM::TEX_1, &m_vecBrush[1]);

	width = m_pWeightTex->GetWidth();
	height = m_pWeightTex->GetHeight();

	m_pWeightMtrl->SetData(SHADER_PARAM::INT_0, &width);
	m_pWeightMtrl->SetData(SHADER_PARAM::INT_1, &height);
	m_pWeightMtrl->SetData(SHADER_PARAM::INT_2, &m_iTileIdx);

	// ======================
	// Picking Compute Shader
	// ======================
	pShader = new CShader;
	pShader->CreateComputeShader(L"Shader\\compute.fx", "CS_Picking", 5, 0);
	CResMgr::GetInst()->AddRes<CShader>(L"CS_Picking", pShader);

	m_pPickMtrl = new CMaterial;
	m_pPickMtrl->SaveDisable();
	m_pPickMtrl->SetShader(pShader);

	m_pPickMtrl->SetData(SHADER_PARAM::TEX_0, &m_pHeightMap);
	m_pPickMtrl->SetData(SHADER_PARAM::RWTEX_0, &m_pOutput);
	CResMgr::GetInst()->AddRes<CMaterial>(L"PickMtrl", m_pPickMtrl);

}

float CTerrain::GetCollisionHeight(Vec3 _vPos)
{
	float fHeight = PickingFeet(_vPos);

	if (-1.f == fHeight)
		return -100.f;

	return fHeight;
}


void CTerrain::LoadResource()
{
	// HeightMap 만들기
	m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"TerrainHeightMap"
		, 1024, 1024
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// WeightTex 만들기
	m_pWeightTex = CResMgr::GetInst()->CreateTexture(L"TerrainWeightTex"
		, m_iXFaceCount * 16, m_iZFaceCount * 16
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Picking Output Texture 만들기
	m_pOutput = CResMgr::GetInst()->CreateTexture(L"PickingOutput"
		, 1, 1
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
		, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Brush Texture
	m_vecBrush.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Brush\\Brush_01.png"));
	m_vecBrush.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Brush\\Brush_02.png"));


	CreateComputeShader();

	// Mesh
	SetFaceCount(m_iXFaceCount, m_iZFaceCount);

	//TileTexArray만들기
	//vector<CResPtr<CTexture>> vecTex;
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_01.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_02.tga"));
	//vecTex.push_back(CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_03.tga"));
	//
	//CResPtr<CTexture> pTileArrTex = CResMgr::GetInst()->CreateArrayTexture(L"Texture\\Tile\\TILE_ARR.dds", vecTex);
	//pTileArrTex->Save(pTileArrTex->GetName());

	CResPtr<CTexture> pTileArrTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_ARR.dds");
	CResPtr<CTexture> pTileArrNormalTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_ARR_N.dds");

	// Material
	MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TerrainMtrl.mtrl"));
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::ARRTEX_0, &pTileArrTex);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::ARRTEX_1, &pTileArrNormalTex);

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_2, &m_pHeightMap);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &m_pWeightTex);

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::INT_0, &m_iXFaceCount);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::INT_1, &m_iZFaceCount);
}

void CTerrain::SaveToScene(FILE * _pFile)
{
	fwrite(&m_iXFaceCount, sizeof(UINT), 1, _pFile);
	fwrite(&m_iZFaceCount, sizeof(UINT), 1, _pFile);

	wstring strPath =  L"Scene\\HeightMap.dds";
	m_pHeightMap->Save(strPath.c_str());

	strPath = L"Scene\\WeightTex.dds";
	m_pWeightTex->Save(strPath.c_str());

	SaveWString(m_pTessCam->GetName().c_str(), _pFile);

}

void CTerrain::LoadFromScene(FILE* _pFile, CGameObject* _pTarget)
{
	fread(&m_iXFaceCount, sizeof(UINT), 1, _pFile);
	fread(&m_iZFaceCount, sizeof(UINT), 1, _pFile);

	SetFaceCount(m_iXFaceCount, m_iZFaceCount, _pTarget);

	wstring strPath = L"Scene\\HeightMap.dds";
	m_pHeightMap = CResMgr::GetInst()->FindRes<CTexture>(strPath);

	strPath = L"Scene\\WeightTex.dds";
	m_pWeightTex = CResMgr::GetInst()->FindRes<CTexture>(strPath);

	wstring camName = LoadWString(_pFile);

	vector<CGameObject*> vecOut;

	CSceneMgr::GetInst()->FindGameObject(camName.c_str(), vecOut);
	if(vecOut.size() != 0)
		m_pTessCam = vecOut[0];

	CResPtr<CTexture> pTileArrTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_ARR.dds");
	CResPtr<CTexture> pTileArrNormalTex = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Tile\\TILE_ARR_N.dds");

	// Material
	_pTarget->MeshRender()->SetMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\TerrainMtrl.mtrl"));
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::ARRTEX_0, &pTileArrTex);
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::ARRTEX_1, &pTileArrNormalTex);
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_2, &m_pHeightMap);
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::TEX_0, &m_pWeightTex);
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::INT_0, &m_iXFaceCount);
	_pTarget->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::INT_1, &m_iZFaceCount);

	_pTarget->AddComponent(this);
}

