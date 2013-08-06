#include "stdafx.h"
#if 0
#include <algorithm>
#include "UISDK\Control\Src\Control\HeaderCtrl\headerctrl.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"

namespace UI
{

HeaderItemBase::HeaderItemBase()
{
	m_nFormat = -1;
	m_nWidth = 0;
	m_lParam = 0;
}

HeaderCtrl::HeaderCtrl()
{
    m_pIHeaderCtrl = NULL;

	m_nDividerWidth = 5;
	m_pRightBlankRender = NULL;
	m_pSplitLineRender = NULL;
	m_pSortArrowRender = NULL;
	m_pHoverItem = NULL;
	m_pCursorDragDivider = NULL;
	m_pCursorDragDividerHidden = NULL;
	m_pPressItem = NULL;

	m_bDragingDivider = false;
	m_bBkgndRenderContainSplitLine = false;
	m_bDrawOffsetPressDown = false;

	m_ptLastDragDivider.x = m_ptLastDragDivider.x = 0;
	m_nOldDragingColoumnWidth = 0;

	m_nHScrollOffset = 0;
}
HeaderCtrl::~HeaderCtrl()
{
	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (; iter != iterEnd; iter++)
	{
		HeaderItemBase* pItem = *iter;
		SAFE_DELETE(pItem);
	}
	m_listHeaderItem.clear();

	SAFE_RELEASE(m_pRightBlankRender);
	SAFE_RELEASE(m_pSortArrowRender);
	SAFE_RELEASE(m_pCursorDragDivider);
	SAFE_RELEASE(m_pCursorDragDividerHidden);
	SAFE_RELEASE(m_pSplitLineRender);

	m_pHoverItem = NULL;
	m_pPressItem = NULL;
}
HRESULT  HeaderCtrl::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IHeaderCtrl, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIHeaderCtrl->ModifyStyle(OBJECT_STYLE_HSCROLL, 0, 0);

    //if (m_pIProgressCtrl->GetUIApplication()->IsDesignMode())
    {
        this->InsertColumn(0, _T("First"), 50, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
        this->InsertColumn(1, _T("Second"), 50, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        this->InsertColumn(2, _T("Third"), 50, DT_RIGHT|DT_SINGLELINE|DT_VCENTER);
    }
    return S_OK;
}
void HeaderCtrl::ResetAttribute()
{
	DO_PARENT_PROCESS(IHeaderCtrl, IControl)

	SAFE_RELEASE(m_pCursorDragDivider);
	SAFE_RELEASE(m_pCursorDragDividerHidden);
	SAFE_RELEASE(m_pRightBlankRender);
	SAFE_RELEASE(m_pSortArrowRender);
	SAFE_RELEASE(m_pSplitLineRender);
	m_bBkgndRenderContainSplitLine = false;
	m_bDrawOffsetPressDown = false;

	m_pIHeaderCtrl->ModifyStyle(HEADERCTRL_STYLE_CANDRAG, CONTROL_STYLE_TABSTOP, false);  // 默认能拖拽，不抢listview的焦点
}

void  HeaderCtrl::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IHeaderCtrl, IControl);

