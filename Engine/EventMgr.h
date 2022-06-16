#pragma once

class CGameObject;

class CEventMgr
{
	SINGLE(CEventMgr);
private:
	vector<tEvent>			m_vecEvent;
	vector<CGameObject*>	m_vecDeleteObj;
	vector<CGameObject*>	m_vecNewObj;

public:
	void update();

public:
	void AddEvent(tEvent& _event) { m_vecEvent.push_back(_event); }

};