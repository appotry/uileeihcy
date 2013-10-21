#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Inc\Interface\iuiinterface.h"

namespace UI
{

class PopupControlWindow : public ICustomWindow, public IPreTranslateMessage
{
public:
    PopupControlWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupControlWindow)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_MOUSEACTIVATE(OnMouseActivate)
        UIMSG_WM_ACTIVATEAPP(OnActivateApp)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_SKINCHANGING(OnSkinChanging)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
   UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnSkinChanging(BOOL* pbChange);
    BOOL  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  OnDestroy();
    int   OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
    void  OnActivateApp(BOOL bActive, DWORD dwThreadID);
    LRESULT  OnGetGraphicsRenderType();

public:
    void  Create(IObject*  pBindObj, const TCHAR* szId, HWND hParentWnd = NULL);
    void  Show(POINT pt, BOOL bDoModal);
    void  Hide();
    void  Destroy();

protected:
    void   _DestroyPopupWindow();

protected:
    IObject*  m_pObject;   // 弹出窗口中装载的对象指针，如ListBox* Menu*
    bool      m_bExitLoop; // 在收到WM_EXITPOPUPLOOP之后，该变量为true
    bool      m_bMouseIn;  // 鼠标在窗口上，用于触发WM_MOUSELEAVE
};

class PopupListBoxWindow : public PopupControlWindow
{
public:
    PopupListBoxWindow();
   
    UI_BEGIN_MSG_MAP_Ixxx(PopupListBoxWindow)
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UIALT_MSG_MAP(1)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)
 
protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

class Menu;
class PopupMenuWindow : public PopupControlWindow
{
public:
    PopupMenuWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupMenuWindow)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);
    void  OnInitialize();

protected:
    Menu*    m_pMenu;
    POINT    m_ptLastMousePos; // 用于区分是子菜单创建、移动、隐藏导致系统发出来的WM_MOUSEMOVE消息。
    Menu*    m_pLastHoverMenu; // TODO: 突然出现的问题，鼠标移出菜单手收不到WM_MOUSELEAVE消息。
                               // 目前推测是因为当收到一个鼠标移出的WM_MOUSEMOVE后直接RETURN TRUE了，导致
                               // TrackMouseEvent没法检测出MOUSELEAVE，因此没有给窗口发出消息。
                               // 为了解决该问题，在这里记录一下上一次鼠标下的菜单，当鼠标移出后，手动发送
                               // WM_MOUSELEAVE
};
}