#pragma once
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

// msdn url:
// http://msdn.microsoft.com/zh-cn/library/dd370994(v=vs.85).aspx
// http://technet.microsoft.com/zh-tw/library/dd370971.aspx
// http://msdn.microsoft.com/en-us/magazine/ee819134.aspx

namespace UI
{
#if 1
class Direct2DRenderGlobalData
{
public:
	Direct2DRenderGlobalData();
	~Direct2DRenderGlobalData();

	long     AddRef();
	long     Release();

	HRESULT  CreateD2D();

public:
	ID2D1Factory*        m_pD2DFactory;    // 貌似如果不释放它，将一直占用很大内存
	IDWriteFactory*      m_pDWriteFactory;
	IWICImagingFactory*  m_pWICFactory;

private:
	long    m_dwRef;
};
}
extern Direct2DRenderGlobalData  g_D2DGlobalData;
namespace UI
{
class Direct2DRenderBitmap : public IRenderBitmap
{
protected:
	Direct2DRenderBitmap(IRenderBitmap** ppOutRef): IRenderBitmap((IRenderResource**)ppOutRef)
	{
		g_D2DGlobalData.AddRef();

		m_nWidth = m_nHeight = 0;
		m_pConverter = NULL;
	}

public:
	virtual ~Direct2DRenderBitmap()
	{
		UI_LOG_DEBUG(_T("Direct2DRenderBitmap Delete. ptr=0x%08X"), this);

		SAFE_RELEASE(m_pConverter);
		g_D2DGlobalData.Release();
	}


	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_DIRECT2D; }

	static  void CreateInstance( IRenderBitmap** ppOutRef )
	{
		UIASSERT(NULL != ppOutRef);
		if( NULL == ppOutRef )
			return;

		Direct2DRenderBitmap* p = new Direct2DRenderBitmap(ppOutRef);
		*ppOutRef = p;
	}

	//
	// Direct2D 使用 Windows 图像处理组件 (WIC) 来加载位图。若要从文件加载位图，
	// 请首先使用 WIC 对象来加载图像，并将其转换为与 Direct2D 兼容的格式，然后
	// 使用 CreateBitmapFromWicBitmap 方法创建 ID2D1Bitmap。
	//
	// An ID2D1Bitmap is a device-dependent resource: your application should 
	// create bitmaps after it initializes the render target with which the 
	// bitmap will be used, and recreate the bitmap whenever the render target 
	// needs recreated. (For more information about resources, see Resources 
	// Overview.)
	//
	virtual bool  LoadFromFile(const String& strPath, bool bCreateAlphaChannel, const ATTRMAP& mapAttrib)
	{
		if (NULL == g_D2DGlobalData.m_pWICFactory)
			g_D2DGlobalData.CreateD2D();
		if (NULL == g_D2DGlobalData.m_pWICFactory)
			return false;

		//////////////////////////////////////////////////////////////////////////
		// 1.使用 IWICImagingFactory::CreateDecoderFromFileName 方法创建 IWICBitmapDecoder。

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
//		IWICFormatConverter *pConverter = NULL;

		HRESULT hr = g_D2DGlobalData.m_pWICFactory->CreateDecoderFromFilename(
			strPath.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);

		//////////////////////////////////////////////////////////////////////////
		// 2.从图像中检索某一帧并将该帧存储在 IWICBitmapFrameDecode 对象中。

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		//////////////////////////////////////////////////////////////////////////
		// 3.位图必须转换为 Direct2D 可以使用的格式，因此请将图像的像素格式转换为 32bppPBGRA。
		//  （有关支持的格式的列表，请参见像素格式和 Alpha 模式。）调用 IWICImagingFactory::
		//   CreateFormatConverter 方法创建 IWICFormatConverter 对象，然后调用 IWICFormatConverter 
		//   对象的 Initialize 方法执行转换。 

		if (SUCCEEDED(hr))
		{

			// Convert the image format to 32bppPBGRA
			// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
			hr = g_D2DGlobalData.m_pWICFactory->CreateFormatConverter(&m_pConverter);

		}

		if (SUCCEEDED(hr))
		{
			hr = m_pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}

		//////////////////////////////////////////////////////////////////////////
		// 4. 调用 CreateBitmapFromWicBitmap 方法创建 ID2D1Bitmap 对象，该对象可以
		//    通过呈现器目标绘制并与其他 Direct2D 对象一起使用。

// 		ID2D1Bitmap* pBitmap = NULL;
// 		if (SUCCEEDED(hr))
// 		{
// 			// Create a Direct2D bitmap from the WIC bitmap.
// 			hr = pRenderTarget->CreateBitmapFromWicBitmap(
// 				pConverter,
// 				NULL,
// 				&pBitmap
// 				);
// 		}

		pSource->GetSize(&m_nWidth, &m_nHeight);
		SAFE_RELEASE(pDecoder);
		SAFE_RELEASE(pSource);
//		SAFE_RELEASE(pConverter);

		if (SUCCEEDED(hr))
			return true;
		return false;
	}

