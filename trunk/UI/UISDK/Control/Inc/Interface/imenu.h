#ifndef IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB
#define IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

#define MENU_ITEM_SEPARATOR_ID  (-1)

// 获取菜单图标区域的宽度
// Return: long lWidth
#define UI_WM_MENU_GETICONGUTTERWIDTH     136111802

// 获取菜单右侧弹出箭头的宽度
// Return: long lWidth
#define UI_WM_MENU_GETPOPUPTRIANGLEWIDTH  136111803

#define UI_LISTITEM_TYPE_MENU_STRING      136111859
#define UI_LISTITEM_TYPE_MENU_SEPARATOR   136111900
#define UI_LISTITEM_TYPE_MENU_POPUP       136111901

class MenuItemBase;
class MenuItem;
class Menu;
interface IMenu;

//////////////////////////////////////////////////////////////////////////
class MenuSeparatorItemShareData;
interface IMenuSeparatorItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuSeparatorItemShareData, MenuSeparatorItemShareData);
};
class MenuSeparatorItem;
interface IMenuSeparatorItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuSeparatorItem, MenuSeparatorItem);
};

//////////////////////////////////////////////////////////////////////////
class MenuStringItemShareData;
interface IMenuStringItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuStringItemShareData, MenuStringItemShareData);
};

class MenuStringItem;
interface IMenuStringItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuStringItem, MenuStringItem);
};
//////////////////////////////////////////////////////////////////////////
class MenuPopupItemShareData;
interface IMenuPopupItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuPopupItemShareData, MenuPopupItemShareData);
};
class MenuPopupItem;
interface IMenuPopupItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuPopupItem, MenuPopupItem);
};
//////////////////////////////////////////////////////////////////////////

#if 0
interface UICTRLAPI IMenuItemBase : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuItemBase, MenuItemBase);

    void  SetMenu(Menu* p);
    void  SetSubMenu(IMenu* p);
    IMenu* GetSubMenu();

    bool  IsSeparator();   
    bool  IsPopup();       
    bool  IsChecked();     
    bool  IsRadioChecked();

    void  SetFlag(UINT nFlag);
    void  SetID(UINT nID);
    UINT  GetFlag();
    UINT  GetID();
};

interface UICTRLAPI IMenuItem : public IMenuItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenuItem, MenuItem);
};

#endif
interface UICTRLAPI IMenu : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IMenu, Menu)

    void  OnNewChildElement(IUIElement* pUIElement);
    int   TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj);
 
    IListItemBase*  AppendString(const TCHAR* szText, UINT nId);
    IListItemBase*  AppendSeparator(UINT nId);
    IListItemBase*  AppendPopup(const TCHAR* szText, UINT nId, IMenu* pSubMenu);

    IMenu*  GetSubMenuByPos(int nPos);
    IMenu*  GetSubMenuById(long lId);
// 
//     IMenu*  GetRootMenu();
//     IMenu*  GetLastMenu();
//     IMenu*  GetPrevMenu();
//     IMenu*  GetNextMenu();
//     IMenu*  GetMenuByHWND(HWND hWnd);
//     IMenu*  GetMenuByPos(const POINT& ptScreen);
//     HWND    GetPopupWindowHandle();
// 
//     IRenderBase*  GetSeparatorRender();
//     IRenderBase*  GetPopupTriangleRender();
//     IRenderBase*  GetRadioIconRender();
//     IRenderBase*  GetCheckIconRender();
//     IRenderBase*  GetRadioCheckIconBkRender();
// 
//     bool  IsItemHilight(IMenuItem* p);
      int   GetIconGutterWidth();
//     int   GetPopupTriangleWidth();
//     int   GetSeperatorHeight();
};

}

#endif // IMENU_H_31481C5E_BC0B_4872_9C54_FF8D06736CFB