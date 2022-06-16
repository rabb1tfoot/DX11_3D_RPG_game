#include "stdafx.h"
#include "CollisionMgr.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Core.h"

#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"
#include "Collider2D.h"
#include "Collider3D.h"
#include "Camera.h"

#include "Transform.h"

#include "ResMgr.h"


CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::update()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		for (UINT j = i; j < MAX_LAYER; ++j)
		{
			if (m_arrCheck[i] & (1 << j))
			{
				// i ObjTYpe, j ObjType 충돌 진행
				CollisionGroup3D(i, j);
			}
		}
		//마우스 클릭시
		if (KEYTAB(KEY_TYPE::KEY_LBTN))
		{
			CheckMouseClick(i);
		}

	}
}

void CCollisionMgr::CollisionGroup(int _leftIdx, int _rightIdx)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	const vector<CGameObject*>& vecLeft = pCurScene->GetLayer(_leftIdx)->GetAllObject();
	const vector<CGameObject*>& vecRight = pCurScene->GetLayer(_rightIdx)->GetAllObject();

	COL_ID colID;

	for (UINT i = 0; i < vecLeft.size(); ++i)
	{
		for (UINT j = 0; j < vecRight.size(); ++j)
		{
			// 두 오브젝트 중에서 충돌체가 없는 경우가 있다면
			if (vecLeft[i]->Collider2D() == nullptr || vecRight[j]->Collider2D() == nullptr)
				continue;

			colID.left = vecLeft[i]->Collider2D()->GetID();
			colID.right = vecRight[j]->Collider2D()->GetID();

			map<unsigned long long, bool>::iterator iter = m_mapID2D.find(colID.ID);

			if (IsCollision(vecLeft[i]->Collider2D(), vecRight[j]->Collider2D()))
			{
				// 충돌 했다.
				if (iter == m_mapID2D.end())
				{
					// 충돌 조합 등록된적 없음 ==> 충돌한적 없음					
					vecLeft[i]->Collider2D()->OnCollisionEnter(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollisionEnter(vecLeft[i]->Collider2D());
					m_mapID2D.insert(make_pair(colID.ID, true));
				}
				else if (false == iter->second)
				{
					// 이전에 충돌하지 않고 있었다.
					vecLeft[i]->Collider2D()->OnCollisionEnter(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollisionEnter(vecLeft[i]->Collider2D());
					iter->second = true;
				}
				else {
					// 충돌 중이다.
					vecLeft[i]->Collider2D()->OnCollision(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollision(vecLeft[i]->Collider2D());
				}
			}
			else // 충돌하지 않고 있다.
			{
				if (iter != m_mapID2D.end() && true == iter->second)
				{
					// 이전에는 충돌 중이었다. ==>이번에 떨어진 상황
					vecLeft[i]->Collider2D()->OnCollisionExit(vecRight[j]->Collider2D());
					vecRight[j]->Collider2D()->OnCollisionExit(vecLeft[i]->Collider2D());
					iter->second = false;
				}
			}
		}
	}
}

void CCollisionMgr::CollisionGroup3D(int _leftIdx, int _rightIdx)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	const vector<CGameObject*>& vecLeft = pCurScene->GetLayer(_leftIdx)->GetAllObject();
	const vector<CGameObject*>& vecRight = pCurScene->GetLayer(_rightIdx)->GetAllObject();

	COL_ID colID;

	for (UINT i = 0; i < vecLeft.size(); ++i)
	{
		for (UINT j = 0; j < vecRight.size(); ++j)
		{
			// 두 오브젝트 중에서 충돌체가 없는 경우가 있다면
			if (vecLeft[i]->Collider3D() == nullptr || vecRight[j]->Collider3D() == nullptr
				|| vecLeft[i]->Collider3D() == vecRight[j]->Collider3D())
				continue;

			colID.left = vecLeft[i]->Collider3D()->GetID();
			colID.right = vecRight[j]->Collider3D()->GetID();

			map<unsigned long long, bool>::iterator iter = m_mapID3D.find(colID.ID);

			if (IsCollision(vecLeft[i]->Collider3D(), vecRight[j]->Collider3D()))
			{
				// 충돌 했다.
				if (iter == m_mapID3D.end())
				{
					// 충돌 조합 등록된적 없음 ==> 충돌한적 없음					
					vecLeft[i]->Collider3D()->OnCollisionEnter(vecRight[j]->Collider3D());
					vecRight[j]->Collider3D()->OnCollisionEnter(vecLeft[i]->Collider3D());
					m_mapID3D.insert(make_pair(colID.ID, true));
				}
				else if (false == iter->second)
				{
					// 이전에 충돌하지 않고 있었다.
					vecLeft[i]->Collider3D()->OnCollisionEnter(vecRight[j]->Collider3D());
					vecRight[j]->Collider3D()->OnCollisionEnter(vecLeft[i]->Collider3D());
					iter->second = true;
				}
				else {
					// 충돌 중이다.
					vecLeft[i]->Collider3D()->OnCollision(vecRight[j]->Collider3D());
					vecRight[j]->Collider3D()->OnCollision(vecLeft[i]->Collider3D());
				}
			}
			else // 충돌하지 않고 있다.
			{
				if (iter != m_mapID3D.end() && true == iter->second)
				{
					// 이전에는 충돌 중이었다. ==>이번에 떨어진 상황
					vecLeft[i]->Collider3D()->OnCollisionExit(vecRight[j]->Collider3D());
					vecRight[j]->Collider3D()->OnCollisionExit(vecLeft[i]->Collider3D());
					iter->second = false;
				}
			}
		}
	}
}

bool CCollisionMgr::IsCollision(CCollider2D * _pLeft, CCollider2D * _pRight)
{
	if (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::RECT && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::RECT)
	{
		// Rect Rect 충돌
		return CollisionRect(_pLeft, _pRight);
	}
	else if (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::CIRCLE && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::CIRCLE)
	{
		// Circle Circle

	}
	else
	{
		// Rect, Circle
	}

	return false;
}

bool CCollisionMgr::CollisionRect(CCollider2D * _pLeft, CCollider2D * _pRight)
{
	static CResPtr<CMesh> pRectMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ColliderRectMesh");

	Matrix matLeftWorld = _pLeft->GetWorldMat();
	Matrix matRightWorld = _pRight->GetWorldMat();

	VTX* pVtx = (VTX*)pRectMesh->GetVtxSysMem();

	Vec3 vLeftWorldPos[4] = {};
	Vec3 vRightWorldPos[4] = {};

	for (UINT i = 0; i < 4; ++i)
	{
		vLeftWorldPos[i] = XMVector3TransformCoord(pVtx[i].vPos, matLeftWorld);
		vLeftWorldPos[i].z = 0.f;
	}

	for (UINT i = 0; i < 4; ++i)
	{
		vRightWorldPos[i] = XMVector3TransformCoord(pVtx[i].vPos, matRightWorld);
		vRightWorldPos[i].z = 0.f;
	}

	Vec3 vLeftCenter = (vLeftWorldPos[3] + vLeftWorldPos[0]) / 2.f;
	Vec3 vRightCenter = (vRightWorldPos[3] + vRightWorldPos[0]) / 2.f;

	Vec3 vCenter = vLeftCenter - vRightCenter;

	Vec3 vProj[4] = {};
	Vec3 vAxis[4] = {};

	vProj[0] = vLeftWorldPos[0] - vLeftWorldPos[2];
	vAxis[0] = vProj[0];

	vProj[1] = vLeftWorldPos[3] - vLeftWorldPos[2];
	vAxis[1] = vProj[1];

	vProj[2] = vRightWorldPos[0] - vRightWorldPos[2];
	vAxis[2] = vProj[2];

	vProj[3] = vRightWorldPos[3] - vRightWorldPos[2];
	vAxis[3] = vProj[3];

	for (UINT i = 0; i < 4; ++i)
	{
		if (vAxis[i] == Vec3::Zero)
			return false;

		vAxis[i].Normalize();
	}

	float fSum = 0.f;

	for (UINT i = 0; i < 4; ++i)
	{
		fSum = 0.f;
		for (UINT j = 0; j < 4; ++j)
		{
			fSum += abs(vProj[j].Dot(vAxis[i]));
		}

		// 충돌체 각 변을 투영시킨 거리의 절반
		fSum /= 2.f;

		// 중심을 이은 벡터를 투영시킨 거리
		float fDist = abs(vCenter.Dot(vAxis[i]));

		if (fDist > fSum)
			return false;
	}

	return true;
}

bool CCollisionMgr::CollisionCircle(CCollider2D * _pLeft, CCollider2D * _pRight)
{
	return false;
}

bool CCollisionMgr::IsCollision(CCollider3D * _pLeft, CCollider3D * _pRight)
{
	if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::CUBE && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::CUBE)
	{
		// Cube 충돌
		return CollisionCube(_pLeft, _pRight);
	}
	else if (_pLeft->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE && _pRight->GetCollider3DType() == COLLIDER3D_TYPE::SPHERE)
	{
		// SPHERE 충돌

	}
	else
	{
	}

	return false;
}


