#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Src\Control\Menu\menu.h"
#include "UISDK\Control\Src\Control\Menu\item\menustringitem.h"
#include "UISDK\Control\Src\Control\Menu\item\menuseparatoritem.h"
#include "UISDK\Control\Src\Control\Menu\item\menupopupitem.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuSeparatorItem, MenuSeparatorItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuStringItem,    MenuStringItem,    IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuPopupItem,     MenuPopupItem,     IListItemBase);

UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuStringItemShareData, MenuStringItemShareData, IListItemTypeShareData);
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuSeparatorItemShareData, MenuSeparatorItemShareData, IListItemTypeShareData);
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuPopupItemShareData, MenuPopupItemShareData, IListItemTypeShareData);

UI_IMPLEMENT_Ixxx_INTERFACE2(IMenu, Menu, IListCtrlBase)

void  IMenu::OnNewChildElement(IUIElement* pUIElement) { m_pMenuImpl->OnNewChildElement(pUIElement); }
IMenu*  IMenu::GetSubMenuByPos(int nPos) { return m_pMenuImpl->GetSubMenuByPos(nPos); }
IMenu*  IMenu::GetSubMenuById(long lId)  { return m_pMenuImpl->GetSubMenuById(lId); }

IListItemBase*  IMenu::AppendString(const TCHAR* szText, UINT nId) { return m_pMenuImpl->AppendString(szText, nId); }
IListItemBase*  IMenu::AppendSeparator(UINT nId) { return m_pMenuImpl->AppendSeparator(nId); }
IListItemBase*  IMenu::AppendPopup(const TCHAR* szText, UINT nId, IMenu* pSubMenu) { return m_pMenuImpl->AppendPopup(szText, nId, pSubMenu); }

int   IMenu::TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj, HWND hWndClickFrom, RECT* prcClickFrom) 
{ 
	return m_pMenuImpl->TrackPopupMenu(nFlag, x, y, pNotifyObj, hWndClickFrom, prcClickFrom); 
}

IWindow*  IMenu::GetPopupWindow()
{
	return m_pMenuImpl->GetPopupWindow();
}
#if 0
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenuItemBase, MenuItemBase, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IMenuItem, MenuItem, MenuItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE2(IMenu, Menu, IListCtrlBase)

bool  IMenuItemBase::IsSeparator() { return m_pMenuItemBaseImpl->IsSeparator(); }
bool  IMenuItemBase::IsPopup() { return m_pMenuItemBaseImpl->IsPopup(); }     
bool  IMenuItemBase::IsChecked() { return m_pMenuItemBaseImpl->IsChecked(); }
bool  IMenuItemBase::IsRadioChecked() { return m_pMenuItemBaseImpl->IsRadioChecked(); }

void  IMenuItemBase::SetMenu(Menu* p) { m_pMenuItemBaseImpl->SetMenu(p); }
void  IMenuItemBase::SetSubMenu(IMenu* p) {  m_pMenuItemBaseImpl->SetSubMenu(p); }
IMenu*  IMenuItemBase::GetSubMenu() { return m_pMenuItemBaseImpl->GetSubMenu(); }

void  IMenuItemBase::SetFlag(UINT nFlag) { m_pMenuItemBaseImpl->SetFlag(nFlag); }
void  IMenuItemBase::SetID(UINT nID) { m_pMenuItemBaseImpl->SetID(nID); }
UINT  IMenuItemBase::GetFlag() { return m_pMenuItemBaseImpl->GetFlag(); }
UINT  IMenuItemBase::GetID() { return m_pMenuItemBaseImpl->GetID(); }

IMenu*  IMenu::GetRootMenu()
{
    Menu* p = m_pMenuImpl->GetRootMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetLastMenu()
{
    Menu* p = m_pMenuImpl->GetLastMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetPrevMenu()
{
    Menu* p = m_pMenuImpl->GetPrevMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetNextMenu()
{
    Menu* p = m_pMenuImpl->GetNextMenu();
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetMenuByHWND(HWND hWnd)
{
    Menu* p = m_pMenuImpl->GetMenuByHWND(hWnd);
    if (p)
        return p->GetIMenu();

    return NULL;
}
IMenu*  IMenu::GetMenuByPos(const POINT& ptScreen)
{
    Menu* p = m_pMenuImpl->GetMenuByPos(ptScreen);
    if (p)
        return p->GetIMenu();

    return NULL;
}

IRenderBase*  IMenu::GetSeparatorRender() { return m_pMenuImpl->GetSeparatorRender(); }
IRenderBase*  IMenu::GetPopupTriangleRender() { return m_pMenuImpl->GetPopupTriangleRender(); }
IRenderBase*  IMenu::GetRadioIconRender() { return m_pMenuImpl->GetRadioIconRender(); }
IRenderBase*  IMenu::GetCheckIconRender() { return m_pMenuImpl->GetCheckIconRender(); }
IRenderBase*  IMenu::GetRadioCheckIconBkRender() { return m_pMenuImpl->GetRadioCheckIconBkRender(); }

bool  IMenu::IsItemHilight(IMenuItem* p) 
{
    if (NULL == p)
        return false;

    return m_pMenuImpl->IsItemHilight(p->GetImpl()); 
}
HWND  IMenu::GetPopupWindowHandle() { return m_pMenuImpl->GetPopupWindowHandle(); }
int   IMenu::GetPopupTriangleWidth() { return m_pMenuImpl->GetPopupTriangleWidth(); }
int   IMenu::GetSeperatorHeight() { return m_pMenuImpl->GetSeperatorHeight(); }
#endif

int   IMenu::GetIconGutterWidth() { return m_pMenuImpl->GetIconGutterWidth(); }
}