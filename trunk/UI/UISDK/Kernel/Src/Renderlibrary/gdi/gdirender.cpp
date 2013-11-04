#include "stdafx.h"
#include "gdirender.h"
#include "UISDK\Kernel\Src\UIEffect\��˹ģ��\gaussblur.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusrender.h"



GdiRenderTarget::GdiRenderTarget(HWND hWnd):IRenderTarget(hWnd)
{
 	m_hDC = NULL;
	m_bDrawing = false;
}
GdiRenderTarget::~GdiRenderTarget()
{
// 	if( NULL != m_hWnd && NULL != m_hDC)
// 	{
// 		::ReleaseDC(m_hWnd,m_hDC);
// 	}
 	m_hDC = NULL;
}

// IRenderTarget* GdiRenderTarget::CreateCompatibleHRDC( int nWidth, int nHeight )
// {
// 	UIASSERT(0);  // TODO: �ú���������
// 	return (IRenderTarget*)new GDIMemRenderDC(m_hDC, nWidth, nHeight);
// }

HDC GdiRenderTarget::GetHDC()
{
	return m_hDC;
}
void GdiRenderTarget::ReleaseHDC( HDC hDC )
{
	return ;
}
// ��HDC����Ҫ�ͷ� 
HDC GdiRenderTarget::GetBindHDC()
{
	return m_hDC;
}

HRGN GdiRenderTarget::GetClipRgn()
{
	HRGN hRgn = ::CreateRectRgn(0,0,0,0);
	if( 1 != ::GetClipRgn(m_hDC,hRgn) )  // �ջ���ʧ��
	{
		::DeleteObject(hRgn);
		hRgn = NULL;
	}
	return hRgn;
}
int GdiRenderTarget::SelectClipRgn( HRGN hRgn, int nMode )
{
	if (RGN_COPY == nMode/* && NULL != hRgn*/)
	{
		return ::SelectClipRgn(m_hDC, hRgn);
	}

	int nRet = ExtSelectClipRgn(m_hDC, hRgn, nMode);
	return nRet;
}

BOOL GdiRenderTarget::GetViewportOrgEx( LPPOINT lpPoint )
{
	if (NULL == lpPoint)
	{
		return FALSE;
	}

	::GetViewportOrgEx(m_hDC, lpPoint );
	return TRUE;
}
BOOL GdiRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint ) 
{
	BOOL bRet1 = ::SetViewportOrgEx( m_hDC, x, y, lpPoint);
	return bRet1;
}
BOOL GdiRenderTarget::OffsetViewportOrgEx(int x, int y, LPPOINT lpPoint)
{
	BOOL bRet1 = ::OffsetViewportOrgEx( m_hDC, x, y, lpPoint );
	return bRet1;
}

void GdiRenderTarget::BindHDC(HDC hDC)
{
	if (NULL == hDC)
		return;

	m_hDC = hDC;
}
//
// �����Ҫͬʱ��������item�������ṩ����RECT���вü�
//
// bClear��Ҫ�����ڷֲ㴰���е�͸��������Ŀǰû�з���GDI����Ҫ��ˢ�����ĵط�
//
bool GdiRenderTarget::BeginDraw(RECT* prcArray, int nrcCount, bool bClear)
{
#if 1
	if (false == m_bDrawing)  // ����Ѿ��ڻ����ˣ����ܰ���һ�εļ��������������
	{
		if (nrcCount > 0 && NULL != prcArray)
		{
			RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT) ];
			pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
			pRgnData->rdh.iType      = RDH_RECTANGLES;
			pRgnData->rdh.nCount     = nrcCount;
			pRgnData->rdh.nRgnSize   = nrcCount*sizeof(RECT);
			for (int k = 0; k < nrcCount; k++)
			{
				RECT* prc = (RECT*)pRgnData->Buffer;
				prc[k] = prcArray[k];
			}

			HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT), pRgnData);
			SAFE_ARRAY_DELETE(pRgnData);

			this->SelectClipRgn(hRgn, RGN_COPY);
			SAFE_DELETE_GDIOBJECT(hRgn);
		}
	}