    IUIApplication*  pUIApp = m_pIHeaderCtrl->GetUIApplication();
    ITextRenderBase*  pTextRender = NULL;
    pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApp, 
                                    static_cast<IObject*>(m_pIHeaderCtrl), &pTextRender);
    if (pTextRender)
    {
        m_pIHeaderCtrl->SetTextRender(pTextRender);
        pTextRender->Release();
    }
	if (NULL == m_pIHeaderCtrl->GetTextRender())
	{
        pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIHeaderCtrl, &pTextRender);
        if (pTextRender)
        {
            INormalTextRender* p = (INormalTextRender*)UISendMessage(pTextRender, UI_WM_QUERYINTERFACE, (WPARAM)&uiiidof(INormalTextRender));
            if (p)
            {
                Color c(76,96,122,255);
                p->SetColor(&c);
            }

            pTextRender->SetAttribute(pMapAttrib, NULL, true);
            this->m_pIHeaderCtrl->SetTextRender(pTextRender);
            pTextRender->Release();
        }
	}

    SAFE_RELEASE(m_pRightBlankRender);
    pMapAttrib->GetAttr_RenderBase(XML_HEADERCTRL_RIGHT_BLANK_RENDER_PRIFIX, XML_RENDER_TYPE, true, pUIApp, m_pIHeaderCtrl, &m_pRightBlankRender);

    SAFE_RELEASE(m_pSplitLineRender);
    pMapAttrib->GetAttr_RenderBase(XML_HEADERCTRL_SPLIT_LINE_RENDER_PREFIX, XML_RENDER_TYPE, true, pUIApp, m_pIHeaderCtrl, &m_pSplitLineRender);

    if (NULL == m_pRightBlankRender)  // 右侧背景直接使用背景图进行渲染
    {
        m_pRightBlankRender = m_pIHeaderCtrl->GetBkRender();
        if (m_pRightBlankRender)
            m_pRightBlankRender->AddRef();
    }
    
	if (NULL == m_pIHeaderCtrl->GetBkRender())
	{
        IRenderBase* pRenderBase = NULL;
        pUIApp->CreateRenderBase(HEADER_TYPE_THEME_HEADERCTRL_ITEM_BKGND, m_pIHeaderCtrl, &pRenderBase);
        if (pRenderBase)
        {
    		pRenderBase->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
	    	m_bBkgndRenderContainSplitLine = true;
            m_pIHeaderCtrl->SetBkgndRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
	}
	if (NULL == m_pRightBlankRender)
	{
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_HEADERCTRL_RIGHT_BLANK, m_pIHeaderCtrl, &m_pRightBlankRender);
		if (m_pRightBlankRender)
    		m_pRightBlankRender->SetAttribute(pMapAttrib, NULL, true);
	}

	ICursorRes* pCursorRes = pUIApp->GetActiveSkinCursorRes();
	if (pCursorRes)
	{
		if (NULL == m_pCursorDragDivider)
		{
			pCursorRes->GetCursor(XML_CURSOR_IDC_DRAG_SEPERATOR, &m_pCursorDragDivider);
		}
		if (NULL == m_pCursorDragDividerHidden)
		{
			pCursorRes->GetCursor(XML_CURSOR_IDC_SPLITBAR_H, &m_pCursorDragDividerHidden);
		}
	}
}

void  HeaderCtrl::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = GetColumnTotalWidth();
    pSize->cy = 20;

    IRenderBase* pBkgndRender = m_pIHeaderCtrl->GetBkRender();
	if (pBkgndRender)
	{
		int nHeight = pBkgndRender->GetDesiredSize().cy;
		if (0 != nHeight)
			pSize->cy = nHeight;
	}
}

int HeaderCtrl::GetColumnItemIndex(HeaderItemBase* pItem)
{
	if (NULL == pItem)
		return -1;
	
	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; iter != iterEnd; i++, iter++)
	{
		if (*iter == pItem)
			return i;
	}

	return -1;
}

bool HeaderCtrl::AddColumn(HeaderItemBase* pItem)
{
	m_listHeaderItem.push_back(pItem);

	FireTotalWidthChanged();
	return true;
}

bool HeaderCtrl::InsertColumn(HeaderItemBase* pItem, int nPos)
{
	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; i < nPos; i++)
	{
		iter++;
		if (iter == iterEnd)
			break;
	}

	if (iter == iterEnd)
	{
		m_listHeaderItem.push_back(pItem);
	}
	else
	{
		m_listHeaderItem.insert(iter, pItem);
	}

	FireTotalWidthChanged();
	return true;
}

bool HeaderCtrl::RemoveColumn(HeaderItemBase* pItem)
{
	list<HeaderItemBase*>::iterator iter = std::find(m_listHeaderItem.begin(), m_listHeaderItem.end(), pItem);
	if (iter != m_listHeaderItem.end())
	{
		HeaderItemBase* p = *iter;
		SAFE_DELETE(p);
		m_listHeaderItem.erase(iter);

		return true;
	}

	FireTotalWidthChanged();
	return false;
}