bool CCollisionMgr::CollisionCube(CCollider3D * _pLeft, CCollider3D * _pRight)
{
	static CResPtr<CMesh> pCubeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeColliderMesh");

	Matrix matLeftWorld = _pLeft->GetWorldMat();
	Matrix matRightWorld = _pRight->GetWorldMat();
	bool bRot = false;

	VTX* pVtx = (VTX*)pCubeMesh->GetVtxSysMem();

	Vec3 vLeftWorldPos[4] = {};
	Vec3 vRightWorldPos[4] = {};

	//아랫면 접점 가져오기
	//큐브의 밑변 끼리만 검사함 어자피 높이를 이용해서 할일은 없으니까...
	vLeftWorldPos[0] = XMVector3TransformCoord(pVtx[0].vPos, matLeftWorld);
	vLeftWorldPos[1] = XMVector3TransformCoord(pVtx[1].vPos, matLeftWorld);
	vLeftWorldPos[3] = XMVector3TransformCoord(pVtx[2].vPos, matLeftWorld);
	vLeftWorldPos[2] = XMVector3TransformCoord(pVtx[3].vPos, matLeftWorld);
	vRightWorldPos[0] = XMVector3TransformCoord(pVtx[0].vPos, matRightWorld);
	vRightWorldPos[1] = XMVector3TransformCoord(pVtx[1].vPos, matRightWorld);
	vRightWorldPos[3] = XMVector3TransformCoord(pVtx[2].vPos, matRightWorld);
	vRightWorldPos[2] = XMVector3TransformCoord(pVtx[3].vPos, matRightWorld);
	for (int i = 0; i < 4; ++i)
	{
		vLeftWorldPos[i].y = 0.f;
		vRightWorldPos[i].y = 0.f;
	}

	Vec3 vLeftCenter = (vLeftWorldPos[3] + vLeftWorldPos[0]) / 2.f;
	Vec3 vRightCenter = (vRightWorldPos[3] + vRightWorldPos[0]) / 2.f;

	Vec3 vCenter = vLeftCenter - vRightCenter;

	Vec3 vProj[4] = {};
	Vec3 vAxis[4] = {};

	vProj[0] = vLeftWorldPos[0] - vLeftWorldPos[2];
	vAxis[0] = vProj[0];

	vProj[1] = vLeftWorldPos[3] - vLeftWorldPos[2];
	vAxis[1] = vProj[1];

	vProj[2] = vRightWorldPos[0] - vRightWorldPos[2];
	vAxis[2] = vProj[2];

	vProj[3] = vRightWorldPos[3] - vRightWorldPos[2];
	vAxis[3] = vProj[3];

	for (UINT i = 0; i < 4; ++i)
	{
		if (vAxis[i] == Vec3::Zero)
			return false;

		vAxis[i].Normalize();
	}

	float fSum = 0.f;

	for (UINT i = 0; i < 4; ++i)
	{
		fSum = 0.f;
		for (UINT j = 0; j < 4; ++j)
		{
			fSum += abs(vProj[j].Dot(vAxis[i]));
		}

		// 충돌체 각 변을 투영시킨 거리의 절반
		fSum /= 2.f;

		// 중심을 이은 벡터를 투영시킨 거리
		float fDist = abs(vCenter.Dot(vAxis[i]));

		if (fDist > fSum)
			return false;
	}

	return true;
}

