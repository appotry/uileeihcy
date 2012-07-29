#include "stdafx.h"

ListItemBase::ListItemBase(ListCtrlBase* pCtrl)
{
	m_nLineIndex = 0;
	m_DesiredSize.cx = m_DesiredSize.cy = 0;
	m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = NULL;
	m_rcParent.SetRectEmpty();

	m_bDisable = false;
	m_bChecked = false;
	m_pData = NULL;
	m_pCtrl = pCtrl;
}
ListItemBase::~ListItemBase()
{
	m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = NULL;
	m_pData = NULL;
	m_pCtrl = NULL;
}
TreeListItemBase::TreeListItemBase(ListCtrlBase* pCtrl):ListItemBase(pCtrl)
{
	m_pChild = m_pParent = NULL;
}
TreeListItemBase::~TreeListItemBase()
{
	m_pChild = m_pParent = NULL;
}

//////////////////////////////////////////////////////////////////////////



ListCtrlBase::ListCtrlBase()
{
	m_pFirstItem         = NULL;
	m_pLastItem          = NULL;
	m_pFirstVisibleItem  = NULL;
	m_pLastVisibleItem   = NULL;
	m_pFirstSelectedItem = NULL;
	m_pHoverItem         = NULL;
	m_pPressItem         = NULL;
	m_nFixeddItemHeight  = 20;
	m_pCompareProc       = NULL;
	m_nItemCount         = 0;
	m_nItemGap           = 0;

	this->m_MgrScrollbar.SetBindObject(this);
	this->m_MgrScrollbar.SetVScrollLine(m_nFixeddItemHeight);
//	this->m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
}
ListCtrlBase::~ListCtrlBase()
{
}

void ListCtrlBase::RemoveItem(ListItemBase* pItem, bool bUpdate)
{
	if( NULL == pItem )
		return;

	// 从selection列表中删除
	bool bNeedUpdateObject = true;   // TODO: 该逻辑没有处理！
	if( m_pFirstSelectedItem == pItem )
	{
		SetSelectedItem(pItem->GetNextSelection(), bNeedUpdateObject);
	}
	if( NULL != pItem->GetPrevSelection() )
	{
		pItem->GetPrevSelection()->SetNextSelection(pItem->GetNextSelection());
	}
	if( NULL != pItem->GetNextSelection() )
	{
		pItem->GetNextSelection()->SetPrevSelection(pItem->GetPrevSelection());
	}

	// 从列表中删除
	if( NULL != pItem->GetPrevItem() )
	{
		pItem->GetPrevItem()->SetNextItem(pItem->GetNextItem());
	}
	if( NULL != pItem->GetNextItem() )
	{
		pItem->GetNextItem()->SetPrevItem(pItem->GetPrevItem());
	}

	if (m_pFirstItem == pItem)
	{
		m_pFirstItem = pItem->GetNextItem();
	}
	if (m_pLastItem == pItem)
	{
		m_pLastItem = pItem->GetPrevItem();
	}

	if( m_pFirstVisibleItem == pItem )
	{
		if( NULL != pItem->GetPrevItem() )
			m_pFirstVisibleItem = pItem->GetPrevItem();
		else
			m_pFirstVisibleItem = pItem->GetNextItem();
	}

	this->UpdateItemRect(pItem->GetNextItem());
	
	this->OnDeleteItem(pItem);
	delete pItem;
	pItem = NULL;

	m_nItemCount--;
	if(bNeedUpdateObject && bUpdate)
	{
		this->UpdateObject();
	}
}
void ListCtrlBase::RemoveAllItem()
{
	ListItemBase* p = m_pFirstItem;
	while ( p!=NULL )
	{
		ListItemBase* pNext = p->GetNextItem();  // Save
		this->OnDeleteItem(p);
		delete  p;
		p = pNext;
	}
	m_pFirstItem = NULL;
	m_pLastItem = NULL;
	m_pFirstVisibleItem = NULL;
	m_pLastVisibleItem = NULL;
	m_pFirstSelectedItem = NULL;
	m_pHoverItem = NULL;
	m_pPressItem = NULL;
	m_nItemCount = 0;
}
void ListCtrlBase::SetSortCompareProc( ListItemCompareProc p )
{
	m_pCompareProc = p;
}

