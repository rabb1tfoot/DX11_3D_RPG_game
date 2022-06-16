#pragma once
#include <Script.h>
#include <GameObject.h>
#include <Transform.h>
#include <SceneMgr.h>
#include <Scene.h>

template<typename T>
class CSingleScript :
	public CScript
{
protected:
	CSingleScript(SCRIPT_TYPE _eType)
		: CScript((UINT)_eType)
	{
	}
	virtual ~CSingleScript()
	{
		m_pThis = nullptr;
	}

private:
	static T*		m_pThis;

public:
	static T* GetInst()
	{
		if (nullptr == m_pThis)
		{
			m_pThis = new T;

			vector<CGameObject*> vecObject;
			CSceneMgr::GetInst()->FindGameObject(L"DummyObject", vecObject);

			if (vecObject.empty())
			{
				CGameObject* pObject = new CGameObject;
				pObject->SetName(L"DummyObject");
				pObject->AddComponent(new CTransform);
				CSceneMgr::GetInst()->GetCurScene()->AddObject(L"Default", pObject);

				pObject->AddComponent(m_pThis);
			}
			else
			{
				vecObject[0]->AddComponent(m_pThis);
			}
		}

		return m_pThis;
	}
};

template<typename T>
T*	CSingleScript<T>::m_pThis = nullptr;

