#include "stdafx.h"
#include "controldrawcaret.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{

// 根据CreateCaret的参数创建一个RenderBitmap用于绘制 
void  CreateCaretRenderBitmap(HBITMAP hbmp, UI::IRenderBitmap** pp)
{
    if ((HBITMAP)0 == hbmp || (HBITMAP)1 == hbmp || NULL == pp)
        return;

    BITMAP bm = {0};
    GetObject(hbmp, sizeof(bm), &bm);

    IRenderBitmap* pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);

    if (/*1 == bm.bmBitsPixel*/ bm.bmBitsPixel != 32)
    {
        UI::IImage image;
        image.Create(bm.bmWidth, bm.bmHeight, 32, UI::IImage::createAlphaChannel);

        HDC hDC = image.BeginDrawToMyself();
        HDC hMemDC = ::CreateCompatibleDC(NULL);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hbmp);
        ::BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCPAINT);
        ::SelectObject(hMemDC, hOldBmp);
        ::DeleteDC(hMemDC);
        image.EndDrawToMyself();

        byte* pBitsDest = image.GetBits();
        int nPitch = image.GetPitch();
        for (int y = 0; y < bm.bmHeight; y++)
        {
            DWORD* pdwBits = (DWORD*)pBitsDest;
            for (int x = 0; x < bm.bmWidth; x++, pdwBits++)
            {
                if (*pdwBits != 0)
                    *pdwBits = 0xFF000000;
            }
            pBitsDest += nPitch;
        }

        pRenderBitmap->Attach(image.Detach(), true);
    }
    else
    {
        pRenderBitmap->Attach(hbmp, false);
    }
    *pp = pRenderBitmap;
}

ControlDrawCaret::ControlDrawCaret()
{
    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_bVisible = false;
    m_bOn = false;
    m_nTimerId = 0;
    m_hBitmap = NULL;
    m_pRenderBitmap = NULL;
}
ControlDrawCaret::~ControlDrawCaret()
{
    DestroyTimer();
    SAFE_RELEASE(m_pRenderBitmap);
}

BOOL  ControlDrawCaret::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    Destroy(false);

    m_hWnd = hWndParent;
    m_pObject = pObj;
    m_sizeCaret.cx = nWidth;
    m_sizeCaret.cy = nHeight;
    m_hBitmap = hbmp;

    if (m_hBitmap > (HBITMAP)1)
    {
        BITMAP bm = {0};
        ::GetObject(m_hBitmap, sizeof(bm), &bm);
        m_sizeCaret.cx = bm.bmWidth;
        m_sizeCaret.cy = bm.bmHeight;

        CreateCaretRenderBitmap(hbmp, &m_pRenderBitmap);
    }

    return TRUE;
}
void  ControlDrawCaret::Destroy(bool bRedraw)
{
    Hide(bRedraw);

    m_hWnd = NULL;
    m_pObject = NULL;
    m_ptLast.x = m_ptLast.y = -1;
    m_sizeCaret.cx = m_sizeCaret.cy = 0;
    m_rcDraw.SetRectEmpty();
    m_hBitmap = NULL;
    SAFE_RELEASE(m_pRenderBitmap);
}
void  ControlDrawCaret::SetPos(int x, int y, bool bRedraw)
{
    m_ptLast.x = x;
    m_ptLast.y = y;
    
    if (!m_bOn)
        return;

    DestroyTimer();
    CreateTimer();
    m_bVisible = true;  // 保持显示

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
    
    m_bOn = true;
    m_bVisible = true;

    if (bRedraw && m_pObject)
        m_pObject->UpdateObject(true);
}
void  ControlDrawCaret::Hide(bool bRedraw)
{
    DestroyTimer();

    m_bOn = false;
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

    if (m_pRenderBitmap)
    {
        DRAWBITMAPPARAM param;
        param.xDest = m_ptLast.x;
        param.yDest = m_ptLast.y;
        param.wSrc = param.wDest = m_pRenderBitmap->GetWidth();
        param.hSrc = param.hDest = m_pRenderBitmap->GetHeight();
        param.xSrc = param.ySrc = 0;
        param.nFlag = DRAW_BITMAP_BITBLT;

        p->DrawBitmap(m_pRenderBitmap, &param);
    }
    else
    {
        UI::Color c(0,0,0,255);
        if ((HBITMAP)1 == m_hBitmap)
            c.ReplaceRGB(128,128,128);

        IRenderPen* pRenderPen = p->CreateSolidPen(m_sizeCaret.cx, &c);

        int x = m_ptLast.x;
        int y = m_ptLast.y;

        p->DrawLine(x, y, x, y+m_sizeCaret.cy-1, pRenderPen);  // 会多出一px来，未深究，先减1再说
        SAFE_RELEASE(pRenderPen);
    }
}

}

