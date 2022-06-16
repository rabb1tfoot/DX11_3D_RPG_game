#include "stdafx.h"
#include "Animator3D.h"

#include "TimeMgr.h"
#include "MeshRender.h"
#include "Material.h"
#include "PathMgr.h"

CAnimator3D::CAnimator3D()
	: m_iCurClip(0)
	, m_fCurTime(0.f)
	, m_iFrameCount(30)
	, m_bPlay(true)
	, m_VecAnimCutClip{}
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
}


CAnimator3D::~CAnimator3D()
{
	Safe_Delete_Vec(m_VecAnimCutClip);
}

void CAnimator3D::update()
{
}

void CAnimator3D::lateudpate()
{
}

void CAnimator3D::finalupdate()
{
	if (m_VecAnimCutClip.empty())
		return;

	m_fCurTime = 0.f;
	//현재 재생중인Clip의 시간을 진행한다. m_vecClipUpdateTime[m_iCurClip]인데 0으로 변경 이유 어자피 통짜 애니메이션만 쓸예정임
	// 대신 m_iCurClip 변수를 분할한 curAnim의 인덱스로 쓴다.

	if(m_bPlay)
		m_vecClipUpdateTime[0] += DT;

	if (m_vecClipUpdateTime[0] >= m_VecAnimCutClip[m_iCurClip]->fEndFrm)
	{
		m_vecClipUpdateTime[0] = m_VecAnimCutClip[m_iCurClip]->fStartFrm;
	}

	m_fCurTime = (float)(m_pVecClip->at(0).dStartTime + m_vecClipUpdateTime[0]);

	//본 갯수만큼 반복하며 현재시간에 맞게 모든 본 행렬을 보간해준다.
	for (size_t i = 0; i < m_pVecBones->size(); ++i)
	{
		if (m_pVecBones->at(i).vecKeyFrame.empty())
		{
			// 키프레임 별 행렬이 없는 본일 경우
			m_vecFinalBoneMat[i] = m_pVecBones->at(i).matBone;
			continue;
		}

		// 시간을 통하여 인덱스를 구한다.
		int iFrameIndex = (int)(m_fCurTime * m_iFrameCount);
		int iFrameNextIndex = 0;

		if (iFrameIndex >= m_pVecClip->at(0).iFrameLength - 1)
		{
			iFrameNextIndex = iFrameIndex;
		}
		else
		{
			iFrameNextIndex = iFrameIndex + 1;
		}

		const tMTKeyFrame& tKeyFrame = m_pVecBones->at(i).vecKeyFrame[iFrameIndex];
		const tMTKeyFrame& tNextKeyFrame = m_pVecBones->at(i).vecKeyFrame[iFrameNextIndex];

		float fFrameTime = tKeyFrame.fTime;
		float fNextFrameTime = tNextKeyFrame.fTime;

		//프레임간의 시간에 따른 비율을 구해준다.
		float fPercent = (m_fCurTime - fFrameTime) / (1.f / m_iFrameCount);

		XMVECTOR vS1 = tKeyFrame.vScale;
		XMVECTOR vS2 = tNextKeyFrame.vScale;

		XMVECTOR vT1 = tKeyFrame.vTranslate;
		XMVECTOR vT2 = tNextKeyFrame.vTranslate;

		XMVECTOR vQ1 = XMLoadFloat4(&tKeyFrame.qRot);
		XMVECTOR vQ2 = XMLoadFloat4(&tNextKeyFrame.qRot);

		XMVECTOR vS = XMVectorLerp(vS1, vS2, fPercent);
		XMVECTOR vT = XMVectorLerp(vT1, vT2, fPercent);
		XMVECTOR vQ = XMQuaternionSlerp(vQ1, vQ2, fPercent);

		XMVECTOR vQZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		//오프셋 행렬을 곱하여 최종 본 행렬을 만든다.
		m_vecFinalBoneMat[i] = m_pVecBones->at(i).matOffset * XMMatrixAffineTransformation(vS, vQZero, vQ, vT);
	}
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	m_pVecClip = _vecAnimClip;
	m_vecClipUpdateTime.resize(m_pVecClip->size());
}

void CAnimator3D::AddAnimCutClip(float _iStart, float _iEnd, wstring _Name)
{
	tAnim3D  *t = new tAnim3D();
	t->fStartFrm = _iStart;
	t->fEndFrm = _iEnd;
	t->strName = _Name;

	m_VecAnimCutClip.push_back(t);
}

