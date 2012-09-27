#pragma once

namespace UI
{

template<class T>
class GDIRenderBitmapImpl : public T
{
protected:
	GDIRenderBitmapImpl(IRenderBitmap** ppOutRef): T((IRenderResource**)ppOutRef)
	{

	}
public:
	virtual ~GDIRenderBitmapImpl()
	{
		UI_LOG_DEBUG(_T("GDIRenderBitmap Delete. ptr=0x%08X"), this);
	}

	virtual void SetAttribute( const ATTRMAP& mapAttrib ){};
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDI; }

	Image*  GetBitmap() { return &m_image; }

public:
	virtual bool  LoadFromFile( const String& strPath )
	{
		if( ! m_image.IsNull() )
		{
			m_image.Destroy();
		}

		String strExt = strPath.substr(strPath.length()-4, 4);
		if( 0 == _tcsicmp(strExt.c_str(), _T(".ico")) )
		{
			const int ICON_SIZE = 16;
			HICON hIcon = (HICON)::LoadImage ( NULL, strPath.c_str(), IMAGE_ICON,ICON_SIZE,ICON_SIZE, LR_LOADFROMFILE );
			HDC hMemDC = UI_GetCacheDC();

			m_image.Create( ICON_SIZE, ICON_SIZE, 32, Image::createAlphaChannel );
			HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)m_image );

			::DrawIconEx( hMemDC, 0,0, hIcon, ICON_SIZE, ICON_SIZE, 0, NULL, DI_NORMAL );
			::SelectObject(hMemDC, hOldBmp);
			::UI_ReleaseCacheDC(hMemDC);
			::DestroyIcon(hIcon);
		}
		else
		{
			m_image.Load( strPath.c_str() );
		}

		if( m_image.IsNull() )
			return false;
		else
			return true;
	}
	virtual bool  Create(int nWidth, int nHeight)
	{
		UIASSERT(nHeight < 0);   // 使用反向的
		if( ! m_image.IsNull() )
		{
			m_image.Destroy();
		}
		m_image.Create(nWidth, nHeight, 32, Image::createAlphaChannel );

		if( m_image.IsNull() )
			return false;
		else
			return true;
	}

	virtual int   GetWidth()
	{
		return m_image.GetWidth();
	}
	virtual int   GetHeight()
	{
		return m_image.GetHeight();
	}

	virtual BYTE* LockBits()
	{
		return (BYTE*)m_image.GetBits();
	}
	virtual void  UnlockBits()
	{
		// Nothing.
	}

	virtual bool  SaveBits( ImageData* pImageData )
	{
		return m_image.SaveBits(pImageData);
	}
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag )
	{
		return m_image.ChangeHLS(pOriginImageData, h, l, s, nFlag );
	}

protected:
	Image   m_image;
};

class GDIRenderBitmap : public GDIRenderBitmapImpl<IRenderBitmap>
{
protected:
	GDIRenderBitmap(IRenderBitmap** ppOutRef);
public:
	static  void CreateInstance( IRenderBitmap** pOutRef );

};
class GDIIconRenderBitmap : public GDIRenderBitmap
{
protected:
	GDIIconRenderBitmap(IRenderBitmap** ppOutRef);	
public:
	static  void CreateInstance( IRenderBitmap** pOutRef );
	virtual void SetAttribute( const ATTRMAP& mapAttrib );
	virtual bool  LoadFromFile( const String& strPath );
private:
	int    m_nIconWidth;
	int    m_nIconHeight;
};

class GDIImageListRenderBitmap : public GDIRenderBitmapImpl<IImageListRenderBitmap>
{
protected:
	GDIImageListRenderBitmap(IRenderBitmap** ppOutRef);
public:
	static  void CreateInstance(IRenderBitmap** pOutRef );
	virtual void SetAttribute( const ATTRMAP& mapAttrib );

	virtual int  GetItemWidth();
	virtual int  GetItemHeight();
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType();
	virtual bool GetIndexPos(int nIndex, POINT* pPoint);

private:
	IMAGELIST_LAYOUT_TYPE   m_eLayout;
	int     m_nCount;
};

//////////////////////////////////////////////////////////////////////////

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
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish ){}
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
	virtual void     DrawBitmap( IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                         int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region );
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy );
	virtual void     DrawBitmap( HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam );

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
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
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