#pragma once

class CTimeMgr
{
private:
	LARGE_INTEGER	m_llOldCount;
	LARGE_INTEGER   m_llCurCount;
	LARGE_INTEGER   m_llFrequency;

	float			m_fDeltaTime;
	double			m_dAccTime;
	float			m_fFPS;

	bool			m_bFreeze; // DT 가 과하게 넘어갈때 동결

public:
	void init();
	void update();
	void render(HDC _dc);

public:
	float GetDeltaTime() { return m_fDeltaTime; }
	float GetFPS() { return m_fFPS; }
	void DTFreeze() { m_bFreeze = true; }
	SINGLE(CTimeMgr);
};

