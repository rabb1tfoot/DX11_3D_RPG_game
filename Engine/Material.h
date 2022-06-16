#pragma once
#include "Resource.h"

#include "ResPtr.h"
#include "Shader.h"
#include "Texture.h"


class CMaterial :
	public CResource
{
protected:
	CResPtr<CShader> m_pShader;
	tShaderParam m_param;
	CResPtr<CTexture> m_arrTex[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0];
	CResPtr<CTexture> m_arrRWTex[(UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0];
	bool m_bSave;

public:
	CResPtr<CShader> GetShader() { return m_pShader; }
	void SetShader(CResPtr<CShader> _pShader);
	virtual void SetData(SHADER_PARAM _eType, void* _pSrc);
	const tShaderParam& GetData() { return m_param; }
	CResPtr<CTexture>* GetTexData() { return m_arrTex; }
	CResPtr<CTexture>* GetRWTexData() { return m_arrRWTex; }
	void SaveDisable() { m_bSave = false; }

public:
	void UpdateData();
	void UpdateDataInstancing();
	void ExcuteComputeShader(UINT _x, UINT _y, UINT _z);

public:
	virtual void Load(const wstring& _strFilePath);
	virtual void Save();
	virtual bool LoadFromScene(FILE* _pFile);

	CLONE(CMaterial);

public:
	CMaterial();
	virtual ~CMaterial();
};

