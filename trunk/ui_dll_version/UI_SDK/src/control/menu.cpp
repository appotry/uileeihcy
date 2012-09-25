#include "stdafx.h"

MenuItem::MenuItem(ListCtrlBase* pCtrl) : ListItemBase(pCtrl)
{
	m_pMenu = dynamic_cast<MenuBase*>(pCtrl);
	m_nFlag = MF_STRING;
	m_nID = 0;
	m_pSubMenu = NULL;
	m_pIconRender = NULL;
}
MenuItem::~MenuItem()
{
	if (NULL != m_pSubMenu && m_pMenu->m_nStyle & MENU_STYLE_AUTO_DELETE_SUBMENU)
	{
		SAFE_DELETE(m_pSubMenu);
	}
	SAFE_DELETE(m_pIconRender);
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

void MenuItem::SetAttribute(ATTRMAP& mapAttrib)
{
	ATTRMAP::const_iterator iter = mapAttrib.find(XML_MENU_ITEM_ICON_RENDER_PREFIX XML_RENDER_TYPE);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pIconRender);
		const String& strType = iter->second;
		m_pIconRender = RenderFactory::GetRender(strType, m_pMenu);
		mapAttrib.erase(iter);
	}
	if (NULL != m_pIconRender)
	{
		m_pIconRender->SetAttribute(XML_MENU_ITEM_ICON_RENDER_PREFIX, mapAttrib);
	}
}

MenuBase::MenuBase()
{
	m_pPopupWrapWnd = NULL;
	m_pNextMenu = NULL;
	m_pPrevMenu = NULL;
	m_pSeperatorRender = NULL;
	m_pPopupTriangleRender = NULL;
	m_pCheckIconRender = NULL;
	m_pRadioIconRender = NULL;
	m_pRadioCheckIconBkRender = NULL;
	m_nTrackPopupMenuFlag = 0;
	m_nRetCmd = 0;
	m_bLayered = false;

	m_nIconGutterWidth = 28;
	m_nSeperatorHeight = 3;
	m_nPopupTriangleWidth = 20;
	m_nTextMarginLeft = 0;
	m_nTextMarginRight = 0;

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
	SAFE_DELETE(m_pCheckIconRender);
	SAFE_DELETE(m_pRadioIconRender);
	SAFE_DELETE(m_pRadioCheckIconBkRender);
}
void MenuBase::DestroyPopupWindow()
{
	m_pNextMenu = NULL;
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
	if (NULL != m_pPrevMenu)
	{
		m_pPrevMenu->m_pNextMenu = NULL;
		m_pPrevMenu = NULL;
	}
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
		
			MenuItem* pMenuItem = GetMenuItemBySubMenu(m_pNextMenu);
			this->ReDrawItem(pMenuItem);
		}
		m_nTimerIDHidePopupSubMenu = 0;
	}
	else if (nIDEvent == m_nTimerIDShowPopupSubMenu)
	{
		MenuBase* pSubMenu = ((MenuItem*)(pItem->wParam))->GetSubMenu();
		if (NULL != m_pNextMenu && pSubMenu != m_pNextMenu)  // 上一个未关闭的popup菜单
		{
			m_pNextMenu->DestroyPopupWindow();
			m_pNextMenu = NULL;
		}

		if (NULL != pSubMenu)
		{
			this->PopupSubMenu((MenuItem*)(pItem->wParam));
		}

		m_nTimerIDShowPopupSubMenu = 0;
	}
}

