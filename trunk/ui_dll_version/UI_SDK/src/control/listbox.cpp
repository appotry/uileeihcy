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

bool ListItemBase::OnMouseMove(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnMouseEnter()
{ 
	return false;
}
bool ListItemBase::OnMouseLeave()
{ 
	return false;
}

bool ListItemBase::OnLButtonDown(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnRButtonDown(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnLButtonUp(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnRButtonUp(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnLButtonDBClick(POINT pt, UINT nFlag)
{ 
	return false;
}
bool ListItemBase::OnRButtonDBClick(POINT pt, UINT nFlag)
{ 
	return false;
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
	m_nItemHeight        = 20;
	m_pCompareProc       = NULL;
	m_nItemCount         = 0;
	m_nItemGap           = 0;

	this->m_MgrScrollbar.SetBindObject(this);
	this->m_MgrScrollbar.SetVScrollLine(m_nItemHeight);
//	this->m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
}
ListCtrlBase::~ListCtrlBase()
{
	
}
void ListCtrlBase::InitialRelease()
{
	__super::InitialRelease();
	this->RemoveAllItem();
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
	SAFE_DELETE(pItem);

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

void ListCtrlBase::SetItemHeight(int nHeight, bool bUpdate)
{
	if (m_nItemHeight == nHeight)
	{
		return;
	}

	m_nItemHeight = nHeight;
//	if (0 == (m_nStyle&LISTCTRLBASE_ITEM_VARIABLE_HEIGHT))
	{
		this->MeasureAllItem();
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
	if (NULL != pItem && bUpdate)
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

	this->MakeItemVisible(m_pFirstSelectedItem, bNeedUpdateObject);

	if (m_pFirstSelectedItem != pOldSelectoinItem)
	{
		bNeedUpdateObject = true;

		UIMSG  msg;
		msg.message = UI_WM_NOTIFY;
		msg.code = UI_LCN_SELCHANGED;
		msg.wParam = (WPARAM)pOldSelectoinItem;
		msg.lParam = (LPARAM)m_pFirstSelectedItem;
		this->DoNotify(&msg);
	}

// 	if( false == bNeedUpdateObject )
// 	{
// 		if( pOldSelectoinItem != m_pFirstSelectedItem )
// 		{
// 			this->ReDrawItem(pOldSelectoinItem);
// 			this->ReDrawItem(m_pFirstSelectedItem);
// 		}
// 	}
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
		SetHoverItem(this->HitTest(pt));
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

// 设置滚动条的位置，确保pItem可见
void ListCtrlBase::MakeItemVisible(ListItemBase* pItem, bool& bNeedUpdateObject)
{
	if (NULL == pItem)
		return;

	LISTITEM_VISIBLE_POS_TYPE ePosType;
	this->IsItemVisibleEx(pItem, ePosType);

	if (LISTITEM_UNVISIBLE_TOP == ePosType || LISTITEM_VISIBLE_COVERTOP == ePosType)
	{
		bNeedUpdateObject = true;

		CRect rc;
		pItem->GetParentRect(&rc);
		this->m_MgrScrollbar.SetVScrollPos(rc.top);
	}
	else if (LISTITEM_UNVISIBLE_BOTTOM == ePosType || LISTITEM_VISIBLE_COVERBOTTOM == ePosType)
	{
		bNeedUpdateObject = true;

		CRect rc, rcClient;
		pItem->GetParentRect(&rc);
		this->GetClientRect(&rcClient);
		
		this->m_MgrScrollbar.SetVScrollPos(rc.bottom - rcClient.Height());
	}
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
		SetHoverItem(pNewHover);

		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);
	}
	if (NULL != m_pHoverItem)
	{
		m_pHoverItem->OnMouseMove(point, nFlags);
	}
}

void ListCtrlBase::OnMouseLeave()
{
	if (m_nStyle & LISTCTRLBASE_SEL_HOVER_MODE)
		return;

	bool bNeedUpdate = false;
	if( NULL != m_pHoverItem || NULL != m_pPressItem )
	{
		bNeedUpdate = true;
	}
	ListItemBase* pSave = m_pHoverItem;
	m_pPressItem = NULL;
	SetHoverItem(NULL);

	if( bNeedUpdate )
	{
		this->ReDrawItem(pSave);
	}
}

void ListCtrlBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if( NULL != m_pHoverItem )
	{
		this->SetPressItem(m_pHoverItem, point, nFlags);

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
		this->SetPressItem(NULL, point, nFlags);
		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);
	}
}
void ListCtrlBase::OnDBClick(UINT nFlags, POINT point)
{
	if (NULL != m_pHoverItem)
	{
		UIMSG  msg;
		msg.message = UI_WM_NOTIFY;
		msg.code = UI_LCN_DBCLICK;
		msg.wParam = MAKEWPARAM(point.x, point.y);
		msg.lParam = (LPARAM)m_pHoverItem;
		msg.pObjMsgFrom = this;

		this->DoNotify(&msg);
	}
}

void ListCtrlBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
   	bool bNeedUpdateObject = false;
	if( VK_DOWN == nChar )
	{
		if (m_nStyle & LISTCTRLBASE_SEL_HOVER_MODE)  // 菜单或者弹出式列表框
		{
			if (NULL == m_pHoverItem)
			{
				if( NULL == m_pFirstSelectedItem )
					SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
				else if( NULL != m_pFirstSelectedItem->GetNextItem() )
					SetSelectedItem(m_pFirstSelectedItem->GetNextItem(), bNeedUpdateObject);
			}
			else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
			{
				if (NULL != m_pHoverItem->GetNextItem())
					SetSelectedItem(m_pHoverItem->GetNextItem(), bNeedUpdateObject);
				else
					SetSelectedItem(m_pHoverItem, bNeedUpdateObject);
			}

			if(NULL != m_pFirstSelectedItem)  // 清除hover对象，显示selection对象
				SetHoverItem(NULL);
		}
		else  // 正常列表框
		{
			if( NULL == m_pFirstSelectedItem )
				SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
			else if( NULL != m_pFirstSelectedItem->GetNextItem() )
				SetSelectedItem(m_pFirstSelectedItem->GetNextItem(), bNeedUpdateObject);
		}
	}
	else if( VK_UP == nChar )
	{
		if (m_nStyle & LISTCTRLBASE_SEL_HOVER_MODE)  // 菜单或者弹出式列表框
		{
			
			if (NULL == m_pHoverItem)
			{
				if( NULL == m_pFirstSelectedItem )
					SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
				else if( NULL != m_pFirstSelectedItem->GetPrevItem() )
					SetSelectedItem(m_pFirstSelectedItem->GetPrevItem(), bNeedUpdateObject);
			}
			else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
			{
				if (NULL != m_pHoverItem->GetPrevItem())
					SetSelectedItem(m_pHoverItem->GetPrevItem(), bNeedUpdateObject);
				else
					SetSelectedItem(m_pHoverItem, bNeedUpdateObject);
			}

			if(NULL != m_pFirstSelectedItem)  // 清除hover对象，显示selection对象
				SetHoverItem(NULL);
		}
		else  // 正常列表框
		{
			if( NULL == m_pFirstSelectedItem )
				SetSelectedItem(m_pFirstItem, bNeedUpdateObject);
			else if( NULL != m_pFirstSelectedItem->GetPrevItem() )
				SetSelectedItem(m_pFirstSelectedItem->GetPrevItem(), bNeedUpdateObject);
		}
	}

	if( bNeedUpdateObject )
	{
		this->UpdateObject();
	}
}

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
		CRect  rcParent;
		pItem->GetParentRect(&rcParent);
		if (rcParent.bottom - yOffset < rcClient.top)  // top cover unvisible item
		{
			pItem = pItem->GetNextItem();
			continue;
		}

		if( pItem->GetParentRect().top - yOffset >= rcClient.bottom )  // last visible item
			break;

		this->OnDrawItem(hRDC,pItem);  // 绘制背景
		pItem->OnDrawItem(hRDC);       // 绘制元素
		
		pItem = pItem->GetNextItem();
	}
}

void ListCtrlBase::OnSize( UINT nType, int cx, int cy )
{
	this->m_MgrScrollbar.ProcessMessage(m_pCurMsg, 0);  // 先设置page大小，再做其它处理，否则容易造成死循环
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
bool ListCtrlBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	m_MgrScrollbar.SetAttribute(mapAttrib, bReload);
	return true;
}

void ListCtrlBase::SetHoverItem(ListItemBase* pItem)
{
	if (m_pHoverItem == pItem)
		return;

	if (NULL != m_pHoverItem)
	{
		m_pHoverItem->OnMouseLeave();
	}
	if (NULL != pItem)
	{
		pItem->OnMouseEnter();
	}

	m_pHoverItem = pItem;
}
void ListCtrlBase::SetPressItem(ListItemBase* pItem, const POINT& pt, UINT nFlags)
{
	if (m_pPressItem == pItem)
		return;

	if (NULL != m_pPressItem)
	{
		m_pPressItem->OnLButtonUp(pt, nFlags);
	}
	if (NULL != pItem)
	{
		pItem->OnLButtonDown(pt, nFlags);
	}

	m_pPressItem = pItem;
}
//////////////////////////////////////////////////////////////////////////

int ListBoxCompareProc( ListItemBase* p1, ListItemBase* p2 );

ListBox::ListBox()
{
	m_nItemHeight = 24;

	this->ModifyStyle(LISTCTRLBASE_SIZE_2_CONTENT);
	__super::SetSortCompareProc( ListBoxCompareProc );

	CRegion4 r(1,1,1,1);
	SetPaddingRegion(&r);

	m_pBindObject = NULL;
	m_pPopupWrapWnd = NULL;
}
ListBox::~ListBox()
{
	if (NULL != m_pPopupWrapWnd)
	{
		m_pPopupWrapWnd->DestroyPopupWindow();
	}
}

int ListBoxCompareProc( ListItemBase* p1, ListItemBase* p2 )
{
	ListBoxItem* pItem1 = (ListBoxItem*)p1;
	ListBoxItem* pItem2 = (ListBoxItem*)p2;
	
	if (NULL == pItem1 && NULL == pItem2)
		return 0;
	else if (NULL == pItem2)
		return 1;
	else if (NULL == pItem1)
		return -1;

	return( pItem1->m_strText.compare(pItem2->m_strText) );
}
ListBoxItem* ListBox::AddString(const String& strText, bool bUpdate)
{
	ListBoxItem* pItem = new ListBoxItem(this);

	pItem->m_strText = strText;
	this->AddItem(pItem, bUpdate);

	return pItem;
}

int  ListBox::GetListBoxStyle()
{
	return m_nStyle & LISTBOX_STYLE_MASK;
}
void ListBox::SetListBoxStyle(int n)
{
	if(n > LISTBOX_STYLE_MASK)
		return;

	m_nStyle &= ~LISTBOX_STYLE_MASK;
	m_nStyle |= n;
}

void ListBox::SetBindObject(Object* pCombobox)
{
	m_pBindObject = pCombobox;
}

void ListBox::ResetAttribute()
{
	__super::ResetAttribute();
	m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
	m_MgrScrollbar.SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
}
bool ListBox::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender(RENDER_TYPE_THEME, this);
	}
	if (NULL == m_pForegndRender)
	{
		if (0 == this->GetListBoxStyle())
		{
			m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_COLORLIST, this);
			ColorListRender* p = dynamic_cast<ColorListRender*>(m_pForegndRender);
			p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED, RGB(51,153,255),true, 0,false);
		}
	}

	return true;
}