void ListCtrlBase::SetFixedItemHeight(int nHeight, bool bUpdate)
{
	if (m_nFixeddItemHeight == nHeight)
	{
		return;
	}

	m_nFixeddItemHeight = nHeight;
	if (0 == (m_nStyle&LISTCTRLBASE_ITEM_VARIABLE_HEIGHT))
	{
		this->UpdateItemRect(m_pFirstItem);
	}
	if (bUpdate)
	{
		this->UpdateObject();
	}
}
//
//	在末尾添加一项，根据排序结果，最后调用InsertItem
//
void ListCtrlBase::AddItem(ListItemBase* pItem, bool bUpdate)
{
	ListItemBase* pInsertAfter = m_pLastItem;

	bool bAscendSort = m_nStyle&LISTCTRLBASE_SORT_ASCEND ? true:false;
	bool bDescendSort = m_nStyle&LISTCTRLBASE_SORT_DESCEND ? true:false;
	if( (bAscendSort||bDescendSort) && NULL != m_pCompareProc )
	{
		// 排序决定位置(由于采用了链接的数据结构，不能采用二分查找的方式...)
		ListItemBase* pEnumItem = m_pFirstItem;
		while(NULL != pEnumItem)
		{
			int nResult = m_pCompareProc(pEnumItem,pItem);
			if(bAscendSort)
			{
				// 查找第一个大于自己的对象
				if(nResult>0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}
			else if(bDescendSort)
			{
				// 查找第一个小于自己的对象 
				if(nResult<0)
				{
					pInsertAfter = pEnumItem->GetPrevItem();
					break;
				}
			}

			pEnumItem = pEnumItem->GetNextItem();
		}
	}

	// 插入
	this->InsertItem(pItem,pInsertAfter);
	if (bUpdate)
	{
		this->UpdateObject();
	}
}

void ListCtrlBase::RemoveItem(int nIndex, bool bUpdate)
{
	ListItemBase* pItem = this->Index2Item(nIndex);
	if (NULL == pItem)
		return ;

	this->RemoveItem(pItem, bUpdate);
}

//
//	更新从pStart开始后面的每一个item的 m_rcParent，比如新插入一个pItem
//
void ListCtrlBase::UpdateItemRect( ListItemBase* pStart )
{
	if( NULL == pStart )
		pStart = m_pFirstItem;
		
	if (NULL == pStart)
		return;

	CRect rcClient;
	this->GetClientRect(&rcClient);

	int nMaxDesiredWidth = 0;
	if (m_nStyle&LISTCTRLBASE_SIZE_2_CONTENT)
	{ }
	else
	{
		nMaxDesiredWidth = this->GetMaxDisiredWidth();
		if (0 == nMaxDesiredWidth)
		{
			nMaxDesiredWidth = rcClient.Width();
		}
	}


	ListItemBase* p = pStart;
	while( p != NULL )
	{
		int y = 0;
		if (NULL == p->GetPrevItem())   // 第一个
		{
			p->SetLineIndex(0);
		}
		else
		{
			y = p->GetPrevItem()->GetParentRect().bottom + m_nItemGap;
			p->SetLineIndex(p->GetPrevItem()->GetLineIndex()+1);
		}

		if (m_nStyle & LISTCTRLBASE_CONTENT_2_SIZE)
		{
			CRect rc(0, y, nMaxDesiredWidth, y+p->GetDesiredSize().cy);
			p->SetParentRect(&rc);
		}
		else if (m_nStyle & LISTCTRLBASE_SIZE_2_CONTENT)
		{
			CRect rc(0, y, rcClient.Width(), y+p->GetDesiredSize().cy);
			p->SetParentRect(&rc);
		}
		else
		{
			CRect rc(0, y, nMaxDesiredWidth, y+p->GetDesiredSize().cy);
			p->SetParentRect(&rc);
		}
			
		p = p->GetNextItem();
	}

	this->m_MgrScrollbar.SetScrollRange(nMaxDesiredWidth, 
		  NULL == m_pLastItem ? 0:m_pLastItem->GetParentRect().bottom);
}

void ListCtrlBase::SetSelectedItem(ListItemBase* pItem, bool& bNeedUpdateObject )
{
	ListItemBase* pOldSelectoinItem = m_pFirstSelectedItem;
	m_pFirstSelectedItem = pItem;
#if 0
	RECT rcClient;
	this->m_MgrScrollbar.GetClientRect(&rcClient);

	// 分析滚动
	if( NULL != m_pFirstSelectedItem )
	{
		LISTITEM_VISIBLE_POS_TYPE eType;
		this->IsItemVisibleEx( m_pFirstSelectedItem, eType );

		switch (eType)
		{
		case LISTITEM_VISIBLE_COVERBOTTOM:
		case LISTITEM_UNVISIBLE_BOTTOM:
			{
				int nNewScrollY = m_pFirstSelectedItem->m_rcParent.bottom-rcClient.bottom;

				ListItemBase* pItem = m_pFirstSelectedItem;
				while( pItem )
				{
					if( pItem->m_rcParent.bottom-nNewScrollY <= rcClient.top )
						break;

					m_pFirstVisibleItem = pItem;
					pItem = pItem->m_pPrev;
				}
				this->SetScrollY( nNewScrollY, bNeedUpdateObject );
			}
			break;

		case LISTITEM_VISIBLE_COVERTOP:
		case LISTITEM_UNVISIBLE_TOP:
			m_pFirstVisibleItem = m_pFirstSelectedItem;
			this->SetScrollY(m_pFirstVisibleItem->m_rcParent.top - m_rcPadding.top, bNeedUpdateObject );
			break;
		}
	}

	if( false == bNeedUpdateObject )
	{
		if( pOldSelectoinItem != m_pFirstSelectedItem )
		{
			this->ReDrawItem(pOldSelectoinItem);
			this->ReDrawItem(m_pFirstSelectedItem);
		}
	}
#endif

}

void ListCtrlBase::SetScrollY( int nY, bool& bNeedUpdateObject  )
{
	this->SetScrollPoint( -1, nY, bNeedUpdateObject );
}
void ListCtrlBase::SetScrollX( int nX, bool& bNeedUpdateObject  )
{
	this->SetScrollPoint( nX, -1, bNeedUpdateObject );
}
void ListCtrlBase::SetScrollPoint(int nX, int nY, bool& bNeedUpdateObject)
{
	bNeedUpdateObject = false;
#if 0
	if( nX < 0 )
		nX = 0;
	if( nY < 0 )
		nY = 0;

	RECT rcClient;
	this->m_MgrScrollbar.GetClientRect(&rcClient);

	if( m_pLastItem->m_rcParent.bottom > rcClient.bottom &&
		m_pLastItem->m_rcParent.bottom - nY < rcClient.bottom )  // 保证最后一个在最下面
		nY = m_pLastItem->m_rcParent.bottom - rcClient.bottom;

	int xOffsetOld = 0, yOffsetOld = 0;
	int xOffsetNow = 0, yOffsetNow = 0;
	this->m_MgrScrollbar.GetScrollOffset(&xOffsetOld, &yOffsetOld);
	if (-1 == nX) nX = xOffsetOld;
	if (-1 == nY) nY = yOffsetOld;

	this->m_MgrScrollbar.SetScrollOffset(nX, nY);
	this->m_MgrScrollbar.GetScrollOffset(&xOffsetNow, &yOffsetNow);

	// 重新计算first visible item
	int yOffset = yOffsetNow - yOffsetOld;
	if( yOffset > 0 ) // 从下往上滚动，取下一个
	{
		
		ListItemBase* p = m_pFirstVisibleItem;
		while(p)
		{
			m_pFirstVisibleItem = p;
			if(p->m_rcParent.bottom-yOffsetNow > rcClient.top )
				break;

			p = p->m_pNext;
		}
	}
	else if( yOffset < 0 )  // 从上往下滚动，取上一个
	{
		ListItemBase* p = m_pFirstVisibleItem;
		while(p)
		{
			if(p->m_rcParent.bottom-yOffsetNow <= rcClient.top )
				break;

			m_pFirstVisibleItem = p;
			p = p->m_pPrev;
		}
	}

	// 更新鼠标下的对象
	POINT pt;
	::GetCursorPos(&pt);
	::MapWindowPoints(NULL, GetHWND(), &pt, 1 );
	if (xOffsetOld != xOffsetNow || yOffsetOld != yOffsetNow)
	{
		m_pHoverItem = this->HitTest(pt);
		bNeedUpdateObject = true;
	}
	else
	{
		this->OnMouseMove( 0, pt );
	}
#endif
}
//
// 在pInsertAfter前面插入pItem，如果pInsertAfter为NULL表示插入最前面
//
void ListCtrlBase::InsertItem( ListItemBase*  pItem, ListItemBase* pInsertAfter )
{
	if( NULL == pItem )
		return;

	int nMaxDesiredWidth = 0;
	int nTotalDesiredHeight = 0;
	if (m_nStyle & LISTCTRLBASE_SIZE_2_CONTENT)
		{}
	else if (m_nStyle & LISTCTRLBASE_CONTENT_2_SIZE)
		nMaxDesiredWidth = this->GetMaxDisiredWidth(&nTotalDesiredHeight);
	else
		nMaxDesiredWidth = this->GetMaxDisiredWidth();

	if( NULL == pInsertAfter )
	{
		if( NULL == m_pFirstItem )
		{
			m_pFirstItem = m_pLastItem = pItem;
		}
		else
		{
			m_pFirstItem->SetPrevItem(pItem);
			pItem->SetNextItem(m_pFirstItem);
			m_pFirstItem = pItem;
		}
	}
	else
	{
		if( NULL == pInsertAfter->GetNextItem() )
		{
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
			m_pLastItem = pItem;
		}
		else
		{
			pInsertAfter->GetNextItem()->SetPrevItem(pItem);
			pItem->SetNextItem(pInsertAfter->GetNextItem());
			pInsertAfter->SetNextItem(pItem);
			pItem->SetPrevItem(pInsertAfter);
		}
	}

	if( 0 == m_nItemCount )
		m_pFirstVisibleItem = pItem;

	m_nItemCount++;

// 	SIZE s = this->GetItemDisiredSize();
// 	pItem->SetParentRect(0,0,s.cx, s.cy);

	this->MeasureItem(pItem);

	if (m_nStyle&LISTCTRLBASE_SIZE_2_CONTENT)
	{
		this->UpdateItemRect(pItem);
	}
	else if (m_nStyle & LISTCTRLBASE_CONTENT_2_SIZE)
	{
		//if (pItem->GetDesiredSize().cx > nMaxDesiredWidth)  // 改项为最宽项，导致所有子项需要重新更新自己的区域
		{
			nMaxDesiredWidth = this->GetMaxDisiredWidth(&nTotalDesiredHeight);

			CRegion4 rcNonClient;
			this->GetNonClientRegion(&rcNonClient);
			this->SetObjectPos(0,0, 
				nMaxDesiredWidth + rcNonClient.left + rcNonClient.right,
				nTotalDesiredHeight + rcNonClient.top + rcNonClient.bottom,
				SWP_NOMOVE|SWP_NOREDRAW );
		}
	}
	else
	{
		if (pItem->GetDesiredSize().cx > nMaxDesiredWidth)  // 改项为最宽项，导致所有子项需要重新更新自己的区域
			this->UpdateItemRect(m_pFirstItem);
		else
			this->UpdateItemRect(pItem);
	}
}

void ListCtrlBase::MeasureItem(ListItemBase* pItem)
{
	if (NULL == pItem)
		return;

	SIZE s = this->OnMeasureItem(pItem);
	pItem->SetDesiredSize(s);
}

void ListCtrlBase::MeasureAllItem()
{
	ListItemBase* p = m_pFirstItem;
	while ( p!=NULL )
	{
		this->MeasureItem(p);
		p = p->GetNextItem();
	}
}

// 获取所有item中，最大的宽度值(如果pDesiredHeight不为空，则同时返回总共需要的高度)
int ListCtrlBase::GetMaxDisiredWidth(int* pDesiredHeight)
{
	if (NULL != pDesiredHeight)
		*pDesiredHeight = 0;

	int nRet = 0;
	ListItemBase* p = m_pFirstItem;
	while ( p!=NULL )
	{
		SIZE s = p->GetDesiredSize();
		int nWidth = s.cx;
		nRet = max(nRet, nWidth);

		if (NULL != pDesiredHeight)
		{
			*pDesiredHeight += s.cy;
			if (p != m_pFirstItem)
			{
				*pDesiredHeight += m_nItemGap;
			}
		}
		p = p->GetNextItem();
	}

	return nRet;
}
ListItemBase* ListCtrlBase::HitTest(POINT ptWindow)
{
	CRect rcClient;
	this->GetClientRect(&rcClient);

	// 1. 转换为内部坐标

	POINT pt;
	this->WindowPoint2ObjectPoint(&ptWindow, &pt);
	if (FALSE == rcClient.PtInRect(pt))
		return NULL;

	this->ObjectPoint2ObjectClientPoint(&pt, &pt);

	int nxOffset=0, nyOffset=0;
	this->GetScrollOffset(&nxOffset, &nyOffset);
	pt.x += nxOffset;
	pt.y += nyOffset;

	// 2. 判断
	
	ListItemBase* p = m_pFirstVisibleItem;
	while( NULL != p )
	{
		if( PtInRect(&(p->GetParentRect()), pt) )
			return p;

		p = p->GetNextItem();
	}

	return NULL;
}

ListItemBase* ListCtrlBase::Index2Item(int nIndex)
{
	if(0 < nIndex || nIndex >= m_nItemCount)
		return NULL;

	int n = 0;
	ListItemBase* p = m_pFirstItem;
	while (NULL != p)
	{
		if (nIndex == n++)
			return p;

		p = p->GetNextItem();
	}
	return p;
}

void ListCtrlBase::ReDrawItem( ListItemBase* pItem )
{
	if( false == IsItemVisible(pItem) )
		return;

	//if( this->IsTransparent() )
	{
		this->UpdateObject();
	}
// 	else
// 	{
// 		WindowBase* pWindow = this->GetWindowObject();
// 
// 		HRGN hClipRgn = NULL;
// 		HRDC hRDC = pWindow->BeginDrawObject(this, hClipRgn);
// 
// 		HRGN hClipClient = NULL;
// 
// 		int nxOffset=0, nyOffset=0;
// 		this->m_MgrScrollbar.GetScrollPos(&nxOffset, &nyOffset);
// 
// 		OffsetViewportOrgEx(hRDC, -nxOffset, -nyOffset, NULL);
// 		this->OnDrawItem(hRDC, pItem);
// 		OffsetViewportOrgEx(hRDC, nxOffset, nyOffset, NULL);
// 
// 		CRect rcWindow;
// 		this->ItemRect2WindowRect(&(pItem->GetParentRect()), &rcWindow);
// 		pWindow->EndDrawObject( &rcWindow, hClipRgn );
// 	}
}


bool ListCtrlBase::IsItemVisible(ListItemBase* pItem)
{
	if( NULL == pItem )
		return false;

	CRect rcClient;
	this->GetClientRect(&rcClient);

	int xOffset = 0, yOffset = 0;
	this->m_MgrScrollbar.GetScrollPos(&xOffset, &yOffset);

	CRect rcParent;
	pItem->GetParentRect(&rcParent);
	if (rcParent.bottom - yOffset <= 0) 
		return false;

	if (rcParent.top - yOffset >= rcClient.Height())  
		return false;

	return true;
}
bool ListCtrlBase::IsItemVisibleEx(ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos)
{
	if( NULL == pItem )
		return false;

	CRect rcClient, rcItemParent;
	this->GetClientRect(&rcClient);
	pItem->GetParentRect(&rcItemParent);

	int xOffset = 0, yOffset = 0;
	this->m_MgrScrollbar.GetScrollPos(&xOffset, &yOffset);

	int yTop = rcItemParent.top - yOffset;
	int yBottom = rcItemParent.bottom - yOffset;
	if(  yBottom <= 0 ) 
	{
		ePos = LISTITEM_UNVISIBLE_TOP;
		return false;
	}
	else if( yTop >= rcClient.Height() )
	{
		ePos = LISTITEM_UNVISIBLE_BOTTOM;
		return false;
	}
	else if( yTop < 0 )
	{
		ePos = LISTITEM_VISIBLE_COVERTOP;
		return true;
	}
	else if( yBottom > rcClient.Height() )
	{
		ePos = LISTITEM_VISIBLE_COVERBOTTOM;
		return true;
	}

	ePos = LISTITEM_VISIBLE;
	return true;
}

void ListCtrlBase::ItemRect2WindowRect( CRect* prc, CRect* prcRet )
{
	if( NULL == prc || NULL == prcRet )
		return;

	CRect rcItem(*prc);

	int xOffset = 0, yOffset = 0;
	this->m_MgrScrollbar.GetScrollPos(&xOffset, &yOffset);
	rcItem.OffsetRect(-xOffset, -yOffset);

	CRect rcClient;
	this->GetClientRect(&rcClient);

	IntersectRect(prcRet, &rcItem, &rcClient);
// 	if( prcRet->left < rcClient.left )
// 		prcRet->left = rcClient.left;
// 	if( prcRet->right > rcClient.right )
// 		prcRet->right = rcClient.right;
// 	if( prcRet->top < rcClient.top )
// 		prcRet->top = rcClient.top;
// 	if( prcRet->bottom > rcClient.bottom )
// 		prcRet->bottom = rcClient.bottom;
}

void ListCtrlBase::OnMouseMove(UINT nFlags, POINT point)
{
	ListItemBase* pNewHover = this->HitTest(point);
	if( pNewHover != m_pHoverItem )
	{
		ListItemBase* pSave = m_pHoverItem;
		m_pHoverItem = pNewHover;

		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);
	}
}

