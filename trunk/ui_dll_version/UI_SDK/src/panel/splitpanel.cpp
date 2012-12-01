#include "stdafx.h"

SplitDragPanel::SplitDragPanel()
{
	m_ptLButtonDown.x = -1;
	m_ptLButtonDown.y = -1;
	m_ptLastMouseMove.x = -1;
	m_ptLastMouseMove.y = -1;

	m_pLayout = NULL;
	m_bButtonDown = false;
}
void SplitDragPanel::SetSplitLayout(SplitLayout* pLayout)
{
	m_pLayout = pLayout;
}

void SplitDragPanel::OnLButtonDown(UINT nFlags, POINT point)
{
	m_ptLButtonDown = point;
	m_bButtonDown = true;
}
void SplitDragPanel::OnLButtonUp(UINT nFlags, POINT point)
{
	m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
	m_ptLastMouseMove.x = m_ptLastMouseMove.y = -1;
	m_bButtonDown = false;
}
void SplitDragPanel::OnMouseMove(UINT nFlags, POINT point)
{
	if (m_bButtonDown)
	{
		// 防止外部其它因素产生的mousemove消息持续发送，导致CPU占用率高的问题
		if (point.x == m_ptLastMouseMove.x && point.y == m_ptLastMouseMove.y)
			return;

		m_pLayout->OnDraging(point);
		m_ptLastMouseMove = point;
	}
}

void SplitLayout::OnDragBegin(POINT point)
{
	
}
void SplitLayout::OnDraging(POINT point)
{
	if (m_eDirection == LAYOUT_SPLIT_DIRECTION_H)
	{
	}
	else
	{
// 		CRect rcClient;
// 		m_pPanel->GetClientRectAsWin32(&rcClient);

		POINT ptObjClient;
		m_pPanel->WindowPoint2ObjectClientPoint(&point, &ptObjClient);

		m_pPanelDragBar->SetObjectPos(m_pPanelDragBar->GetMarginL(), ptObjClient.y, 0,0, SWP_NOSIZE|SWP_NOREDRAW);
		this->Arrange(NULL, false);
		m_pPanel->UpdateObject();
	}
}
void SplitLayout::OnDragEnd(POINT point)
{

}
//////////////////////////////////////////////////////////////////////////

SplitLayout::SplitLayout()
{
	m_pPanelDragBar = NULL;
	m_pBtnLeftUp = NULL;
	m_pBtnRightDown = NULL;
	m_eDirection = LAYOUT_SPLIT_DIRECTION_H;
}
SplitLayout::~SplitLayout()
{
	m_pPanelDragBar = NULL;   // 将被parent object销毁
	m_pBtnLeftUp = NULL;
	m_pBtnRightDown = NULL;

// 	SAFE_DELETE(m_pPanelDragBar);
// 	SAFE_DELETE(m_pBtnLeftUp);
// 	SAFE_DELETE(m_pBtnRightDown);
}
SIZE SplitLayout::MeasureChildObject()
{
	UIASSERT(0);  // split layout需要外部指定size
	SIZE s = {0,0};
	return s;
}
void SplitLayout::ArrangeChildObject(Object* pObjToArrage, bool bReDraw)
{
	// 调用该函数时，自己的大小已经被求出来了
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	// 只布局两个子对象

	if (m_eDirection == LAYOUT_SPLIT_DIRECTION_H)
	{
		
	}
	else
	{
		m_pPanelDragBar->SetObjectPos(0, 0, nWidth, 10, SWP_NOMOVE|SWP_NOREDRAW);

		CRect rcDragBar;
		m_pPanelDragBar->GetParentRect(&rcDragBar);

		Object* pChild = NULL;
		int i = 0;
		while (pChild = m_pPanel->EnumChildObject(pChild))
		{
			if (i == 0 && pChild->IsMySelfVisible())
				pChild->SetObjectPos(0, 0, nWidth, rcDragBar.top, SWP_NOREDRAW);
			else if (i == 1 && pChild->IsMySelfVisible())
				pChild->SetObjectPos(0, rcDragBar.bottom, nWidth, nHeight-rcDragBar.bottom, SWP_NOREDRAW);
			else
				break;  // 只能布局两个子对象

			i++;
		}
	}
}
bool SplitLayout::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	if (NULL == m_pPanel)
		return false;

	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	SAFE_DELETE(m_pPanelDragBar);
	UICreateInstance(&m_pPanelDragBar);
	if (NULL == m_pPanelDragBar)
		return false;
	m_pPanelDragBar->SetSplitLayout(this);

	this->m_pPanel->AddNcChild(m_pPanelDragBar);

