#pragma once

class CScene;
class CCamera;
class CGameObject;

class CSceneMgr
{
	SINGLE(CSceneMgr);
private:
	CScene*	m_pCurScene;
	CScene* m_pNextScene;

public:
	CScene* GetCurScene() { return m_pCurScene; }

public:
	void init();
	void progress();
	void progress_pause();

	void FindGameObject(const wstring& _strTagName, vector<CGameObject*>& _vecOut);
	void ChangeScene(CScene* _pNextScene);
};

