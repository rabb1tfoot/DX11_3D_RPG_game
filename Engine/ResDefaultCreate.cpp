#include "stdafx.h"
#include "ResMgr.h"
#include "RenderMgr.h"

#include "Collider2D.h"
#include "Collider3D.h"


void CResMgr::CreateDefaultMesh()
{
	vector<VTX> vecVtx;	VTX v;
	vector<UINT> vecIdx;

	// ==========
	// Point Mesh
	// ==========
	CMesh* pMesh = new CMesh;

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);

	UINT iIdx = 0;

	pMesh->CreateMesh(sizeof(VTX), 1, D3D11_USAGE_DEFAULT, &v,
		1, D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &iIdx);

	pMesh->SetName(L"PointMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"PointMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	// ==========
	// Rect Mesh
	// ==========

	// 0 --- 1
	// |  \  |
	// 2-----3

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.5f, 0.5f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	// Create Index Buffer
	vecIdx.push_back(0);
	vecIdx.push_back(3);
	vecIdx.push_back(2);
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(3);

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(),D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"RectMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"RectMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	// ===================
	// Collider Rect Mesh
	// ===================
	// 0 --- 1
	// |	 |
	// 2-----3

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.5f, 0.5f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	// Create Index Buffer
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(3);
	vecIdx.push_back(2);
	vecIdx.push_back(0);

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"ColliderRectMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"ColliderRectMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	// ===========
	// Circle Mesh
	// ===========
	UINT iSliceCount = 40;
	float fThetaStride = XM_2PI / (float)iSliceCount;
	float fTheta = 0.f;

	// 원의 중심
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// 테두리 정점
	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{
		v.vPos.x = cosf(fTheta);
		v.vPos.y = sinf(fTheta);
		v.vPos.z = 0.f;

		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

		vecVtx.push_back(v);

		fTheta += fThetaStride;
	}

	// 인덱스
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"CircleMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"CircleMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	// =========
	// Cube Mesh
	// =========
	VTX arrCube[24] = {};

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[0].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[0].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[1].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[1].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[2].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[2].vBinormal = Vec3(0.f, 0.f, -1.f);


	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);
	arrCube[3].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[3].vBinormal = Vec3(0.f, 0.f, -1.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[4].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[4].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[5].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[5].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[6].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[6].vBinormal = Vec3(0.f, 0.f, -1.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);
	arrCube[7].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[7].vBinormal = Vec3(0.f, 0.f, -1.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[8].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[8].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[9].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[9].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[10].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[10].vBinormal = Vec3(0.f, -1.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);
	arrCube[11].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[11].vBinormal = Vec3(0.f, -1.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[12].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[12].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[13].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[13].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[14].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[14].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);
	arrCube[15].vTangent = Vec3(0.f, 0.f, 1.f);
	arrCube[15].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[16].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[16].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[17].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[17].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[18].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[18].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);
	arrCube[19].vTangent = Vec3(-1.f, 0.f, 0.f);
	arrCube[19].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[20].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[20].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[21].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[21].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[22].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[22].vBinormal = Vec3(0.f, 1.f, 0.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);
	arrCube[23].vTangent = Vec3(1.f, 0.f, 0.f);
	arrCube[23].vBinormal = Vec3(0.f, 1.f, 0.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), 24, D3D11_USAGE_DEFAULT, arrCube
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"CubeMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"CubeMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	//==================
	//Cube Collider Mesh
	//==================
	memset(arrCube, 0, sizeof(VTX) * 24);

	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(0.f, 0.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(0.f, 0.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 0.f);

	// 왼쪽 면
	arrCube[4].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[4].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);

	arrCube[5].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[5].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(0.f, 0.f);

	arrCube[6].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[6].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(0.f, 0.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 0.f);

	// 뒷 면
	arrCube[8].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
			
	arrCube[9].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[9].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(0.f, 0.f);
			
	arrCube[10].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[10].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(0.f, 0.f);
			
	arrCube[11].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(0.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(0.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 0.f);

	// 앞 면
	arrCube[16].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[16].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);

	arrCube[17].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[17].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[17].vUV = Vec2(0.f, 0.f);

	arrCube[18].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[18].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[18].vUV = Vec2(0.f, 0.f);

	arrCube[19].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[19].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 0.f);

	// 아랫 면	
	arrCube[20].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[20].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);

	arrCube[21].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[21].vUV = Vec2(0.f, 0.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[22].vUV = Vec2(0.f, 0.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 0.f);

	//인덱스
	for (int i = 0; i < 2; ++i)
	{
		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 1);
		vecIdx.push_back(i * 4 + 2);
		vecIdx.push_back(i * 4 + 3);
	}
	vecIdx.push_back(4);
	for (int i = 2; i < 3; ++i)
	{
		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 1);
		vecIdx.push_back(i * 4 + 2);
		vecIdx.push_back(i * 4 + 3);
	}
	vecIdx.push_back(8);
	for (int i = 3; i < 4; ++i)
	{
		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 1);
		vecIdx.push_back(i * 4 + 2);
		vecIdx.push_back(i * 4 + 3);
	}
	vecIdx.push_back(12);
	for (int i = 4; i < 6; ++i)
	{
		vecIdx.push_back(i * 4);
		vecIdx.push_back(i * 4 + 1);
		vecIdx.push_back(i * 4 + 2);
		vecIdx.push_back(i * 4 + 3);
	}
	


	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), 24, D3D11_USAGE_DEFAULT, arrCube
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"CubeColliderMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"CubeColliderMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();


	// ============
	// Sphere Mesh
	// ============

	float fRadius = 1.f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	iSliceCount = 40; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vTangent.Cross(v.vNormal, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"SphereMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"SphereMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();

	//=========
	//CONE MESH
	//=========

	iSliceCount = 40;
	fThetaStride = XM_2PI / (float)iSliceCount;
	fTheta = 0.f;

	// 원의 중심
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 테두리 정점
	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{
		v.vPos.x = cosf(fTheta);
		v.vPos.y = 0;
		v.vPos.z = sinf(fTheta);

		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();
		v.vTangent.x = -fRadius * sinf(fTheta);
		v.vTangent.y = 0.f;
		v.vTangent.z = fRadius * cosf(fTheta);
		v.vTangent.Normalize();

		v.vTangent.Cross(v.vNormal, v.vBinormal);
		v.vBinormal.Normalize();
		vecVtx.push_back(v);

		fTheta += fThetaStride;
	}

	fRadius = 1.f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	//바닥 인덱스
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 1);
		vecIdx.push_back(i + 2);
	}
	//사이드 인덱스
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		//마지막점이 TOP점이라 42번으로 넣음
		vecIdx.push_back(42);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->CreateMesh(sizeof(VTX), vecVtx.size(), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, vecIdx.size(), D3D11_USAGE_DEFAULT, DXGI_FORMAT_R32_UINT, &vecIdx[0]);

	pMesh->SetName(L"ConeMesh");
	m_mapRes[(UINT)RES_TYPE::MESH].insert(make_pair(L"ConeMesh", pMesh));

	vecVtx.clear();
	vecIdx.clear();
}