#else
	if (NULL == prc && NULL == prc2)
	{
	}
	else if (prc && NULL != prc2)
	{
		HRGN hRgn = CreateRectRgnIndirect(prc);
		if (prc2)
		{
			HRGN hRgn2 = ::CreateRectRgnIndirect(prc2);
			::CombineRgn(hRgn, hRgn, hRgn2, RGN_OR);
			SAFE_DELETE_GDIOBJECT(hRgn2);
		}
		this->SelectClipRgn(hRgn, RGN_COPY);
		SAFE_DELETE_GDIOBJECT(hRgn);
	}
	else
	{
		RECT *pNonNullRect = prc==NULL?prc2:prc;
		HRGN hRgn = CreateRectRgnIndirect(pNonNullRect);
		this->SelectClipRgn(hRgn, RGN_COPY);
		SAFE_DELETE_GDIOBJECT(hRgn);
	}
#endif

	if (bClear && nrcCount > 0)
	{
		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		for (int i = 0; i < nrcCount; i++)
		{
			::FillRect(m_hDC, &(prcArray[i]), hBrush);
		}
	}
	if (false == m_bDrawing)
	{
		m_bDrawing = true;
		return true;
	}
	else
    {
		return false;
    }
}
void GdiRenderTarget::EndDraw()
{
	if (m_bDrawing)
	{
		this->SetViewportOrgEx(0,0);
		this->SelectClipRgn(NULL);
		m_bDrawing = false;
	}
}

void GdiRenderTarget::Clear()
{
	if (NULL == m_hDC)
		return;

	BITMAP  bm;
	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP);
	if (NULL == hBitmap)
		return;

	::GetObject(hBitmap, sizeof(bm), &bm);
	RECT  rc = {0,0, bm.bmWidth, bm.bmHeight};
	::FillRect(m_hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
}

