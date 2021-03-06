#include "stdafx.h"
#include "ParticleSystem.h"

#include "StructuredBuffer.h"
#include "ResMgr.h"
#include "KeyMgr.h"
#include "TimeMgr.h"

#include "Transform.h"

CParticleSystem::CParticleSystem()
	: CComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_pParticleBuffer(nullptr)
	, m_pSharedBuffer(nullptr)
	, m_iMaxParticle(1000)
	, m_fFrequency(0.002f)
	, m_fAccTime(0.f)
	, m_fMinLifeTime(0.5f)
	, m_fMaxLifeTime(1.f)
	, m_fMinSpeed(100)
	, m_fMaxSpeed(50.f)
	, m_fStartScale(30.f)
	, m_fEndScale(1.f)
	, m_vStartColor(Vec4(1.0f, 0.6f, 0.1f, 0.8f))
	, m_vEndColor(Vec4(0.1f, 0.1f, 0.1f, 0.2f))
{
	// 구조화 버퍼 생성
	m_pParticleBuffer = new CStructuredBuffer;
	m_pParticleBuffer->Create(sizeof(tParticle), m_iMaxParticle, nullptr);

	m_pSharedBuffer = new CStructuredBuffer;
	m_pSharedBuffer->Create(sizeof(tParticleShared), 1, nullptr);

	// 사각형 Mesh
	m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh");

	// Material
	m_pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleMtrl");
	CResPtr<CTexture> pParticle = CResMgr::GetInst()->FindRes<CTexture>(L"Texture\\Particle\\CartoonSmoke.png");
	m_pMtrl->SetData(SHADER_PARAM::TEX_0, &pParticle);

	// ParticleUpdate
	m_pUpdateMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleUpdateMtrl");
}

CParticleSystem::~CParticleSystem()
{
	SAFE_DELETE(m_pParticleBuffer);
	SAFE_DELETE(m_pSharedBuffer);
}

void CParticleSystem::finalupdate()
{
	// 추가될 파티클 개수 계산
	m_fAccTime += DT;

	int iAdd = 0;
	if (m_fFrequency < m_fAccTime)
	{
		m_fAccTime = m_fAccTime - m_fFrequency;
		iAdd = 1;
	}

	m_pParticleBuffer->UpdateRWData(6);
	m_pSharedBuffer->UpdateRWData(7);

	m_pUpdateMtrl->SetData(SHADER_PARAM::INT_0, &m_iMaxParticle);
	m_pUpdateMtrl->SetData(SHADER_PARAM::INT_1, &iAdd);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_0, &m_fMinLifeTime);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_1, &m_fMaxLifeTime);

	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_2, &m_fMinSpeed);
	m_pUpdateMtrl->SetData(SHADER_PARAM::FLOAT_3, &m_fMaxSpeed);

	m_pUpdateMtrl->ExcuteComputeShader(1, 1, 1);

	m_pParticleBuffer->ClearRWData(6);
	m_pSharedBuffer->ClearRWData(7);
}

void CParticleSystem::render()
{
	Transform()->UpdateData();

	m_pParticleBuffer->UpdateData(12);

	m_pMtrl->SetData(SHADER_PARAM::VEC4_0, &m_vStartColor);
	m_pMtrl->SetData(SHADER_PARAM::VEC4_1, &m_vEndColor);
	m_pMtrl->SetData(SHADER_PARAM::FLOAT_0, &m_fStartScale);
	m_pMtrl->SetData(SHADER_PARAM::FLOAT_1, &m_fEndScale);

	m_pMtrl->UpdateData();
	m_pMesh->SetLayout(m_pMtrl->GetShader());
	m_pMesh->render_particle(m_iMaxParticle);

	m_pParticleBuffer->ClearData(10);
}

void CParticleSystem::SaveToScene(FILE * _pFile)
{
}

void CParticleSystem::LoadFromScene(FILE * _pFile)
{
}