int HeaderCtrl::GetColumnCount()
{
	return (int)m_listHeaderItem.size();
}

HeaderItemBase* HeaderCtrl::GetHeaderItem(int nCol)
{
	if (nCol < 0 || nCol >= GetColumnCount())
		return NULL;

	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		if (i == nCol)
			return *iter;
	}
	return NULL;
}

// 将每一列的宽度返回给pWidthArray数组，外部需要保证pWidthArray数据大小大于column count
void HeaderCtrl::GetColumnWidthArray(int* pWidthArray)
{
	if (NULL == pWidthArray)
		return;
	
	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		pWidthArray[i] = (*iter)->m_nWidth;
	}
}

// 获取某一列所在区间坐标
void HeaderCtrl::GetColumnLeftRight(int nColumnIndex, int* pLeft, int* pRight)
{
	int nCount = GetColumnCount();
	if (nColumnIndex < 0 || nColumnIndex > nCount)
		return;

	int xLeft = 0, xRight = 0;

	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		xLeft = xRight;
		xRight += (*iter)->m_nWidth;

		if (i == nColumnIndex)
		{
			if (pLeft) *pLeft = xLeft;
			if (pRight) *pRight = xRight;
			return;
		}
	}
}

int HeaderCtrl::GetColumnTotalWidth()
{
	int nRet = 0;

	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (int i = 0; iter != iterEnd; iter++, i++)
	{
		nRet += (*iter)->m_nWidth;
	}

	return nRet;
}

bool HeaderCtrl::SetItemWidth(int nColumn, int nNewWidth, bool bNotify)
{
	HeaderItemBase* pItem = GetHeaderItem(nColumn);
	if (NULL == pItem)
		return false;

	if (pItem->m_nWidth == nNewWidth)
		return true;

	pItem->m_nWidth = nNewWidth;

	if (bNotify)
	{
		UIMSG  msg;
		msg.pMsgFrom = static_cast<IMessage*>(m_pIHeaderCtrl);
		msg.message = UI_WM_NOTIFY;
		msg.nCode = UI_HDCN_TRACKING;
		msg.wParam = nColumn;
		msg.lParam = (LPARAM)&(pItem->m_nWidth);

		if (m_pIHeaderCtrl->DoNotify(&msg))
		{
			FireTotalWidthChanged();
		}
	}

	

	return false;
}

void HeaderCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
// 	POINT pt = {0,0};
// 	if (0 != m_nHScrollOffset)
// 	{
// 		pRenderTarget->GetViewportOrgEx(&pt);
// 		pRenderTarget->OffsetViewportOrgEx(-m_nHScrollOffset, 0);
// 	}
	
	CRect rcClient;
	m_pIHeaderCtrl->GetClientRectAsWin32(&rcClient);

	CRect rcItem(0, rcClient.top, 0, rcClient.bottom);
	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (; iter != iterEnd; iter++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += (*iter)->m_nWidth;

		this->OnDrawItem(pRenderTarget, *iter, &rcItem);
	}

	// 右侧空白 
	if (rcItem.right < rcClient.right)
	{
		rcItem.left = rcItem.right;
		rcItem.right = rcClient.right;

		if (m_pRightBlankRender)
		{
			m_pRightBlankRender->DrawState(pRenderTarget, &rcItem, 0);  // TODO: HOVER/PRESS状态??
		}
		else if (!m_bBkgndRenderContainSplitLine)
		{
            IRenderBase* pBkRender = m_pIHeaderCtrl->GetBkRender();
            if (pBkRender)
			    pBkRender->DrawState(pRenderTarget, &rcItem, 0);
		}
	}

// 	if (0 != m_nHScrollOffset)
// 	{
// 		pRenderTarget->SetViewportOrgEx(pt.x, pt.y);
// 	}
}

void HeaderCtrl::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	// 覆盖默认行为。因为这里的 bkgndrender是item的render
}