void ListBox::OnDrawItem(HRDC hRDC, ListItemBase* p)
{
	if (NULL == p)
		return;
	ListBoxItem* pData = (ListBoxItem*)p;

	CRect rcItem;
	p->GetParentRect(&rcItem);

	if (NULL != m_pForegndRender)
	{
		if (LISTBOX_STYLE_COMBOBOX == GetListBoxStyle())
		{
			if (m_pHoverItem == p)
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
			}
			else if(NULL == m_pHoverItem &&
				(m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL) )
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED);
			}
		}
		else
		{
			if (p->IsDisable())
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE);
			}
			else if( m_pFirstSelectedItem == p || p->GetPrevSelection() != NULL || p->GetNextSelection() != NULL )
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED);
			}
			else if( m_pPressItem == p )
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS);
			}
			else if( NULL == m_pPressItem && m_pHoverItem == p )
			{
				m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
			}
		}
		// 正常状态不绘制
	}

	if (NULL != pData && NULL != m_pTextRender)
	{
		rcItem.DeflateRect(2,0,2,0);
		m_pTextRender->DrawState(hRDC, &rcItem, 0, pData->m_strText);
	//	DrawString( hRDC, pData->m_strText.c_str(), &rcItem, DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER, this->GetFont() );
	}
}

