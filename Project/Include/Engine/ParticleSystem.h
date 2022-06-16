#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Material.h"
#include "Mesh.h"

class CStructuredBuffer;

class CParticleSystem :
	public CComponent
{
private:
	CStructuredBuffer*	m_pParticleBuffer; // ��ƼŬ ������ ����
	CStructuredBuffer*	m_pSharedBuffer;   // ��ƼŬ ���� �޸�
	UINT				m_iMaxParticle;    // �ִ� ��ƼŬ ����

	CResPtr<CMaterial>	m_pUpdateMtrl;		// ��ǻƮ ���̴�
	CResPtr<CMaterial>	m_pMtrl;			// ��ƼŬ ��������
	CResPtr<CMesh>		m_pMesh;			// ��� �޽�(Point Mesh �⺻)

	float				m_fFrequency;		// ��ƼŬ ���� �ֱ�
	float				m_fAccTime;		   // �����ð� üũ�뵵

	float				m_fMinLifeTime;   // ��ƼŬ �ּ� ���� �ð�
	float				m_fMaxLifeTime;   // ��ƼŬ �ִ� ���� �ð�

	float				m_fMinSpeed;	  // ��ƼŬ ���� �ӵ�
	float				m_fMaxSpeed;	  // ��ƼŬ ���� �ӵ�

	float				m_fStartScale;   // ��ƼŬ ���� ũ��
	float				m_fEndScale;     // ��ƼŬ ���� ũ��

	Vec4				m_vStartColor;
	Vec4				m_vEndColor;

public:
	virtual void update() {}
	virtual void finalupdate();
	virtual void render();

public:
	void SetStartColor(Vec4 _vColor) { m_vStartColor = _vColor; }

public:
	CLONE(CParticleSystem);
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CParticleSystem();
	virtual ~CParticleSystem();
};

