#pragma once
class CLayer;
class CGameObject;

class CSaveLoadMgr
{
private:
	CSaveLoadMgr();
	~CSaveLoadMgr();

public:
	static void SaveScene(const wstring& _strPath);
	static void SaveResource(FILE* _pFile);
	static void SaveLayer(CLayer* _pLayer, FILE* _pFile);
	static void SaveGameObject(CGameObject* _pObject, FILE* _pFile);


	static CGameObject* LoadGameObject(FILE* _pFile);
	static void LoadScene(const wstring& _strPath);
	static void LoadResource(FILE* _pFile);
	static CLayer* LoadLayer(FILE* _pFile);

};

