#ifndef ISCROLLBARMANAGER_H_7997B686_91D7_4568_849F_89E03BC79A1D
#define ISCROLLBARMANAGER_H_7997B686_91D7_4568_849F_89E03BC79A1D
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{

#define UISIF_RANGE           0x0001
#define UISIF_PAGE            0x0002
#define UISIF_POS             0x0004
#define UISIF_DISABLENOSCROLL 0x0008
#define UISIF_BUTTONLINE      0x0010
#define UISIF_WHEELLINE       0x0020
#define UISIF_ALL            (UISIF_RANGE | UISIF_PAGE | UISIF_POS | UISIF_KEYBOARDLINE | UISIF_WHEELLINE)

typedef struct tagUISCROLLINFO
{
    tagUISCROLLINFO(){ nMask = nRange = nPage = nPos = 0; nButtonLine = nWheelLine = 10; }
    int  nMask;
    int  nRange;
    int  nPos;
    int  nPage;
    int  nButtonLine;
    int  nWheelLine;
}UISCROLLINFO, *LPUISCROLLINFO;

// ScrollBarManager通知滚动条参数改变的消息
//  wParam: UISCROLLINFO
//  lParam: 
//  lRet: 0(), 
//        1(需要重新计算客户区域，如自己的显隐发生改变)
//
#define UI_WM_SCROLLBAR_INFO_CHANGED  136081044

// ScrollBarManager通知滚动条控件大小发生变化，需要更新滚动条的位置
#define UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED  136091826

// 为滚动条设置管理器指针(由xml中创建滚动条时)。
// WPARAM:  IScrollBarMgr*
#define UI_WM_SCROLLBAR_SETISCROLLBARMGR   136130933

class ScrollBarManager;
interface UISDKAPI IScrollBarManager : public IMessage
{
    IScrollBarManager();
    ~IScrollBarManager();
    ScrollBarManager*  GetImpl();

    void  SetBindObject(IObject* pBindObj);
    IObject*  GetBindObject();

    void  SetHScrollBar(IMessage* p);
    void  SetVScrollBar(IMessage* p);
    IMessage*  GetHScrollBar();
    IMessage*  GetVScrollBar();

    void  SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType);
    SCROLLBAR_VISIBLE_TYPE GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType);

    virtual BOOL  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook); 
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
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

private:
    ScrollBarManager*  m_pImpl;
};

}

#endif // ISCROLLBARMANAGER_H_7997B686_91D7_4568_849F_89E03BC79A1D