void  GdiRenderTarget::DrawString(IRenderFont* pRenderFont, DRAWTEXTPARAM* pParam)
{
    if (NULL == pParam || NULL == pRenderFont)
        return;

	if (NULL == pParam->prc || pParam->prc->Width() <= 0 || pParam->prc->Height() <= 0)
		return;
	

    GRAPHICS_RENDER_LIBRARY_TYPE  eType = pRenderFont->GetGraphicsRenderLibraryType();
	if (eType != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
        if (eType == GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
        {
            Gdiplus::Graphics g(m_hDC);
            GdiplusRenderTarget::DrawStringEx(&g, m_hDC, pRenderFont, pParam);
            return;
        }
        else
        {
		    UI_LOG_WARN(_T("GdiRenderTarget::DrawString pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDI"));
	    	return;
        }
	}

	HDC hDC = m_hDC;	

    switch (pParam->nEffectFlag)
    {
    case TEXT_EFFECT_NONE:
        {
            HFONT hOldFont = (HFONT)::SelectObject(hDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());

            COLORREF col = pParam->color.GetGDICompatibleValue();
            COLORREF oldCol = ::SetTextColor(hDC, col);

            ::DrawText(hDC, pParam->szText, _tcslen(pParam->szText), (RECT*)pParam->prc, pParam->nFormatFlag);

            ::SetTextColor(hDC,oldCol);
            ::SelectObject(hDC, hOldFont);
        }
        break;

    case TEXT_EFFECT_HALO:
        {
#if 0
            RECT rcRealInMemBmp = *(pParam->prc); // ���������ţ�������ƫ��
            ::LPtoDP(hDC, (LPPOINT)&rcRealInMemBmp, 2);

            HFONT hOldFont = (HFONT)::SelectObject(hDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());
            HBITMAP hMemBmp = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
            COLORREF colorShadow = pParam->bkcolor.GetGDICompatibleValue();
            COLORREF colorText = pParam->color.GetGDICompatibleValue();
            COLORREF oldColor = ::SetTextColor(hDC, colorShadow);

            // ��Ӱ
            ::DrawText(hDC, pParam->szText, _tcslen(pParam->szText), (LPRECT)pParam->prc, pParam->nFormatFlag);

            // ģ��
            recursive_blur blur;
            blur.blur(hMemBmp, (double)pParam->wParam, (LPRECT)&rcRealInMemBmp);

            // ����
            ::SetTextColor(hDC, colorText);
            ::DrawText(hDC, pParam->szText, _tcslen(pParam->szText), (LPRECT)pParam->prc, pParam->nFormatFlag);

            ::SetTextColor(hDC, oldColor);
            ::SelectObject(hDC, hOldFont);

#elif 1 // �Լ�Ҫÿ�δ���һ���ڴ�λ�ã�Ч��̫�ͣ�����
            HDC hMemDC = CreateCompatibleDC(NULL);
            SetBkMode(hMemDC, TRANSPARENT);

            int nWidth = pParam->prc->Width();
            int nHeight = pParam->prc->Height();
            RECT rcMem = { 0, 0, pParam->prc->Width(), pParam->prc->Height() };

            HBITMAP hMemBmp = CacheBitmap::GetInstance()->Create(nWidth, nHeight);
            HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

            // �Ȼ��ϱ�������ΪĬ�ϵ�DCͼƬ��ȫ0
            ::BitBlt(hMemDC, 0, 0, pParam->prc->Width(), pParam->prc->Height(), 
                hDC, pParam->prc->left, pParam->prc->top, SRCCOPY);

            HFONT hOldFont = (HFONT)::SelectObject(hMemDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());
            COLORREF colorShadow = pParam->bkcolor.GetGDICompatibleValue();
            COLORREF oldColor = ::SetTextColor(hMemDC, colorShadow);

            // ��Ӱ
            ::DrawText(hMemDC, pParam->szText, _tcslen(pParam->szText), (RECT*)&rcMem, pParam->nFormatFlag);

            // ģ��
            recursive_blur<> blur;
            RECT rc = {0, 0, nWidth, nHeight};
            blur.blur(hMemBmp, (double)pParam->wParam, &rc, 0);

            // ����
            COLORREF color = pParam->color.GetGDICompatibleValue();
            ::SetTextColor(hMemDC, color);
            ::DrawText(hMemDC, pParam->szText, _tcslen(pParam->szText), (RECT*)&rcMem, pParam->nFormatFlag);

            ::BitBlt(hDC, pParam->prc->left, pParam->prc->top, pParam->prc->Width(), pParam->prc->Height(), hMemDC, 0, 0, SRCCOPY);
            ::SetTextColor(hMemDC, oldColor);
            ::SelectObject(hMemDC, hOldFont);
            ::SelectObject(hMemDC, hOldBmp);
            ::DeleteDC(hMemDC);
#endif
#if 0  // gdi �Ŵ�ʽ��Ӱʵ�֣�Ч������
            float fScale = (float)1.5;
            CRect rcSmall(0, 0, pParam->prc->Width()/fScale, pParam->prc->Height()/fScale);

            HDC hTempDC = CreateCompatibleDC(NULL);
            SetBkMode(hTempDC, TRANSPARENT);
            Image image;
            image.Create(rcSmall.Width(), rcSmall.Height(), 24, 0/*Image::createAlphaChannel*/);
            //image.SetAlpha(255);
            HBITMAP hTempBmp = image.Detach();
            SelectObject(hTempDC, hTempBmp);

            SetGraphicsMode(hTempDC, GM_ADVANCED);
            XFORM xForm = {1/fScale, 0, 0, 1/fScale, 0, 0};
            SetWorldTransform(hTempDC, &xForm);

            {
                HFONT hOldFont = (HFONT)::SelectObject(hTempDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());
                COLORREF oldCol = ::SetTextColor(hTempDC, RGB(255,255,255));
                RECT rc = { 0, 0, pParam->prc->Width(), pParam->prc->Height() };
                ::DrawText(hTempDC, pParam->szText, _tcslen(pParam->szText), (RECT*)&rc, pParam->nFormatFlag);
                ::SelectObject(hTempDC, hOldFont);
            }
            ModifyWorldTransform(hTempDC, NULL, MWT_IDENTITY);
      //      Util::FixGdiAlpha(hTempDC, &rcSmall);
//             image.Attach(hTempBmp);
//             image.Save(L"C:\\aaa.bmp", Gdiplus::ImageFormatBMP);

            SetStretchBltMode(hDC, HALFTONE);
            ::StretchBlt(hDC, pParam->prc->left, pParam->prc->top, pParam->prc->Width(), pParam->prc->Height(), hTempDC, 0, 0, rcSmall.Width(), rcSmall.Height(), SRCCOPY);

            ::DeleteObject(hTempBmp);
            ::DeleteDC(hTempDC);

            pParam->nEffectFlag = TEXT_EFFECT_NONE;
            DrawString(pRenderFont, pParam);
#endif
        }
        break;
    }

	
}



void GdiRenderTarget::FillRgn( HRGN hRgn, UI::Color* pColor)
{
    if (NULL == pColor)
        return;

	HBRUSH hBrush = ::CreateSolidBrush(RGB(pColor->r, pColor->g, pColor->b));
	::FillRgn(m_hDC, hRgn, hBrush);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::FillRect(const CRect* lprc, UI::Color* pColor)
{
    if (NULL == pColor)
        return;

    COLORREF gdicolor = 0;
    if (0 == pColor->a)
        return;
    
    gdicolor = pColor->GetGDICompatibleValue();

	HBRUSH hBrush = ::CreateSolidBrush(gdicolor);
	int n = ::FillRect(m_hDC, lprc, hBrush);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::TileRect(const CRect* lprc, IRenderBitmap* pRenderBitmap)
{
	if (NULL == pRenderBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)pRenderBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	HBRUSH hBrush = ::CreatePatternBrush(pBitmap->GetBitmap()->operator HBITMAP());
	::FillRect(m_hDC, lprc, hBrush);
	::DeleteObject(hBrush);
}

//
// ע�����ڿؼ�����ʱ�Ѿ����ü��������⽫����ʹ��pen���Ʊ߿�ʱ����һ���ֻ��Ƴ�����
//     �߿�λ�ڿؼ�����������á�
//
void GdiRenderTarget::Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack)
{
	COLORREF gdicolBorder = 0;
	COLORREF gdicolBack = 0;
	
    if (!pColBorder)
        return;

    if (0 == pColBorder->a)
        return;

    gdicolBorder = pColBorder->GetGDICompatibleValue();

	if (!bNullBack && pColBack && 0 != pColBack->a)
	{
        gdicolBack = pColBack->GetGDICompatibleValue();
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nBorder, gdicolBorder);
	HBRUSH hBrush = NULL;
	if (bNullBack)
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	else
	{
		hBrush = ::CreateSolidBrush(gdicolBack);
	}

	HDC hDC = m_hDC;

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);

	::Rectangle(hDC, lprc->left, lprc->top, lprc->right, lprc->bottom);

	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::DrawFocusRect( const CRect* lprc )
{
	::DrawFocusRect(m_hDC, lprc);
}
void GdiRenderTarget::DrawLine(int x1, int y1, int x2, int y2, IRenderPen* pPen)
{
	if (NULL == pPen)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GdiPen* pGdiPen = (GdiPen*)pPen;
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, pGdiPen->m_hPen);

	::MoveToEx(m_hDC, x1, y1, NULL);
	::LineTo(m_hDC, x2, y2);

	::SelectObject(m_hDC, hOldPen);
}
void GdiRenderTarget::DrawPolyline(POINT* lppt, int nCount, IRenderPen* pPen)
{
	if (NULL == pPen || NULL == lppt)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GdiPen* pGdiPen = (GdiPen*)pPen;
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, pGdiPen->m_hPen);

	::Polyline(m_hDC, lppt, nCount);

	::SelectObject(m_hDC, hOldPen);
}