void MenuBase::ShowPopupSubMenu(MenuItem* pItem)
{
	if (0 != m_nTimerIDShowPopupSubMenu)
		TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
	
	if (NULL != m_pNextMenu && pItem->GetSubMenu() == m_pNextMenu) // 当前显示的就是该子菜单
		return;

	TimerItem  ti;
	ti.nRepeatCount = 1;
	ti.pNotify = this;
	ti.wParam = (WPARAM)pItem;
	m_nTimerIDShowPopupSubMenu = TimerHelper::GetInstance()->SetNewTimer(501, &ti);
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

// 用于支持分层窗口样式的菜单 
// 由于菜单刚创建时没有窗口，因此给Menu分配字体时，不知道使用哪种类型
// 因此给Menu发一个GetRenderType来获取
LRESULT MenuBase::OnGetRenderType()
{
	ATTRMAP::iterator iter = m_mapAttribute.find(XML_WINDOW_TRANSPARENT_TYPE);
	if (m_mapAttribute.end() != iter &&
		iter->second == XML_WINDOW_TRANSPARENT_TYPE_LAYERED )
	{
		return GRAPHICS_RENDER_TYPE_GDIPLUS;
	}
	return GRAPHICS_RENDER_TYPE_GDI;
}

//
//	被GetNextSelectableItem内部调用
//  当一次没有找到时，会被再调用一次重头找起
//
MenuItem* MenuBase::_GetNextSelectableItem(MenuItem* pItem)
{
	if (NULL == pItem)
	{
		pItem = (MenuItem*)m_pFirstItem;
		if (NULL == pItem)
			return NULL;
	}
	else
	{
		pItem = (MenuItem*)pItem->GetNextItem();
		if (NULL == pItem)
			return NULL;
	}

	while (NULL != pItem)	
	{
		if(pItem->IsSeparator() || pItem->IsDisable())
		{
			pItem = (MenuItem*)pItem->GetNextItem();
			continue;
		}
		else
		{
			break;
		}
	}

	return pItem;
}
MenuItem* MenuBase::GetNextSelectableItem(MenuItem* pItem)
{
	MenuItem* pItemRet = _GetNextSelectableItem(pItem);
	if (NULL == pItemRet)
	{
		if (NULL == pItem)  // 已经是从头开始查找的，但还是返回NULL
			return NULL;
		else                // 再重头找一次
			pItemRet = _GetNextSelectableItem(NULL);
	}

	return pItemRet;
}

MenuItem* MenuBase::_GetPrevSelectableItem(MenuItem* pItem)
{
	if (NULL == pItem)
	{
		pItem = (MenuItem*)m_pLastItem;
		if (NULL == pItem)
			return NULL;
	}
	else
	{
		pItem = (MenuItem*)pItem->GetPrevItem();
		if (NULL == pItem)
			return NULL;
	}

	while (NULL != pItem)	
	{
		if(pItem->IsSeparator() || pItem->IsDisable())
		{
			pItem = (MenuItem*)pItem->GetPrevItem();
			continue;
		}
		else
		{
			break;
		}
	}

	return pItem;
}
MenuItem* MenuBase::GetPrevSelectableItem(MenuItem* pItem)
{
	MenuItem* pItemRet = _GetPrevSelectableItem(pItem);
	if (NULL == pItemRet)
	{
		if (NULL == pItem)  // 已经是从头开始查找的，但还是返回NULL
			return NULL;
		else                // 再重头找一次
			pItemRet = _GetPrevSelectableItem(NULL);
	}

	return pItemRet;
}

// TODO: VK_LEFT VK_RIGHT键
void MenuBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_DOWN == nChar)
	{
		ListItemBase* pOldHoverItem = m_pHoverItem;
		SetHoverItem(GetNextSelectableItem((MenuItem*)m_pHoverItem));
		
		if (m_pHoverItem != pOldHoverItem)
			this->ReDrawItem(m_pHoverItem, pOldHoverItem);
	}
	else if (VK_UP == nChar)
	{
		ListItemBase* pOldHoverItem = m_pHoverItem;
		SetHoverItem(GetPrevSelectableItem((MenuItem*)m_pHoverItem));
		
		if (m_pHoverItem != pOldHoverItem)
			this->ReDrawItem(m_pHoverItem, pOldHoverItem);
	}
	else if (VK_RIGHT == nChar)
	{
		if (NULL != m_pHoverItem && ((MenuItem*)m_pHoverItem)->IsPopup())
		{
			if (0 != m_nTimerIDHidePopupSubMenu)
			{
				TimerHelper::GetInstance()->KillTimer(m_nTimerIDHidePopupSubMenu);
			}
			if (0 != m_nTimerIDShowPopupSubMenu)
			{
				TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
			}
			MenuBase* pSubMenu = ((MenuItem*)m_pHoverItem)->GetSubMenu();
			if (NULL != pSubMenu)
			{
				this->PopupSubMenu(((MenuItem*)m_pHoverItem));
			}
		}
	}
	else if (VK_LEFT == nChar)
	{
		if (NULL != m_pPrevMenu)
		{
			this->DestroyPopupWindow();
		}
	}

}

