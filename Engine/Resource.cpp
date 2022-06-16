#include "stdafx.h"
#include "Resource.h"


CResource::CResource(RES_TYPE _eType)
	: m_eType(_eType)
	, iMemCheck(0x12121212)
{
}

CResource::~CResource()
{
}

void CResource::SaveToScene(FILE * _pFile)
{
	// 키값
	SaveWString(GetName().c_str(), _pFile);

	// 경로
	SaveWString(m_strPath.c_str(), _pFile);
}

bool CResource::LoadFromScene(FILE * _pFile)
{
	SetName(LoadWString(_pFile));

	m_strPath = LoadWString(_pFile);

	return true;
}


