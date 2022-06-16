#pragma once
#include "Component.h"
#include "ResPtr.h"
#include "Texture.h"

class CAnimation2D;

class CAnimator2D :
	public CComponent
{
private:
	map<wstring, CAnimation2D*> m_mapAnim;
	CAnimation2D*				m_pCurAnim;
	bool						m_bPlayFirst;
	bool						m_bRepeat;

public:
	virtual void awake();
	virtual void start();
	virtual void update();
	virtual void lateupdate();
	virtual void finalupdate();
	void UpdateData();

public:
	void AddAnimation(const wstring & _strKey, CResPtr<CTexture> _pTex, Vec2 _vLT, Vec2 _vCropSize, int _iFrmCount, float _fTerm);
	void AddAnimation(const wstring& _strKey, const wstring& _strFolder, float _fTerm);
	void PlayAnimation(const wstring& _strKey, bool _bRepeat);
	void DeleteAnimation(const wstring& _strKey);


	void Pause();
	void SetAnimPlayFirst(bool _bTrue) { m_bPlayFirst = _bTrue; }

	CAnimation2D* FindAnim(const wstring& _strKey)
	{
		map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strKey);
		if (iter == m_mapAnim.end())
			return nullptr;
		return iter->second;
	}

	const map<wstring, CAnimation2D*>& GetAnimList() { return m_mapAnim; }
	CAnimation2D* GetCurAnim() { return m_pCurAnim; }

	bool IsRepeat() { return m_bRepeat; }
	void SetRepeat(bool _bTrue) { m_bRepeat = _bTrue; }

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

public:
	CLONE(CAnimator2D);

public:
	CAnimator2D();
	CAnimator2D(const CAnimator2D& _other);
	virtual ~CAnimator2D();
};

