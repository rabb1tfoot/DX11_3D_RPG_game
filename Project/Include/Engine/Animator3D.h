#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "Texture.h"

struct tAnim3D
{
	wstring strName;

	float fStartFrm;
	float fEndFrm;
};

class CAnimator3D :
	public CComponent
{
private:
	const vector<tMTBone>*	   m_pVecBones;
	const vector<tMTAnimClip>* m_pVecClip;
	vector<tAnim3D*>		   m_VecAnimCutClip;

	vector<float>			m_vecClipUpdateTime;
	vector<Matrix>			m_vecFinalBoneMat;
	int						m_iFrameCount;
	float					m_fCurTime;
	int						m_iCurClip;
	CResPtr<CTexture>		m_pBoneTex;
	bool					m_bPlay;

public:
	void SetBones(const vector<tMTBone>* _vecBones) {
		m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size());
	}
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
	void SetBoneTex(CResPtr<CTexture> _pBoneTex) { m_pBoneTex = _pBoneTex; }

	//추가내용=================================================
	void AddAnimCutClip(float _iStart, float _iEnd, wstring _name);
	void SetCurAnimCutClip(wstring _name);
	tAnim3D* FindAnimCutClip(wstring _name);
	void SetCurFrm(float _Frm) { m_vecClipUpdateTime[0] = _Frm; }
	void StopAnim() { m_bPlay = false; }
	void PlayAnim() { m_bPlay = true; }
	void SetCutAnimIdx(int _iIdx) { m_iCurClip = _iIdx; }
	tAnim3D* GetCurAnimCutClip() { return m_VecAnimCutClip[m_iCurClip]; }
	const vector<tAnim3D*>& GetAnimClips() { return m_VecAnimCutClip; }
	float GetCurFrm() { return m_vecClipUpdateTime[0]; }
	const vector<tMTAnimClip>* GetvecAnimClip() { return m_pVecClip; }
	float GetCurEndFrm() { return m_VecAnimCutClip[m_iCurClip]->fEndFrm; }
	bool IsPlay() { return m_bPlay; }
	void LoadFromFile(wstring _str);
	void DeleteAnimCurClip(wstring _str);
	void SetFromFileAnim(wstring _str);// 프로젝트 로드하기전 임시로딩 함수
	//=========================================================

	CResPtr<CTexture> GetBoneTex() { return m_pBoneTex; }
	void UpdateData();
	void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }
	vector<Matrix>& GetFinalBoneMat() { return m_vecFinalBoneMat; }

public:
	virtual void awake() {};
	virtual void start() {};
	virtual void update();
	virtual void lateudpate();
	virtual void finalupdate();

public:
	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);
	CAnimator3D* Clone() { return new CAnimator3D(*this); }


public:
	CAnimator3D();
	virtual ~CAnimator3D();
};

