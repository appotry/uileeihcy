#pragma once

namespace UI
{
class GdiplusRenderBitmap : public IRenderBitmap
{
private:
	GdiplusRenderBitmap(IRenderBitmap** ppOutRef);
public:
	~GdiplusRenderBitmap();
	static  void  CreateInstance( IRenderBitmap** ppOutRef );
	virtual void  SetAttribute( const ATTRMAP& mapAttrib ){};
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }

	Gdiplus::Bitmap*  GetBitmap() { return m_pBitmap; }
	
public:
	virtual bool  LoadFromFile( const String& strPath );
	virtual bool  Create( int nWidth, int nHeight );

	virtual int   GetWidth();
	virtual int   GetHeight();

	virtual BYTE* LockBits();
	virtual void  UnlockBits();

	virtual bool  SaveBits( ImageData* pImageData );;
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag );

private:
	Gdiplus::Bitmap*     m_pBitmap;

	Gdiplus::BitmapData* m_pBitmapData;        // LockBits 过程中使用到的临时变量
	HBITMAP              m_hBitmapToFixIcon;   // 解决Gdiplus加载ICON时丢失alpha的问题
};


class  GdiplusRenderFont : public IRenderFont
{
protected:
	GdiplusRenderFont(IRenderFont** ppOutRef);

public:
	~GdiplusRenderFont();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }
	static  void  CreateInstance( IRenderFont** ppOutRef );

public:
	virtual bool    Load( LOGFONT* plogfont );
	virtual bool    ModifyFont(LOGFONT* plogfont);

	virtual void    Attach(HFONT hFont);
	virtual HFONT   Detach();

	virtual UINT    GetTextMetricsHeight( );
	virtual SIZE    MeasureString( const TCHAR* szText, int nLimitWidth = -1 );

	virtual HFONT   GetHFONT();
	Gdiplus::Font*  GetFont() { return m_pFont; }

private:
	Gdiplus::Font*  m_pFont;
	HFONT    m_hFontAttach;
	HFONT    m_hFontForGDI;
};

class GdiplusRenderDC : public IRenderDC
{
public:
	GdiplusRenderDC();
	GdiplusRenderDC(HDC hDC);
	GdiplusRenderDC(HWND hWnd);
	virtual ~GdiplusRenderDC();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }

	virtual HRDC     CreateCompatibleHRDC( int nWidth, int nHeight );
	virtual HDC      GetHDC();
	virtual void     ReleaseHDC( HDC hDC );

// 	virtual HRFONT   SelectFont( HRFONT hFont );
// 	virtual HRFONT   GetFont();
	virtual HRGN     GetClipRgn() ;
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );
// 	virtual COLORREF SetTextColor( COLORREF color, byte Alpha = 254 ) ;
// 	virtual COLORREF GetTextColor() ;

	// 只有GdiplusMemRenderDC才支持
	virtual bool     BeginDraw( HDC hDC ) {return false;}  
	virtual void     EndDraw( ){};
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish ){};
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){}; 
	virtual BYTE*    LockBits() {return NULL;};
	virtual void     UnlockBits(){};
	virtual void     Save( const String& strPath ){};

	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col );
	virtual void     FillRgn( HRGN hRgn, COLORREF color );
	virtual void     FillRect( const CRect* lprc, COLORREF col);
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap );
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack );
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderDC* pSrcHDC, int xSrc, int ySrc, DWORD dwRop );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int x, int y );
	virtual void     DrawBitmap( IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                         int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region );
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy );

protected:
	void   Init();

protected:
	Gdiplus::Graphics*  m_pGraphics;
//	Gdiplus::Color      m_colorText;    // 当前字体颜色
//	GdiplusRenderFont*  m_pFont;        // 当前字体
//	HFONT               m_hFont_GetHDC; // 当需要使用HDC时，同时需要select一个HFONT类型字体，但Font对象只能通过GetLogFont来新建一个字体

};

class GdiplusMemRenderDC : public GdiplusRenderDC
{
public:
	GdiplusMemRenderDC(int nWidth, int nHeight);
	GdiplusMemRenderDC(HWND hWnd, int nWidth, int nHeight);
	virtual ~GdiplusMemRenderDC();

public:
	Gdiplus::Bitmap*  GetMemBitmap();

	virtual bool     BeginDraw( HDC hDC );
	virtual void     EndDraw( );
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight );
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Save( const String& strPath );

protected:
	GdiplusRenderBitmap*  m_pMemBitmap;    // 双缓冲内存位图
	Gdiplus::Graphics*    m_pWndGraphics;  // 显示用的
	int      m_nWidth;
	int      m_nHeight;
};

}