	virtual bool  Modify(const String& strFilePath, bool bCreateAlphaChannel)
	{
		UIASSERT(0); // TODO:
		return false;
	}
	virtual bool  Create( int nWidth, int nHeight )
	{
		UIASSERT(0); // TODO:
		return false;
	}

	virtual int   GetWidth()
	{
		return m_nWidth;
	}
	virtual int   GetHeight()
	{
		return m_nHeight;
	}

	virtual BYTE* LockBits() {return NULL;}
	virtual void  UnlockBits() {return;}

	virtual bool  SaveBits( ImageData* pImageData ) {return false;}
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l, short s, int nFlag ) {return false;}

	virtual void  SetAttribute( const ATTRMAP& mapAttrib ) {return;}

public:
	ID2D1Bitmap*  GetBitmap(ID2D1RenderTarget* pRenderTarget)
	{
		if (NULL == pRenderTarget || NULL == m_pConverter)
			return NULL;

		map<ID2D1RenderTarget*, ID2D1Bitmap*>::iterator iter = m_mapBitmap.find(pRenderTarget);
		if (iter != m_mapBitmap.end())
			return iter->second;

 		ID2D1Bitmap* pBitmap = NULL;
 		HRESULT	hr = pRenderTarget->CreateBitmapFromWicBitmap(m_pConverter, NULL, &pBitmap);
		if (SUCCEEDED(hr))
		{
			m_mapBitmap[pRenderTarget] = pBitmap;
			return pBitmap;
		}
		else
		{
			UI_LOG_ERROR(_T("%s CreateBitmapFromWicBitmap failed"), FUNC_NAME);
			return NULL;
		}
	}
private:
	IWICFormatConverter*    m_pConverter;
	UINT   m_nWidth;
	UINT   m_nHeight;
	map<ID2D1RenderTarget*, ID2D1Bitmap*>  m_mapBitmap;  // bitmap是一个设备依赖的资源...fuck!
};

class Direct2DRenderFont : public IRenderFont
{
	Direct2DRenderFont(IRenderFont** ppOutRef);

public:
	~Direct2DRenderFont();
	static  void  CreateInstance( IRenderFont** ppOutRef );
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_DIRECT2D; }

public:
	virtual bool  Load( LOGFONT* plogfont );
	virtual bool  ModifyFont(LOGFONT* plogfont);

	virtual void  Attach(HFONT hFont);
	virtual HFONT Detach();

	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 );
	virtual UINT  GetTextMetricsHeight( );

	virtual HFONT GetHFONT();
	virtual bool  GetLogFont(LOGFONT* plf);

public:
	IDWriteTextFormat*  GetFont() { return m_pTextFormat; }
protected:
	void    DestroyFont();

protected:
	IDWriteTextFormat*  m_pTextFormat;
	LOGFONT*            m_pLogFont;   // 记录传递进行的LOGFONT结构体，用于GetLogFont

	HFONT   m_hFont;   // For Attach
	bool    m_bCreateOrAttach;
};

class Direct2DRenderTarget : public IRenderTarget
{
public:
//	Direct2DRenderTarget();
//	Direct2DRenderTarget(HDC hDC);
	Direct2DRenderTarget(HWND hWnd);
//	Direct2DRenderTarget(HWND hWnd, int nWidth, int nHeight);
	virtual ~Direct2DRenderTarget();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() { return GRAPHICS_RENDER_TYPE_DIRECT2D; }

	virtual HRDC     CreateCompatibleHRDC( int nWidth, int nHeight ){return NULL;}
	virtual HDC      GetHDC(){return NULL;}
	virtual void     ReleaseHDC( HDC hDC ){}
	virtual HRGN     GetClipRgn(){return NULL;}
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY ){return 0;}
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint ){return FALSE;}
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ){return FALSE;}
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ){return FALSE;}

	virtual bool     BeginDraw(HDC hDC, RECT* prc, RECT* prc2=NULL, bool bClear=false);
	virtual void     EndDraw();
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ){}
	virtual BYTE*    LockBits() {return NULL;};
	virtual void     UnlockBits(){};
	virtual void     Clear(){};
	virtual void     Save( const String& strPath ){};
	virtual HBITMAP  CopyRect(RECT *prc) {return NULL;}

	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, IRenderFont* pFont, COLORREF col );
	virtual void     FillRgn( HRGN, COLORREF color ){}
	virtual void     FillRect( const CRect* lprc, COLORREF col){}
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap ){}
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack ){}
	virtual void     DrawFocusRect( const CRect* lprc ){}
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo ){}
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo ){}
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop ){}
	virtual void     DrawBitmap(IRenderBitmap* pBitmap, int x, int y){}
	virtual void     DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc){}
	virtual void     DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ){}
	virtual void     DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region ){}
	virtual void     ImageList_Draw(IRenderBitmap* pBitmap, int x, int y, int col, int row, int cx, int cy ){}
	virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam );

protected:
	HWND     m_hWnd;
	ID2D1DCRenderTarget*  m_pRenderTarget;
};

// class Direct2DMemRenderDC : public Direct2DRenderTarget
// {
// public:
// 	Direct2DMemRenderDC(HWND hWnd, int nWidth, int nHeight);
// 	virtual ~Direct2DMemRenderDC();
// };
#endif
}