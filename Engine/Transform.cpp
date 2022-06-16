#include "stdafx.h"
#include "Transform.h"

#include "Device.h"
#include "ConstBuffer.h"

Vec3 vAsis[3] = { Vec3::Right, Vec3::Up, Vec3::Front };

tTransform g_transform;

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
{
}

CTransform::~CTransform()
{
}

void CTransform::update()
{
}

void CTransform::finalupdate()
{
	Matrix matScale = XMMatrixScaling(m_vLocalScale.x, m_vLocalScale.y, m_vLocalScale.z);

	//Matrix matRotation = XMMatrixRotationRollPitchYaw(m_vLocalRot.x, m_vLocalRot.y, m_vLocalRot.z);
	//matRotation *= m_matAddRot;

	Matrix matRotation = XMMatrixRotationX(m_vLocalRot.x);
	matRotation *= XMMatrixRotationY(m_vLocalRot.y);
	matRotation *= XMMatrixRotationZ(m_vLocalRot.z);
	matRotation *= m_matAddRot;

	// 추가 회전행렬이 있는 경우
	if (m_matAddRot != Matrix::Identity)
	{
		// 오일러 각을 다시 재 계산한다.
		m_vLocalRot = DecomposeRotMat(matRotation);
	}

	Matrix matTrans = XMMatrixTranslation(m_vLocalPos.x, m_vLocalPos.y, m_vLocalPos.z);

	m_matWorld = matScale * matRotation * matTrans;

	// Local Dir 계산하기
	m_vLocalDir[(UINT)DIR_TYPE::DIR_RIGHT] = XMVector3TransformNormal(Vec3::Right, matRotation);
	m_vLocalDir[(UINT)DIR_TYPE::DIR_UP] = XMVector3TransformNormal(Vec3::Up, matRotation);
	m_vLocalDir[(UINT)DIR_TYPE::DIR_FRONT] = XMVector3TransformNormal(Vec3::Front, matRotation);

	m_vLocalDir[(UINT)DIR_TYPE::DIR_RIGHT].Normalize();
	m_vLocalDir[(UINT)DIR_TYPE::DIR_UP].Normalize();
	m_vLocalDir[(UINT)DIR_TYPE::DIR_FRONT].Normalize();

	// 부모 오브젝트가 있으면, 부모 월드행렬을 뒤에 곱해준다.
	CGameObject* pParentObj = ParentObject();
	if (pParentObj)
	{
		const Matrix& matParentWorld = pParentObj->Transform()->GetWorldMat();
		m_matWorld *= matParentWorld;

		for (UINT i = 0; i < (UINT)DIR_TYPE::END; ++i)
		{
			m_vWorldDir[i] = XMVector3TransformNormal(vAsis[i], m_matWorld);
			m_vWorldDir[i].Normalize();
		}
	}

	m_matAddRot = Matrix::Identity;
}

void CTransform::UpdateData()
{
	g_transform.matWorld = m_matWorld;
	g_transform.matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
	g_transform.matWV = m_matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	static CConstBuffer* pCB = CDevice::GetInst()->FindConstBuffer(L"Transform");

	pCB->AddData(&g_transform, sizeof(tTransform));
	pCB->UpdateData();
	pCB->SetRegisterAll();
}

void CTransform::SaveToScene(FILE * _pFile)
{
	fwrite(&m_vLocalPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vLocalScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vLocalRot, sizeof(Vec3), 1, _pFile);
}

void CTransform::LoadFromScene(FILE * _pFile)
{
	fread(&m_vLocalPos, sizeof(Vec3), 1, _pFile);
	fread(&m_vLocalScale, sizeof(Vec3), 1, _pFile);
	fread(&m_vLocalRot, sizeof(Vec3), 1, _pFile);
}

void CTransform::AddLocalRot(const Matrix & _matRot)
{
	m_matAddRot *= _matRot;
}

Vec3 CTransform::GetWorldPos()
{
	Vec3 vec3zero = Vec3(0.f, 0.f, 0.f);
	vec3zero = XMVector3TransformCoord(vec3zero, m_matWorld);
	return vec3zero;
}


void CTransform::SetLookAt(const Vec3 & _vDir)
{
	Vec3 vFront = _vDir;
	vFront.Normalize();

	Vec3 vRight = Vec3::Up.Cross(_vDir);
	vRight.Normalize();

	Vec3 vUp = vFront.Cross(vRight);
	vUp.Normalize();

	Matrix matRot = XMMatrixIdentity();

	matRot.Right(vRight);
	matRot.Up(vUp);
	matRot.Front(vFront);

	m_vLocalRot = DecomposeRotMat(matRot);

	// 방향벡터(우, 상, 전) 갱신하기	
	Matrix matRotate = XMMatrixRotationX(m_vLocalRot.x);
	matRotate *= XMMatrixRotationY(m_vLocalRot.y);
	matRotate *= XMMatrixRotationZ(m_vLocalRot.z);

	for (UINT i = 0; i < (UINT)DIR_TYPE::END; ++i)
	{
		m_vLocalDir[i] = XMVector3TransformNormal(vAsis[i], matRotate);
		m_vLocalDir[i].Normalize();
		m_vWorldDir[i] = m_vLocalDir[i];
	}
}
