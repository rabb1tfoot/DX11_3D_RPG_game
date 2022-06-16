#include "stdafx.h"
#include "CameraEx.h"

#include <Transform.h>
#include <Device.h>
#include <RenderMgr.h>

CCameraEx::CCameraEx()
{
}

CCameraEx::~CCameraEx()
{
}


void CCameraEx::finalupdate()
{
	Vec3 vPos = Transform()->GetLocalPos();

	// View ��� �����
	m_matView = XMMatrixIdentity();
	m_matView._41 = -vPos.x;
	m_matView._42 = -vPos.y;
	m_matView._43 = -vPos.z;

	Vec3 vRight = Transform()->GetLocalDir(DIR_TYPE::DIR_RIGHT);
	Vec3 vUp = Transform()->GetLocalDir(DIR_TYPE::DIR_UP);
	Vec3 vFront = Transform()->GetLocalDir(DIR_TYPE::DIR_FRONT);

	// ��, ��, �� ����� �������(��������) �� ����� �ִ� ȸ������� �ٷ� �佺���̽� ȸ�����
	// �������� ����� ��������̱� ������, �佺���̽� ȸ������� ��, ��, �� ����� ������̴�.
	// ��, ��, �� ���� ������� ����� ��������̱� ������ ��ġ����� �� �����

	Matrix matViewRot;
	matViewRot = Matrix::Identity;
	matViewRot._11 = vRight.x; matViewRot._12 = vUp.x; matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y; matViewRot._22 = vUp.y; matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z; matViewRot._32 = vUp.z; matViewRot._33 = vFront.z;

	m_matView *= matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);

	// Projection ��� �����	
	tResolution tRes = CRenderMgr::GetInst()->GetResolution();

	if (m_eType == PROJ_TYPE::PERSPECTIVE)
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, tRes.fWidth / tRes.fHeight, m_fNear, m_fFar);
	else
		m_matProj = XMMatrixOrthographicLH(tRes.fWidth * m_fScale, tRes.fHeight * m_fScale, m_fNear, m_fFar);
}
