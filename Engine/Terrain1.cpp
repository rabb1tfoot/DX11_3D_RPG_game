#include "stdafx.h"
#include "Terrain.h"

#include "MeshRender.h"
#include "ResMgr.h"
#include "Texture.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"

#include "KeyMgr.h"
#include "Core.h"

CTerrain::CTerrain()
	: CComponent(COMPONENT_TYPE::TERRAIN)
	, m_eMod(TERRAIN_MOD::END)
	, m_vBrushScale(Vec2(0.1f, 0.1f))
	, m_iBrushIdx(0)
	, m_iTileIdx(0)
	, m_pToolCam(nullptr)
	, m_pTessCam(nullptr)
{
}

CTerrain::~CTerrain()
{
}

void CTerrain::init(UINT _iXFace, UINT _iZFace)
{
	m_iXFaceCount = _iXFace;
	m_iZFaceCount = _iZFace;

	LoadResource();

	// Transform
	Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
}

void CTerrain::ChangeXZFaceCount(UINT _iXFace, UINT _iZFace)
{
	m_iXFaceCount = _iXFace;
	m_iZFaceCount = _iZFace;
}

void CTerrain::finalupdate()
{
	if (CCore::GetInst()->GetState() == SCENE_STATE::STOP)
	{
		Vec4 vScale = Vec4(Transform()->GetLocalScale());
		MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_0, &vScale);

		ModCheck();

		KeyCheck();
	}
	if (m_pTessCam != nullptr)
	{
		Vec3 pCamWorldPos = m_pTessCam->Transform()->GetWorldPos();
		MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::VEC4_1, &pCamWorldPos);
	}
}
