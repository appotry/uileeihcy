#include "stdafx.h"

MenuItem::MenuItem(ListCtrlBase* pCtrl) : ListItemBase(pCtrl)
{
	m_pMenu = dynamic_cast<MenuBase*>(pCtrl);
	m_nFlag = MF_STRING;
	m_nID = 0;
	m_pSubMenu = NULL;
}
MenuItem::~MenuItem()
{
//  目前是由外部负责销毁的
// 	if (NULL != m_pSubMenu)
// 	{
// 		SAFE_DELETE(m_pSubMenu);
// 	}
}

bool MenuItem::OnMouseEnter()
{
	if (this->IsPopup() && NULL != m_pSubMenu && !IsDisable())
	{
		m_pMenu->ShowPopupSubMenu(this);
	}
	else
	{
		m_pMenu->HidePopupSubMenu();
	}
	return true;
}
bool MenuItem::OnMouseLeave()
{
	return true;
}

MenuBase::MenuBase()
{
	m_pPopupWrapWnd = NULL;
	m_pNextMenu = NULL;
	m_pPrevMenu = NULL;
	m_pSeperatorRender = NULL;
	m_pPopupTriangleRender = NULL;

	m_nIconGutterWidth = 28;
	m_nSeperatorHeight = 3;
	m_nPopupTriangleWidth = 20;

	m_nTimerIDShowPopupSubMenu = 0;
	m_nTimerIDHidePopupSubMenu = 0;

	this->ModifyStyle(LISTCTRLBASE_CONTENT_2_SIZE);
	m_MgrScrollbar.SetScrollBarVisibleType(HSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
	m_MgrScrollbar.SetScrollBarVisibleType(VSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
}

MenuBase::~MenuBase()
{
	DestroyPopupWindow();
	SAFE_DELETE(m_pSeperatorRender);
	SAFE_DELETE(m_pPopupTriangleRender);
}
void MenuBase::DestroyPopupWindow()
{
	if(NULL != m_pNextMenu)
	{
		m_pNextMenu->DestroyPopupWindow();
		m_pNextMenu = NULL;
	}
	if (NULL != m_pPopupWrapWnd)
	{
		m_pPopupWrapWnd->DestroyPopupWindow(); 
		m_pPopupWrapWnd = NULL;
	}
	if (0 != m_nTimerIDHidePopupSubMenu)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDHidePopupSubMenu);
	}
	if (0 != m_nTimerIDShowPopupSubMenu)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
	}
	m_pPrevMenu = NULL;
}


void MenuBase::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	TimerItem* pItem = (TimerItem*)lParam;
	if (NULL == pItem)
		return;

	if (nIDEvent == m_nTimerIDHidePopupSubMenu)
	{
		if (NULL != m_pNextMenu)
		{
			m_pNextMenu->DestroyPopupWindow();
			m_pNextMenu = NULL;

//			this->ReDrawItem(m_pNextMenu->get)
			this->UpdateObject(); // TODO: 更新当前弹出项即可
		}
		m_nTimerIDHidePopupSubMenu = 0;
	}
	else if (nIDEvent == m_nTimerIDShowPopupSubMenu)
	{
		MenuBase* pSubMenu = ((MenuItem*)(pItem->wParam))->GetSubMenu();
		if (NULL != pSubMenu)
		{
			this->PopupSubMenu((MenuItem*)(pItem->wParam));
		}

		m_nTimerIDShowPopupSubMenu = 0;
	}
}

void MenuBase::ShowPopupSubMenu(MenuItem* pItem)
{
	if (0 != m_nTimerIDHidePopupSubMenu)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDHidePopupSubMenu);
	}
	if (0 != m_nTimerIDShowPopupSubMenu)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
	}
	if (NULL != m_pNextMenu)
	{
		return;
	}

	TimerItem  ti;
	ti.nRepeatCount = 1;
	ti.pNotify = this;
	ti.wParam = (WPARAM)pItem;
	m_nTimerIDShowPopupSubMenu = TimerHelper::GetInstance()->SetNewTimer(500, &ti);
}
void MenuBase::HidePopupSubMenu()
{
	if (0 != m_nTimerIDShowPopupSubMenu)
	{
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
	}
	if (0 == m_nTimerIDHidePopupSubMenu && NULL != m_pNextMenu)  // 有菜单弹出没有关闭
	{
		TimerItem  ti;
		ti.nRepeatCount = 1;
		ti.pNotify = this;
		m_nTimerIDHidePopupSubMenu = TimerHelper::GetInstance()->SetNewTimer(500, &ti);
	}
}


void MenuBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bNeedUpdateObject = false;
	if( VK_DOWN == nChar )
	{
		if (NULL == m_pHoverItem)
		{
			SetHoverItem(m_pFirstItem);
		}
		else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
		{
			if (NULL != m_pHoverItem->GetNextItem())
				SetHoverItem(m_pHoverItem->GetNextItem());
			else
				SetHoverItem(m_pFirstItem);
		}

		bNeedUpdateObject = true;
	}
	else if( VK_UP == nChar )
	{
		if (NULL == m_pHoverItem)
		{
			SetHoverItem(m_pLastItem);
		}
		else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
		{
			if (NULL != m_pHoverItem->GetPrevItem())
				SetHoverItem(m_pHoverItem->GetPrevItem());
			else
				SetHoverItem(m_pLastItem);
		}

		bNeedUpdateObject = true;
	}

	if( bNeedUpdateObject )
	{
		this->UpdateObject();
	}
}


void MenuBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if( NULL != m_pHoverItem )
	{
		this->SetPressItem(m_pHoverItem, point, nFlags);
	}
}
void MenuBase::OnLButtonUp(UINT nFlags, POINT point)
{
	if( NULL != m_pPressItem )
	{
		MenuItem* pSave = (MenuItem*)m_pPressItem;
		this->SetPressItem(NULL, point, nFlags);

		if(pSave->IsSeparator() || pSave->IsDisable())
			return;

		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);

// 		if (NULL != m_pPopupWrapWnd)
// 		{
// 			m_pPopupWrapWnd->DestroyPopupWindow();
// 		}
		this->GetRootMenu()->DestroyPopupWindow();

		UIMSG  msg;
		msg.message = UI_WM_NOTIFY;
		msg.code = UI_MENU_CLICK;
		msg.wParam = (WPARAM)pSave;
		msg.pObjMsgFrom = this;
		DoNotify(&msg);
	}
}

MenuItem* MenuBase::AppendMenu(UINT uFlags, UINT_PTR uItemID, LPCTSTR lpNewItem)
{
	MenuItem *pItem = NULL;
	if (uFlags & MF_SEPARATOR)
	{
		pItem = new MenuItem(this);
		pItem->SetFlag(uFlags);
		pItem->SetID(XML_MENU_SEPARATOR_ID);
		this->AddItem(pItem, false);
	}
	else if (uFlags & MF_BITMAP)
	{
		UIASSERT(0);
	}
	else if (uFlags & MF_OWNERDRAW)
	{
		UIASSERT(0);
	}	
	else if (uFlags & MF_POPUP)
	{
		if (NULL == lpNewItem || NULL == uItemID)
			return false;

		pItem = new MenuItem(this);

		pItem->SetFlag(uFlags);
		pItem->SetText(lpNewItem);
		pItem->SetID(XML_MENU_POPUP_ID);
		pItem->SetSubMenu((MenuBase*)uItemID);

		this->AddItem(pItem, false);
		
	}
	else // MF_STRING
	{
		if (NULL == lpNewItem)
			return false;

		pItem = new MenuItem(this);

		pItem->SetFlag(uFlags);
		pItem->SetText(lpNewItem);
		pItem->SetID(uItemID);

		this->AddItem(pItem, false);
	}
	
	return pItem;
}

