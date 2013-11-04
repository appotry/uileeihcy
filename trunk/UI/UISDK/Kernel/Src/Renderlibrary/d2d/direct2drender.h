#ifndef DIRECT2DRENDER_H_C734C4CB_CCC1_464b_8C07_AB893622714C
#define DIRECT2DRENDER_H_C734C4CB_CCC1_464b_8C07_AB893622714C
#ifdef UI_D2D_RENDER

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
	ID2D1Factory*        m_pD2DFactory;    // ò��������ͷ�������һֱռ�úܴ��ڴ�
	IDWriteFactory*      m_pDWriteFactory;
	IWICImagingFactory*  m_pWICFactory;

private:
	long    m_dwRef;
};
}
extern Direct2DRenderGlobalData  g_D2DGlobalData;
namespace UI
{
class Direct2DRenderBitmap : public IRenderResourceImpl<IRenderBitmap>
{
protected:
	Direct2DRenderBitmap()
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


	virtual GRAPHICS_RENDER_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }

	static  void CreateInstance( IRenderBitmap** ppOutRef )
	{
		UIASSERT(NULL != ppOutRef);
		if (NULL == ppOutRef)
			return;

		Direct2DRenderBitmap* p = new Direct2DRenderBitmap();
		*ppOutRef = p;
	}

	//
	// Direct2D ʹ�� Windows ͼ������� (WIC) ������λͼ����Ҫ���ļ�����λͼ��
	// ������ʹ�� WIC ����������ͼ�񣬲�����ת��Ϊ�� Direct2D ���ݵĸ�ʽ��Ȼ��
	// ʹ�� CreateBitmapFromWicBitmap �������� ID2D1Bitmap��
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
		// 1.ʹ�� IWICImagingFactory::CreateDecoderFromFileName �������� IWICBitmapDecoder��

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
		// 2.��ͼ���м���ĳһ֡������֡�洢�� IWICBitmapFrameDecode �����С�

		if (SUCCEEDED(hr))
		{
			// Create the initial frame.
			hr = pDecoder->GetFrame(0, &pSource);
		}

		//////////////////////////////////////////////////////////////////////////
		// 3.λͼ����ת��Ϊ Direct2D ����ʹ�õĸ�ʽ������뽫ͼ������ظ�ʽת��Ϊ 32bppPBGRA��
		//  ���й�֧�ֵĸ�ʽ���б���μ����ظ�ʽ�� Alpha ģʽ�������� IWICImagingFactory::
		//   CreateFormatConverter �������� IWICFormatConverter ����Ȼ����� IWICFormatConverter 
		//   ����� Initialize ����ִ��ת���� 

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
		// 4. ���� CreateBitmapFromWicBitmap �������� ID2D1Bitmap ���󣬸ö������
		//    ͨ��������Ŀ����Ʋ������� Direct2D ����һ��ʹ�á�

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
	map<ID2D1RenderTarget*, ID2D1Bitmap*>  m_mapBitmap;  // bitmap��һ���豸��������Դ...fuck!
};

class Direct2DRenderFont : public IRenderResourceImpl<IRenderFont>
{
	Direct2DRenderFont();

public:
	~Direct2DRenderFont();
	static  void  CreateInstance(IRenderFont** ppOutRef);
	virtual GRAPHICS_RENDER_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }

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
	LOGFONT*            m_pLogFont;   // ��¼���ݽ��е�LOGFONT�ṹ�壬����GetLogFont

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
	virtual GRAPHICS_RENDER_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }

	virtual HDC      GetHDC(){return NULL;}
	virtual void     ReleaseHDC( HDC hDC ){}
	virtual HRGN     GetClipRgn(){return NULL;}
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY ){return 0;}
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint ){return FALSE;}
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ){return FALSE;}
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ){return FALSE;}

	virtual bool     BeginDraw(HDC hDC, RECT* prcArray, int rcCount, bool bClear=false);
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
	virtual void     TileRect( const CRect* lprc, IRenderBitmap* hBitmap ){}
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack ){}
	virtual void     DrawFocusRect( const CRect* lprc ){}
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo ){}
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo ){}
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop ){}
	virtual void     ImageList_Draw(IRenderBitmap* pBitmap, int x, int y, int col, int row, int cx, int cy ){}
	virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam );

protected:
	void    DrawBitmap(IRenderBitmap* pBitmap, int x, int y){}
	void    DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc){}
	void    DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int nDestWidth, 
			           int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ){}
	void    DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int nDestWidth, 
					   int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					   Image9Region* p9Region ){}

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

#endif

#endif  // DIRECT2DRENDER_H_C734C4CB_CCC1_464b_8C07_AB893622714C