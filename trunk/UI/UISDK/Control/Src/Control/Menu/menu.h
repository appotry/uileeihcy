#pragma once
#include "UISDK\Control\Inc\Interface\imenu.h"

namespace UI
{
class PopupMenuWindow;

class Menu : public MessageProxy
{
public:
    Menu();
    ~Menu();

    UI_DECLARE_OBJECT3(Menu, OBJ_CONTROL|CONTROL_MENU, _T("UICtrl/ListControl"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_NOTIFY(UI_LCN_HOVERITEMCHANGED, OnHoverItemChanged)
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_WM_TIMER(OnTimer)
        UIMESSAGE_HANDLER_EX(UI_WM_MENU_GETICONGUTTERWIDTH, OnGetIconGutterWidth);
        UIMESSAGE_HANDLER_EX(UI_WM_MENU_GETPOPUPTRIANGLEWIDTH, OnGetPopupTriangleWidth)
        UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
        UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)

        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_MOUSELEAVE(OnMouseLeave)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)

    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Menu, IListCtrlBase)

public:
    static bool  LoadMenu(IUIApplication* pUIApplication, const TCHAR*  szMenuID, IMenu** ppMenu);
    static HRESULT  UIParseLayoutElement(IUIElement*, IUIApplication*  pUIApp, IObject* pParent, IObject** ppOut);
    void  OnNewChildElement(IUIElement* pUIElement);

public:
    void  SetIMenu(IMenu* p) { m_pIMenu = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IMenu*  GetIMenu() { return m_pIMenu; }

    int    TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj, HWND hWndClickFrom = NULL, RECT* prcClickFrom = NULL);
    IListItemBase*  AppendString(const TCHAR* szText, UINT nId);
    IListItemBase*  AppendSeparator(UINT nId);
    IListItemBase*  AppendPopup(const TCHAR* szText, UINT nId, IMenu* pSubMenu);

    IMenu*  GetSubMenuByPos(int nPos);
    IMenu*  GetSubMenuById(long lId);
    IListItemBase*  GetMenuItemBySubMenu(IMenu* pSubMenu);

    Menu*  GetPrevMenu() { return m_pPrevMenu; }
    Menu*  GetNextMenu() { return m_pNextMenu; }
    Menu*  GetRootMenu();
    Menu*  GetLastMenu();
    Menu*  GetMenuByHWND(HWND hWnd);
    Menu*  GetMenuByPos(const POINT& ptScreen);
    HWND   GetPopupWindowHandle();
	IWindow*  GetPopupWindow();
    int    GetIconGutterWidth()
    {
        return m_nIconGutterWidth;
    }

protected:
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();
    LRESULT  OnGetGraphicsRenderType();
    HRESULT  FinalConstruct(IUIApplication* p);
    void  FinalRelease();
    void  OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
    LRESULT  OnHoverItemChanged(WPARAM w, LPARAM l);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnMouseMove(UINT nFlags, CPoint point);
    void  OnMouseLeave();
    void  OnSubMenuMouseMove(IMenu* pSubMenu);
    void  OnThemeChanged();

    void  ShowPopupSubMenu(IListItemBase* pItem);
    void  HidePopupSubMenu();
    int   PopupSubMenu(IListItemBase* pSubMenu);
    int   PopupAsSubMenu(UINT nFlags, Menu* pParentMenu, IListItemBase* pItem);

    void  OnInitPopupControlWindow();
    void  OnUnInitPopupControlWindow();
    LRESULT  OnGetIconGutterWidth(UINT, WPARAM, LPARAM) { return (LRESULT) m_nIconGutterWidth; }
    LRESULT  OnGetPopupTriangleWidth(UINT, WPARAM, LPARAM) { return (LRESULT) m_nPopupTriangleWidth; }

protected:
    IListItemBase*  LoadMenuItem(IUIElement* pUIElement, const TCHAR* szTagName, IMapAttribute* pMapAttrib);

    void  SetReturnCmd(UINT n);
    bool  IsMyChildMenu(Menu* pMenu);
    void  OnClick(IListItemBase* pItem);

    IListItemBase*  GetNextSelectableItem(IListItemBase* pItem);
    IListItemBase*  GetPrevSelectableItem(IListItemBase* pItem);
    IListItemBase*  _GetNextSelectableItem(IListItemBase* pItem);
    IListItemBase*  _GetPrevSelectableItem(IListItemBase* pItem);

    void  _DestroyPopupWindow();
    void  _HidePopupWindow();
    void  _DestroyAllSubMenu();
private:
    IMenu*  m_pIMenu;
    PopupMenuWindow*  m_pPopupWrapWnd;    // 用于包装菜单的窗口

    Menu*  m_pNextMenu;                 // 当前弹出的菜单列表中，自己的子菜单
    Menu*  m_pPrevMenu;                 // 当前弹出的菜单列表中，自己的父菜单
    UINT   m_nTimerIDShowPopupSubMenu;  // 计时器ID，用于弹出子窗口
    UINT   m_nTimerIDHidePopupSubMenu;  // 计时器ID，用于关闭子窗口

    UINT  m_nTrackPopupMenuFlag;        // 菜单弹出样式，例如指定TPM_RETURNCMD
    UINT  m_nRetCmd;                    // 当指定TPM_RETURNCMD类型时，返回的ID
    bool  m_bLayered;                   // 是否使用分层样式
    bool  m_bPopupAtParentRight;        // 记录自己作为子菜单时在父菜单的哪一个方向弹出来了，用于VK_LEFT/VK_RIGHT关闭

    int   m_nIconGutterWidth;           // 菜单左侧图标列 的宽度
    int   m_nPopupTriangleWidth;        // 菜单右侧弹出菜单类型的三角形
    int   m_nSubMenuGap;                // 子菜单与父菜单之间的间隙
};
}