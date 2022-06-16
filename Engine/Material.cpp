#include "stdafx.h"
#include "Material.h"

#include "Device.h"
#include "ConstBuffer.h"
#include "PathMgr.h"
#include "ResMgr.h"

#include "Core.h"

CMaterial::CMaterial()
	: CResource(RES_TYPE::MATERIAL)
	, m_pShader(nullptr)
	, m_param{}
	, m_arrTex{}
	, m_arrRWTex{}
	, m_bSave(true)
{
}


CMaterial::~CMaterial()
{
}

void CMaterial::SetShader(CResPtr<CShader> _pShader)
{
	m_pShader = _pShader;

	if (CCore::GetInst()->GetState() == SCENE_STATE::STOP && m_bSave)
	{
		Save();
	}
}

void CMaterial::SetData(SHADER_PARAM _eType, void * _pSrc)
{
	int iIdx = 0;

	switch (_eType)
	{
	case SHADER_PARAM::INT_0:
	case SHADER_PARAM::INT_1:
	case SHADER_PARAM::INT_2:
	case SHADER_PARAM::INT_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::INT_0;
		m_param.arrInt[iIdx] = *((int*)_pSrc);
		break;
	case SHADER_PARAM::FLOAT_0:
	case SHADER_PARAM::FLOAT_1:
	case SHADER_PARAM::FLOAT_2:
	case SHADER_PARAM::FLOAT_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::FLOAT_0;
		m_param.arrFloat[iIdx] = *((float*)_pSrc);
		break;
	case SHADER_PARAM::VEC2_0:
	case SHADER_PARAM::VEC2_1:
	case SHADER_PARAM::VEC2_2:
	case SHADER_PARAM::VEC2_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::VEC2_0;
		m_param.arrVec2[iIdx] = *((Vec2*)_pSrc);
		break;
	case SHADER_PARAM::VEC4_0:
	case SHADER_PARAM::VEC4_1:
	case SHADER_PARAM::VEC4_2:
	case SHADER_PARAM::VEC4_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::VEC4_0;
		m_param.arrVec4[iIdx] = *((Vec4*)_pSrc);
		break;
	case SHADER_PARAM::MAT_0:
	case SHADER_PARAM::MAT_1:
	case SHADER_PARAM::MAT_2:
	case SHADER_PARAM::MAT_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::MAT_0;
		m_param.arrMat[iIdx] = *((Matrix*)_pSrc);
		break;
	case SHADER_PARAM::TEX_0:
	case SHADER_PARAM::TEX_1:
	case SHADER_PARAM::TEX_2:
	case SHADER_PARAM::TEX_3:
	case SHADER_PARAM::TEX_4:
	case SHADER_PARAM::TEX_5:
	case SHADER_PARAM::TEX_6:
	case SHADER_PARAM::TEX_7:
	case SHADER_PARAM::ARRTEX_0:
	case SHADER_PARAM::ARRTEX_1:
	case SHADER_PARAM::ARRTEX_2:
	case SHADER_PARAM::ARRTEX_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::TEX_0;
		m_arrTex[iIdx] = *((CResPtr<CTexture>*)_pSrc);
		break;

	case SHADER_PARAM::RWTEX_0:
	case SHADER_PARAM::RWTEX_1:
	case SHADER_PARAM::RWTEX_2:
	case SHADER_PARAM::RWTEX_3:
		iIdx = (UINT)_eType - (UINT)SHADER_PARAM::RWTEX_0;
		m_arrRWTex[iIdx] = *((CResPtr<CTexture>*)_pSrc);
		assert(nullptr != m_arrRWTex[iIdx]->GetUAV());
		break;

	default:
		assert(nullptr);
		break;
	}

	//애니메이션이 있는 3D메쉬 메테리얼은 세이브하지않는다.
	if (CCore::GetInst()->GetState() == SCENE_STATE::STOP && m_bSave && nullptr == m_arrTex[7])
	{
		Save();
	}
}

