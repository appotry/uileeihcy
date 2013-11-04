#include "stdafx.h"
#include "bufferrenderlayer.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"

BufferRenderLayer::BufferRenderLayer()
{
    m_pIBufferRenderLayer = NULL;

    m_hOldBitmap = NULL;
    m_hMemDC = NULL;
}

BufferRenderLayer::~BufferRenderLayer()
{
    if (m_hMemDC)
    {
        ::SelectObject(m_hMemDC, m_hOldBitmap);
        m_hOldBitmap = NULL;
        DeleteDC(m_hMemDC);
    }
}

void  BufferRenderLayer::OnSize(UINT nType, int nWidth, int nHeight)
{
    SetMsgHandled(FALSE);
    if (nType == SIZE_MINIMIZED)
        return;
     
    if (!m_buffer.IsNull() && m_buffer.GetWidth() == nWidth && m_buffer.GetHeight()==nHeight)
        return;

    m_buffer.Destroy();
    m_buffer.Create(nWidth, nHeight, 32, Image::createAlphaChannel);

    if (NULL == m_hMemDC)
    {
        m_hMemDC = ::CreateCompatibleDC(NULL);
        ::SetBkMode(m_hMemDC, TRANSPARENT);
        ::SetStretchBltMode(m_hMemDC, HALFTONE);

        if (m_pRenderTarget)
            m_pRenderTarget->BindHDC(m_hMemDC);
    }

    if (NULL == m_pRenderTarget)
    {
        m_pRenderTarget = UICreateRenderTarget(m_pWindow->GetHWND());
        m_pRenderTarget->BindHDC(m_hMemDC);
    }

    m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, (HBITMAP)m_buffer);
}


bool  BufferRenderLayer::SaveLayer(const TCHAR*  szPath)
{
    if (NULL == szPath)
        return false;

    if (m_buffer.IsNull())
        return false;

    return SUCCEEDED(m_buffer.Save(szPath, Gdiplus::ImageFormatPNG))? true:false;
}

