#pragma once

namespace UI
{
class GDIRenderBitmap : public IRenderBitmap
{
private:
	GDIRenderBitmap(IRenderBitmap** ppOutRef);
public:
	~GDIRenderBitmap();
	static  void CreateInstance( IRenderBitmap** pOutRef );
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDI; }

	Image*  GetBitmap() { return &m_image; }

public:
	virtual bool  LoadFromFile( const String& strPath );
	virtual bool  Create(int nWidth, int nHeight);

	virtual int   GetWidth();
	virtual int   GetHeight();

	virtual BYTE* LockBits();
	virtual void  UnlockBits();

	virtual bool  SaveBits( ImageData* pImageData );
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag );

private:
	Image   m_image;
};


class  GDIRenderFont : public IRenderFont
{
protected:
	GDIRenderFont(IRenderFont** ppOutRef);

public:
	~GDIRenderFont();
	static  void  CreateInstance( IRenderFont** ppOutRef );
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDI; }

public:
	virtual bool  Load( LOGFONT* plogfont );
	virtual bool  ModifyFont(LOGFONT* plogfont);

	virtual void  Attach(HFONT hFont);
	virtual HFONT Detach();

	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 );
	virtual UINT  GetTextMetricsHeight( );

	virtual HFONT GetHFONT();

protected:
	void    DestroyFont();

private:
	HFONT   m_hFont;
	bool    m_bCreateOrAttach;  // 标明该m_hFont的来源，用于区分如何释放
};

class GDIRenderDC : public IRenderDC
{
public:
	GDIRenderDC();
	GDIRenderDC(HDC hDC);
	GDIRenderDC(HWND hWnd);
	virtual ~GDIRenderDC();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDI; }

	virtual HRDC     CreateCompatibleHRDC( int nWidth, int nHeight );
	virtual HDC      GetHDC();
	virtual void     ReleaseHDC( HDC hDC );

// 	virtual HRFONT   SelectFont( HRFONT hRFont );
// 	virtual HRFONT   GetFont();
	virtual HRGN     GetClipRgn();
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );
// 	virtual COLORREF SetTextColor( COLORREF color, byte Alpha = 255 ) ;
// 	virtual COLORREF GetTextColor() ;

	// 只有GdiMemRenderDC才支持 DoNothing
	virtual bool     BeginDraw( HDC hDC ) {return false;}
	virtual void     EndDraw( ) {}
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc ){}
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){}
	virtual BYTE*    LockBits() {return NULL;};
	virtual void     UnlockBits(){};
	virtual void     Save( const String& strPath ){};

	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col );
	virtual void     FillRgn( HRGN, COLORREF color );
	virtual void     FillRect( const CRect* lprc, COLORREF col);
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap );
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack );
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderDC* pSrcHDC, int xSrc, int ySrc, DWORD dwRop );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int x, int y);
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                         int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region );
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy );

protected:
	HDC       m_hDC;
};

class GDIMemRenderDC : public GDIRenderDC
{
public:
	GDIMemRenderDC(HDC hDC, int nWidth, int nHeight);
	GDIMemRenderDC(HWND hWnd, int nWidth, int nHeight);
	virtual ~GDIMemRenderDC();

	virtual bool     BeginDraw( HDC hDC ) ;
	virtual void     EndDraw( );
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight );
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Save( const String& strPath );

protected:
	GDIRenderBitmap*  m_pMemBitmap;
	HBITMAP   m_hOldBitmap; // selectobject的返回值，用于将m_pMemBitmap从m_hDC中选出

	HDC       m_hWndDC;     // m_hWnd对应的HDC，有可能等于m_hOldWndDC，有可能是直接GetDC(m_hWnd)
	HDC       m_hOldWndDC;  // 需要根据m_hOldWndDC是否为空来判断是否需要ReleaseDC(m_hWndDC)

	int       m_nWidth;
	int       m_nHeight;
};
}