SIZE ListBox::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,m_nItemHeight};
	return s;
}
void ListBox::OnDeleteItem( ListItemBase* p )
{

}

void ListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (LISTBOX_STYLE_COMBOBOX == GetListBoxStyle())
	{
		this->CloseUp();
	}
	else
	{
		this->SetMsgHandled(FALSE);
	}
}

void ListBox::DropDown()
{
	if (NULL != m_pPopupWrapWnd)
	{
		UI_LOG_WARN(_T("ListBox::DropDown NULL != m_pPopupWrapWnd, the prev dropdown window isnot destroyed"));
		return;
	}

	// TODO: 优化，在这里决定popupwindow的位置，而不是让popupwindow自己在里面根据bindobj来计算
	m_pPopupWrapWnd = new PopupListBoxWindow(this, m_pBindObject);
	m_pPopupWrapWnd->Create(_T(""), NULL);
	::SetWindowPos(m_pPopupWrapWnd->m_hWnd, NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
}
void ListBox::CloseUp()
{
	if (NULL != m_pPopupWrapWnd)
	{
		m_pPopupWrapWnd->DestroyPopupWindow();
	}
}

void ListBox::OnInitPopupControlWindow(Object* pObjMsgFrom)
{
	if (NULL != m_pBindObject)
	{
		UISendMessage(m_pBindObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);
	}
}

void ListBox::OnUnInitPopupControlWindow(Object* pObjMsgFrom)
{
	m_pPopupWrapWnd = NULL;
	if (NULL != m_pBindObject)
	{
		UISendMessage(m_pBindObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);
	}
}
