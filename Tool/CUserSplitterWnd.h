#pragma once
#include <afxext.h>

class CUserSplitterWnd :
	public CSplitterWnd
{
public:
	void SetBorder(POINT _ptEdge, POINT _ptBorder)
	{
		m_cxBorder = _ptEdge.x;
		m_cyBorder = _ptEdge.y;
		m_cxSplitterGap = _ptBorder.x;
		m_cxSplitterGap = _ptBorder.y;
	}
	CUserSplitterWnd();
	virtual ~CUserSplitterWnd();
};

