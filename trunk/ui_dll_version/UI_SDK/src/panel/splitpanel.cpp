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

void SplitDragPanel::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_ESCAPE == nChar)
	{
		UIASSERT(0);
	}
}	

void SplitDragPanel::OnLButtonDown(UINT nFlags, POINT point)
{
	m_ptLButtonDown = point;
	m_bButtonDown = true;
}
void SplitDragPanel::OnLButtonUp(UINT nFlags, POINT point)
{
	this->OnCancelMode();
}
void SplitDragPanel::OnCancelMode()
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
		POINT ptObjClient;
		m_pPanel->WindowPoint2ObjectClientPoint(&point, &ptObjClient);

		m_pPanelDragBar->SetObjectPos(ptObjClient.x, m_pPanelDragBar->GetMarginT() ,0, SWP_NOSIZE|SWP_NOREDRAW);
		this->Arrange(NULL, false);
		m_pPanel->UpdateObject();
	}
	else
	{
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
	m_eDragbarAlign = LAYOUT_SPLIT_DRAGBAR_ALIGN_LEFT;
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

//
// 注：arrange时，有两种情况：
//     1. 窗口大小改变，这时候需要判断拖拽条的对齐属性来设置相应child object的大小
//     2. 拖拽条位置改变，这时候需要修改两个子对象的大小
//
void SplitLayout::ArrangeChildObject(Object* pObjToArrage, bool bReDraw)
{
	// 调用该函数时，自己的大小已经被求出来了
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	// 获取需要布局的子对象指针（只布局两个子对象）
	Object* pChild = NULL;
	Object* pFirstChild = NULL;
	Object* pSecondChild = NULL;
	int i = 0;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		int nIndex = i++;
		if (!pChild->IsMySelfVisible())
			continue;

		if (nIndex > 1)
			break;    // 只能布局两个子对象

		if (nIndex == 0)
			pFirstChild = pChild;
		else if (nIndex == 1)
			pSecondChild = pChild;
	}

	if (m_eDirection == LAYOUT_SPLIT_DIRECTION_H)
	{
		// 1. 设置拖拽条的位置
		if (m_pPanelDragBar->IsDraging())
		{
			m_pPanelDragBar->SetObjectPos(0, 0, 10, nHeight-m_pPanelDragBar->GetMarginH(), SWP_NOMOVE|SWP_NOREDRAW);
		}
		else
		{
			if (m_eDragbarAlign == LAYOUT_SPLIT_DRAGBAR_ALIGN_LEFT)
			{
				// 拖拽条相对底部高度不变
				int nFirstChildWidth = 0;
				if (NULL == pFirstChild)
				{
					nFirstChildWidth = 0;
				}
				else
				{
					if (0 == pFirstChild->GetWidth())  // 可能是还没有初始化，使用layout.dragbarpos的值
					{
						String strInitPos;
						if (m_pPanel->GetAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS, strInitPos))
						{
							nFirstChildWidth = _ttoi(strInitPos.c_str());
							m_pPanel->EraseAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS);
						}
					}
					else
					{
						nFirstChildWidth = pFirstChild->GetWidthWithMargins();
					}
				}

				m_pPanelDragBar->SetObjectPos(
					nFirstChildWidth + m_pPanelDragBar->GetMarginL(), 
					m_pPanelDragBar->GetMarginT(), 
					10, 
					nHeight-m_pPanelDragBar->GetMarginH(), 
					SWP_NOREDRAW);
			}
			else if (m_eDragbarAlign == LAYOUT_SPLIT_DRAGBAR_ALIGN_BOTTOM)
			{
				// 拖拽条相对底部高度不变
				int nSecondChildWidth = 0;
				if (NULL == pSecondChild)
				{
					nSecondChildWidth = 0;
				}
				else
				{
					if (0 == pSecondChild->GetWidth())  // 可能是还没有初始化，使用layout.dragbarpos的值
					{
						String strInitPos;
						if (m_pPanel->GetAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS, strInitPos))
						{
							nSecondChildWidth = _ttoi(strInitPos.c_str());
							m_pPanel->EraseAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS);
						}
					}
					else
					{
						nSecondChildWidth = pSecondChild->GetWidthWithMargins();
					}
				}

				m_pPanelDragBar->SetObjectPos(
					nWidth - 10 - nSecondChildWidth - m_pPanelDragBar->GetMarginR(), 
					m_pPanelDragBar->GetMarginT(), 
					10,
					nHeight-m_pPanelDragBar->GetMarginH(), 
					SWP_NOREDRAW);
			}
		}

		// 2. 根据拖拽条的位置设置子对象的位置
		CRect rcDragBar;
		m_pPanelDragBar->GetParentRect(&rcDragBar);

		if (NULL != pFirstChild)
		{
			pFirstChild->SetObjectPos(
				pFirstChild->GetMarginL(), 
				pFirstChild->GetMarginT(), 
				rcDragBar.left-pFirstChild->GetMarginW() - m_pPanelDragBar->GetMarginL(), 
				nHeight - pFirstChild->GetMarginH(), 
				SWP_NOREDRAW);
		}

		if (NULL != pSecondChild)
		{
			pSecondChild->SetObjectPos(
				rcDragBar.right + pSecondChild->GetMarginL() + m_pPanelDragBar->GetMarginR(),
				pSecondChild->GetMarginT(), 
				nWidth - rcDragBar.right - pSecondChild->GetMarginW() - m_pPanelDragBar->GetMarginR(), 
				nHeight - pSecondChild->GetMarginH(),
				SWP_NOREDRAW);
		}
	}
	else
	{
		// 1. 设置拖拽条的位置
		if (m_pPanelDragBar->IsDraging())
		{
			m_pPanelDragBar->SetObjectPos(0, 0, nWidth-m_pPanelDragBar->GetMarginW(), 10, SWP_NOMOVE|SWP_NOREDRAW);
		}
		else
		{
			if (m_eDragbarAlign == LAYOUT_SPLIT_DRAGBAR_ALIGN_TOP)
			{
				// 拖拽条相对底部高度不变
				int nFirstChildHeight = 0;
				if (NULL == pFirstChild)
				{
					nFirstChildHeight = 0;
				}
				else
				{
					if (0 == pFirstChild->GetHeight())  // 可能是还没有初始化，使用layout.dragbarpos的值
					{
						String strInitPos;
						if (m_pPanel->GetAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS, strInitPos))
						{
							nFirstChildHeight = _ttoi(strInitPos.c_str());
							m_pPanel->EraseAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS);
						}
					}
					else
					{
						nFirstChildHeight = pFirstChild->GetHeightWithMargins();
					}
				}

				m_pPanelDragBar->SetObjectPos(
					m_pPanelDragBar->GetMarginL(), 
					nFirstChildHeight + m_pPanelDragBar->GetMarginT(), 
					nWidth-m_pPanelDragBar->GetMarginW(), 
					10, SWP_NOREDRAW);
			}
			else if (m_eDragbarAlign == LAYOUT_SPLIT_DRAGBAR_ALIGN_BOTTOM)
			{
				// 拖拽条相对底部高度不变
				int nSecondChildHeight = 0;
				if (NULL == pSecondChild)
				{
					nSecondChildHeight = 0;
				}
				else
				{
					if (0 == pSecondChild->GetHeight())  // 可能是还没有初始化，使用layout.dragbarpos的值
					{
						String strInitPos;
						if (m_pPanel->GetAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS, strInitPos))
						{
							nSecondChildHeight = _ttoi(strInitPos.c_str());
							m_pPanel->EraseAttribute(XML_LAYOUT_SPLIT_DRAGBAR_INITPOS);
						}
					}
					else
					{
						nSecondChildHeight = pSecondChild->GetHeightWithMargins();
					}
				}
					
				m_pPanelDragBar->SetObjectPos(
					m_pPanelDragBar->GetMarginL(), 
					nHeight-10-nSecondChildHeight - m_pPanelDragBar->GetMarginB(), 
					nWidth-m_pPanelDragBar->GetMarginW(), 
					10, SWP_NOREDRAW);
			}
		}

		// 2. 根据拖拽条的位置设置子对象的位置
		CRect rcDragBar;
		m_pPanelDragBar->GetParentRect(&rcDragBar);

		if (NULL != pFirstChild)
		{
			pFirstChild->SetObjectPos(
				pFirstChild->GetMarginL(), 
				pFirstChild->GetMarginT(), 
				nWidth - pFirstChild->GetMarginW(), 
				rcDragBar.top-pFirstChild->GetMarginH() - m_pPanelDragBar->GetMarginT(), 
				SWP_NOREDRAW);
		}

		if (NULL != pSecondChild)
		{
			pSecondChild->SetObjectPos(
				pSecondChild->GetMarginL(), 
				rcDragBar.bottom + pSecondChild->GetMarginT() + m_pPanelDragBar->GetMarginB(),
				nWidth - pSecondChild->GetMarginW(),
				nHeight - rcDragBar.bottom - pSecondChild->GetMarginH() - m_pPanelDragBar->GetMarginB(), 
				SWP_NOREDRAW);
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

	iter = mapAttrib.find(XML_LAYOUT_SPLIT_DRAGBAR_ALIGN);
	if (iter != mapAttrib.end())
	{
		const String&  str = iter->second;
		if (str == XML_LAYOUT_SPLIT_DRAGBAR_ALIGN_LEFT)
		{
			m_eDragbarAlign = LAYOUT_SPLIT_DRAGBAR_ALIGN_LEFT;
		}
		else if (str == XML_LAYOUT_SPLIT_DRAGBAR_ALIGN_RIGHT)
		{
			m_eDragbarAlign = LAYOUT_SPLIT_DRAGBAR_ALIGN_RIGHT;
		}
		else if (str == XML_LAYOUT_SPLIT_DRAGBAR_ALIGN_TOP)
		{
			m_eDragbarAlign = LAYOUT_SPLIT_DRAGBAR_ALIGN_TOP;
		}
		else if (str == XML_LAYOUT_SPLIT_DRAGBAR_ALIGN_BOTTOM)
		{
			m_eDragbarAlign = LAYOUT_SPLIT_DRAGBAR_ALIGN_BOTTOM;
		}
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