//
//  鼠标在子菜单上移动时，在父窗口上显示高亮路径（递归）
//
void MenuBase::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	if (NULL != m_pPrevMenu)
	{
		m_pPrevMenu->OnSubMenuMouseMove(this);
	}
}
void MenuBase::OnSubMenuMouseMove(MenuBase* pSubMenu)
{
	 if (0 != m_nTimerIDHidePopupSubMenu)  // 防止父菜单的销毁定时器将子菜单又销毁了
	 	TimerHelper::GetInstance()->KillTimer(m_nTimerIDHidePopupSubMenu);

	MenuItem* pItem = (MenuItem*)m_pFirstItem;
	while(NULL != pItem)
	{
		if (pItem->GetSubMenu() == pSubMenu)
		{
			ListItemBase* pOldHover = m_pHoverItem;
			SetHoverItem(pItem);
			this->ReDrawItem(pItem, pOldHover);
			
			if (NULL != m_pPrevMenu)
			{
				m_pPrevMenu->OnSubMenuMouseMove(this);
			}
			break;
		}
		pItem = (MenuItem*)pItem->GetNextItem();
	}
}
void MenuBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if( NULL != m_pHoverItem )
	{
		this->SetPressItem(m_pHoverItem, point, nFlags);
	}
	if (NULL != m_pPressItem && ((MenuItem*)m_pPressItem)->IsPopup())
	{
		if (0 != m_nTimerIDHidePopupSubMenu)
		{
			TimerHelper::GetInstance()->KillTimer(m_nTimerIDHidePopupSubMenu);
		}
		if (0 != m_nTimerIDShowPopupSubMenu)
		{
			TimerHelper::GetInstance()->KillTimer(m_nTimerIDShowPopupSubMenu);
		}
		MenuBase* pSubMenu = ((MenuItem*)m_pPressItem)->GetSubMenu();
		if (NULL != pSubMenu)
		{
			this->PopupSubMenu(((MenuItem*)m_pPressItem));
		}
		this->SetPressItem(NULL, point, nFlags);

		return;
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

		this->GetRootMenu()->DestroyPopupWindow();

		if(m_nTrackPopupMenuFlag & TPM_RETURNCMD)
		{
			this->GetRootMenu()->SetReturnCmd(pSave->GetID());
		}
		else
		{
			UIMSG  msg;
			msg.message = UI_WM_NOTIFY;
			msg.code = UI_MENU_CLICK;
			msg.wParam = (WPARAM)pSave->GetID();
			msg.pObjMsgFrom = this;
			DoNotify(&msg, true);
		}
	}
}
void MenuBase::OnThemeChanged()
{
	SetMsgHandled(FALSE);
	ON_RENDER_THEME_CHANGED(m_pSeperatorRender);
	ON_RENDER_THEME_CHANGED(m_pPopupTriangleRender);
	ON_RENDER_THEME_CHANGED(m_pRadioIconRender);
	ON_RENDER_THEME_CHANGED(m_pCheckIconRender);
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

	m_nTrackPopupMenuFlag = nFlag;
	m_nRetCmd = 0;

	m_pPopupWrapWnd = new PopupMenuWindow(this);
	m_pPopupWrapWnd->Create(_T(""),NULL);

	ATTRMAP map = m_mapAttribute;
	map[XML_ID] = _T("PopupMenuWindow");
	m_pPopupWrapWnd->SetAttribute(map, false);  // 使用剩下的属性给窗口赋值

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
	return m_nRetCmd;
}