int  MenuBase::TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

	if (NULL != m_pPopupWrapWnd)
	{
		UI_LOG_WARN(_T("%s NULL != m_pPopupWrapWnd, the prev popup window isnot destroyed"), _T(__FUNCTION__));
		return -1;
	}

	m_pPopupWrapWnd = new PopupMenuWindow(this);
	m_pPopupWrapWnd->Create(_T(""),NULL);
	CRect rcWindow;
	this->GetWindowRect(&rcWindow);

	CRect  rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
	if (x < rcWorkArea.left)
		x = rcWorkArea.left;
	if (x + rcWindow.Width() > rcWorkArea.right)
		x = rcWorkArea.right - rcWindow.Width();
	if (y < rcWorkArea.top)
		y = rcWorkArea.top;
	if (y + rcWindow.Height() > rcWorkArea.bottom)
		y = rcWorkArea.bottom - rcWindow.Height();

	::SetWindowPos(m_pPopupWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	this->AddNotify(pNotifyObj, 0);

	::SendMessage(m_pPopupWrapWnd->m_hWnd, UI_WM_ENTERPOPUPLOOP, 0, 0);
	return 0;
}

int  MenuBase::PopupSubMenu(MenuItem* pItem)
{
	if (NULL == pItem)
		return -1;

	MenuBase* pSubMenu = pItem->GetSubMenu();
	if (NULL == pSubMenu)
		return -1;

	if (m_pNextMenu != NULL)
	{
		m_pNextMenu->DestroyPopupWindow();
		m_pNextMenu = NULL;
	}

	int nRet = pSubMenu->PopupAsSubMenu(this, pItem);
	if( -1 != nRet )
	{
		m_pNextMenu = pSubMenu;
	}
	return nRet;
}

// 自己作为子菜单弹出来
int  MenuBase::PopupAsSubMenu(MenuBase* pParentMenu, MenuItem* pItem)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

	if (NULL != m_pPopupWrapWnd)
	{
		UI_LOG_WARN(_T("%s NULL != m_pPopupWrapWnd, the prev popup window hasnot been destroyed"), _T(__FUNCTION__));
		return -1;
	}

	// 计算弹出位置
	HWND hParentWnd = pParentMenu->GetPopupWindowHandle();
	int x = 0, y = 0;
	CRect rcParent, rcItem, rcWindow;
	::GetWindowRect(hParentWnd, &rcParent);
	this->GetWindowRect(&rcWindow);
	pItem->GetParentRect(&rcItem);

	x = rcParent.right;
	y = rcParent.top + rcItem.top;

	CRect  rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
	if (x + rcWindow.Width() > rcWorkArea.right)
		x = rcParent.left - rcWindow.Width();
	if (x < rcWorkArea.left)
		x = rcWorkArea.left;

	if (y < rcWorkArea.top)
		y = rcWorkArea.top;
	if (y + rcWindow.Height() > rcWorkArea.bottom)
		y = rcWorkArea.bottom - rcWindow.Height();
	//
	m_pPopupWrapWnd = new PopupMenuWindow(this);
	m_pPopupWrapWnd->Create(_T(""), hParentWnd);
	::SetWindowPos(m_pPopupWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	this->CopyNotify(pParentMenu);

	m_pPrevMenu = pParentMenu;

	return 0;
}
int  MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}

SIZE MenuBase::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,0};
	if (NULL != p)
	{
		MenuItem* pItemData = (MenuItem*)p;
		if (NULL == pItemData)
			return s;

		if (pItemData->IsSeparator())
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

	MenuItem* pItem = (MenuItem*)p;

	if (pItem->IsSeparator())
	{
		this->OnDrawSeperatorItem(hRDC,pItem);
	}
	else if (pItem->IsPopup())
	{
		this->OnDrawPopupItem(hRDC, pItem);
	}
	else
	{
		this->OnDrawStringItem(hRDC, pItem);
	}
}

