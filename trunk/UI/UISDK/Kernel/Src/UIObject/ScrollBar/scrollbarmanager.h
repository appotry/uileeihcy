#pragma once
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "inertiascroll.h"

namespace UI
{
#define UI_MSGMAP_ID_INERTIA_MOUSE_CAPTURE     138191528
#define UI_MSGMAP_ID_INERTIA_KEYBOARD_CAPTURE  138191529

class ScrollBarManager : public Message, public IInertiaScrollCallback
{
public:
    ScrollBarManager(IScrollBarManager* p);
    ~ScrollBarManager();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETSCROLLOFFSET(OnGetScrollOffset)
        UIMSG_WM_GETSCROLLRANGE(OnGetScrollRange)
        UIMSG_WM_HSCROLL(OnHScroll)
        UIMSG_WM_VSCROLL(OnVScroll)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
    UIALT_MSG_MAP(UI_MSGMAP_ID_INERTIA_MOUSE_CAPTURE)
        UIMESSAGE_HANDLER_EX(WM_LBUTTONDOWN, OnCaptureStopInertiaScroll)
        UIMESSAGE_HANDLER_EX(WM_RBUTTONDOWN, OnCaptureStopInertiaScroll)
        UIMESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK, OnCaptureStopInertiaScroll)
        UIMESSAGE_HANDLER_EX(WM_RBUTTONDBLCLK, OnCaptureStopInertiaScroll)
    UIALT_MSG_MAP(UI_MSGMAP_ID_INERTIA_KEYBOARD_CAPTURE)
        OnCaptureStopInertiaScroll(uMsg, wParam, lParam);
        return FALSE;
    UI_END_MSG_MAP

public:
    IScrollBarManager*  GetIScrollBarMgr();
    void  SetBindObject(IObject* pBindObj);
    IObject*  GetBindObject();

    void  SetHScrollBar(IMessage* p) { m_pHScrollBar = p; }
    void  SetVScrollBar(IMessage* p) { m_pVScrollBar = p; }
    IMessage*  GetHScrollBar() { return m_pHScrollBar; }
    IMessage*  GetVScrollBar() { return m_pVScrollBar; }


    void  SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType);
    SCROLLBAR_VISIBLE_TYPE GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType);
    void  FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nCode, int nTrackPos = 0);
    
    // Set
    void  SetHScrollPos(int nPos);
    void  SetVScrollPos(int nPos);

    void  SetVScrollLine(int nLine);
    void  SetHScrollLine(int nLine);
    void  SetVScrollWheel(int nWheel);
    void  SetHScrollWheel(int nWheel);

    void  SetScrollRange(int nX, int nY);
    void  SetHScrollRange(int nX);
    void  SetVScrollRange(int nY);
    void  SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange);

    // Get
    void  GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo);

    void  GetScrollPos(int* pX, int* pY);
    int   GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollPos();
    int   GetVScrollPos();

    void  GetScrollPage(int* pX, int* pY);
    int   GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollPage();
    int   GetVScrollPage();

    void  GetScrollRange(int *pX, int* pY);
    int   GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType);
    int   GetHScrollRange();
    int   GetVScrollRange();

    // 避免在子类中(listbox)中处理了SetAttribute，导致(listctrlbaes中的)ScrollBarManager无法再从消息映射中响应
    // 因此将这两个消息作为接口实现
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);

protected:
    void  OnGetScrollOffset(int* pxOffset, int* pyOffset);
    void  OnGetScrollRange(int* pxRange, int* pyRange);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    void  OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom);

protected:
    long  NotifyHScrollInfoChanged();
    long  NotifyVScrollInfoChanged();

    bool  _SetScrollInfo1(SCROLLBAR_DIRECTION_TYPE eType, UISCROLLINFO* lpsi);
    long  _SetScrollInfo2(SCROLLBAR_DIRECTION_TYPE eType, UISCROLLINFO* lpsi);

    void  UpdateBindObjectNonClientRect();

    void  OnHMouseWheel(short zDelta, POINT pt);
    void  OnVMouseWheel(short zDelta, POINT pt);

    // 惯性滚动相关函数
    LRESULT  OnCaptureStopInertiaScroll(UINT uMsg, WPARAM, LPARAM);
    virtual  void InertiaScroll_Start();
    virtual  void InertiaScroll_Stop();
    virtual  BOOL InertiaScroll_Scroll(int nDeltaPos);

private:
    IScrollBarManager*  m_pIScrollBarManager;
    IObject*       m_pBindObject;

    // 数据
    UISCROLLINFO   m_hScrollInfo;
    UISCROLLINFO   m_vScrollInfo;

    // 渲染
    IMessage*  m_pHScrollBar;
    IMessage*  m_pVScrollBar;
    
    SCROLLBAR_VISIBLE_TYPE  m_evScrollbarVisibleType;
    SCROLLBAR_VISIBLE_TYPE  m_ehScrollbarVisibleType;

    InertiaScroll  m_inertiaScroll;
};

}