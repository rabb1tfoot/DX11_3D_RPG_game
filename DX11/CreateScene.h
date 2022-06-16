#pragma once

#include <Material.h>
#include <Shader.h>
#include <Texture.h>
#include <ResPtr.h>
#include <MeshRender.h>

class CGameObject;

class CCreateScene
{
private:
	CGameObject* m_pToolCam;
	CGameObject* m_pToolUICam;
	CGameObject* m_pGridObject;

	vector<CGameObject*> m_vecToolUI;
	CMaterial* m_pGridMtrl;
	CShader* m_pGridShader;
	CMeshRender* m_pComputeRender;

public:
	void init();
	void update();

private:
	void LoadResource(wstring _strFolderPath);

	void update_tool();
	void render_tool();

	void CreateToolResource();
	void CreateToolCamera();
	void CreateToolObject();

public:
	void Create();

public:
	CCreateScene();
	~CCreateScene();
};

