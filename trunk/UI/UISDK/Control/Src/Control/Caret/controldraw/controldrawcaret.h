#pragma once
#include "UISDK\Control\Src\Control\Caret\icaret.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{

// 由控件自己调用绘制，每次timer时重新刷新整个控件。
// 这种类型的光标一般只用于需要滚动的Edit/RichEdit上面，
// 解决光标绘制裁剪的问题。因此每次闪烁都有一定的开销

class ControlDrawCaret : public ICaret
{
public:
    ControlDrawCaret();
    ~ControlDrawCaret();

    virtual CARET_TYPE  GetType() { return CARET_TYPE_CONTROLDRAW; }
    virtual BOOL  Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight);
    virtual void  Destroy();
    virtual void  SetPos(int x, int y, bool bRedraw);
    virtual void  Show(bool bRedraw);
    virtual void  Hide(bool bRedraw);
    virtual void  OnControlPaint(IRenderTarget* p);

    void  OnTimer();
protected:
    void  CreateTimer();
    void  DestroyTimer();

private:
    HWND   m_hWnd;
    IObject*  m_pObject;
    POINT  m_ptLast;
    SIZE   m_sizeCaret;

    CRect  m_rcDraw;
    UINT_PTR  m_nTimerId;
    bool   m_bShow;  // 当前是否调用了ShowCaret
    bool   m_bVisible; // 记录光标的闪烁状态
};

}