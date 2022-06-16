#pragma once

template<typename T, int iSize>
void Safe_Delete_Array(T* (&_pArr)[iSize])
{
	for (int i = 0; i < iSize; ++i)
	{
		SAFE_DELETE(_pArr[i]);
	}
}

template<typename T, int iSize>
void Safe_Release_Array(T* (&_pArr)[iSize])
{
	for (int i = 0; i < iSize; ++i)
	{
		SAFE_RELEASE(_pArr[i]);
	}
}

template<typename T>
void Safe_Delete_Vec(vector<T> _vec)
{
	for (UINT i = 0; i < _vec.size(); ++i)
	{
		SAFE_DELETE(_vec[i]);
	}
}

template<typename T1, typename T2>
void Safe_Delete_Map(map<T1, T2>& _map)
{
	for (auto& pair : _map)
	{
		SAFE_DELETE(pair.second);
	}
	_map.clear();
}

void SaveWString(const wchar_t* _pStr, FILE * _pFile);
wchar_t* LoadWString(FILE* _pFile);

float GetAngle(float _fRadian);
float GetRadian(float _fAngle);

bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());
Vec3 DecomposeRotMat(const Matrix & _matRot);


int GetSizeofFormat(DXGI_FORMAT _eFormat);

Matrix GetMatrix(FbxAMatrix & _mat);