#include "stdafx.h"

MenuListItem::MenuListItem(ListCtrlBase* pCtrl):ListItemBase(pCtrl)
{
	m_nFlag = MF_STRING;
	m_nID = 0;
}

MenuBase::MenuBase()
{
	m_pWrapWnd = NULL;
	m_nItemHeight = 24;
	m_nSeperatorHeight = 3;
}

MenuBase::~MenuBase()
{
	SAFE_DELETE(m_pWrapWnd);
}

bool  MenuBase::AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem)
{
	MenuListItem* pItem = NULL;

	if (uFlags & MF_SEPARATOR)
	{
		UIASSERT(0);
	}
	else if (uFlags & MF_BITMAP)
	{
		UIASSERT(0);
	}
	else if (uFlags & MF_OWNERDRAW)
	{
		UIASSERT(0);
	}	
	else // MF_STRING
	{
		if (NULL == lpNewItem)
			return false;

		pItem = new MenuListItem(this);
		pItem->SetFlag(uFlags);
		pItem->SetText(lpNewItem);
	}

	this->AddItem(pItem, false);
	return true;
}

int  MenuBase::TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

//	SAFE_DELETE(m_pWrapWnd);

	m_pWrapWnd = new PopupMenuWindow(this);
	m_pWrapWnd->Create(_T(""),NULL);
	::SetWindowPos(m_pWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

	return 0;
}

int MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}

SIZE MenuBase::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,0};
	if (NULL != p)
	{
		MenuListItem* pItem = dynamic_cast<MenuListItem*>(p);
		if (NULL == pItem)
			return s;

		if (pItem->IsSeperator())
		{
			s.cx = 0;
			s.cy = m_nSeperatorHeight;
		}
		else
		{
			s.cx = 100;
			s.cy = m_nItemHeight;
		}
	}
	return s;
}