void CResMgr::CreateDefaultShader()
{
	wstring strKey;

	// ===================
	// Vertex Color Shader
	// ===================
	CShader* pShader = nullptr;

	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS", 5, 0);

	strKey = L"VtxColorShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ===================
	// Collider2D Shader
	// ===================	
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Collider2D", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Collider2D", 5, 0);

	pShader->AddParam(SHADER_PARAM::INT_0, L"Collision Check", false);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::WIREFRAME);

	strKey = L"Collider2DShader";
	pShader->SetName(strKey);

	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ===================
	// Collider3D Shader
	// ===================	
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_Collider3D", 5, 0);
	pShader->CreateVertexInstShader(L"Shader\\std3d.fx", "VS_Collider3D_Inst", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_Collider3D", 5, 0);
	
	pShader->AddParam(SHADER_PARAM::INT_0, L"Collision Check", false);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	strKey = L"Collider3DShader";
	pShader->SetName(strKey);
	
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ==============
	// Texture Shader
	// ==============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Tex", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Tex", 5, 0);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));
	pShader->AddParam(SHADER_PARAM::TEX_0, L"Output Image");
	pShader->SetEventTime(SHADER_EVENT_TIME::FORWARD);

	strKey = L"TextureShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));


	// ==============
	// Std2D Shader
	// ==============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\std.fx", "VS_Std2D", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std.fx", "PS_Std2D", 5, 0);
	pShader->SetEventTime(SHADER_EVENT_TIME::FORWARD);

	pShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ALPHABLEND));


	pShader->AddParam(SHADER_PARAM::INT_0, L"INT_0 Test");
	pShader->AddParam(SHADER_PARAM::VEC4_0, L"Vec4_0 Test");
	pShader->AddParam(SHADER_PARAM::TEX_0, L"Output Image");

	strKey = L"Std2DShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ====================
	// Shadow Effect Shader
	// ====================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\effect.fx", "VS_2DShadow", 5, 0);
	pShader->CreatePixelShader(L"Shader\\effect.fx", "PS_2DShadow", 5, 0);

	pShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ALPHABLEND));

	strKey = L"2DShdowShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ==============
	// Skybox Shader
	// ==============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\skybox.fx", "VS_Skybox", 5, 0);
	pShader->CreatePixelShader(L"Shader\\skybox.fx", "PS_Skybox", 5, 0);

	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::LESS_EQUAL));

	strKey = L"SkyboxShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ==============
	// STD3D Shader
	// ==============
	pShader = new CShader;
	pShader->SetEventTime(SHADER_EVENT_TIME::DEFERRED);
	pShader->CreateVertexShader(L"Shader\\std3d.fx", "VS_STD3D", 5, 0);
	pShader->CreateVertexInstShader(L"Shader\\std3d.fx", "VS_STD3D_Inst", 5, 0);
	pShader->CreatePixelShader(L"Shader\\std3d.fx", "PS_STD3D", 5, 0);

	pShader->AddParam(SHADER_PARAM::TEX_0, L"Diffuse Texture");
	pShader->AddParam(SHADER_PARAM::TEX_1, L"NormalMap Texture");
	//pShader->AddParam(SHADER_PARAM::TEX_2, L"Specular Texture");
	//pShader->AddParam(SHADER_PARAM::TEX_3, L"Emissive Texture");

	strKey = L"Std3DShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	//// ==============
	//// Compute Shader
	//// ==============
	//pShader = new CShader;
	//pShader->SetDeferred();
	//pShader->CreateComputeShader(L"Shader\\Compute.fx", "CS_Test", 5, 0);
	//
	////pShader->AddParam(SHADER_PARAM::TEX_0, L"Diffuse Texture");
	////pShader->AddParam(SHADER_PARAM::TEX_1, L"NormalMap Texture");
	////pShader->AddParam(SHADER_PARAM::TEX_2, L"Specular Texture");
	////pShader->AddParam(SHADER_PARAM::TEX_3, L"Emissive Texture");
	//
	//strKey = L"ComputeShader";
	//pShader->SetName(strKey);
	//m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ===================
	// Tesselation Shader
	// ===================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\terrain.fx", "VS_Tess", 5, 0);
	pShader->CreateHullShader(L"Shader\\terrain.fx", "HS_Tess", 5, 0);
	pShader->CreateDomainShader(L"Shader\\terrain.fx", "DS_Tess", 5, 0);
	pShader->CreatePixelShader(L"Shader\\terrain.fx", "PS_Tess", 5, 0);

	//pShader->SetRSType(RS_TYPE::WIREFRAME);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	strKey = L"TessShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ==============
	// Terrain Shader
	// ==============
	pShader = new CShader;
	pShader->SetEventTime(SHADER_EVENT_TIME::DEFERRED);
	pShader->CreateVertexShader(L"Shader\\terrain.fx", "VS_Terrain", 5, 0);
	pShader->CreateHullShader(L"Shader\\terrain.fx", "HS_Terrain", 5, 0);
	pShader->CreateDomainShader(L"Shader\\terrain.fx", "DS_Terrain", 5, 0);
	pShader->CreatePixelShader(L"Shader\\terrain.fx", "PS_Terrain", 5, 0);

	//pShader->SetRSType(RS_TYPE::WIREFRAME);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	pShader->AddParam(SHADER_PARAM::TEX_0, L"Diffuse Texture");
	pShader->AddParam(SHADER_PARAM::TEX_1, L"NormalMap Texture");
	pShader->AddParam(SHADER_PARAM::TEX_2, L"Height Map");

	strKey = L"TerrainShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));


	// ========================
	// Directional Light Shader
	// ========================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_DirLight", 5, 0);
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_DirLight", 5, 0);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));
	// pShader->AddParam(SHADER_PARAM::TEX_0, L"Normal Target Texture");
	// pShader->AddParam(SHADER_PARAM::TEX_1, L"Position Target Texture");	
	// pShader->AddParam(SHADER_PARAM::INT_0, L"Light Index");

	strKey = L"DirLightShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));


	// ========================
	// Point Light Shader
	// ========================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_PointLight", 5, 0);
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_PointLight", 5, 0);

	pShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ONE_ONE));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));
	pShader->SetRSType(RS_TYPE::CULL_FRONT);

	strKey = L"PointLightShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ========================
	// Spot Light Shader
	// ========================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_SpotLight", 5, 0);
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_SpotLight", 5, 0);

	pShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ONE_ONE));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));
	pShader->SetRSType(RS_TYPE::CULL_FRONT);

	strKey = L"SpotLightShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// =============
	// Merge Shader
	// =============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_Merge", 5, 0);
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_Merge", 5, 0);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_WRITE));

	strKey = L"MergeShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	//================
	//ShadowMap Shader
	//================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\light.fx", "VS_ShadowMap", 5, 0);
	pShader->CreatePixelShader(L"Shader\\light.fx", "PS_ShadowMap", 5, 0);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::DEFAULT));

	strKey = L"ShadowMapShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// =============
	// ClearTex Shader
	// =============
	pShader = new CShader;
	pShader->CreateComputeShader(L"Shader\\compute.fx", "CS_Clear", 5, 0);

	strKey = L"ClearTextureShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	//===================
	// Distortion Shader
	//===================
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\posteffect.fx", "VS_Distortion", 5, 0);
	pShader->CreatePixelShader(L"Shader\\posteffect.fx", "PS_Distortion", 5, 0);

	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_WRITE));

	// Shader Param
	pShader->AddParam(SHADER_PARAM::TEX_0, L"Posteffect Target Texture");

	// Shader Event Time
	pShader->SetEventTime(SHADER_EVENT_TIME::POSTEFFECT);

	strKey = L"DistortionShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));

	// ===============
	// Particle Shader
	// ===============
	pShader = new CShader;
	pShader->CreateVertexShader(L"Shader\\particle.fx", "VS_Particle", 5, 0);
	pShader->CreateGeometryShader(L"Shader\\particle.fx", "GS_Particle", 5, 0);
	pShader->CreatePixelShader(L"Shader\\particle.fx", "PS_Particle", 5, 0);

	pShader->SetEventTime(SHADER_EVENT_TIME::PARTICLE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetBlendState(CRenderMgr::GetInst()->GetBlendState(BLEND_TYPE::ALPHABLEND));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->GetDepthStencilState(DEPTH_STENCIL_TYPE::NO_DEPTH_WRITE));

	strKey = L"ParticleShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));


	// ======================
	// Particle Update Shader
	// ======================
	pShader = new CShader;
	pShader->CreateComputeShader(L"Shader\\particle.fx", "CS_ParticleUpdate", 5, 0);
	pShader->SetEventTime(SHADER_EVENT_TIME::PARTICLE);

	strKey = L"ParticleUpdateShader";
	pShader->SetName(strKey);
	m_mapRes[(UINT)RES_TYPE::SHADER].insert(make_pair(strKey, pShader));
}

