#include "stdafx.h"

LEDCtrlBase::LEDCtrlBase()
{
	m_pImagelist = NULL;
}

void  LEDCtrlBase::ResetAttribute()
{
	__super::ResetAttribute();
	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0);
	m_pImagelist = NULL;
}
bool  LEDCtrlBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	if(NULL == m_pImagelist)
	{
		m_pImagelist = dynamic_cast<ImageListRender*>(m_pForegndRender);
	}

	if (mapAttrib.count(XML_LEDCTRL_INDEXMAP))
	{
		this->SetIndexMap(mapAttrib[XML_LEDCTRL_INDEXMAP]);
		m_mapAttribute.erase(XML_LEDCTRL_INDEXMAP);
	}

	if (false == bReload)
	{
		if (mapAttrib.count(XML_TEXT))
		{
			this->SetText(mapAttrib[XML_TEXT], false);
			m_mapAttribute.erase(XML_TEXT);
		}
	}

	return true;
}
SIZE  LEDCtrlBase::GetAutoSize( HRDC hRDC )
{
	if (NULL == m_pImagelist)
	{
		SIZE s = {0,0};
		return s;
	}

	SIZE s = {
		m_pImagelist->GetItemWidth() * (int)m_vecIndex.size(),
		m_pImagelist->GetItemHeight()
	};
	return s;
}

// 设置图片中的每一项对应的字符
void  LEDCtrlBase::SetIndexMap(const String& str)
{
	m_mapIndex.clear();

	int nSize = str.length();
	for (int i = 0; i < nSize; i++)
	{
		m_mapIndex[str[i]] = i;
	}
}

// 设置要显示的文本
void LEDCtrlBase::SetText(const String& str, bool bUpdate)
{
	m_vecIndex.clear();
	m_strText = str;

	int nLength = m_strText.length();
	for (int i = 0; i < nLength; i++)
	{
		if (m_mapIndex.count(m_strText[i]))
		{
			m_vecIndex.push_back(m_mapIndex[m_strText[i]]);
		}
		else
		{
			m_vecIndex.push_back(-1);   //  无效字符
		}
	}	
	if (bUpdate)
	{
		this->UpdateObject();
	}
}

void LEDCtrlBase::OnPaint(HRDC hRDC)
{
	if (NULL == m_pForegndRender)
		return;

	int nSize = (int)m_vecIndex.size();
	CRect  rcClient;
	this->GetClientRectAsWin32(&rcClient);
	
	CRect  rcItem = rcClient;
	rcItem.right = rcItem.left;
	rcItem.left = rcItem.right - m_pImagelist->GetItemWidth(); 

	for (int i = 0; i < nSize; i++)
	{
		rcItem.OffsetRect(m_pImagelist->GetItemWidth(), 0);
		int nIndex = m_vecIndex[i];
		if (-1 == nIndex)
			continue;

		m_pForegndRender->DrawState(hRDC, &rcItem, nIndex);
	}	
}