void CMaterial::UpdateData()
{
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateData();

	UINT iCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;
	for (UINT i = 0; i < iCount; ++i)
	{
		if(nullptr == m_arrTex[i])
		{
			CTexture::ClearRegister(i, (UINT)SHADER_TYPE::ALL_SHADER);
			m_param.arrTexCheck[i] = 0;
		}
		else
		{
			m_arrTex[i]->SetRegister(i, (UINT)SHADER_TYPE::ALL_SHADER);
			m_param.arrTexCheck[i] = 1;
		}
	}

	// RWTexture Update 하기
	iCount = (UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0;
	for (UINT i = 0; i < iCount; ++i)
	{
		if (nullptr == m_arrRWTex[i])
		{
			CTexture::ClearRWRegister(i);
			m_param.arrRWTexCheck[i] = 0;
		}
		else
		{
			m_arrRWTex[i]->SetRWRegister(i);
			m_param.arrRWTexCheck[i] = 1;
		}
	}

	static CConstBuffer* pConstBuffer = CDevice::GetInst()->FindConstBuffer(L"ShaderParam");

	pConstBuffer->AddData(&m_param, sizeof(tShaderParam));
	pConstBuffer->UpdateData();
	pConstBuffer->SetRegisterAll();
}

void CMaterial::UpdateDataInstancing()
{
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateDataInstancing();

	//Texture 업데이트
	UINT iCount = (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0;

	for (UINT i = 0; i < iCount; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			CTexture::ClearRegister(i, (UINT)SHADER_TYPE::ALL_SHADER);
			m_param.arrTexCheck[i] = 0;
		}
		else
		{
			m_arrTex[i]->SetRegister(1, (UINT)SHADER_TYPE::ALL_SHADER);
			m_param.arrTexCheck[i] = 1;
		}
	}

	//RWTexture 업데이트
	iCount = (UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0;
	
	for (UINT i = 0; i < iCount; ++i)
	{
		if(nullptr == m_arrRWTex[i])
		{
			CTexture::ClearRWRegister(i);
			m_param.arrRWTexCheck[i] = 0;
		}
		else
		{
			m_arrRWTex[i]->SetRWRegister(i);
			m_param.arrRWTexCheck[i] = 1;
		}
	}

	static CConstBuffer* pConstBuffer = CDevice::GetInst()->FindConstBuffer(L"ShaderParam");

	pConstBuffer->AddData(&m_param, sizeof(tShaderParam));
	pConstBuffer->UpdateData();
	pConstBuffer->SetRegisterAll();
}

void CMaterial::ExcuteComputeShader(UINT _x, UINT _y, UINT _z)
{
	UpdateData();
	CONTEXT->Dispatch(_x, _y, _z);
	CTexture::ClearAllRegister();
}

void CMaterial::Save()
{
	if (GetPath().empty())
		return;

	wstring strFileName = CPathMgr::GetResPath();
	strFileName += GetPath();

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	//메테리얼 하위 폴더가 없는경우 폴더를 만들어서 파일을 연다
	if (err == 2)
	{
		for (UINT i = wcslen(strFileName.c_str()); i > 0; --i)
		{
			if (strFileName[i] == '/')
			{
				wchar_t FolderName[255] = {};
				wcscpy_s(FolderName,strFileName.c_str());
				strFileName.erase(i, wcslen(strFileName.c_str()));
				_wmkdir(strFileName.c_str());
				_wfopen_s(&pFile, FolderName, L"wb");
				break;
			}
		}
	}

	if (pFile == nullptr)
		return;

	wstring strKey;
	if (nullptr != m_pShader)
		strKey = m_pShader->GetName();

	//킷값 저장 쉐이더 파라미터 저장 쉐이더 택스처 저장 택스처 마감데이터 저장

	SaveWString(strKey.c_str(), pFile);

	fwrite(&m_param, sizeof(tShaderParam), 1, pFile);

	for (UINT i = 0; i < (UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0; ++i)
	{
		if (nullptr != m_arrTex[i])
		{
			fwrite(&i, sizeof(int), 1, pFile);
			SaveWString(m_arrTex[i]->GetName().c_str(), pFile);
			SaveWString(m_arrTex[i]->GetPath().c_str(), pFile);
		}
	}
	int iTexEnd = (UINT)SHADER_PARAM::TEX_END;
	fwrite(&iTexEnd, sizeof(int), 1, pFile);

	// Shader Parameter Texture 
	for (UINT i = 0; i < (UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0; ++i)
	{
		if (nullptr != m_arrRWTex[i])
		{
			fwrite(&i, sizeof(int), 1, pFile);
			SaveWString(m_arrRWTex[i]->GetName().c_str(), pFile);
			SaveWString(m_arrRWTex[i]->GetPath().c_str(), pFile);
		}
	}

	// Texture 마감 데이터
	iTexEnd = (UINT)SHADER_PARAM::RWTEX_END;
	fwrite(&iTexEnd, sizeof(int), 1, pFile);


	fclose(pFile);
}

void CMaterial::Load(const wstring & _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Shader Key 읽기
	wstring strShaderName = LoadWString(pFile);
	m_pShader = CResMgr::GetInst()->FindRes<CShader>(strShaderName);

	// Shader Parameter 읽기
	fread(&m_param, sizeof(tShaderParam), 1, pFile);

	// Textrue Parameter 읽기
	while (true)
	{
		int index = 0;
		fread(&index, sizeof(int), 1, pFile);

		if (index == (UINT)SHADER_PARAM::TEX_END)
			break;

		wstring strKey = LoadWString(pFile);
		wstring strPath = LoadWString(pFile);

		m_arrTex[index] = CResMgr::GetInst()->FindRes<CTexture>(strKey);
		if (nullptr == m_arrTex[index] && 7 != index)
		{
			m_arrTex[index] = CResMgr::GetInst()->Load<CTexture>(strKey, strPath);
		}
	}
	while (true)
	{
		int index = 0;
		fread(&index, sizeof(int), 1, pFile);

		if (index == (UINT)SHADER_PARAM::RWTEX_END)
			break;

		wstring strKey = LoadWString(pFile);
		wstring strPath = LoadWString(pFile);

		m_arrRWTex[index] = CResMgr::GetInst()->FindRes<CTexture>(strKey);
		if (nullptr == m_arrRWTex[index])
		{
			m_arrRWTex[index] = CResMgr::GetInst()->Load<CTexture>(strKey, strPath);
		}
	}


	fclose(pFile);
}

bool CMaterial::LoadFromScene(FILE * _pFile)
{
	CResource::LoadFromScene(_pFile);

	CResPtr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(GetName());

	if (nullptr != pMtrl)
		return false;

	wstring strPath = CPathMgr::GetResPath();
	strPath += GetPath();

	Load(strPath);

	return true;
}
