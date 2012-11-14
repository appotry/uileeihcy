#pragma once

namespace UI
{
template<class T>
class GdiplusRenderBitmapImpl : public T
{
protected:
	GdiplusRenderBitmapImpl(IRenderBitmap** ppOutRef) : T((IRenderResource**)ppOutRef)
	{
		m_pBitmap = NULL;
		m_pBitmapData = NULL;
	}
public:
	virtual ~GdiplusRenderBitmapImpl()
	{
		SAFE_DELETE(m_pBitmapData);
		SAFE_DELETE(m_pBitmap);

		UI_LOG_DEBUG(_T("GdiplusRenderBitmap Delete. ptr=0x%08X"), this);
	}

	virtual void      SetAttribute( const ATTRMAP& mapAttrib ){};
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }
	Gdiplus::Bitmap*  GetBitmap() { return m_pBitmap; }
	
public:

	//
	//	Remark:
	//		使用Gdiplus加载出来的ICON会丢失透明阴影，在这里对ICON进行了特殊处理
	//		因此在这里先使用GDI DrawIcon获取一次完整的数据
	//		TODO: 这里只默认支持16*16大小的，ico中其它大小的图标暂时没有增加接口分别加载
	//
	virtual bool  LoadFromFile( const String& strPath )
	{
		SAFE_DELETE(m_pBitmap);
		m_pBitmap = Gdiplus::Bitmap::FromFile(strPath.c_str());

		if (NULL == m_pBitmap)
			return false;
		else
			return true;
	}
	virtual bool  Create( int nWidth, int nHeight )
	{
		SAFE_DELETE(m_pBitmap);

		// The default value is PixelFormat32bppARGB
		m_pBitmap = new Gdiplus::Bitmap(nWidth, nHeight);

		if (NULL == m_pBitmap)
			return false;
		else
			return true;
	}

	virtual int   GetWidth()
	{
		if (NULL == m_pBitmap)
			return 0;

		return m_pBitmap->GetWidth();
	}
	virtual int   GetHeight()
	{
		if (NULL == m_pBitmap)
			return 0;

		return m_pBitmap->GetHeight();
	}

	//
	// WARNING: 不支持多线程使用,不支持嵌套使用
	//
	virtual BYTE* LockBits()
	{
		if( NULL == m_pBitmap )
		{
			return NULL;
		}

		if( NULL != m_pBitmapData )
		{
			return NULL;
		}

		m_pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			PixelFormat32bppARGB,
			m_pBitmapData);

		return (BYTE*)m_pBitmapData->Scan0;
	}
	virtual void  UnlockBits()
	{
		if( NULL != m_pBitmapData )
		{
			m_pBitmap->UnlockBits(m_pBitmapData);
			delete m_pBitmapData;
			m_pBitmapData = NULL;
		}
	}

	
	// 将图片数据清0。用于带透明的图片重绘前先刷新上一次的数据，避免alpha叠加
	// 废弃，有直接的Graphic->Clear方法