void ListCtrlBase::OnMouseLeave()
{
	bool bNeedUpdate = false;
	if( NULL != m_pHoverItem || NULL != m_pPressItem )
	{
		bNeedUpdate = true;
	}
	ListItemBase* pSave = m_pHoverItem;
	m_pHoverItem = m_pPressItem = NULL;
	if( bNeedUpdate )
	{
		this->ReDrawItem(pSave);
	}
}

void ListCtrlBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if( NULL != m_pHoverItem )
	{
		m_pPressItem = m_pHoverItem;

		ListItemBase* pSave = m_pFirstSelectedItem;
		if( this->HitTest(point) == m_pPressItem )
		{
			bool bNeedUpdateObject = false;
			SetSelectedItem(m_pPressItem, bNeedUpdateObject);
			if( bNeedUpdateObject )
			{
				this->UpdateObject();
			}
		}
	}
}
void ListCtrlBase::OnLButtonUp(UINT nFlags, POINT point)
{
	if( NULL != m_pPressItem )
	{
		ListItemBase* pSave = m_pPressItem;
		m_pPressItem = NULL;
		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);
	}
#ifdef _DEBUG
#endif
}
void ListCtrlBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	bool bNeedUpdateObject = false;
	if( VK_DOWN == nChar )
	{
		if( NULL == m_pFirstSelectedItem )
		{
			SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
		}
		else
		{
			if( NULL != m_pFirstSelectedItem->GetNextItem() )
			{
				ListItemBase* pSave = m_pFirstSelectedItem;
				SetSelectedItem(m_pFirstSelectedItem->GetNextItem(), bNeedUpdateObject);
			}
		}
	}
	else if( VK_UP == nChar )
	{
		if( NULL == m_pFirstSelectedItem )
		{
			SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
		}
		else
		{
			if( NULL != m_pFirstSelectedItem->GetPrevItem() )
			{
				ListItemBase* pSave = m_pFirstSelectedItem;
				SetSelectedItem(m_pFirstSelectedItem->GetPrevItem(), bNeedUpdateObject);
			}
		}
	}
	if( bNeedUpdateObject )
	{
		this->UpdateObject();
	}
}
// BOOL ListCtrlBase::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
// {
// 	int nScroll = 20;
// 	if( m_bFixedItemHeight )
// 		nScroll = m_nFixeddItemHeight+m_nItemGap;
// 
// 	int xOffset = 0, yOffset = 0;
// 	this->m_MgrScrollbar.GetScrollPos(&xOffset, &yOffset);
// 
// 	if( zDelta > 0 )
// 		this->m_MgrScrollbar.SetScrollPos(xOffset, yOffset-nScroll);
// 	else
// 		this->m_MgrScrollbar.SetScrollPos(xOffset, yOffset+nScroll);
// 
// 	int xOffsetNow = 0, yOffsetNow = 0;
// 	this->m_MgrScrollbar.GetScrollPos(&xOffsetNow, &yOffsetNow);
// 
// 	if (xOffset != xOffsetNow || yOffset != yOffsetNow)
// 		this->UpdateObject();
// 
// 	UI_LOG_DEBUG(_T("now x=%d, y=%d"), xOffsetNow, yOffsetNow);
// 
// 	return 0;
// }