bool CCollisionMgr::CollisionSphere(CCollider3D * _pLeft, CCollider3D * _pRight)
{
	return false;
}

void CCollisionMgr::CheckMouseClick(int _iLayer)
{
	//2d 또는 3d collider가 있는경우만 체크
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pCurLayer = pCurScene->GetLayer(_iLayer);
	int iUIidx = pCurScene->FindLayer(L"UI")->GetLayerIdx();
	if (pCurLayer == nullptr)
		return;
	const vector<CGameObject*>& vecObject = pCurLayer->GetAllObject();

	for (size_t i = 0; i < vecObject.size(); ++i)
	{
		if (vecObject[i]->Collider2D() == nullptr && vecObject[i]->Collider3D() == nullptr)
			continue;

		//2D 물체 마우스 피킹체크
		if (vecObject[i]->Collider2D() != nullptr && vecObject[i]->GetLayerIdx() == iUIidx)
		{
			POINT mousePos = MOUSEPOS;
			tResolution vRes = CRenderMgr::GetInst()->GetInst()->GetResolution();

			//나중에 클라로 하면 바꿔야함 스타트버튼 자리때문에
			mousePos.x -= 1120 / 2;
			mousePos.y *= -1; mousePos.y += 630 / 2;

			Vec3 objPos = vecObject[i]->Collider2D()->GetFinalPos();
			Vec3 objSize = vecObject[i]->Transform()->GetLocalScale();

			Vec2 ObjLT = Vec2(objPos.x - objSize.x / 2.f, objPos.y - objSize.y / 2.f);
			Vec2 ObjRB = Vec2(objPos.x + objSize.x / 2.f, objPos.y + objSize.y / 2.f);

			if (ObjLT.x  < mousePos.x && ObjRB.x > mousePos.x
				&& ObjLT.y  < mousePos.y && ObjRB.y > mousePos.y)
			{
				vecObject[i]->Collider2D()->OnCollisionClick();
			}

		}
		//3D 물체 마우스 피킹체크
		if (vecObject[i]->Collider3D() != nullptr)
		{
			const tRay ray = m_pMainCam->Camera()->GetRay();
			//삼각형정점을 모두 가져와서 인터섹트한뒤 하나라도 true면 true
			static CResPtr<CMesh> pCubeMouseMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CubeColliderMesh");

			Matrix matLeftWorld = vecObject[i]->Collider3D()->GetWorldMat();

			VTX* pVtx = (VTX*)pCubeMouseMesh->GetVtxSysMem();

			Vec3 vWorldPos[24] = {};
			//월드 정점 가져오기
			for (UINT i = 0; i < 24; ++i)
			{
				vWorldPos[i] = XMVector3TransformCoord(pVtx[i].vPos, matLeftWorld);
			}
			//인덱스 버퍼 가져오기
			const tIndexInfo& info = pCubeMouseMesh->GetIdxInfo();
			int iSize = pCubeMouseMesh->GetIdxCount() / 3;
			UINT* pIdx = (UINT*)pCubeMouseMesh->GetIdxSysMem();
			float iOut;
			XMFLOAT3 xmStart = (XMFLOAT3)((Vector3)ray.vStart);
			XMFLOAT3 xmDir = (XMFLOAT3)((Vector3)ray.vDir);
			XMVECTOR vStart = XMLoadFloat3(&xmStart);
			XMVECTOR vDir = XMLoadFloat3(&xmDir);
			XMVECTOR vPos1{};
			XMVECTOR vPos2{};
			XMVECTOR vPos3{};
			//검사
			for (int j = 0; j < iSize; ++j)
			{
				XMFLOAT3 xmpos1 = vWorldPos[pIdx[j * 3 + 0]];
				XMFLOAT3 xmpos2 = vWorldPos[pIdx[j * 3 + 1]];
				XMFLOAT3 xmpos3 = vWorldPos[pIdx[j * 3 + 2]];
				vPos1 = XMLoadFloat3(&xmpos1);
				vPos2 = XMLoadFloat3(&xmpos2);
				vPos3 = XMLoadFloat3(&xmpos3);
				if (Intersects(vStart, vDir, vPos1, vPos2, vPos3, iOut))
				{
					//피킹성공
					vecObject[i]->Collider3D()->OnCollisionClick3D();
				}
				int b = 0;
			}
		}
	}
}

