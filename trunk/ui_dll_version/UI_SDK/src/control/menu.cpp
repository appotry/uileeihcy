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

	m_nIconGutterWidth = 28;
	m_nItemHeight = 24;
	m_nSeperatorHeight = 3;
	m_nPopupTriangleWidth = 20;

	this->ModifyStyle(LISTCTRLBASE_CONTENT_2_SIZE);
}

MenuBase::~MenuBase()
{
	SAFE_DELETE(m_pSeperatorRender);

	if (NULL != m_pPopupWrapWnd)
	{
		m_pPopupWrapWnd->DestroyPopupWindow();
	}
}

HRESULT MenuBase::FinalConstruct()
{
	this->ResetAttribute();

	ATTRMAP attrmap;
	::UI_LoadStyle( this->GetObjectName(), _T(""), m_strID, attrmap ); // 如何实现换肤？

	this->SetAttribute(attrmap, false);

	return S_OK;
}


void MenuBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
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
		ListItemBase* pSave = m_pPressItem;
		this->SetPressItem(NULL, point, nFlags);
		this->ReDrawItem(pSave);
		this->ReDrawItem(m_pHoverItem);
// 
// 		UIMSG  msg;
// 		msg.message = WM_COMMAND;


		if (NULL != m_pPopupWrapWnd)
		{
			m_pPopupWrapWnd->DestroyPopupWindow();
		}
	}
}

bool MenuBase::AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem)
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

int  MenuBase::GetMenuItemCount()
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

// 	if (NULL != m_pIconBkRender)
// 	{
// 		CRect rc(rcItem);
// 		rc.left = 0;
// 		rc.right = rc.left+28;
// 		m_pIconBkRender->DrawState(hRDC, &rc, 0);
// 	}

	int  nTextState = 0;
	if (m_pHoverItem == p)
	{
		m_pForegndRender->DrawState(hRDC, &rcItem, LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER);
		nTextState = 1;
	}


	if (NULL != pMenuData && NULL != m_pTextRender)
	{
		rcItem.DeflateRect(32,0,2,0);
		m_pTextRender->DrawState(hRDC, &rcItem, nTextState, pMenuData->GetText());
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
// 	if (NULL == m_pIconBkRender)
// 	{
// 		m_pIconBkRender = RenderFactory::GetRender(RENDER_TYPE_THEME_MENUICONBK, this);
// 	}

	return true;
}