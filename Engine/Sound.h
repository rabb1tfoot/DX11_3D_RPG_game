#pragma once
#include "Resource.h"

#include <fmod.h>
#include <fmod.hpp>
#include <fmod_codec.h>

#ifdef _DEBUG

#ifdef _WIN32
//Debug 32
#pragma comment(lib, "fmodL_vc.lib")
#else
//Debug 64
#pragma comment(lib, "fmodL64_vc.lib")
#endif

#else
#ifdef _WIN32
// Release 32
#pragma comment(lib, "fmod_vc.lib")
#else
// Release 64
#pragma comment(lib, "fmod64_vc.lib")
#endif
#endif


class CSound :
	public CResource
{
public:
	static FMOD::System* g_pFMOD;

private:
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChanel;

public:
	void Play(int _iRoopCount); // -1이면 무한
	void Stop();

public:
	virtual void Load(const wstring& _strFilePath);
	virtual bool LoadFromScene(FILE* _pFile);

public:
	CLONE_DISABLE(CSound);
public:
	CSound();
	virtual ~CSound();
};