bool MenuBase::IsItemHilight(MenuItem* p)
{
	if (m_pHoverItem == p || (m_pHoverItem == NULL && m_pNextMenu!=NULL && m_pNextMenu==p->GetSubMenu())) // 鼠标Hover或者弹出了子菜单 
		return true;
	else
		return false;
}
void MenuBase::OnDrawSeperatorItem(HRDC hRDC, MenuItem* pMenuItem)
{
	if (NULL != m_pSeperatorRender)
	{
		CRect rcItem;
		pMenuItem->GetParentRect(&rcItem);
		rcItem.left += m_nIconGutterWidth;

		m_pSeperatorRender->DrawState(hRDC, &rcItem, 0);
	}
}
void MenuBase::OnDrawPopupItem(HRDC hRDC, MenuItem* pMenuItem)
{
	CRect rcItem;
	pMenuItem->GetParentRect(&rcItem);
	rcItem.left = rcItem.right - m_nPopupTriangleWidth;

	this->OnDrawStringItem(hRDC, pMenuItem);

	// 绘制弹出标识
	if (NULL != m_pPopupTriangleRender)
	{
		m_pPopupTriangleRender->DrawState(hRDC, &rcItem, IsItemHilight(pMenuItem)?1:0);
	}
}
void MenuBase::OnDrawStringItem(HRDC hRDC, MenuItem* pMenuItem)
{
	CRect rcItem;
	pMenuItem->GetParentRect(&rcItem);

	int  nTextState = 0;
	if (IsItemHilight(pMenuItem))
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
			nTextState = 1;
		}
	}

	if (NULL != pMenuItem && NULL != m_pTextRender)
	{
		rcItem.DeflateRect(m_nIconGutterWidth,0,m_nPopupTriangleWidth,0);
		m_pTextRender->DrawState(hRDC, &rcItem, nTextState, pMenuItem->GetText());
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
	CRegion4 rc(1,1,1,1);
	this->SetPaddingRegion(&rc);
	SAFE_DELETE(m_pSeperatorRender);
}

bool Menu::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
// 	bool bHaveDefineTextRender = false;
// 	if( mapAttrib.count( XML_TEXTRENDER_TYPE) )
// 		bHaveDefineTextRender = true;

	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

// 	if( !bHaveDefineTextRender )
// 	{
// 		SAFE_DELETE(m_pTextRender);
// 	}
// 	if( NULL == m_pTextRender )
// 	{
// 		m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_COLORLIST, this);
// 		if( NULL != m_pTextRender )
// 		{
// 			ColorListTextRender* p = dynamic_cast<ColorListTextRender*>(m_pTextRender);
// 
// 			HRFONT hRFont = this->GetFont();
// 			p->SetHRFont(hRFont);
// 			p->SetCount(2);
// 			p->SetColor(0, RGB(0,0,0));
// 			p->SetColor(1, RGB(255,255,255));
// 		}
// 	}

	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender(RENDER_TYPE_THEME, this);
	}
	if (NULL == m_pForegndRender)
	{
		m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUSTRINGITEM, this);
	}
	if (NULL == m_pSeperatorRender)
	{
		m_pSeperatorRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUSEPERATOR, this);
	}
	if (NULL == m_pPopupTriangleRender)
	{
		m_pPopupTriangleRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUPOPUPTRIANGLE, this);
	}

	return true;
}

HWND MenuBase::GetPopupWindowHandle()
{
	if (NULL == m_pPopupWrapWnd)
	{
		return NULL;
	}
	return m_pPopupWrapWnd->m_hWnd;
}

MenuBase* MenuBase::GetRootMenu()
{
	MenuBase* pRet = this;
	while (NULL != pRet)
	{
		if (NULL == pRet->GetPrevMenu())
			break;

		pRet = pRet->GetPrevMenu();
	}
	return pRet;
}

MenuBase* MenuBase::GetLastMenu()
{
	MenuBase* pRet = this;
	while (NULL != pRet)
	{
		if (NULL == pRet->GetNextMenu())
			break;

		pRet = pRet->GetNextMenu();
	}
	return pRet;
}

MenuBase* MenuBase::GetMenuByHWND(HWND hWnd)
{
	MenuBase* pMenu = this->GetRootMenu();
	while(NULL != pMenu)
	{
		if (hWnd == pMenu->GetPopupWindowHandle())
			return pMenu;

		pMenu = pMenu->GetNextMenu();
	}
	return NULL;
}
MenuBase* MenuBase::GetMenuByPos(POINT ptScreen)
{
	HWND hWnd = WindowFromPoint(ptScreen);
	return GetMenuByHWND(hWnd);
}