void GdiRenderTarget::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillH(m_hDC, lprc, colFrom, colTo );
}
void GdiRenderTarget::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillV(m_hDC, lprc, colFrom, colTo );
}

void GdiRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop)
{
	if (NULL == pSrcHDC)
		return;
	if (pSrcHDC->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	::BitBlt(m_hDC, xDest,yDest,wDest,hDest, pSrcHDC->GetHDC(),xSrc,ySrc,dwRop );
}

void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int x, int y)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, x,y);
}
void GdiRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if (NULL == pBitmap)
		return;

	if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pRenderBitmap = static_cast<GDIRenderBitmap*>(pBitmap);
	if (NULL == pBitmap)
		return;

	Image* pImage = pRenderBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest,yDest, wDest,hDest, xSrc, ySrc, wDest, hDest);
}
void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}

void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					Image9Region* p9Region )
{
	if( NULL == hBitmap )
		return;

	if( NULL == p9Region )
		return this->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}

void GdiRenderTarget::ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->ImageList_Draw(m_hDC, x,y,col,row,cx,cy);
}

void GdiRenderTarget::DrawBitmapEx(HDC hDC, IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
	IRenderBitmap* p = (IRenderBitmap*)pBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
		if (p->GetGraphicsRenderLibraryType() == GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS) // ��Ҫ����ݵ����
		{
			Gdiplus::Graphics g(hDC);
			GdiplusRenderTarget::DrawBitmapEx(&g, hDC, p, pParam);
		}
		return;
	}

	Image* pImage =  ((GDIRenderBitmap*)p)->GetBitmap();
	Image  imageDisable;
	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		imageDisable.CopyGrayImageFrom(pImage);
		pImage = &imageDisable;
	}

	if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pParam->nAlpha);
        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT)
    {
        int nW = min(pParam->wDest, pParam->wSrc);
        int nH = min(pParam->hDest, pParam->hSrc);
        pImage->Draw(hDC, pParam->xDest, pParam->yDest, nW, nH, pParam->xSrc, pParam->ySrc, nW, nH, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+nW, pParam->yDest+nH);
        }
    }
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
	{
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, false, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		if (NULL == pBitmap)
			return;

		RECT rc = {pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest};
		HBRUSH hBrush = ::CreatePatternBrush(pImage->operator HBITMAP());
		::FillRect(hDC, &rc, hBrush);
		::DeleteObject(hBrush);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		pImage->Draw(hDC, x,y, pParam->wSrc, pParam->hSrc, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(x, y, x+pParam->wSrc, y+pParam->hSrc);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
	{
		if (pParam->wSrc == 0 || pParam->hSrc == 0)
			return;

		if (pParam->wDest == 0 || pParam->hDest == 0)
			return;


		bool bNeedToStretch = false;
		int  wImage = pParam->wSrc;
		int  hImage = pParam->hSrc;

		if (pParam->wDest < pParam->wSrc || pParam->hDest < pParam->hSrc)
		{
			bNeedToStretch = true;

			double tan_x_y_image = (double)pParam->wSrc / (double)pParam->hSrc;
			double tan_x_y_dest = (double)pParam->wDest / (double)pParam->hDest;

			if( tan_x_y_image > tan_x_y_dest ) // ����ռ��
			{
				wImage = pParam->wDest;
				hImage = (int)((double)wImage/tan_x_y_image);
			}
			else   // ����ռ��
			{
				hImage = pParam->hDest;
				wImage = (int)(hImage*tan_x_y_image);
			}
		}

		// ����ͼƬ��ʾλ��
		int xDisplayPos = pParam->xDest + (pParam->wDest-wImage)/2;
		int yDisplayPos = pParam->yDest + (pParam->hDest-hImage)/2;

		if (bNeedToStretch)
		{
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pParam->nAlpha);
		}
		else
		{
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, wImage, hImage, pParam->nAlpha);
		}

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDisplayPos, yDisplayPos, xDisplayPos+wImage, yDisplayPos+hImage);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT_RIGHTTOP)
    {
        int nW = min(pParam->wDest, pParam->wSrc);
        int nH = min(pParam->hDest, pParam->hSrc);

        int xDest = pParam->xDest + (pParam->wDest-nW);
        int xSrc = pParam->xSrc+ (pParam->wSrc-nW);

        pImage->Draw(hDC, xDest, pParam->yDest, nW, nH, xSrc, pParam->ySrc, nW, nH, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDest, pParam->yDest, xDest+nW, pParam->yDest+nH);
        }
    }
}
void GdiRenderTarget::DrawBitmap(IRenderBitmap* pRenderBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == pRenderBitmap || NULL == pParam)
		return;

	GdiRenderTarget::DrawBitmapEx(m_hDC, pRenderBitmap, pParam);
}