void HeaderCtrl::OnDrawItem(IRenderTarget* pRenderTarget, HeaderItemBase* pItem, CRect* prcItem)
{
	int nState = HEADERCTRL_ITEM_RENDER_STATE_NORMAL;
	if (m_pHoverItem == pItem && m_pPressItem == pItem)
	{
		nState = HEADERCTRL_ITEM_RENDER_STATE_PRESS;
	}
	else if (m_pHoverItem == pItem || m_pPressItem == pItem)
	{
		nState = HEADERCTRL_ITEM_RENDER_STATE_HOVER;
	}

    IRenderBase* pBkgndRender = m_pIHeaderCtrl->GetBkRender();
	if (pBkgndRender)
	{
		if (m_bBkgndRenderContainSplitLine)
		{
			pBkgndRender->DrawState(pRenderTarget, prcItem, nState);
		}
		else
		{
			int nSplitWidth = 0;
			if (m_pSplitLineRender)
				nSplitWidth = m_pSplitLineRender->GetDesiredSize().cx;

			CRect rcBack(prcItem);
			CRect rcSplit(prcItem);
			rcSplit.left = rcSplit.right-nSplitWidth;

			pBkgndRender->DrawState(pRenderTarget, &rcBack, nState);
			if (m_pSplitLineRender)
				m_pSplitLineRender->DrawState(pRenderTarget, &rcSplit, nState);
		}
	}

    ITextRenderBase* pTextRender = m_pIHeaderCtrl->GetTextRender();
	if (pTextRender)
	{
		CRect rcText(prcItem);
		rcText.left += m_nDividerWidth;
		rcText.right -= m_nDividerWidth;

		// 产生两像素按下的效果
		if ((nState & RENDER_STATE_PRESS) && m_bDrawOffsetPressDown)
		{
			rcText.top += 2;
			rcText.left += 2;
		}

		pTextRender->DrawState(pRenderTarget, &rcText, nState, pItem->m_strText.c_str(), pItem->m_nFormat);
	}
}

void HeaderCtrl::SetHoverItem(HeaderItemBase* pHoverItem, bool bUpdate)
{
	if (m_pHoverItem == pHoverItem)
		return;

	m_pHoverItem = pHoverItem;
	if (bUpdate)
	{
		m_pIHeaderCtrl->UpdateObject();
	}
}
void HeaderCtrl::SetPressItem(HeaderItemBase* pPressItem, bool bUpdate)
{
	if (m_pPressItem == pPressItem)
		return;

	m_pPressItem = pPressItem;
	if (bUpdate)
		m_pIHeaderCtrl->UpdateObject();
}
void HeaderCtrl::OnMouseMove(UINT nFlags, POINT point)
{
	if (m_bDragingDivider)
	{
		point.x  += m_nHScrollOffset;

		UIASSERT(NULL != m_pHoverItem);
		int nSaveWidth = m_pHoverItem->m_nWidth;
		int nNewWidth = m_nOldDragingColoumnWidth - (m_ptLastDragDivider.x - point.x);
		if (nNewWidth < 0)
			nNewWidth = 0;

		m_pHoverItem->m_nWidth = nNewWidth;
		if (m_pHoverItem->m_nWidth != nSaveWidth)
		{
			if (NULL == m_pIHeaderCtrl->GetNotifyObj())
			{
				m_pIHeaderCtrl->UpdateObject();
			}
			else
			{
				UIMSG  msg;
				msg.pMsgFrom = static_cast<IMessage*>(m_pIHeaderCtrl);
				msg.message = UI_WM_NOTIFY;
				msg.nCode = UI_HDCN_TRACKING;
				msg.wParam = GetColumnItemIndex(m_pHoverItem);
				msg.lParam = (LPARAM)&(m_pHoverItem->m_nWidth);

				if (m_pIHeaderCtrl->DoNotify(&msg))
				{
					FireTotalWidthChanged();
				}
				else  // 没处理
				{
					m_pIHeaderCtrl->UpdateObject();
				}
			}
		}
		return;
	}


	HeaderItemBase* pHoverItem = this->HitTest(point);
	if (pHoverItem == m_pHoverItem)
		return;

	if (NULL == m_pPressItem)
	{
		this->SetHoverItem(pHoverItem, true);
	}
	else
	{
		// 当前有按下的对象
		if (pHoverItem != m_pPressItem)
		{
			this->SetHoverItem(NULL, true);
		}
		else
		{
			this->SetHoverItem(pHoverItem, true);
		}
	}
}