void CCollisionMgr::CollisionCheck(const wstring & _strLayerName1, const wstring & _strLayerName2)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLeftLayer = pCurScene->FindLayer(_strLayerName1);
	CLayer* pRightLayer = pCurScene->FindLayer(_strLayerName2);

	assert(!(nullptr == pLeftLayer || nullptr == pRightLayer));

	CollisionCheck(pLeftLayer->GetLayerIdx(), pRightLayer->GetLayerIdx());
}

void CCollisionMgr::CollisionCheck(int _iLayerIdx1, int _iLayerIdx2)
{
	// 배열의 인덱스
	UINT iIdx = (UINT)(_iLayerIdx1 < _iLayerIdx2 ? _iLayerIdx1 : _iLayerIdx2);
	UINT iBitIdx = iIdx == (UINT)_iLayerIdx1 ? (UINT)_iLayerIdx2 : (UINT)_iLayerIdx1;
	m_arrCheck[iIdx] |= (1 << iBitIdx);
}

bool CCollisionMgr::CheckSperateAxisFaces(Vec3 _vCenter, const Vec3* _vLeftWorldPos, const Vec3* _vRightWorldPos)
{
	Vec3 vProj[4] = {};
	Vec3 vAxis[4] = {};

	vProj[0] = _vLeftWorldPos[0] - _vLeftWorldPos[2];
	vAxis[0] = vProj[0];

	vProj[1] = _vLeftWorldPos[3] - _vLeftWorldPos[1];
	vAxis[1] = vProj[1];

	vProj[2] = _vRightWorldPos[0] - _vRightWorldPos[2];
	vAxis[2] = vProj[2];

	vProj[3] = _vRightWorldPos[3] - _vRightWorldPos[1];
	vAxis[3] = vProj[3];

	for (UINT i = 0; i < 4; ++i)
	{
		if (vAxis[i] == Vec3::Zero)
			return false;

		vAxis[i].Normalize();
	}

	float fSum = 0.f;

	for (UINT i = 0; i < 4; ++i)
	{
		fSum = 0.f;
		for (UINT j = 0; j < 4; ++j)
		{
			fSum += abs(vProj[j].Dot(vAxis[i]));
		}
		fSum /= 2.f;

		// 중심을 이은 벡터를 투영시킨 거리
		float fDist = abs(_vCenter.Dot(vAxis[i]));

		if (fDist > fSum)
			return false;
	}

	return true;
}