void ListCtrlBase::OnPaint(HRDC hRDC)
{
	ListItemBase* pItem = m_pFirstVisibleItem;

	HRGN hClipClient = NULL;

	CRect rcClient;
	this->GetClientRect(&rcClient);

	int xOffset = 0, yOffset = 0;
	this->m_MgrScrollbar.GetScrollPos(&xOffset, &yOffset);

	while(pItem != NULL)
	{
		if( pItem->GetParentRect().top - yOffset >= rcClient.bottom )  // last visible item
			break;

		this->OnDrawItem(hRDC,pItem);
		
		if( pItem == m_pLastItem )
			break;
		else
			pItem = pItem->GetNextItem();
	}
}

void ListCtrlBase::OnSize( UINT nType, int cx, int cy )
{
	SetMsgHandled(FALSE);
	this->UpdateItemRect(m_pFirstItem);
}


SIZE ListCtrlBase::GetAutoSize( HRDC hRDC ) 
{
	SIZE s = {0,0};
	return s;
}

void ListCtrlBase::ResetAttribute()
{
	__super::ResetAttribute();
	m_MgrScrollbar.ResetAttribute( );
}
bool ListCtrlBase::SetAttribute(map<String,String>& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	m_MgrScrollbar.SetAttribute(mapAttrib, bReload);
	return true;
}

