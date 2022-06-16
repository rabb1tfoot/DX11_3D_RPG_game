#pragma once
#include <Script.h>
#include <struct.h>

class CInventory :
	public CScript
{
private:
	CGameObject*		m_arrItem[MAX_INVENTORY];
	Vec2				m_arrItemLocation[MAX_INVENTORY];
	CGameObject*		m_pBackTexture;
	vector<tItem>		m_vItemList;
	bool				m_bEnable;

public:
	void InventoryOn();
	void InventoryOff();
	void AddItem(wstring _name, int _num);
	bool GetEnable() { return  m_bEnable; };

private:
	CGameObject* CraeteItem(wstring _name, int _num, int _location);
	void CreateItemList();

public:
	virtual void start();
	virtual void update();



public:
	CInventory();
	virtual ~CInventory();

public:
	CLONE(CInventory);
};