void CAnimator3D::SetCurAnimCutClip(wstring _name)
{
	for (size_t i = 0; i < m_VecAnimCutClip.size(); ++i)
	{
		if (!wcscmp(_name.c_str(), m_VecAnimCutClip[i]->strName.c_str()))
		{
			m_vecClipUpdateTime[0] = m_VecAnimCutClip[i]->fStartFrm;
			m_iCurClip = i;
			break;
		}
	}
}

tAnim3D* CAnimator3D::FindAnimCutClip(wstring _name)
{
	for (size_t i = 0; i < m_VecAnimCutClip.size(); ++i)
	{
		if (!wcscmp(m_VecAnimCutClip[i]->strName.c_str(), _name.c_str()))
		{
			return m_VecAnimCutClip[i];
		}
	}
	return nullptr;
}

void CAnimator3D::LoadFromFile(wstring _str)
{
	FILE* pFile = nullptr;

	wstring strFileName = CPathMgr::GetResPath();
	strFileName += L"Animation3D\\";
	strFileName += _str;
	strFileName += L".Anim";
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"rb");

	if (pFile == nullptr)
		return;

	int iLength = 0;
	float fStart = 0.f;
	float fEnd = 0.f;
	wstring strName;
	fread(&iLength, sizeof(int), 1, pFile);
	for (int i = 0; i < iLength; ++i)
	{
		fread(&fStart, sizeof(float), 1, pFile);
		fread(&fEnd, sizeof(float), 1, pFile);
		strName = LoadWString(pFile);

		this->AddAnimCutClip(fStart, fEnd, strName);
	}
	SetCurAnimCutClip(L"TotalAnimClip");
	fclose(pFile);
}

void CAnimator3D::DeleteAnimCurClip(wstring _str)
{
	for (size_t i = 0; i < m_VecAnimCutClip.size(); ++i)
	{
		if (!wcscmp(_str.c_str(), m_VecAnimCutClip[i]->strName.c_str()))
		{
			vector<tAnim3D*>::iterator iter = m_VecAnimCutClip.begin();
			iter += i;
			SAFE_DELETE(m_VecAnimCutClip[i]);
			m_VecAnimCutClip.erase(iter);
			return;
		}
	}
	return;
}

void CAnimator3D::SetFromFileAnim(wstring _str)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += L"Animation3D\\";
	strFileName += _str;
	FILE * pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"rb");

	int Animlength = 0;
	fread(&Animlength, sizeof(int), 1, pFile);

	for (int i = 0; i < Animlength; ++i)
	{
		float fStartFrm;
		float fEndFrm;
		wstring AnimName;
		fread(&fStartFrm, sizeof(float), 1, pFile);
		fread(&fEndFrm, sizeof(float), 1, pFile);
		AnimName = LoadWString(pFile);
		if (!wcscmp(AnimName.c_str(), L"TotalAnimClip"))
			continue;
		tAnim3D *t = new tAnim3D;
		t->fStartFrm = fStartFrm;
		t->fEndFrm = fEndFrm;
		t->strName = AnimName;

		m_VecAnimCutClip.push_back(t);
	}

	fclose(pFile);
}

void CAnimator3D::UpdateData()
{
	//Bone Texture Update
	m_pBoneTex->SetData(&m_vecFinalBoneMat[0], sizeof(Matrix) * m_vecFinalBoneMat.size());

	UINT iMtrlCount = MeshRender()->GetMaterialCount();
	CResPtr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		pMtrl->SetData(SHADER_PARAM::TEX_7, &m_pBoneTex);
	}
}

void CAnimator3D::SaveToScene(FILE * _pFile)
{
	//애니목록을 세이브한다
}

void CAnimator3D::LoadFromScene(FILE * _pFile)
{
	fread(&m_VecAnimCutClip, sizeof(int), 1, _pFile);
	int vecAnimlength = m_VecAnimCutClip.size();

	for (int i = 0; i < vecAnimlength; ++i)
	{
		fread(&m_VecAnimCutClip[i]->fStartFrm, sizeof(float), 1, _pFile);
		fread(&m_VecAnimCutClip[i]->fEndFrm, sizeof(float), 1, _pFile);
		m_VecAnimCutClip[i]->strName = LoadWString(_pFile);
	}
}