#ifdef _DEBUG
	m_pPanelDragBar->SetObjectPos(0, 300, 0, 10, SWP_NOREDRAW);
#endif

	ATTRMAP::iterator iter = mapAttrib.find(XML_LAYOUT_SPLIT_DIRECTION);
	if (iter != mapAttrib.end())
	{
		if (iter->second == XML_LAYOUT_SPLIT_DIRECTION_V)
		{
			m_eDirection = LAYOUT_SPLIT_DIRECTION_V;
			this->m_pPanelDragBar->SetCursor(XML_CURSOR_IDC_SIZENS);
		}
		else
		{
			m_eDirection = LAYOUT_SPLIT_DIRECTION_H;
			this->m_pPanelDragBar->SetCursor(XML_CURSOR_IDC_SIZEWE);
		}
		mapAttrib.erase(iter);
	}

	// 设置dragpanel的属性
	iter = mapAttrib.find(XML_LAYOUT_SPLIT_DRAGBAR_PREFIX XML_ID);
	if (iter == mapAttrib.end())
	{
		mapAttrib[XML_LAYOUT_SPLIT_DRAGBAR_PREFIX XML_ID] = _T("splitdragpanel");
	}
	m_pPanel->SetChildObjectAttribute(m_pPanelDragBar, XML_LAYOUT_SPLIT_DRAGBAR_PREFIX, mapAttrib, bReload);

	// 设置按钮的属性
	SAFE_DELETE(m_pBtnLeftUp);
	SAFE_DELETE(m_pBtnRightDown);
	iter = mapAttrib.find(XML_LAYOUT_SPLIT_SHOW_BUTTON);
	if (iter != mapAttrib.end())
	{
		UICreateInstance(&m_pBtnLeftUp);
		UICreateInstance(&m_pBtnRightDown);
		this->m_pPanelDragBar->AddChild(m_pBtnLeftUp);
		this->m_pPanelDragBar->AddChild(m_pBtnRightDown);

		mapAttrib.erase(iter);

		// 设置按钮属性，如果没有配置按钮ID，这里设置一个默认值
		if (m_eDirection == LAYOUT_SPLIT_DIRECTION_H)
		{
			iter = mapAttrib.find(XML_LAYOUT_SPLIT_LEFT_BUTTON_PREFIX XML_ID);
			if (iter == mapAttrib.end())
			{
				mapAttrib[XML_LAYOUT_SPLIT_LEFT_BUTTON_PREFIX XML_ID] = _T("leftsplitbtn");
			}
			m_pPanelDragBar->SetChildObjectAttribute(m_pBtnLeftUp, XML_LAYOUT_SPLIT_LEFT_BUTTON_PREFIX, mapAttrib, bReload);

			iter = mapAttrib.find(XML_LAYOUT_SPLIT_RIGHT_BUTTON_PREFIX XML_ID);
			if (iter == mapAttrib.end())
			{
				mapAttrib[XML_LAYOUT_SPLIT_RIGHT_BUTTON_PREFIX XML_ID] = _T("rightsplitbtn");
			}
			m_pPanelDragBar->SetChildObjectAttribute(m_pBtnRightDown, XML_LAYOUT_SPLIT_RIGHT_BUTTON_PREFIX, mapAttrib, bReload);
		}
		else
		{
			iter == mapAttrib.find(XML_LAYOUT_SPLIT_UP_BUTTON_PREFIX XML_ID);
			if (iter == mapAttrib.end())
			{
				mapAttrib[XML_LAYOUT_SPLIT_UP_BUTTON_PREFIX XML_ID] = _T("upsplitbtn");
			}
			m_pPanelDragBar->SetChildObjectAttribute(m_pBtnLeftUp, XML_LAYOUT_SPLIT_UP_BUTTON_PREFIX, mapAttrib, bReload);

			iter = mapAttrib.find(XML_LAYOUT_SPLIT_DOWN_BUTTON_PREFIX XML_ID);
			if (iter == mapAttrib.end())
			{
				mapAttrib[XML_LAYOUT_SPLIT_DOWN_BUTTON_PREFIX XML_ID] = _T("downsplitbtn");
			}
			m_pPanelDragBar->SetChildObjectAttribute(m_pBtnRightDown, XML_LAYOUT_SPLIT_DOWN_BUTTON_PREFIX, mapAttrib, bReload);
		}
	}

	return true;
}
