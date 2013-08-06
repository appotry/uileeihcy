#include "stdafx.h"
#include "controldrawcaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

ControlDrawCaret::ControlDrawCaret()
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_bVisible = false;
    m_bShow = false;
    m_nTimerId = 0;
}
ControlDrawCaret::~ControlDrawCaret()
{
    DestroyTimer();
}

BOOL  ControlDrawCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    m_hWnd = hWndParent;
    m_pObject = pObj;
    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;

    return TRUE;
}
void  ControlDrawCaret::Destroy()
{
    Hide(true);

    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
}
void  ControlDrawCaret::SetPos(int x, int y, bool bRedraw)
{
    m_ptLast.x = x;
    m_ptLast.y = y;
    
    if (!m_bShow)
        return;

    DestroyTimer();
    CreateTimer();
    m_bVisible = true;  // ±£³ÖÏÔÊ¾

    if (bRedraw && m_pObject)
        m_pObject->UpdateObject(true);
}

void  ControlDrawCaret::OnTimer()
{
    m_bVisible = !m_bVisible;
    if (m_pObject)
        m_pObject->UpdateObject(true);
}

void  ControlDrawCaret::Show(bool bRedraw)
{
    CreateTimer();
    
    m_bShow = true;
    m_bVisible = true;

    if (bRedraw && m_pObject)
        m_pObject->UpdateObject(true);
}
void  ControlDrawCaret::Hide(bool bRedraw)
{
    DestroyTimer();

    m_bShow = false;
    m_bVisible = false;

    if (bRedraw && m_pObject)
        m_pObject->UpdateObject(true);   
}

void  TimerProc_ControlDrawCaret(UINT_PTR nTimerID, TimerItem* pTimerItem)
{
    ControlDrawCaret* pThis = (ControlDrawCaret*)pTimerItem->lParam;
    pThis->OnTimer();
}
void  ControlDrawCaret::CreateTimer()
{
    if (!m_pObject)
        return;

    if (m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);

    TimerItem item;
    item.pProc = TimerProc_ControlDrawCaret;
    item.lParam = (LPARAM)this;
    m_nTimerId = m_pObject->GetUIApplication()->SetTimer(
        GetCaretBlinkTime(), &item);
}
void  ControlDrawCaret::DestroyTimer()
{
    if (0 != m_nTimerId)
        m_pObject->GetUIApplication()->KillTimer(m_nTimerId);
}

void  ControlDrawCaret::OnControlPaint(IRenderTarget* p)
{
    if (!m_bVisible)
        return;

    UI::Color c(0,0,0,255);
    IRenderPen* pRenderPen = p->CreateSolidPen(m_sizeCaret.cx, &c);

    int x = m_ptLast.x;
    int y = m_ptLast.y;

    p->DrawLine(x, y, x, y+m_sizeCaret.cy, pRenderPen);
    SAFE_RELEASE(pRenderPen);
}

}