void CResMgr::CreateDefaultMaterial()
{
	CResPtr<CMaterial> pMtrl = nullptr;

	// Color Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\VtxColorMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"VtxColorShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Collider2D Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\Collider2DMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"Collider2DShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Collider3D Mtrl
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\Collider3DMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"Collider3DShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std2D
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\Std2DMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"Std2DShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Texture
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\TextureMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"TextureShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// SkyBox
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\Skybox.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"SkyboxShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Std3D
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\Std3DMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"Std3DShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	//// Compute
	//pMtrl = new CMaterial;
	//pMtrl->SetName(L"Material\\Compute.mtrl");
	//pMtrl->SetShader(FindRes<CShader>(L"ComputeShader"));
	//pMtrl->SaveDisable();
	//AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// tesselation Test
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\TessMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"TessShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// terrain
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\TerrainMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"TerrainShader"));
	pMtrl->SaveDisable();
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Directional Light
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\DirLightMtrl.mtrl");
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"DirLightShader"));

	CResPtr<CTexture> pTex = FindRes<CTexture>(L"NormalTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_0, &pTex);

	pTex = FindRes<CTexture>(L"PositionTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_1, &pTex);

	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Point Light
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\PointLightMtrl.mtrl");
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"PointLightShader"));

	pTex = FindRes<CTexture>(L"NormalTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_0, &pTex);

	pTex = FindRes<CTexture>(L"PositionTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_1, &pTex);

	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Spot Light
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\SpotLightMtrl.mtrl");
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"SpotLightShader"));

	pTex = FindRes<CTexture>(L"NormalTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_0, &pTex);

	pTex = FindRes<CTexture>(L"PositionTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_1, &pTex);

	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Merge
	pMtrl = new CMaterial;
	pMtrl->SetName(L"Material\\MergeMtrl.mtrl");
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"MergeShader"));

	pTex = FindRes<CTexture>(L"DiffuseTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_0, &pTex);

	pTex = FindRes<CTexture>(L"LightTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_1, &pTex);

	pTex = FindRes<CTexture>(L"SpecularTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_2, &pTex);

	pTex = FindRes<CTexture>(L"PositionTargetTex");
	pMtrl->SetData(SHADER_PARAM::TEX_3, &pTex);

	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	//Clear Texture Material
	pMtrl = new CMaterial;
	pMtrl->SaveDisable();
	pMtrl->SetName(L"Material\\ClearTextureMtrl.mtrl");
	pMtrl->SetShader(FindRes<CShader>(L"ClearTextureShader"));
	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	CTexture::g_pClearMtrl = (CMaterial*)pMtrl.GetPointer();

	//ShadowMapShader
	pMtrl = new CMaterial;
	pMtrl->SetName(L"ShadowMapMtrl");
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"ShadowMapShader"));

	AddRes<CMaterial>(pMtrl->GetName(), pMtrl);

	// Particle Mtrl
	pMtrl = new CMaterial;
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"ParticleShader"));
	AddRes(L"ParticleMtrl", pMtrl);

	// Particle Update
	pMtrl = new CMaterial;
	pMtrl->SaveDisable();
	pMtrl->SetShader(FindRes<CShader>(L"ParticleUpdateShader"));
	CResPtr<CTexture> pNoiseTex = Load<CTexture>(L"Texture\\noise.png", L"Texture\\noise.png");
	pMtrl->SetData(SHADER_PARAM::TEX_0, &pNoiseTex);
	pMtrl->SetData(SHADER_PARAM::VEC2_0, &Vec2(pNoiseTex->GetWidth(), pNoiseTex->GetHeight()));
	AddRes(L"ParticleUpdateMtrl", pMtrl);

	CCollider3D::CreateMaterial();

}

void CResMgr::CreateDefaultFilter()
{
	CResPtr<CMaterial> pMtrl = nullptr;
	CResPtr<CMesh> pMesh = nullptr;

	CFilter23* pFilter = new CFilter23;

	pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Material\\MergeMtrl.mtrl");

	pFilter->Create(pMesh, pMtrl);
	pFilter->SetName(L"MergeFilter");
	m_mapRes[(UINT)RES_TYPE::FILTER].insert(make_pair(pFilter->GetName(), pFilter));

	CRenderMgr::GetInst()->SetMergeFilter(pFilter);
}