bool CCollisionMgr::CheckSperateAxisEdges(Vec3 * _vLeftWorldPos, Vec3* _vRightWorldPos, const Vec3& _vLeftCenter, const Vec3& _vRightCenter)
{
	Vec3 vProj[6] = {};
	Vec3 vAxis[6] = {};
	float extent[6] = {};

	//왼쪽 큐브
	vProj[0] = _vLeftWorldPos[1] - _vLeftWorldPos[0]; //x축
	vAxis[0] = vProj[0];
	extent[0] = _vLeftWorldPos[1].x - _vLeftCenter.x;

	vProj[1] = _vLeftWorldPos[1] - _vLeftWorldPos[2]; //z축
	vAxis[1] = vProj[1];
	extent[1] = _vLeftWorldPos[1].z - _vLeftCenter.z;

	vProj[2] = _vLeftWorldPos[9] - _vLeftWorldPos[10]; //y축
	vAxis[2] = vProj[2];
	extent[2] = _vLeftWorldPos[1].y - _vLeftCenter.y;

	//오른쪽 큐브
	vProj[3] = _vRightWorldPos[1] - _vRightWorldPos[0]; //x축
	vAxis[3] = vProj[3];
	extent[3] = _vRightWorldPos[1].x - _vRightCenter.x;

	vProj[4] = _vRightWorldPos[1] - _vRightWorldPos[2]; //z축
	vAxis[4] = vProj[4];
	extent[4] = _vRightWorldPos[1].z - _vRightCenter.z;

	vProj[5] = _vRightWorldPos[9] - _vRightWorldPos[10]; //y축
	vAxis[5] = vProj[5];
	extent[5] = _vRightWorldPos[1].y - _vRightCenter.y;

	for (UINT i = 0; i < 6; ++i)
	{
		if (vAxis[i] == Vec3::Zero)
			return false;

		vAxis[i].Normalize();

		if (extent[i] < 0)
			extent[i] *= -1;
	}


	Vec3 CenterDistance = Vec3(fabs(_vLeftCenter.x - _vRightCenter.x), fabs(_vLeftCenter.y - _vRightCenter.y), fabs(_vLeftCenter.z - _vRightCenter.z));

	Vec3 C[3] = {};
	Vec3 AD;
	Vec3 absC[3] = {};
	float R, R0, R1, R01;

	C[0].x = vAxis[0].Dot(vAxis[3]);
	C[0].y = vAxis[0].Dot(vAxis[4]);
	C[0].z = vAxis[0].Dot(vAxis[5]);
	AD.x = vAxis[0].Dot(CenterDistance);
	absC[0].x = fabs(C[0].x);
	absC[0].y = fabs(C[0].y);
	absC[0].z = fabs(C[0].z);
	R = fabs(AD[0]);
	R1 = extent[3] * absC[0][0] + extent[5] * absC[0][1] + extent[4] * absC[0][2];
	R01 = extent[0] + R1;
	if (R > R01)return false;

	C[1].x = vAxis[1].Dot(vAxis[3]);
	C[1].y = vAxis[1].Dot(vAxis[4]);
	C[1].z = vAxis[1].Dot(vAxis[5]);
	AD[1] = vAxis[1].Dot(CenterDistance);
	absC[1].x = fabs(C[1].x);
	absC[1].y = fabs(C[1].y);
	absC[1].z = fabs(C[1].z);
	R = fabs(AD[1]);
	R1 = extent[3] * absC[1][0] + extent[5] * absC[1][1] + extent[4] * absC[1][2];
	R01 = extent[1] + R1;
	if (R > R01)return false;

	C[2].x = vAxis[2].Dot(vAxis[3]);
	C[2].y = vAxis[2].Dot(vAxis[4]);
	C[2].z = vAxis[2].Dot(vAxis[5]);
	AD[2] = vAxis[2].Dot(CenterDistance);
	absC[2].x = fabs(C[2].x);
	absC[2].y = fabs(C[2].y);
	absC[2].z = fabs(C[2].z);
	R = fabs(AD[2]);
	R1 = extent[3] * absC[2][0] + extent[5] * absC[2][1] + extent[4] * absC[2][2];
	R01 = extent[2] + R1;
	if (R > R01)return false;

	R = (fabs(vAxis[3].Dot(CenterDistance)));
	R0 = extent[0] * absC[0][0] + extent[2] * absC[1][0] + extent[1] * absC[2][0];
	R01 = R0 + extent[3];
	if (R > R01)return false;

	R = (fabs(vAxis[4].Dot(CenterDistance)));
	R0 = extent[0] * absC[0][1] + extent[2] * absC[1][1] + extent[1] * absC[2][1];
	R01 = R0 + extent[4];
	if (R > R01)return false;

	R = (fabs(vAxis[5].Dot(CenterDistance)));
	R0 = extent[0] * absC[0][2] + extent[2] * absC[1][2] + extent[1] * absC[2][2];
	R01 = R0 + extent[5];
	if (R > R01)return false;

	//box0.extent0 -> extent 0, box0.extent1 -> extent 2, box0.extent2 -> extent 1
	//box1.extent0 -> extent 3, box1.extent1 -> extent 5, box1.extent2 -> extent 4

	//A0 * B0
	R = (fabs(AD[2] * C[1][0] - AD[1] * C[2][0]));
	R0 = extent[2] * absC[2][0] + extent[1] * absC[1][0];
	R1 = extent[5] * absC[0][2] + extent[4] * absC[0][1];
	R01 = R0 + R1;
	if (R > R01) return false;

	//A0 * B1
	R = (fabs(AD[2] * C[1][1] - AD[1] * C[2][1]));
	R0 = extent[2] * absC[2][1] + extent[1] * absC[1][1];
	R1 = extent[3] * absC[0][2] + extent[4] * absC[0][0];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A0 * B2
	R = (fabs(AD[2] * C[1][2] - AD[1] * C[2][2]));
	R0 = extent[2] * absC[2][2] + extent[1] * absC[1][2];
	R1 = extent[3] * absC[0][1] + extent[5] * absC[0][0];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A1 * B0
	R = (fabs(AD[0] * C[2][0] - AD[2] * C[0][0]));
	R0 = extent[0] * absC[2][0] + extent[1] * absC[0][0];
	R1 = extent[5] * absC[1][2] + extent[4] * absC[1][1];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A1 * B1
	R = (fabs(AD[0] * C[2][1] - AD[2] * C[0][1]));
	R0 = extent[0] * absC[2][1] + extent[1] * absC[0][1];
	R1 = extent[3] * absC[1][2] + extent[4] * absC[1][0];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A1 * B2
	R = (fabs(AD[0] * C[2][2] - AD[2] * C[0][2]));
	R0 = extent[0] * absC[2][2] + extent[1] * absC[0][2];
	R1 = extent[3] * absC[1][1] + extent[5] * absC[1][0];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A2 * B0
	R = (fabs(AD[1] * C[0][0] - AD[0] * C[1][0]));
	R0 = extent[0] * absC[1][0] + extent[2] * absC[0][0];
	R1 = extent[5] * absC[2][2] + extent[4] * absC[2][1];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A2 * B1
	R = (fabs(AD[1] * C[0][1] - AD[0] * C[1][1]));
	R0 = extent[0] * absC[1][1] + extent[2] * absC[0][1];
	R1 = extent[3] * absC[2][2] + extent[5] * absC[2][0];
	R01 = R0 + R1;
	if (R > R01)return false;

	//A2 * B2
	R = (fabs(AD[1] * C[0][2] - AD[0] * C[1][2]));
	R0 = extent[0] * absC[1][2] + extent[2] * absC[0][2];
	R1 = extent[3] * absC[2][1] + extent[5] * absC[2][0];
	R01 = R0 + R1;
	if (R > R01)return false;

}