//////////////////////////////////////////////////////////////////////////

int ListBoxCompareProc( ListItemBase* p1, ListItemBase* p2 );

ListBox::ListBox()
{
	m_nItemHeight = 24;

	this->ModifyStyle(OBJECT_STYLE_VSCROLL | LISTCTRLBASE_SORT_ASCEND | LISTCTRLBASE_SIZE_2_CONTENT);
	__super::SetSortCompareProc( ListBoxCompareProc );
}
ListBox::~ListBox()
{
	this->RemoveAllItem();
}

int ListBoxCompareProc( ListItemBase* p1, ListItemBase* p2 )
{
	ListItemBase* pItem1 = (ListItemBase*)p1;
	ListItemBase* pItem2 = (ListItemBase*)p2;
	
	if (NULL == pItem1 && NULL == pItem2)
		return 0;
	else if (NULL == pItem2)
		return 1;
	else if (NULL == pItem1)
		return -1;
		
	ListBoxItemData* pData1 = (ListBoxItemData*)pItem1->GetData();
	ListBoxItemData* pData2 = (ListBoxItemData*)pItem2->GetData();

	if (NULL == pData1 && NULL == pData2)
		return 0;
	else if (NULL == pData2)
		return 1;
	else if (NULL == pData1)
		return -1;

	return( pData1->m_strText.compare(pData2->m_strText) );
}
bool ListBox::AddString(const String& strText, bool bUpdate)
{
	ListItemBase* pItem = new ListItemBase(this);
	ListBoxItemData *pData = new ListBoxItemData;

	pData->m_strText = strText;
	pItem->SetData((void*)pData);
	this->AddItem(pItem, bUpdate);

	return true;
}