//
//
// ����ͼƬ������תnDegree�Ȼ���ͼƬ
// 
//
// ʹ��SetWorldTransformҲ����ʵ����ת��Ч��������MSDN����Example
//
// The SetWorldTransform function sets a two-dimensional linear transformation between 
// world space and page space for the specified device context. This transformation can 
// be used to scale, rotate, shear, or translate graphics output.
//
// ����ת����ʽ 
//  newx = x * eM11 + y * eM21 + eDx 
//  newy = x * eM12 + y * eM22 + eDy 
//  ����eM11��eM22�ǽǶȵ�����ֵ��eM21�ǽǶȵ����ң�eM12��eM21�ĸ�ֵ��
//
// [ע1] ���hDC��һ��MemDC�Ļ���������ڻ�����֮�����ModifyWorldTransform(hMemDC, NULL, MWT_IDENTITY);
//       �����memdc�ύ������DC�ϵ����ݽ�����ȷ
// [ע2] Ĭ����Щ��ת���ǻ���(0,0)����ģ����ú���Ҫʵ�ֵ�Ŀ���ǻ�������������ת
//
void GdiRenderTarget::DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam)
{

	IRenderBitmap* p = (IRenderBitmap*)pBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

#define M_PI 3.14159265358979323846

	Image* pImage =  ((GDIRenderBitmap*)p)->GetBitmap();
	if (pImage)
	{
		POINT ptCenter = {pParam->xDest+pParam->wDest/2, pParam->yDest+pParam->hDest/2};  // ͼƬĿ����������ģ��Ƹõ���ת��������Ĭ���ǣ�0��0��������ת

		double fAngle =  M_PI * nDegree / 180; // (2*pi *nDegree)/360 ; 
		float cosAngle = (float)cos(fAngle);
		float sinAngle = (float)sin(fAngle);

#if 0  // ʹ��PlgBlt���Ƶ�ͼƬ���޷�ʹ��Alpha����˷���

		POINT ptOri[3] = // Ŀ��������꣨δ��תǰ��
		{
			{pParam->xDest, pParam->yDest}, 
			{pParam->xDest+pParam->wDest, pParam->yDest}, 
			{pParam->xDest, pParam->yDest+pParam->hDest}
		};
		POINT ptOriToCenter[3] = // �����ͼƬ����λ��
		{
			{ptOri[0].x - ptCenter.x, ptOri[0].y - ptCenter.y},
			{ptOri[1].x - ptCenter.x, ptOri[1].y - ptCenter.y},
			{ptOri[2].x - ptCenter.x, ptOri[2].y - ptCenter.y}
		};

		POINT ptArg[3] = {0};  // ������ת���ƽ���ı��е�ǰ��������λ��
		for (int i = 0; i < 3; i++)
		{
			ptArg[i].x = (int)(ptOriToCenter[i].x*cosAngle + ptOriToCenter[i].y*sinAngle); 
			ptArg[i].y = (int)(ptOriToCenter[i].y*cosAngle - ptOriToCenter[i].x*sinAngle);

			// �ָ�Ϊ����ڣ�0��0������
			ptArg[i].x += ptCenter.x;
			ptArg[i].y += ptCenter.y;
		}

		pImage->PlgBlt(m_hDC, ptArg, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, NULL, 0, 0);

#else

 		int nOldGraphicsMode = ::GetGraphicsMode(m_hDC);
 		::SetGraphicsMode(m_hDC, GM_ADVANCED);
  		XFORM xForm = {0};
		xForm.eM11 = cosAngle;
		xForm.eM12 = sinAngle;
		xForm.eM21 = -sinAngle;
		xForm.eM22 = cosAngle;
		xForm.eDx = ptCenter.x - cosAngle*ptCenter.x + sinAngle*ptCenter.y;
		xForm.eDy = ptCenter.y - cosAngle*ptCenter.y - sinAngle*ptCenter.x;
		SetWorldTransform(m_hDC, &xForm);
 		DrawBitmapEx(m_hDC, pBitmap, pParam);

		ModifyWorldTransform(m_hDC, NULL, MWT_IDENTITY);
 		::SetGraphicsMode(m_hDC, nOldGraphicsMode);
#endif
	}
}


