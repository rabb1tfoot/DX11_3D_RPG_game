#include "stdafx.h"
#include "MeshRender.h"
#include "GameObject.h"
#include "Transform.h"

#include "Collider2D.h"
#include "Collider3D.h"
#include "Animator2D.h"
#include "Animation2D.h"
#include "Animator3D.h"
#include "ResMgr.h"
#include "Scene.h"
#include "SCeneMgr.h"
#include "Layer.h"
#include "Camera.h"

CMeshRender::CMeshRender()
	: m_pMesh(nullptr)
	, m_bShadow(true)
	, m_vecMtrl{}
	, CComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

CResPtr<CMaterial> CMeshRender::GetCloneMaterial(UINT _iSubset)
{
	if (nullptr == m_vecMtrl[_iSubset])
		return nullptr;

	m_vecMtrl[_iSubset] = m_vecMtrl[_iSubset]->Clone();

	CResMgr::GetInst()->AddCloneRes(m_vecMtrl[_iSubset]);

	return m_vecMtrl[_iSubset];
}

ULONG64 CMeshRender::GetInstID(UINT _iMtrlIdx)
{
	if (m_pMesh == nullptr || m_vecMtrl[_iMtrlIdx] == nullptr)
		return 0;

	uInstID id{ m_pMesh->GetID(), (WORD)m_vecMtrl[_iMtrlIdx]->GetID(), (WORD)_iMtrlIdx };
	return id.LID;
}

void CMeshRender::update()
{
}

void CMeshRender::render()
{
	Transform()->UpdateData();

	//if (Animator2D())
	//{
	//	Animator2D()->UpdateData();
	//}
	//else
	//{
	//	CAnimation2D::ClearRegister();
	//}

	if (Animator3D())
	{
		Animator3D()->UpdateData();
	}

	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		if (nullptr == m_vecMtrl[i] || nullptr == m_vecMtrl[i]->GetShader())
			continue;

		//layout전달
		m_pMesh->SetLayout(m_vecMtrl[i]->GetShader());
		m_vecMtrl[i]->UpdateData();
		m_pMesh->render(i);
		CTexture::ClearAllRegister();
	}
}

void CMeshRender::render(UINT _iMtrl)
{
	m_pMesh->SetLayout(m_vecMtrl[_iMtrl]->GetShader());
	m_vecMtrl[_iMtrl]->UpdateData();
	m_pMesh->render(_iMtrl);
	CTexture::ClearAllRegister();
}

void CMeshRender::render_shadowmap()
{
	CResPtr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"ShadowMapMtrl");

	Transform()->UpdateData();

	for (size_t i = 0; i < m_vecMtrl.size(); ++i)
	{
		pMtrl->UpdateData();
		m_pMesh->SetLayout(pMtrl->GetShader());
		m_pMesh->render(i);
	}
}


void CMeshRender::SetMesh(CResPtr<CMesh> _pMesh)
{
	m_pMesh = _pMesh;
	UINT iSubsetCount = m_pMesh->GetSubsetCount();

	if (m_vecMtrl.size() < iSubsetCount)
	{
		m_vecMtrl.resize(iSubsetCount);
	}
	else if (m_vecMtrl.size() > iSubsetCount)
	{
		vector<CResPtr<CMaterial>> vecTemp;
		vecTemp.resize(iSubsetCount);
		m_vecMtrl.swap(vecTemp);
	}
}