int  MenuBase::PopupSubMenu(MenuItem* pItem)
{
	if (NULL == pItem)
		return -1;

	MenuBase* pSubMenu = pItem->GetSubMenu();
	if (NULL == pSubMenu)
		return -1;

	if(pSubMenu == m_pNextMenu)
	{
		UI_LOG_WARN(_T("%s m_pNextMenu is same to pSubMenu"), FUNC_NAME);
		return -1;
	}

	if (m_pNextMenu != NULL)
	{
		m_pNextMenu->DestroyPopupWindow();
		m_pNextMenu = NULL;
	}

	int nRet = pSubMenu->PopupAsSubMenu(m_nTrackPopupMenuFlag, this, pItem);
	if( -1 != nRet )
	{
		m_pNextMenu = pSubMenu;
	}
	return nRet;
}

// 自己作为子菜单弹出来
int  MenuBase::PopupAsSubMenu(UINT nFlags, MenuBase* pParentMenu, MenuItem* pItem)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

	if (NULL != m_pPopupWrapWnd)
	{
		UI_LOG_WARN(_T("%s NULL != m_pPopupWrapWnd, the prev popup window hasnot been destroyed"), _T(__FUNCTION__));
		return -1;
	}

	m_nTrackPopupMenuFlag = nFlags;
	m_pPrevMenu = pParentMenu;

	// 计算弹出位置
	HWND hParentWnd = pParentMenu->GetPopupWindowHandle();
	int x = 0, y = 0;
	CRect rcParent, rcItem, rcWindow;
	::GetWindowRect(hParentWnd, &rcParent);
	this->GetWindowRect(&rcWindow);
	pItem->GetParentRect(&rcItem);

	x = rcParent.right-2;
	y = rcParent.top + rcItem.top;

	CRect  rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
	if (x + rcWindow.Width() > rcWorkArea.right)
		x = rcParent.left - rcWindow.Width()+2;
	if (x < rcWorkArea.left)
		x = rcWorkArea.left;

	if (y < rcWorkArea.top)
		y = rcWorkArea.top;
	if (y + rcWindow.Height() > rcWorkArea.bottom)
		y = rcWorkArea.bottom - rcWindow.Height();
	
	m_pPopupWrapWnd = new PopupMenuWindow(this);
	m_pPopupWrapWnd->Create(_T(""), hParentWnd);

	ATTRMAP map = m_mapAttribute;
	map[XML_ID] = _T("SubPopupMenuWindow");
	m_pPopupWrapWnd->SetAttribute(map, false);  // 使用剩下的属性给窗口赋值

	::SetWindowPos(m_pPopupWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	this->ClearNotify();
	this->CopyNotify(pParentMenu);

	return 0;
}
int  MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}

MenuItem* MenuBase::GetMenuItemByPos(int nPos)
{
	MenuItem* pItem = (MenuItem*)__super::GetItemBase(nPos);
	return pItem;
}
MenuItem* MenuBase::GetMenuItemByID(int nID)
{
	MenuItem* pItem = (MenuItem*)m_pFirstItem;

	while(pItem != NULL)
	{
		if(pItem->GetID() == nID)
			return pItem;

		pItem = (MenuItem*)pItem->GetNextItem();
	}
	return NULL;
}
MenuItem* MenuBase::GetMenuItemBySubMenu(MenuBase* pSubMenu)
{
	MenuItem* pItem = (MenuItem*)m_pFirstItem;

	while(pItem != NULL)
	{
		if(pItem->IsPopup() && pItem->GetSubMenu() == pSubMenu)
			return pItem;

		pItem = (MenuItem*)pItem->GetNextItem();
	}
	return NULL;
}

MenuBase* MenuBase::GetSubMenuByPos(int nPos)
{
	MenuItem* pItem = this->GetMenuItemByPos(nPos);
	if (NULL == pItem)
		return NULL;
	return pItem->GetSubMenu();
}

MenuBase* MenuBase::GetSubMenuByID(int nID)
{
	MenuItem* pItem = this->GetMenuItemByID(nID);
	if (NULL == pItem)
		return NULL;
	return pItem->GetSubMenu();
}

