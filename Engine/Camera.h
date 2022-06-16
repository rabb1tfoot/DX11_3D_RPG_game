#pragma once
#include "Component.h"

union uInstID
{
	struct
	{
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};
	ULONG64 LID;
};

struct tInstObj
{
	CGameObject* pObj;
	UINT iMtrlIdx;
};
enum class PROJ_TYPE
{
	ORTHOGRAPHIC,
	PERSPECTIVE,
};

class CCamera :
	public CComponent
{
protected:
	Matrix		m_matView;
	Matrix		m_matViewInv;
	Matrix		m_matProj;

	PROJ_TYPE	m_eType;
	float		m_fScale; // 직교투영 배율
	float		m_fFOV;	  // 시야각

	float		m_fNear;
	float		m_fFar;

	UINT		m_iLayerCheck;
	UINT		m_iCamOrder;

	float		m_fWidth;
	float		m_fHeight;

	tRay		m_tRay;

	bool m_bRegister;

	vector<CGameObject*> m_vecShadowObj;

	//인스턴싱그룹
	map<ULONG64, vector<tInstObj>> m_mapInstGroup_F; //포워드
	map<ULONG64, vector<tInstObj>> m_mapInstGroup_D; // 디퍼드
	map<ULONG64, vector<tInstObj>> m_mapInstGroup_P; // 포스트
	map<ULONG64, vector<CGameObject*>*> m_mapInstGroup_S; // 쉐도우


	//싱글 랜더링용 오브젝트 그룹
	map<INT_PTR, vector<tInstObj>> m_mapSingleObj;

	vector<CGameObject*>		   m_vecParticle;

public:
	virtual void update();
	void finalupdate();
	void render_deferred();
	void render_forward();
	void render_posteffect();
	void render_shadowmap();

	void SortGameObject();
	void SortShadowGameObject();

public:
	void SetProjType(PROJ_TYPE _eType) { m_eType = _eType; }
	void SetScale(float _f) { if (_f == 0.f) m_fScale = 0.01f; else m_fScale = _f; }

	void SetNear(float _N) { m_fNear = _N; }
	void SetFar(float _F) { m_fFar = _F; }
	void SetFOV(float _FOV) { if (_FOV == 0.f) m_fFOV = 0.01f; else m_fFOV = _FOV; }
	void SetCamOrder(UINT _iCamOrder) { m_iCamOrder = _iCamOrder; }
	void SetWidth(float _fWidth) { m_fWidth = _fWidth; }
	void SetHeight(float _fHeight) { m_fHeight = _fHeight; }
	void SetRegisterOnOff(bool _bTrue) { m_bRegister = _bTrue; }

	PROJ_TYPE GetProjType() { return m_eType; }
	float GetScale() { return m_fScale; }
	float GetNear() { return m_fNear; }
	float GetFar() { return m_fFar; }
	float GetFOV() { return m_fFOV; }
	UINT GetCamOrder() { return m_iCamOrder; }
	const tRay& GetRay();

	const Matrix& GetViewMat() { return m_matView; }
	const Matrix& GetViewInvMat() { return m_matViewInv; }
	const Matrix& GetProjMat() { return m_matProj; }

	void CheckLayer(UINT _iLayerIdx);
	void CheckLayerALL() { m_iLayerCheck = 0xffffffff; };
	bool IsValiedLayer(UINT _iLayerIdx) { return m_iLayerCheck & (1 << _iLayerIdx); }

	virtual void SaveToScene(FILE* _pFile);
	virtual void LoadFromScene(FILE* _pFile);

	CLONE(CCamera);

protected:
	void CalRay();

public:
	CCamera();
	virtual ~CCamera();
};