IRenderPen* GdiRenderTarget::CreateSolidPen(int nWidth, Color* pColor)
{
	IRenderPen* p = NULL;
	GdiPen::CreateInstance(&p);

	if (p)
	{
		p->CreateSolidPen(nWidth, pColor);
	}
	return p;
}
IRenderPen* GdiRenderTarget::CreateDotPen(int nWidth, Color* pColor)
{
	IRenderPen* p = NULL;
	GdiPen::CreateInstance(&p);

	if (p)
	{
		p->CreateDotPen(nWidth, pColor);
	}
	return p;
}
IRenderBrush*  GdiRenderTarget::CreateSolidBrush(Color* pColor)
{
	IRenderBrush* p = NULL;
	GdiBrush::CreateInstance(&p);

	if (p)
	{
		p->CreateSolidBrush(pColor);
	}
	return p;
}

#if 0
GDIMemRenderDC::GDIMemRenderDC(HDC hDC, int nWidth, int nHeight ):GdiRenderTarget(NULL)
{
	UIASSERT(0);  // ����ʹ�ø���
	::OutputDebugString(_T("TODO: GDIMemRenderDC::GDIMemRenderDC ɾ���ú��������ٵ��á�\r\n"));
	m_hDC = ::CreateCompatibleDC(hDC);

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );

	::SetBkMode(m_hDC, TRANSPARENT);   // ��ȥ֧�ִ����������֣������Ҫ��ʹ�ñ������
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
}
GDIMemRenderDC::GDIMemRenderDC(HWND hWnd, int nWidth, int nHeight ) : GdiRenderTarget(hWnd) 
{
	UIASSERT(0);  // ����ʹ�ø���

	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC    = NULL;
	m_hOldBitmap = NULL;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
}
GDIMemRenderDC::~GDIMemRenderDC()
{
	if (m_hOldBitmap)
	{
	    if (m_hDC)
		{
			::SelectObject(m_hDC, m_hOldBitmap);
			m_hOldBitmap = NULL;
		}
		else
		{
			UI_LOG_WARN(_T("%s old bitmap is not selected back into dc"),FUNC_NAME);
		}		
	}
	SAFE_DELETE(m_pMemBitmap);

	if( NULL != m_hDC )
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
	m_hWndDC = NULL;
}

