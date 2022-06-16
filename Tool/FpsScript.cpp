#include "stdafx.h"
#include "FpsScript.h"
#include <TimeMgr.h>
#include <GameObject.h>
#include <Font23.h>


CFpsScript::CFpsScript()
	:CScript((UINT)COMPONENT_TYPE::SCRIPT)
{
}


CFpsScript::~CFpsScript()
{
}

void CFpsScript::update()
{
	float fFPS = CTimeMgr::GetInst()->GetFPS();
	float Xpos = CKeyMgr::GetInst()->GetMousePos().x;
	float Ypos = CKeyMgr::GetInst()->GetMousePos().y;
	Vec2 mPos = Vec2(Xpos, Ypos);
	wchar_t buffer[40] = L"";
	_stprintf_s(buffer, 40, L"FPS :%.1f\n MOUSE: %.1f %.1f", fFPS, mPos.x, mPos.y);
	if (Object()->Font())
	{
		Object()->Font()->SetText(buffer);
	}

}