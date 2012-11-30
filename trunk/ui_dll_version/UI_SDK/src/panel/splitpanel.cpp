#include "stdafx.h"


SplitPanel::SplitPanel()
{
	UICreateInstance(&m_pPicObj);

	if (NULL != m_pPicObj)
	{
		this->AddChild(m_pPicObj);
	}
	
}

SplitPanel::~SplitPanel()
{
	SAFE_DELETE(m_pPicObj);
	SAFE_DELETE(m_pBtnLeftUp);
	SAFE_DELETE(m_pBtnRightDown);
}


bool SplitPanel::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	mapAttrib[XML_LAYOUT] = XML_LAYOUT_SPLIT;  // 强制覆盖 或者添加该属性值

	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	ATTRMAP::iterator iter = mapAttrib.find(XML_LAYOUT_SPLIT_DIRECTION);
	if (iter != mapAttrib.end())
	{
		if (iter->second == XML_LAYOUT_SPLIT_DIRECTION_V)
		{
			m_dir = LAYOUT_SPLIT_DIRECTION_V;
		}
		else
		{
			m_dir = LAYOUT_SPLIT_DIRECTION_H;
		}
		mapAttrib.erase(iter);
	}

	iter = mapAttrib.find(XML_LAYOUT_SPLIT_SHOW_BUTTON);
	if (iter != mapAttrib.end())
	{

	}
	else
	{
		SAFE_DELETE(m_pBtnLeftUp);
		SAFE_DELETE(m_pBtnRightDown);
	}

	return true;
}

#pragma region
SplitLayout::SplitLayout()
{
	m_dir = LAYOUT_SPLIT_DIRECTION_H;
}
SIZE SplitLayout::MeasureChildObject()
{
	UIASSERT(0);  // split layout需要外部指定size
	SIZE s = {0,0};
	return s;
}
void SplitLayout::ArrangeChildObject(Object* pObjToArrage, bool bReDraw)
{
	
}
bool SplitLayout::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;


	return true;
}
#pragma endregion