SIZE MenuBase::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {0,0};
	if (NULL != p)
	{
		MenuItem* pItem = (MenuItem*)p;
		if (NULL == pItem)
			return s;

		if (pItem->IsSeparator())
		{
			s.cx = 0;
			s.cy = m_nSeperatorHeight;
		}
		else
		{
			HRFONT hRFont = this->GetFont();
			int nTextWidth = ::MeasureString(hRFont, pItem->GetText().c_str() ).cx;
			s.cx = nTextWidth + m_nIconGutterWidth + m_nTextMarginLeft + m_nTextMarginRight + m_nPopupTriangleWidth;
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

	// 绘制图标
	CRect rcIcon;
	pItem->GetParentRect(&rcIcon);
	rcIcon.right = rcIcon.left + m_nIconGutterWidth;

	UINT nState = MENU_ITEM_RADIOCHECKED_RENDER_STATE_NORMAL;
	if(pItem->IsDisable())
		nState = MENU_ITEM_RADIOCHECKED_RENDER_STATE_DISABLE;
	else if (IsItemHilight(pItem))
		nState = MENU_ITEM_RADIOCHECKED_RENDER_STATE_HOVER;

	RenderBase* pIconRender = pItem->GetIconRender();
	if (pItem->IsChecked())
	{
		if (NULL != m_pRadioCheckIconBkRender)
			m_pRadioCheckIconBkRender->DrawState(hRDC, &rcIcon, nState);

		if (NULL != m_pCheckIconRender && NULL == pIconRender)
			m_pCheckIconRender->DrawState(hRDC, &rcIcon, nState);
	}
	else if (pItem->IsRadioChecked())
	{
		if (NULL != m_pRadioCheckIconBkRender)
			m_pRadioCheckIconBkRender->DrawState(hRDC, &rcIcon, nState);

		if (NULL != m_pRadioIconRender && NULL != pIconRender)
			m_pRadioIconRender->DrawState(hRDC, &rcIcon, nState);
	}
	if (NULL != pIconRender)
		pIconRender->DrawState(hRDC, &rcIcon, 0);
}

bool MenuBase::IsItemHilight(MenuItem* p)
{
	if (m_pHoverItem == p 
		/*|| (m_pHoverItem == NULL && m_pNextMenu!=NULL && m_pNextMenu==p->GetSubMenu())*/ // 鼠标Hover或者弹出了子菜单 
		)
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
		rcItem.left += m_nIconGutterWidth + m_nTextMarginLeft;

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

	int  nTextState = MENU_STRING_ITEM_RENDER_STATE_NORMAL;
	if (pMenuItem->IsDisable())
	{
		nTextState = MENU_STRING_ITEM_RENDER_STATE_DISABLE;
	}
	else if (IsItemHilight(pMenuItem))
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
			nTextState = MENU_STRING_ITEM_RENDER_STATE_HOVER;
		}
	}

	if (NULL != pMenuItem && NULL != m_pTextRender)
	{
		rcItem.DeflateRect(m_nIconGutterWidth + m_nTextMarginLeft,0,m_nPopupTriangleWidth+m_nTextMarginRight,0);
		m_pTextRender->DrawState(hRDC, &rcItem, nTextState, pMenuItem->GetText());
	}
}


void MenuBase::OnInitPopupControlWindow(Object* pObjMsgFrom)
{

}

// 菜单窗口销毁了
void MenuBase::OnUnInitPopupControlWindow(Object* pObjMsgFrom)
{
	m_pPopupWrapWnd = NULL;
	m_pPrevMenu = NULL;
	m_pNextMenu = NULL;
	this->SetHoverItem(NULL);
}

void MenuBase::ResetAttribute()
{
	CRegion4 rc(1,1,1,1);
	this->SetPaddingRegion(&rc);
	SAFE_DELETE(m_pSeperatorRender);
	SAFE_DELETE(m_pPopupTriangleRender);
	SAFE_DELETE(m_pCheckIconRender);
	SAFE_DELETE(m_pRadioIconRender);
	SAFE_DELETE(m_pRadioCheckIconBkRender);

	m_nIconGutterWidth = 28;
	m_nSeperatorHeight = 3;
	m_nPopupTriangleWidth = 20;
	m_nTextMarginLeft = 0;
	m_nTextMarginRight = 0;
}