#if 0  // 由于不支持裁剪，废弃
// http://gccfeli.cn/2009/04/mmx-alpha-blend.html  MMX版本的Alpha Blend算法实现
namespace UI
{
    void AlphaBitBlt(HDC hDest, HDC hSrc, int xDest, int yDest, int xSrc, int ySrc, int width, int height)
    {
        HBITMAP hBitmapDest = (HBITMAP)::GetCurrentObject(hDest, OBJ_BITMAP);
        HBITMAP hBitmapSrc = (HBITMAP)::GetCurrentObject(hSrc, OBJ_BITMAP);

        if (NULL == hBitmapDest || NULL == hBitmapSrc)
            return;

        BITMAP bmSrc, bmDst;
        ::GetObject(hBitmapDest, sizeof(BITMAP), &bmDst);
        ::GetObject(hBitmapSrc,  sizeof(BITMAP), &bmSrc);

        int nHeight = min(height, bmDst.bmHeight-yDest);
        int nWidth  = min(width, bmDst.bmWidth-xDest);

        int ny2 = yDest + nHeight;
        int nx2 = xDest + nWidth;

        BYTE* pBitsSrc = (BYTE*)bmSrc.bmBits;
        BYTE* pBitsDst = (BYTE*)bmDst.bmBits;
        pBitsDst += (bmDst.bmHeight-1-yDest)*bmDst.bmWidthBytes;
        pBitsSrc += (bmSrc.bmHeight-1-ySrc)*bmSrc.bmWidthBytes;

        __asm 
        {
            mov   esi, pBitsSrc
                mov   edi, pBitsDst

                pxor  mm0, mm0;     // 0000000000000000

            mov   eax, 0x00FF00FF
                movd  mm7, eax
                punpckldq  mm7, mm7  // 00FF00FF00FF00FF
        }

        int yd = yDest, ys = ySrc;
        for (; yd < ny2; yd++, ys++)
        {
            __asm
            {
                mov   ebx, xSrc;
                mov   edx, xDest;

                mov   ecx, nx2;       //for (; xd < nx2; xd++, xs++)
                sub   ecx, edx;
PIXEL_LOOP:
                movd  mm1, dword ptr [esi + ebx*4]  // 调用shl ebx, 2;反而更慢
                movd  mm2, dword ptr [edi + edx*4]

                punpcklbw mm1, mm0     // src rgba
                    punpcklbw mm2, mm0     // dst rgba

                    // 将src alpha 拼成4组
                    //	movq  mm3, mm1         // 0A0B0G0R
                    //	punpckhwd  mm3, mm3    // 0A0A0000
                    //	punpckhdq  mm3, mm3    // 0A0A0A0A
                    pshufw mm3, mm1, 0ffh  // 使用SSE指令  0A0A0A0A

                    movq   mm4, mm7
                    psubsw mm4, mm3        // 255 - alpha

                    pmullw mm2, mm4        // (255-alpha)*dst
                    psrlw  mm2, 8          // ((255-alpha)*dst)/256)  注:在MMX指令集中没有除法指令
                    paddsw mm1, mm2        // src + ((255-alpha)*dst)/256)

                    packuswb mm1, mm1      // 0A0B0G0R -> 0000ABGR
                    movd  [edi+edx*4], mm1	

                    inc   ebx
                    inc   edx

                    //	loop  PIXEL_LOOP     // loop的效率非常低，应该使用dec ecx; jnz .loop
                    dec   ecx
                    jnz   PIXEL_LOOP
            }

            _asm  mov  eax, bmDst.bmWidthBytes  // pBitsDst -= bmDst.bmWidthBytes;
                _asm  sub  edi, eax

                _asm  mov  eax, bmSrc.bmWidthBytes  // pBitsSrc -= bmSrc.bmWidthBytes;
                _asm  sub  esi, eax
        }
        _asm emms;

        // 	for (int yd = yDest, ys = ySrc; yd < ny2; yd++, ys++)
        // 	{
        // 		for (int xd = xDest, xs = xSrc; xd < nx2; xd++, xs++)
        // 		{
        // 
        // 			int dpos = xd << 2;
        // 			int spos = xs << 2;
        // 
        // 			byte srcAlpha = pBitsSrc[spos+3];
        // 			byte srcAlpha2 = 255-srcAlpha;
        // 
        // 			pBitsDst[dpos]   = pBitsSrc[spos]   + ((srcAlpha2*pBitsDst[dpos])>>8);
        // 			pBitsDst[dpos+1] = pBitsSrc[spos+1] + ((srcAlpha2*pBitsDst[dpos+1])>>8);
        // 			pBitsDst[dpos+2] = pBitsSrc[spos+2] + ((srcAlpha2*pBitsDst[dpos+2])>>8);
        // 			pBitsDst[dpos+3] = srcAlpha + ((srcAlpha2*pBitsDst[dpos+3])>>8);
        // 		}
        // 		pBitsDst -= bmDst.bmWidthBytes;
        // 		pBitsSrc -= bmSrc.bmWidthBytes;
        // 	}
    };
}
#endif

void  BufferRenderLayer::Commit(HDC hDstDC)
{
    // 组合
    if (hDstDC)
    {
        if (this->IsTransparent())  
        {
            BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

            ::AlphaBlend(hDstDC, m_rcParent.left, m_rcParent.top, m_rcParent.Width(), m_rcParent.Height(), 
                m_hMemDC, 0,0, m_buffer.GetWidth(), m_buffer.GetHeight(), bf);
        }
        else
        {
            ::BitBlt(hDstDC, m_rcParent.left, m_rcParent.top, m_rcParent.Width(), m_rcParent.Height(), 
                m_hMemDC, 0,0, SRCCOPY);            
        }
    }
}



//	当窗口有类型发生改变时(分层窗口<->普通窗口)，重新创建render target
//
void BufferRenderLayer::ReCreateRenderTarget()
{
    if (NULL == m_pWindow || NULL == m_hMemDC)
        return;

    SAFE_DELETE(m_pRenderTarget); // 需要重新创建一个基于新窗口类型的RenderTarget

    m_pRenderTarget = UICreateRenderTarget(m_pWindow->GetHWND());
    m_pRenderTarget->BindHDC(m_hMemDC);
}

void  ControlRenderLayer::Draw()
{
    if (!CanRedraw())
        return;

    if (NULL == m_pRenderTarget)
        return;

    if (m_pRenderTarget->BeginDraw(0, 0))
    {
        CRect  rc(0, 0, GetWidth(), GetHeight());
        RenderContext roc(&rc, true);

        this->DrawChildObject(m_pRenderTarget, roc);

        m_pRenderTarget->EndDraw();
    }
}