// 	void    Clear()
// 	{
// 		Gdiplus::BitmapData data;;
// 		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());
// 		m_pBitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data);
// 		
// 		byte* pTemp = (byte*)data.Scan0;
// 		int   bytesperline = abs(data.Stride);
// 		for (int row = 0; row < (int)data.Height; row ++ )
// 		{
// 			memset(pTemp, 0, bytesperline);
// 			pTemp += data.Stride;
// 		}
// 		
// 		m_pBitmap->UnlockBits(&data);
// 	}

	virtual bool  SaveBits( ImageData* pImageData )
	{
		if( NULL == pImageData || NULL == m_pBitmap )
			return false;

		Gdiplus::BitmapData* pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			PixelFormat32bppARGB,
			pBitmapData);

		//////////////////////////////////////////////////////////////////////////

		BYTE* pThisBits = (BYTE*)pBitmapData->Scan0;
		int   bytesperline = abs(pBitmapData->Stride);

		pImageData->m_nWidth = pBitmapData->Width;
		pImageData->m_nHeight = pBitmapData->Height;
		pImageData->m_nStride = pBitmapData->Stride;
		pImageData->m_nbpp = 32;

		// 创建内存
		int nSize = bytesperline*pBitmapData->Height;
		pImageData->m_ptr = new BYTE[nSize];
		pImageData->m_pScan0 = pImageData->m_ptr;

		if( pBitmapData->Stride < 0 )
			pImageData->m_pScan0 += ((pBitmapData->Height-1)*bytesperline);

		// 内存拷贝
		BYTE* pTemp = pImageData->m_pScan0;
		for (int row = 0; row < (int)pBitmapData->Height; row ++ )
		{
			memcpy(pTemp, pThisBits, bytesperline);
			pThisBits += pBitmapData->Stride;
			pTemp += pBitmapData->Stride;
		}


		//////////////////////////////////////////////////////////////////////////

		m_pBitmap->UnlockBits(pBitmapData);
		SAFE_DELETE(pBitmapData);

		return true;
	}
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag )
	{
		if( NULL == pOriginImageData || NULL == m_pBitmap )
			return false;

		Gdiplus::BitmapData* pBitmapData = new Gdiplus::BitmapData;
		Gdiplus::Rect rect(0,0, m_pBitmap->GetWidth(), m_pBitmap->GetHeight());

		int nPixelFormat = PixelFormat32bppARGB;
		if( 24 == pOriginImageData->m_nbpp )
		{
			nPixelFormat = PixelFormat24bppRGB;
		}
		else if( 32 == pOriginImageData->m_nbpp )
		{
			nPixelFormat = PixelFormat32bppARGB;
		}
		else
		{
			UI_LOG_WARN( _T("%s image pixel format not support :%d"), _T(__FUNCTION__), pOriginImageData->m_nbpp );
			return false;
		}
		m_pBitmap->LockBits(
			&rect,
			Gdiplus::ImageLockModeWrite|Gdiplus::ImageLockModeRead,
			nPixelFormat,
			pBitmapData);

		//////////////////////////////////////////////////////////////////////////

		BYTE* pTemp = pOriginImageData->m_pScan0;
		if( NULL == pTemp )
			return false;

		bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
		bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
		bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
		bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;

		if (l == 0)
			bChangeL = false;
		if (s == 0)
			bChangeS = false;

		if(false == bChangeH && false == bChangeL && false == bChangeS)
			return false;

		BYTE* pNewImageBits = (BYTE*)pBitmapData->Scan0;
		int   bytesperline  = abs(pBitmapData->Stride);
		int   bytesperpx    = pOriginImageData->m_nbpp/8;

		float dL = 0, ds = 0;
		if (bChangeL)
			dL = (float)(l/100.0);   // 避免在循环中重复计算该值
		if (bChangeS)
			ds = (float)(s/100.0);

		for (int row = 0; row < (int)pBitmapData->Height; row ++ )
		{
			for( int i = 0; i < bytesperline; i += bytesperpx )
			{
				BYTE B = pTemp[i];
				BYTE G = pTemp[i+1];
				BYTE R = pTemp[i+2];

				if (nPixelFormat == PixelFormat32bppARGB)
					pNewImageBits[i+3] = pTemp[i+3];

				if (bChangeL)
					ChangeColorLuminance(R,G,B,l,dL);

				if (bChangeH && bChangeS)
				{
					ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
				}
				else
				{
					if (bChangeH)
						ChangeColorHue(R,G,B,h,bSetHueMode);
					if (bChangeS)
						ChangeColorSaturation(R,G,B,s,ds);
				}

				pNewImageBits[i]   = B;
				pNewImageBits[i+1] = G;
				pNewImageBits[i+2] = R;
			}

			pNewImageBits += pBitmapData->Stride;
			pTemp += pOriginImageData->m_nStride;
		}

		//////////////////////////////////////////////////////////////////////////

		m_pBitmap->UnlockBits(pBitmapData);
		SAFE_DELETE(pBitmapData);

		return true;
	}

	HBITMAP CopyRect(RECT* prc)
	{
		if (NULL == prc)
			return NULL;

		int nWidth = prc->right - prc->left;
		int nHight = prc->bottom - prc->top;

		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(prc->left ,prc->top, nWidth, nHight);
		m_pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);

		HBITMAP hBitmap = NULL;
		Gdiplus::Color c(0,0,0,0);
		Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(nWidth, nHight, data.Stride, PixelFormat32bppARGB, (BYTE*)data.Scan0);
		pBitmap->GetHBITMAP(c, &hBitmap);
		delete pBitmap;
		m_pBitmap->UnlockBits(&data);

		return hBitmap;
	}