bool MenuBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	// 默认字体设置
	if (NULL == m_pTextRender)
	{
		m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_COLORLIST, this);
		if( NULL != m_pTextRender )
		{
			ColorListTextRender* p = dynamic_cast<ColorListTextRender*>(m_pTextRender);

			HRFONT hRFont = this->GetFont();
			p->SetHRFont(hRFont);  // hRFONT为空...
			p->SetCount(4);
			p->SetColor(0, RGB(0,0,0));
			p->SetColor(1, RGB(0,0,0));
			p->SetColor(2, RGB(0,0,0));
			p->SetColor(3, RGB(192,192,192));
			p->SetAttribute(_T(""),mapAttrib);  // 用于初始化字体
		}
	}

	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	ATTRMAP::iterator iter = mapAttrib.find(XML_MENU_ICONGUTTERWIDTH);
	if (mapAttrib.end() != iter)
	{
		m_nIconGutterWidth = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(XML_MENU_ICONGUTTERWIDTH);
	}

	iter = mapAttrib.find(XML_MENU_TEXTMARGINLEFT);
	if (mapAttrib.end() != iter)
	{
		m_nTextMarginLeft = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(XML_MENU_TEXTMARGINLEFT);
	}

	iter = mapAttrib.find(XML_MENU_TEXTMARGINRIGHT);
	if (mapAttrib.end() != iter)
	{
		m_nTextMarginRight = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(XML_MENU_TEXTMARGINRIGHT);
	}

	iter = mapAttrib.find(XML_MENU_POPUPTRIANGLEWIDTH);
	if (mapAttrib.end() != iter)
	{
		m_nPopupTriangleWidth = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(XML_MENU_POPUPTRIANGLEWIDTH);
	}

	iter = mapAttrib.find(XML_MENU_SEPARATORHEIGHT);
	if (mapAttrib.end() != iter)
	{
		m_nSeperatorHeight = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(XML_MENU_SEPARATORHEIGHT);
	}

	iter = mapAttrib.find(XML_MENU_RADIO_ICON_RENDER_PREFIX XML_RENDER_TYPE);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pRadioIconRender);
		const String& strRenderType = iter->second;
		m_pRadioIconRender = RenderFactory::GetRender(strRenderType, this);
		if (NULL != m_pRadioIconRender)
		{
			m_pRadioIconRender->SetAttribute(XML_MENU_RADIO_ICON_RENDER_PREFIX, m_mapAttribute);
		}
		else
		{
			UI_LOG_WARN( _T("%s Invalid %s=\"%s\""), FUNC_NAME, XML_MENU_RADIO_ICON_RENDER_PREFIX XML_RENDER_TYPE, strRenderType.c_str() );
		}
		this->m_mapAttribute.erase(XML_MENU_RADIO_ICON_RENDER_PREFIX XML_RENDER_TYPE);
	}

	iter = mapAttrib.find(XML_MENU_CHECK_ICON_RENDER_PREFIX XML_RENDER_TYPE);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pCheckIconRender);
		const String& strRenderType = iter->second;
		m_pCheckIconRender = RenderFactory::GetRender(strRenderType, this);
		if (NULL != m_pCheckIconRender)
		{
			m_pCheckIconRender->SetAttribute(XML_MENU_CHECK_ICON_RENDER_PREFIX, m_mapAttribute);
		}
		else
		{
			UI_LOG_WARN( _T("%s Invalid %s=\"%s\""), FUNC_NAME, XML_MENU_CHECK_ICON_RENDER_PREFIX XML_RENDER_TYPE, strRenderType.c_str() );
		}
		this->m_mapAttribute.erase(XML_MENU_CHECK_ICON_RENDER_PREFIX XML_RENDER_TYPE);
	}

	iter = mapAttrib.find(XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX XML_RENDER_TYPE);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pRadioCheckIconBkRender);
		const String& strRenderType = iter->second;
		m_pRadioCheckIconBkRender = RenderFactory::GetRender(strRenderType, this);
		if (NULL != m_pRadioCheckIconBkRender)
		{
			m_pRadioCheckIconBkRender->SetAttribute(XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX, m_mapAttribute);
		}
		else
		{
			UI_LOG_WARN( _T("%s Invalid %s=\"%s\""), FUNC_NAME, XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX XML_RENDER_TYPE, strRenderType.c_str() );
		}
		this->m_mapAttribute.erase(XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX XML_RENDER_TYPE);
	}

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
	if (NULL != m_pTextRender)
	{	
		m_pTextRender->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	if (NULL == m_pCheckIconRender)
	{
		m_pCheckIconRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUCHECKICON, this);
	}
	if (NULL == m_pRadioIconRender)
	{
		m_pRadioIconRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENURADIOICON, this);
	}
	if (NULL == m_pRadioCheckIconBkRender)
	{
		m_pRadioCheckIconBkRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENURADIOCHECKICONBK, this);
	}


	return true;
}