void ListBox::SetItemHeight(int nHeight)
{
	m_nItemHeight = nHeight;
	this->MeasureAllItem();
	this->UpdateItemRect(m_pFirstItem);
}

void ListBox::OnDrawItem(HRDC hRDC, ListItemBase* p)
{
	if (NULL == p)
		return;
	ListBoxItemData* pData = (ListBoxItemData*)p->GetData();

	CRect rcItem;
	p->GetParentRect(&rcItem);

	if (NULL != m_pForegndRender)
	{
		if (p->IsDisable())
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_DISABLE);
		}
		else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_SELECTED);
		}
		else if( m_pPressItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_PRESS);
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_HOVER);
		}
		
		// 正常状态不绘制
	}

	if (NULL != pData)
	{
		DrawString( hRDC, pData->m_strText.c_str(), &rcItem, DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER, this->GetFont() );
	}
}

SIZE ListBox::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,m_nItemHeight};
	return s;
}
void ListBox::OnDeleteItem( ListItemBase* p )
{
	if (NULL == p)
		return;

	ListBoxItemData *pData = (ListBoxItemData*)p->GetData();
	if (NULL == pData)
		return;

	SAFE_DELETE(pData);
}

void ListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
#ifdef _DEBUG
	//this->AddString(_T("test"));
	this->SetItemHeight(m_nItemHeight + 10);
	this->UpdateObject();