protected:
	Gdiplus::Bitmap*     m_pBitmap;
	Gdiplus::BitmapData* m_pBitmapData;        // LockBits 过程中使用到的临时变量
};


class GdiplusRenderBitmap : public GdiplusRenderBitmapImpl<IRenderBitmap>
{
protected:
	GdiplusRenderBitmap(IRenderBitmap** ppOutRef);
public:
	static  void CreateInstance( IRenderBitmap** pOutRef );

};
class GdiplusIconRenderBitmap : public GdiplusRenderBitmap
{
protected:
	GdiplusIconRenderBitmap(IRenderBitmap** ppOutRef);	
public:
	~GdiplusIconRenderBitmap();
	static  void CreateInstance( IRenderBitmap** pOutRef );
	virtual void SetAttribute( const ATTRMAP& mapAttrib );
	virtual bool  LoadFromFile( const String& strPath );
protected:
	int    m_nIconWidth;
	int    m_nIconHeight;

	HBITMAP  m_hBitmapToFixIcon;   // 解决Gdiplus加载ICON时丢失alpha的问题
};

class GdiplusImageListRenderBitmap : public GdiplusRenderBitmapImpl<IImageListRenderBitmap>
{
protected:
	GdiplusImageListRenderBitmap(IRenderBitmap** ppOutRef);
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
	virtual bool    GetLogFont(LOGFONT* plf);

private:
	Gdiplus::Font*  m_pFont;
	HFONT    m_hFontAttach;
	HFONT    m_hFontForGDI;
};

class GdiplusRenderTarget : public IRenderTarget
{
public:
//	GdiplusRenderTarget();
// 	GdiplusRenderTarget(HDC hDC);
 	GdiplusRenderTarget(HWND hWnd);
	virtual ~GdiplusRenderTarget();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }

	virtual HDC      GetHDC();
	virtual void     ReleaseHDC( HDC hDC );

	virtual HRGN     GetClipRgn() ;
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );

	// 只有GdiplusMemRenderDC才支持
	virtual bool     BeginDraw(HDC hDC, RECT* prc, RECT* prc2=NULL, bool bClear=false);
	virtual void     EndDraw();
//	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish ){};
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){}; 
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Clear(){}
	virtual void     Save( const String& strPath ){};
	virtual HBITMAP  CopyRect(RECT *prc){return NULL;}

	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col );
	virtual void     FillRgn( HRGN hRgn, COLORREF color );
	virtual void     FillRect( const CRect* lprc, COLORREF col);
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap );
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack );
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int x, int y );
	virtual void     DrawBitmap( IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                         int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region );
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy );
	virtual void     DrawBitmap( HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam );

protected:
	HDC      m_hDC;
	Gdiplus::Graphics*  m_pGraphics;
	Gdiplus::Bitmap*    m_pGdiMemBitmap;
};
#if 0
class GdiplusMemRenderDC : public GdiplusRenderTarget
{
public:
	GdiplusMemRenderDC(int nWidth, int nHeight);
	GdiplusMemRenderDC(HWND hWnd, int nWidth, int nHeight);
	virtual ~GdiplusMemRenderDC();

public:
	Gdiplus::Bitmap*  GetMemBitmap();

	virtual bool     BeginDraw( HDC hDC );
	virtual void     EndDraw( );
//	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight );
	virtual BYTE*    LockBits();
	virtual void     UnlockBits();
	virtual void     Clear();
	virtual void     Save( const String& strPath );
	virtual HBITMAP  CopyRect(RECT *prc);

protected:
	GdiplusRenderBitmap*  m_pMemBitmap;    // 双缓冲内存位图
	Gdiplus::Graphics*    m_pWndGraphics;  // 显示用的
	int      m_nWidth;
	int      m_nHeight;
};
#endif
}