#pragma once
#include <Script.h>

class CPlayerScript;

class CItemScript :
	public CScript
{
public:
	CItemScript();
	virtual ~CItemScript();

private:
	Vec2			m_vecMtrlPosRaito;
	Vec2			m_vecMtrlSizeRaito;
	Vec2			m_vBackgroundSize;
	Vec2			m_vIconSize;
	Vec2			m_iIconIdx;
	bool			m_bDrag;
	wstring			Info;
	wstring			m_TexName;
	int				m_iNum;
	int				m_iItemEffectCount;
	float			m_fClickTime;
	ITEM_TYPE		m_eType;
	CGameObject*	m_pIconObj;
	CPlayerScript*	m_pPlayer;


public:
	void SetItemInfo(wstring& _name, wstring& _info, Vec2 _iIdx, int _count, int _location, int _iNum = 1);
	void ChangeItemNum(int _num);
	void AddItemNum(int _num);
	void SubItemNum(int _num);
	void UseItem();
	void OnDisable();
	void OnEnable();
	wstring GetItemName() { return m_TexName; }

private:
	void UseHpPortion();
	void UseMpPortion();
	void UseSword();
	void UseShield();

public:
	virtual void start();
	virtual void update();
	virtual void OnCollisionClick();

private:
	void CalIconIdx();
public:
	CLONE(CItemScript);
};