void CMeshRender::SaveToScene(FILE * _pFile)
{
	bool bMesh = true, bMtrl = true;

	fwrite(&m_bShadow, sizeof(bool), 1, _pFile);

	if (nullptr == m_pMesh
		|| !wcscmp(m_pMesh->GetName().c_str(), L"TerrainRect"))
		bMesh = false;

	fwrite(&bMesh, 1, 1, _pFile);

	if (m_vecMtrl.empty())
		bMtrl = false;

	if (bMesh)
	{
		SaveWString(m_pMesh->GetName().c_str(), _pFile);
		SaveWString(m_pMesh->GetPath().c_str(), _pFile);
	}

	fwrite(&bMtrl, 1, 1, _pFile);
	if (bMtrl)
	{
		UINT iSize = m_vecMtrl.size();
		fwrite(&iSize, sizeof(UINT), 1, _pFile);

		UINT i = 0;
		for (; i < m_vecMtrl.size(); ++i)
		{
			if (nullptr == m_vecMtrl[i])
				continue;

			fwrite(&i, sizeof(UINT), 1, _pFile);
			SaveWString(m_vecMtrl[i]->GetName().c_str(), _pFile);
			SaveWString(m_vecMtrl[i]->GetPath().c_str(), _pFile);

			const tShaderParam tParam = m_vecMtrl[i]->GetData();
			CResPtr<CTexture>* pArrTex = m_vecMtrl[i]->GetTexData();
			CResPtr<CTexture>* pArrRWTex = m_vecMtrl[i]->GetRWTexData();

			//해당 쉐이더 파라미터 저장
			fwrite(&tParam, sizeof(tShaderParam), 1, _pFile);
			int iTexEnd = 0; (UINT)SHADER_PARAM::TEX_END;

			//택스처 파라미터 저장
			for (UINT i = 0; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
			{
				if (nullptr != pArrTex[i])
				{
					//Texture 마감 데이터
					iTexEnd = 1;
					fwrite(&iTexEnd, sizeof(int), 1, _pFile);

					fwrite(&i, sizeof(int), 1, _pFile);
					SaveWString(pArrTex[i]->GetName().c_str(), _pFile);
					SaveWString(pArrTex[i]->GetPath().c_str(), _pFile);
				}
				else
				{
					//Texture 마감 데이터
					iTexEnd = (UINT)SHADER_PARAM::TEX_END;
					fwrite(&iTexEnd, sizeof(int), 1, _pFile);
					break;
				}
			}

			// Shader Parameter Texture 
			for (UINT i = 0; i < (UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0; ++i)
			{
				if (nullptr != pArrRWTex[i])
				{
					//RWTexture 마감 데이터
					iTexEnd = 1;
					fwrite(&iTexEnd, sizeof(int), 1, _pFile);

					fwrite(&i, sizeof(int), 1, _pFile);
					SaveWString(pArrRWTex[i]->GetName().c_str(), _pFile);
					SaveWString(pArrRWTex[i]->GetPath().c_str(), _pFile);
				}
				else
				{
					//RWTexture 마감 데이터
					iTexEnd = (UINT)SHADER_PARAM::RWTEX_END;
					fwrite(&iTexEnd, sizeof(int), 1, _pFile);
					break;
				}
			}
		}
		i = -1; // 마감 용도
		fwrite(&i, sizeof(UINT), 1, _pFile);
	}
}

void CMeshRender::LoadFromScene(FILE * _pFile)
{
	bool bMesh = false, bMtrl = false;

	wstring strKey, strPath;

	fread(&m_bShadow, sizeof(bool), 1, _pFile);

	fread(&bMesh, 1, 1, _pFile);
	if (bMesh)
	{
		strKey = LoadWString(_pFile);
		strPath = LoadWString(_pFile);

		m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(strKey);
		if (nullptr == m_pMesh)
		{
			CResMgr::GetInst()->Load<CMesh>(strKey, strPath);
		}
	}

	fread(&bMtrl, 1, 1, _pFile);
	if (bMtrl)
	{
		UINT iSize = 0;
		fread(&iSize, sizeof(UINT), 1, _pFile);
		m_vecMtrl.resize(iSize);

		UINT iIdx = 0;
		fread(&iIdx, sizeof(UINT), 1, _pFile);

		while (iIdx != -1)
		{
			strKey = LoadWString(_pFile);
			strPath = LoadWString(_pFile);
			SetMaterial(CResMgr::GetInst()->Load<CMaterial>(strKey, strPath), iIdx);

			tShaderParam tParam = {};
			CResPtr<CTexture> pArrTex[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0] = {};
			CResPtr<CTexture> pArrRWTex[(UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0] = {};

			//해당 쉐이더 파라미터 로드
			fread(&tParam, sizeof(tShaderParam), 1, _pFile);
			int iTexEnd = (UINT)SHADER_PARAM::TEX_END;

			//택스처 파라미터 로드
			for (UINT i = 0; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
			{
				fread(&iTexEnd, sizeof(int), 1, _pFile);
				if (iTexEnd == (UINT)SHADER_PARAM::TEX_END)
				{
					break;
				}
				else
				{
					fread(&i, sizeof(int), 1, _pFile);
					wstring strName = LoadWString(_pFile);
					wstring strPath = LoadWString(_pFile);
					pArrTex[i] = CResMgr::GetInst()->FindRes<CTexture>(strName.c_str());
				}
			}

			//RW택스처 파라미터 로드
			for (UINT i = 0; i < (UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0; ++i)
			{
				fread(&iTexEnd, sizeof(int), 1, _pFile);
				if (iTexEnd == (UINT)SHADER_PARAM::RWTEX_END)
				{
					break;
				}
				if (nullptr != pArrRWTex[i])
				{
					fread(&i, sizeof(int), 1, _pFile);
					wstring strName = LoadWString(_pFile);
					wstring strPath = LoadWString(_pFile);
					pArrRWTex[i] = CResMgr::GetInst()->FindRes<CTexture>(strName.c_str());
				}
			}

			fread(&iIdx, sizeof(UINT), 1, _pFile);
			//끝에 도달하면 메테리얼 파라미터 세팅
			if (iIdx == -1)
			{
				for (UINT i = 0; i < 4; ++i)
				{
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::INT_0 + i), &tParam.arrInt[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::FLOAT_0 + i), &tParam.arrFloat[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::VEC2_0 + i), &tParam.arrVec2[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::VEC4_0 + i), &tParam.arrVec4[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::MAT_0 + i), &tParam.arrMat[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::TEX_0 + i), &pArrTex[i]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::TEX_4 + i), &pArrTex[i + 4]);
					GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::ARRTEX_0 + i), &pArrTex[i + 8]);
					//GetCloneMaterial()->SetData(SHADER_PARAM((UINT)SHADER_PARAM::RWTEX_0 + i), &pArrRWTex[i]);
				}
			}
		}
	}
}