bool GDIMemRenderDC::BeginDraw(HDC hDC)
{
	// ע���������Ų���һ�졣��ʱ���ִ��ڱ�ɿհ��ˣ�û�����ݡ�
	//     Ȼ����Թ����У�����Ҳ��������֪�����ĵ����⣬��ӡ������ͼƬ��ʾmem bmpֻ��ʾ��һ���ֿؼ���Ȼ�����һֱҲû���ٱ仯
	//     ͨ������һ��FillRect(RGB(0,0,0)),����mem bmp�����ݾ�Ȼû�з����仯����˿����ƶ���mem bmpû�б���ȷѡ�뵽m_hDC�С�
	//     Ȼ����BeginDraw�з��֣����������ʱ��selectobject����Ϊ�գ�ʧ���ˡ��ٹ۲��ջ������BeginDrawǶ����Ӧ�����Σ���
	//     EndDrawû����Ӧ�����µ�һ�ε�m_hDC����д��ͬʱmem bmpҲ���ڱ���һ�ε�m_hDCѡ���˶��޷��ͷţ����Ҳ�����ٱ�
	//     ����DCѡ�롣���Բŵ������еĻ��Ʋ�������Ч�ˡ�
	//     
	if (m_hDC)  
	{
		UI_LOG_WARN(_T("%s pre invoke has not been finish."),FUNC_NAME);
		return false;
	}

	m_hOldWndDC = hDC;
	m_hWndDC = hDC == NULL ? ::GetDC(m_hWnd):hDC;
	m_hDC = ::CreateCompatibleDC(m_hWndDC);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
	UIASSERT(NULL != m_hOldBitmap);

	::SetBkMode(m_hDC, TRANSPARENT);
	::SetStretchBltMode(m_hDC, COLORONCOLOR);

	return true;
}

