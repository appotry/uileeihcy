#pragma once
/*#include "gdiplusfix.h"*/
#include "gdipluspen.h"
#include "gdiplusbitmap.h"
#include "gdiplusfont.h"

namespace UI
{
//////////////////////////////////////////////////////////////////////////

class GdiplusRenderTarget : public IRenderTarget
{
public:
 	GdiplusRenderTarget(HWND hWnd);
	virtual ~GdiplusRenderTarget();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }

	virtual void     BindHDC(HDC hDC);
	virtual HDC      GetBindHDC();

	virtual HDC      GetHDC();
	virtual void     ReleaseHDC( HDC hDC );

	virtual HRGN     GetClipRgn();
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );

	// 只有GdiplusMemRenderDC才支持
	virtual bool     BeginDraw(RECT* prcArray, int rcCount, bool bClear=false);
	virtual void     EndDraw();
//	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish ){};
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){}; 
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Clear();
	virtual void     Save( const String& strPath ){};
	virtual HBITMAP  CopyRect(RECT *prc){return NULL;}

	virtual void     FillRgn( HRGN hRgn, UI::Color* pColor);
	virtual void     FillRect( const CRect* lprc, UI::Color* pColor);
	virtual void     TileRect( const CRect* lprc, IRenderBitmap* hBitmap );
	virtual void     Rectangle( const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack );
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*);
	virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*);
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop );
	virtual void     ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy );
	
    virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam );
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);
	virtual void     DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam);

    virtual IRenderPen*    CreateSolidPen(int nWidth, Color* pColor);
    virtual IRenderPen*    CreateDotPen(int nWidth, Color* pColor); 
    virtual IRenderBrush*  CreateSolidBrush(Color* pColor);

public:
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int x, int y );
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, Gdiplus::ImageAttributes* pAttr=NULL);
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
                            Image9Region* p9Region, bool bDrawCenter=true, Gdiplus::ImageAttributes* pAttr=NULL);		
	static void  DrawBitmapEx(Gdiplus::Graphics* pGraphics, HDC hBindDC, IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam);
    static void  DrawStringEx(Gdiplus::Graphics* pGraphics, HDC hBindDC, IRenderFont*  pFont, DRAWTEXTPARAM* pParam);

    static void  GetStringFormatByGdiFormat(UINT, Gdiplus::StringFormat* p);
protected:
	HDC      m_hDC;
	Gdiplus::Graphics*  m_pGraphics;
	Gdiplus::Bitmap*    m_pGdiMemBitmap;
	HRGN     m_hRgnMeta;   // 用于模拟实现GDI的SetMetaRgn函数
};
}