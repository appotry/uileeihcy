#include "stdafx.h"

MenuItemData::MenuItemData()
{
	m_nFlag = MF_STRING;
	m_nID = 0;
}

MenuBase::MenuBase()
{
	m_pPopupWrapWnd = NULL;
	m_pSeperatorRender = NULL;
	m_pPopupRender = NULL;

	m_nItemHeight = 24;
	m_nSeperatorHeight = 3;

	this->ModifyStyle(LISTCTRLBASE_CONTENT_2_SIZE);
}

MenuBase::~MenuBase()
{
	SAFE_DELETE(m_pSeperatorRender);
	SAFE_DELETE(m_pPopupRender);

	if (NULL != m_pPopupWrapWnd)
	{
		m_pPopupWrapWnd->DestroyPopupWindow();
	}
}

HRESULT MenuBase::FinalConstruct()
{
	this->ResetAttribute();
	ATTRMAP attrmap;
	this->SetAttribute(attrmap, false);

	return S_OK;
}

bool  MenuBase::AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem)
{
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

		ListItemBase* pItem = new ListItemBase(this);
		MenuItemData *pData = new MenuItemData;

		pData->SetFlag(uFlags);
		pData->SetText(lpNewItem);
		pItem->SetData((void*)pData);
		
		this->AddItem(pItem, false);
	}

	
	return true;
}

int  MenuBase::TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

	if (NULL != m_pPopupWrapWnd)
	{
		UI_LOG_WARN(_T("MenuBase::TrackPopupMenu NULL != m_pPopupWrapWnd, the prev popup window isnot destroyed"));
		return -1;
	}

	m_pPopupWrapWnd = new PopupMenuWindow(this);
	m_pPopupWrapWnd->Create(_T(""),NULL);
	::SetWindowPos(m_pPopupWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

	return 0;
}

int MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}

void MenuBase::OnDeleteItem( ListItemBase* p )
{
	if (NULL == p)
		return;

	MenuItemData *pData = (MenuItemData*)p->GetData();
	if (NULL == pData)
		return;

	SAFE_DELETE(pData);
}

SIZE MenuBase::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,0};
	if (NULL != p)
	{
		MenuItemData* pItemData = (MenuItemData*)p->GetData();
		if (NULL == pItemData)
			return s;

		if (pItemData->IsSeperator())
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

void MenuBase::OnDrawItem( HRDC hRDC, ListItemBase* p )
{
	if (NULL == p)
		return;
	MenuItemData* pData = (MenuItemData*)p->GetData();

	if (pData->IsSeperator())
	{
		this->OnDrawSeperatorItem(hRDC, p, pData);
	}
	else if (pData->IsPopup())
	{
		this->OnDrawPopupItem(hRDC, p, pData);
	}
	else
	{
		this->OnDrawStringItem(hRDC, p, pData);
	}
}

void MenuBase::OnDrawSeperatorItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData)
{
	
}
void MenuBase::OnDrawPopupItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData)
{
	
}
void MenuBase::OnDrawStringItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData)
{
	CRect rcItem;
	p->GetParentRect(&rcItem);

	if (NULL != m_pForegndRender)
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

	if (NULL != pMenuData && NULL != m_pTextRender)
	{
		rcItem.DeflateRect(2,0,2,0);
		m_pTextRender->DrawState(hRDC, &rcItem, 0, pMenuData->GetText());
	}
}


void MenuBase::OnInitPopupControlWindow(Object* pObjMsgFrom)
{

}

void MenuBase::OnUnInitPopupControlWindow(Object* pObjMsgFrom)
{
	m_pPopupWrapWnd = NULL;
}

void Menu::ResetAttribute()
{
	CRegion4 rc(3,3,3,3);
	this->SetPaddingRegion(&rc);
}

bool Menu::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
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
		m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUSTRINGITEM, this);
	}

	return true;
}