void HeaderCtrl::OnMouseLeave()
{
	this->SetHoverItem(NULL, true);
}
void HeaderCtrl::OnLButtonDown(UINT nFlags, POINT point)
{
	HEADER_CTRL_HITTEST eHitTest = HDC_HT_ITEM_NOWHERE;
	this->HitTest(point, &eHitTest);
	point.x += m_nHScrollOffset;
	
	if ((eHitTest == HDC_HT_DIVIDER||eHitTest==HDC_HT_DIVIDER_HIDDEN_ITEM) && NULL != m_pHoverItem)
	{
		UIMSG  msg;
		msg.pMsgFrom = static_cast<IMessage*>(m_pIHeaderCtrl);
		msg.message = UI_WM_NOTIFY;
		msg.nCode = UI_HDCN_BEGIN_TRACK;
		msg.wParam = GetColumnItemIndex(m_pHoverItem);
		
		long lRet = m_pIHeaderCtrl->DoNotify(&msg);
		if (0 == lRet)  // 返回非0取消拖拽
		{
			m_ptLastDragDivider = point;
			m_nOldDragingColoumnWidth = m_pHoverItem->m_nWidth;
			m_bDragingDivider = true;
		}
	}
	else
	{
		this->SetPressItem(m_pHoverItem, true);
	}
}
void HeaderCtrl::OnLButtonUp(UINT nFlags, POINT point)
{
	if (m_bDragingDivider)
	{
		OnCancelMode();
	}

	this->SetPressItem(NULL, true);
}

void HeaderCtrl::OnCancelMode()
{
	if (m_bDragingDivider)
	{
		m_bDragingDivider = false;
		m_ptLastDragDivider.x = m_ptLastDragDivider.x = 0;
		m_nOldDragingColoumnWidth = 0;

		UIMSG  msg;
		msg.pMsgFrom = static_cast<IMessage*>(m_pIHeaderCtrl);
		msg.message = UI_WM_NOTIFY;
		msg.nCode = UI_HDCN_END_TRACK;
		msg.wParam = GetColumnItemIndex(m_pHoverItem);

		m_pIHeaderCtrl->DoNotify(&msg);
	}
}

//
// 备注：
//   1. 如果允许拖拽时，当鼠标位于位于下一列的前5px位置时，也返回鼠标在上一列
//   2. 在检测时，应尽量检测最后一个符合条件的列，如一列被拖拽到隐藏的情况下
//   
HeaderItemBase* HeaderCtrl::HitTest(POINT ptWindow, HEADER_CTRL_HITTEST* peHitTest)
{
	HeaderItemBase* pRetItem = NULL;

	if (peHitTest)
		*peHitTest = HDC_HT_ITEM_NOWHERE;

	CRect rcClient;
	m_pIHeaderCtrl->GetClientRect(&rcClient);

	// 转换为内部坐标

	POINT pt;
	m_pIHeaderCtrl->WindowPoint2ObjectPoint(&ptWindow, &pt);
	if (FALSE == rcClient.PtInRect(pt))
		return NULL;

	pt.x += m_nHScrollOffset;

	m_pIHeaderCtrl->ObjectPoint2ObjectClientPoint(&pt, &pt);

	int x1 = m_pIHeaderCtrl->GetNonClientL();
	int x2 = x1;

	list<HeaderItemBase*>::iterator iter = m_listHeaderItem.begin();
	list<HeaderItemBase*>::iterator iterEnd = m_listHeaderItem.end();
	for (; iter != iterEnd; iter++)
	{
		HeaderItemBase* pItem = *iter;
		x1 = x2;
		x2 += pItem->m_nWidth;

		if (pRetItem != NULL)
		{
			if (pItem->m_nWidth == 0)  // 该列宽度为0，是被隐藏的一列
			{
				if (peHitTest)
					*peHitTest = HDC_HT_DIVIDER_HIDDEN_ITEM;

				pRetItem = pItem;
				continue;
			}
			else
			{
				break;
			}
		}

		if (false == CanDragColumn())
		{
			if (pt.x >= x1 && pt.x <= x2)
			{
				pRetItem = pItem;

				if (peHitTest)
					*peHitTest = HDC_HT_ITEM;

				break;
			}
		}
		else
		{
			if (pt.x >= x1 && pt.x <= (x2+m_nDividerWidth))
			{
				pRetItem = pItem;
				if (peHitTest)
					*peHitTest = HDC_HT_ITEM;

				if (pt.x >= x2 - m_nDividerWidth)  // 判断鼠标是否位于拖拽处
				{
					if (peHitTest)
						*peHitTest = HDC_HT_DIVIDER;

					if (pt.x > x2)   
						continue;      // 继续检测该分隔条后面有没有隐藏项. 当鼠标位于分隔条以左，不去检测隐藏项
				}
				break;
			}
		}
		
	}

	return pRetItem;
}