#endif
}
void ListBox::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	return;
}
//////////////////////////////////////////////////////////////////////////

void TTPlayerPlaylistCtrl::OnDrawItem(HRDC hRDC, ListItemBase* p)
{
	if (NULL == p)
		return;
	TTPlayerPlaylistItemData* pData = (TTPlayerPlaylistItemData*)p->GetData();

	CRect rcItem;
	p->GetParentRect(&rcItem);

	if ( 0 == p->GetLineIndex()%2 )
	{
		::FillRect(hRDC, &rcItem, RGB(0,0,0));
	}
	else
	{
		::FillRect(hRDC, &rcItem, RGB(32,32,32));
	}

	COLORREF rgbText = RGB(0,128,255);
	if (0/*NULL != m_pForegndRender*/)
	{
		if (p->IsDisable())
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_DISABLE);
		}
		else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_SELECTED);
		}
		else if( m_pPressItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_PRESS);
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTITEM_FOREGND_RENDER_STATE_HOVER);
		}
	}
	else
	{
		if (p->IsDisable())
		{
			
		}
		else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
		{
			GradientFillV(hRDC, &rcItem, RGB(47,100,190), RGB(4,10,19));
			Rectangle(hRDC, &rcItem, RGB(255,255,255), NULL, 1, true);

			rgbText = RGB(255,255,255);
		}
		else if( m_pPressItem == p )
		{
		}
		else if( NULL == m_pPressItem && m_pHoverItem == p )
		{
		}
	}

	if (NULL != pData)
	{
		CRect rcNum = rcItem;
		CRect rcTime = rcItem;
		CRect rcText = rcItem;

		rcText.left = rcNum.right = 20;
		rcText.right = rcTime.left = rcItem.right - 25;
		rcTime.right--;
		
		TCHAR szNum[16] = _T("");
		_stprintf(szNum, _T("%d."), p->GetLineIndex()+1);
		DrawString( hRDC, szNum, &rcNum, 
			DT_SINGLELINE|DT_RIGHT|DT_VCENTER, 
			this->GetFont(), rgbText );

		DrawString( hRDC, pData->m_strFileName.c_str(), &rcText, 
			DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER, 
			this->GetFont(), rgbText );

		DrawString( hRDC, pData->m_strFileTime.c_str(), &rcTime, 
			DT_SINGLELINE|DT_RIGHT|DT_VCENTER, 
			this->GetFont(), rgbText );
	}
}
void TTPlayerPlaylistCtrl::AddFileItem(const String& strFilePath, bool bUpdate)
{
	int nPos = strFilePath.rfind(_T('\\'), strFilePath.length());
	TTPlayerPlaylistItemData* pData = new TTPlayerPlaylistItemData;
	pData->m_strFilePath = strFilePath;
	pData->m_strFileName = strFilePath.substr(nPos+1, strFilePath.length()-nPos-1);
	pData->m_strFileTime = _T("3:25");

	ListItemBase* pItem = new ListItemBase(this);
	pItem->SetData((void*)pData);
	__super::AddItem( pItem, bUpdate );
}

SIZE TTPlayerPlaylistCtrl::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,0};
	return s;
}
void TTPlayerPlaylistCtrl::OnDeleteItem( ListItemBase* p )
{
	if (NULL == p)
		return;

	TTPlayerPlaylistItemData* pData = (TTPlayerPlaylistItemData*)p->GetData();
	if (NULL == pData)
		return;

	SAFE_DELETE(pData);
}