void GDIMemRenderDC::EndDraw( )
{
    if (NULL == m_hDC)
		return;
		
	::BitBlt(m_hWndDC, 0,0, m_nWidth, m_nHeight, m_hDC, 0,0, SRCCOPY );

	if (m_hOldBitmap)
	{
		::SelectObject(m_hDC, m_hOldBitmap);
		m_hOldBitmap = NULL;
	}

	::DeleteDC(m_hDC);
	if( NULL == m_hOldWndDC )
	{
		::ReleaseDC(m_hWnd, m_hWndDC);
	}
	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC = NULL;
}
// void GDIMemRenderDC::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish )
// {
// 	if (NULL == m_hDC)
// 		return;
// 
// 	::BitBlt(m_hWndDC, xDest, yDest, wDest, hDest, m_hDC, xSrc,ySrc, SRCCOPY );
// 	if (!bFinish)
// 		return;
// 
// 	if (m_hOldBitmap)
// 	{
// 		::SelectObject(m_hDC, m_hOldBitmap);
// 		m_hOldBitmap = NULL;
// 	}
// 	::DeleteDC(m_hDC);
// 	if( NULL == m_hOldWndDC )
// 	{
// 		::ReleaseDC(m_hWnd, m_hWndDC);
// 	}
// 	m_hWndDC = NULL;
// 	m_hOldWndDC = NULL;
// 	m_hDC = NULL;
// }

void GDIMemRenderDC::ResizeRenderTarget( int nWidth, int nHeight )
{
	if( m_nWidth == nWidth && m_nHeight == nHeight )
		return;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	if (m_hDC && NULL != m_hOldBitmap)
	{
		SelectObject(m_hDC, m_hOldBitmap);
		m_hOldBitmap = NULL;
	}
	SAFE_DELETE(m_pMemBitmap);

	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);

	if( NULL != m_hDC )
	{
		m_hOldBitmap = (HBITMAP)::SelectObject( m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
	}
}
BYTE* GDIMemRenderDC::LockBits()
{
#if 0
	DIBSECTION dibsection;
	int nBytes;

	nBytes = ::GetObject( m_hMemBitmap, sizeof( DIBSECTION ), &dibsection );
	if( nBytes == sizeof( DIBSECTION ) )
	{
		return (BYTE*)dibsection.dsBm.bmBits;
	}
	else
	{
		return NULL;
	}
#else
	if( NULL == m_pMemBitmap )
		return NULL;

	return m_pMemBitmap->LockBits();
#endif
}
void GDIMemRenderDC::UnlockBits()
{
	if( NULL == m_pMemBitmap )
		return;

	m_pMemBitmap->UnlockBits();
}

void GDIMemRenderDC::Save( const String& strPath )
{
	if( m_pMemBitmap->GetBitmap() )
	{
		REFGUID guidType = Gdiplus::ImageFormatBMP;
		String strExt = strPath.substr(strPath.length()-3,3);
		if (0 == _tcsicmp(strExt.c_str(), _T("bmp")))
		{
		}
		else if (0 == _tcsicmp(strExt.c_str(), _T("png")))
		{
			memcpy((void*)&guidType, & Gdiplus::ImageFormatPNG, sizeof(GUID));
		}
		else if (0 == _tcsicmp(strExt.c_str(), _T("jpg")))
		{
			memcpy((void*)&guidType, & Gdiplus::ImageFormatJPEG, sizeof(GUID));
		}
		else
		{
			return ;
		}

		m_pMemBitmap->GetBitmap()->Save( strPath.c_str(), guidType);
	}
}

HBITMAP GDIMemRenderDC::CopyRect(RECT *prc)
{
	if (NULL == m_pMemBitmap)
		return NULL;

	return m_pMemBitmap->CopyRect(prc);
}
#endif