BOOL HeaderCtrl::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	if (CanDragColumn())
	{
		POINT pt;
		GetCursorPos(&pt);
		::MapWindowPoints(NULL, m_pIHeaderCtrl->GetHWND(), &pt, 1);

		HEADER_CTRL_HITTEST eHitTest = HDC_HT_ITEM_NOWHERE;
		this->HitTest(pt, &eHitTest);

		if (eHitTest == HDC_HT_DIVIDER_HIDDEN_ITEM && NULL != m_pCursorDragDividerHidden)
		{
			::SetCursor(m_pCursorDragDividerHidden->GetCursor());
		}
		else if (eHitTest == HDC_HT_DIVIDER && NULL != m_pCursorDragDivider)
		{
			::SetCursor(m_pCursorDragDivider->GetCursor());
		}
		else
		{
			SetMsgHandled(FALSE);
		}
	}
	else
	{
		SetMsgHandled(FALSE);
	}
	
	return TRUE;
}


bool HeaderCtrl::CanDragColumn()
{
	return (m_pIHeaderCtrl->GetStyle()&HEADERCTRL_STYLE_CANDRAG) ? true:false;
}

void  HeaderCtrl::SetHScrollOffset(int nhScrollOffset)
{
	m_nHScrollOffset = nhScrollOffset;
}

// 通知外部header ctrl的宽度发生改变
void  HeaderCtrl::FireTotalWidthChanged()
{
	UIMSG  msg;
	msg.pMsgFrom = static_cast<IMessage*>(m_pIHeaderCtrl);
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_HDCN_TOTAL_WIDTH_CHANGED;
	msg.wParam = GetColumnTotalWidth();

	m_pIHeaderCtrl->DoNotify(&msg);
}

void HeaderCtrl::OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
{
	if (false == m_pIHeaderCtrl->IsMySelfVisible())
		return;

	int nHeight = m_pIHeaderCtrl->GetHeight();
	if (0 == nHeight)
	{
        SIZE s = {0, 0};
        this->GetDesiredSize(&s);
		nHeight = s.cy;
	}
	prcNonClientRegion->top += nHeight;
}

void  HeaderCtrl::OnGetScrollOffset(int* pxOffset, int* pyOffset)
{
    *pxOffset = m_nHScrollOffset;
}

bool HeaderCtrl::AddColumn(const TCHAR* szText, int nWidth, int nFormat)
{
	HeaderItemBase* pItem = new HeaderItemBase;
	if (szText)
		pItem->m_strText = szText;

	pItem->m_nWidth = nWidth;
	pItem->m_nFormat = nFormat;

	return AddColumn(pItem);
}
bool HeaderCtrl::InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat)
{
	HeaderItemBase* pItem = new HeaderItemBase;
	if (szText)
		pItem->m_strText = szText;

	pItem->m_nWidth = nWidth;
	pItem->m_nFormat = nFormat;

	return InsertColumn(pItem, nPos);
}

}
#endif