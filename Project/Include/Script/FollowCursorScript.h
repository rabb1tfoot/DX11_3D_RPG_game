#pragma once
#include <Script.h>
class CFollowCursorScript :
	public CScript
{
public:
	CFollowCursorScript();
	virtual ~CFollowCursorScript();

private:
	CGameObject* m_pCamObj;
	CShader* m_pShader;
	CMaterial* m_pLayto3DMtrl;
	CResPtr<CTexture> m_pOutput;

public:
	void SetCamera(CGameObject* _pCam) { m_pCamObj = _pCam; };

public:
	CLONE(CFollowCursorScript);

private:
	void Collision2DCheck();
	void Collision3DCheck();

public:
	virtual void start();
	virtual void update();

};