// 
//	通过xml属性加载菜单项
//
MenuItem* MenuBase::LoadMenuItem(const String tagName, ATTRMAP& mapAttrib)
{
	ATTRMAP::const_iterator iter = mapAttrib.end();

	if (tagName == XML_MENU_STRINGITEM)
	{
		String strText;
		String strID;
		bool bDisable = false;

		iter = mapAttrib.find(XML_TEXT);
		if (iter != mapAttrib.end())
		{
			strText = iter->second;
		}
		iter = mapAttrib.find(XML_ID);
		if (iter != mapAttrib.end())
		{
			strID = iter->second;
		}
		iter = mapAttrib.find(XML_MENU_DISABLE);
		if (iter != mapAttrib.end())
		{
			String strDisable = iter->second;
			bDisable = strDisable==_T("1") || strDisable== _T("true");
		}

		UINT nFlag = MF_STRING;
		if (bDisable)
			nFlag |= MF_DISABLED;

		MenuItem* pItem = this->AppendMenu(nFlag, _ttoi(strID.c_str()), strText.c_str());
		if (NULL != pItem)
		{
			pItem->SetAttribute(mapAttrib);
		}
		return pItem;
	}
	else if (tagName == XML_MENU_SEPARATORITEM)
	{
		MenuItem* pItem = this->AppendMenu(MF_SEPARATOR, XML_MENU_SEPARATOR_ID, NULL);
		if (NULL != pItem)
		{
			pItem->SetAttribute(mapAttrib);
		}
		return pItem;
	}
	else if (tagName == XML_MENU_POPUPITEM)
	{
		String strText;
		String strID;
		bool bDisable = false;

		iter = mapAttrib.find(XML_TEXT);
		if (iter != mapAttrib.end())
		{
			strText = iter->second;
		}
		iter = mapAttrib.find(XML_MENU_DISABLE);
		if (iter != mapAttrib.end())
		{
			String strDisable = iter->second;
			bDisable = strDisable==_T("1") || strDisable== _T("true");
		}

		UINT nFlag = MF_POPUP;
		if (bDisable)
			nFlag |= MF_DISABLED;

		Menu* pSubMenu = NULL;
		UICreateInstance(&pSubMenu);
		pSubMenu->ModifyStyle(MENU_STYLE_AUTO_DELETE_SUBMENU);

		MenuItem* pItem = this->AppendMenu(nFlag, (UINT_PTR)pSubMenu, strText.c_str());
		if (NULL == pItem)
		{
			UI_LOG_WARN(_T("%s AppendMenu failed."), FUNC_NAME);
			SAFE_DELETE(pSubMenu);
		}
		else
		{
			pItem->SetAttribute(mapAttrib);
		}
		return pItem;
	}
	else
	{
		UI_LOG_WARN(_T("%s unknown tagname:%s"), FUNC_NAME, tagName.c_str());
	}
	return NULL;
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
