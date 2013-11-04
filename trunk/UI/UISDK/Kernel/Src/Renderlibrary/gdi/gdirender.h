#ifndef GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425
#define GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425

#include "gdibitmap.h"
#include "gdifont.h"
#include "gdipen.h"
#include "gdibrush.h"

namespace UI
{

class GdiRenderTarget : public IRenderTarget
{
public:
 	GdiRenderTarget(HWND hWnd);
	virtual ~GdiRenderTarget();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }

	virtual void     BindHDC(HDC hDC);
	virtual HDC      GetHDC();
	virtual void     ReleaseHDC( HDC hDC );
	virtual HDC      GetBindHDC();

	virtual HRGN     GetClipRgn();
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );

	virtual bool     BeginDraw(RECT* prcArray, int rcCount, bool bClear=false);
	virtual void     EndDraw( );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){};
	virtual BYTE*    LockBits() {return NULL;};
	virtual void     UnlockBits(){};
	virtual void     Clear();
	virtual void     Save( const String& strPath ){};
	virtual HBITMAP  CopyRect(RECT *prc) {return NULL;}

	virtual void     FillRgn(HRGN, UI::Color* pColor);
	virtual void     FillRect(const CRect* lprc, UI::Color* pColor);
	virtual void     TileRect(const CRect* lprc, IRenderBitmap* hBitmap);
	virtual void     Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack);
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*);
	virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*);
	virtual void     GradientFillH(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
	virtual void     GradientFillV(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
	virtual void     BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop);
	virtual void     ImageList_Draw(IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy );
	virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);
	virtual void     DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam);
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);
	static  void     DrawBitmapEx(HDC hDC, IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);

	virtual IRenderPen*    CreateSolidPen(int nWidth, Color* pColor);
	virtual IRenderPen*    CreateDotPen(int nWidth, Color* pColor);
	virtual IRenderBrush*  CreateSolidBrush(Color* pColor);

protected:
	void     DrawBitmap(IRenderBitmap* hBitmap, int x, int y);
	void     DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	void     DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	void     DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						Image9Region* p9Region );
protected:
	HDC       m_hDC;     // 通过alphablend能够实现半透明绘制，但无法实现其它类型的绘制alpha通道，如文字、24bits位图
	bool      m_bDrawing;// 标识外部已经调用了一次BeginDraw，再次调用时将直接返回false
};

#if 0
class GDIMemRenderDC : public GdiRenderTarget
{
public:
	GDIMemRenderDC(HDC hDC, int nWidth, int nHeight);
	GDIMemRenderDC(HWND hWnd, int nWidth, int nHeight);
	virtual ~GDIMemRenderDC();

	virtual bool     BeginDraw( HDC hDC ) ;
	virtual void     EndDraw( );
//	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight );
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Clear(){}  // 仅gdiplus中实现了。用于窗口透明背景时避免本次背景贴在上一次的背景上造成alpha叠加
	virtual void     Save( const String& strPath );
	virtual HBITMAP  CopyRect(RECT *prc);

	HBITMAP GetMemBitmap() { return m_hOldBitmap; }
protected:
	GDIRenderBitmap*  m_pMemBitmap;
	HBITMAP   m_hOldBitmap; // selectobject的返回值，用于将m_pMemBitmap从m_hDC中选出

	HDC       m_hWndDC;     // m_hWnd对应的HDC，有可能等于m_hOldWndDC，有可能是直接GetDC(m_hWnd)
	HDC       m_hOldWndDC;  // 需要根据m_hOldWndDC是否为空来判断是否需要ReleaseDC(m_hWndDC)

	int       m_nWidth;
	int       m_nHeight;
};
#endif
}

#endif  // GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425