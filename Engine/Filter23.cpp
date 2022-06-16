#include "stdafx.h"
#include "Filter23.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"

CFilter23::CFilter23()
	: CResource(RES_TYPE::FILTER)
	, m_pFilterObj(nullptr)
{
}

CFilter23::~CFilter23()
{
	SAFE_DELETE(m_pFilterObj);
}

void CFilter23::Create(CResPtr<CMesh> _pMesh, CResPtr<CMaterial> _pMtrl)
{
	m_pFilterObj = new CGameObject;
	m_pFilterObj->AddComponent(new CTransform);
	m_pFilterObj->AddComponent(new CMeshRender);

	m_pFilterObj->MeshRender()->SetMesh(_pMesh);
	m_pFilterObj->MeshRender()->SetMaterial(_pMtrl);
}

void CFilter23::render()
{
	m_pFilterObj->render();
}
