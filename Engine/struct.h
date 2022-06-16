#pragma once

#include "define.h"

struct tResolution
{
	float fWidth = 0.f;
	float fHeight = 0.f;
};

struct tEvent
{
	EVENT_TYPE	eType;
	INT_PTR		wParam;
	INT_PTR		lParam;
};

struct tItem
{
	wstring strName;
	wstring strInfo;
	Vec2	iIdx;
	int		iEffCount;
};

struct tGlobal
{
	float g_fAccTime;
	float g_fDT;
	int   iarr[3];
};

struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};

// ===========
// Particle
// ===========
struct tParticle
{
	Vec3 vWorldPos;		// 위치	
	Vec3 vWorldDir;		// 이동 방향

	float m_fCurTime;	// 현재 시간
	float m_fLifeTime;	// 최대 생명주기

	int  iAlive;
	int  arrPading[3];
};

struct tParticleShared
{
	int iAddCount;	// 매 프레임 마다 추가될 파티클 개수
	int arrPading[3];
};

// Vertex
struct VTX
{
	Vec3 vPos;
	Vec4 vColor;
	Vec2 vUV;
	Vec3 vNormal;  // 법선 벡터
	Vec3 vTangent; // 접선 벡터
	Vec3 vBinormal;// 종법선 벡터

	Vec4 vWeights; //Bone 가중치
	Vec4 vIndices; //Bone 인덱스
};



//===============
// 상수버퍼 구조체
//===============
struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
	Matrix matWV;
	Matrix matWVP;
	Matrix matWorldInv;
	Matrix matViewInv;
};

struct tAnim2D
{
	Vec2 vLT;
	Vec2 vSize;
	int  iAnimCheck;
	int  iArrPadding[3];
};

struct tShaderParam
{
	int  arrInt[4];
	float arrFloat[4];
	Vec2 arrVec2[4];
	Vec4 arrVec4[4];
	Matrix arrMat[4];

	UINT arrTexCheck[(UINT)SHADER_PARAM::TEX_END - (UINT)SHADER_PARAM::TEX_0];
	UINT arrRWTexCheck[(UINT)SHADER_PARAM::RWTEX_END - (UINT)SHADER_PARAM::RWTEX_0];
};

struct tGlobalValue
{
	float fDeltaTime;
	float dAccTime;
	tResolution tResolution;
};

struct tLight3DInfo
{
	Vec4		vPadding = Vec4(0.f, 0.f, 0.f, 0.f);
	Vec4		vDiff;
	Vec4		vSpec;
	Vec4		vAmb;

	Vec4		vPos;
	Vec4		vDir; // 방향성, spot
	UINT		iType;
	float		fRange; // 점광원, Spot
	float		fAngle; // Spot
	UINT		iPadding;
};

//FBX struct
typedef struct _tagMtrlInfo
{
	Vec4 vMtrlDiff;
	Vec4 vMtrlSpec;
	Vec4 vMtrlAmb;
	Vec4 vMtrlEmiv;
	_tagMtrlInfo()
		: vMtrlDiff(1.f, 1.f, 1.f, 1.f)
		, vMtrlSpec(1.f, 1.f, 1.f, 1.f)
		, vMtrlAmb(1.f, 1.f, 1.f, 1.f)
		, vMtrlEmiv(1.f, 1.f, 1.f, 1.f)
	{}
}tMtrlInfo;

typedef struct _tagFbxMat
{
	tMtrlInfo tMtrl;
	wstring strMtrlName;
	wstring strDiff;
	wstring strNormal;
	wstring strSpec;

	void ChangeDiff(wstring& _str) { strDiff = _str; };
	void ChangeNormal(wstring& _str) { strNormal = _str; };
}tFbxMaterial;

typedef struct _tagWeightsAndIndices
{
	int		iBoneIdx;
	double	dWeight;
}tWeightsAndIndices;

typedef struct _tagContainer
{
	wstring								strName;
	vector<Vec3>						vecPos;
	vector<Vec3>						vecTangent;
	vector<Vec3>						vecBinormal;
	vector<Vec3>						vecNormal;
	vector<Vec2>						vecUV;

	vector<Vec4>						vecIndices;
	vector<Vec4>						vecWeights;

	vector<vector<UINT>>				vecIdx;
	vector<tFbxMaterial>				vecMtrl;

	// Animation 관련 정보
	bool								bAnimation;
	vector<vector<tWeightsAndIndices>>	vecWI;

	void Resize(UINT _iSize)
	{
		vecPos.resize(_iSize);
		vecTangent.resize(_iSize);
		vecBinormal.resize(_iSize);
		vecNormal.resize(_iSize);
		vecUV.resize(_iSize);
		vecIndices.resize(_iSize);
		vecWeights.resize(_iSize);
		vecWI.resize(_iSize);
	}

}tContainer;

// Animation
struct tMTKeyFrame
{
	float	fTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};

struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Offset 행렬(뼈 -> 루트 까지의 행렬)
	Matrix				matBone;   // 이거 안씀
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring		strAnimName;
	int			iStartFrame;
	int			iEndFrame;
	int			iFrameLength;

	double		dStartTime;
	double		dEndTime;
	double		dTimeLength;
	float		fUpdateTime; // 이거 안씀

	FbxTime::EMode eMode;
};
