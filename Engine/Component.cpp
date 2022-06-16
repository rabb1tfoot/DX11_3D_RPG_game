#include "stdafx.h"
#include "Component.h"

CComponent::CComponent(COMPONENT_TYPE _eType)
	: m_eComType(_eType)
{
}

CComponent::~CComponent()
{
}

CComponent::CComponent(const CComponent & _com)
	: CEntity(_com)
	, m_eComType(_com.m_eComType)
	, m_pObject(nullptr)
{
}