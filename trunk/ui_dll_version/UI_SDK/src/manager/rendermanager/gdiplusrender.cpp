#include "stdafx.h"

// 由于内存泄露检测机制的new重定义与Gdiplusbase中定义的new有冲突，因此在这里需要屏蔽该功能
// 屏蔽之后，虽然能检测出来内存泄露，但是无法显示代码行所在
#undef new

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


//
//	Remark
//
//		Q: 为什么用Gdiplus在透明窗口上面绘制文字不能用alpha 255?
//		A: (从网上抄的，不一定正确)  alpha值会由于RED值的进位导致255变成0
//

GdiplusRenderBitmap::GdiplusRenderBitmap(IRenderBitmap** ppOutRef) : IRenderBitmap((IRenderResource**)ppOutRef)
{
	m_pBitmap = NULL;
	m_pBitmapData = NULL;
	m_hBitmapToFixIcon = NULL;
}
GdiplusRenderBitmap::~GdiplusRenderBitmap()
{
	SAFE_DELETE(m_pBitmapData);
	SAFE_DELETE(m_pBitmap);
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);

	UI_LOG_DEBUG(_T("GdiplusRenderBitmap Delete. ptr=0x%08X"), this);
}
void GdiplusRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if(NULL == ppOutRef)
		return ;

	GdiplusRenderBitmap* p = new GdiplusRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

int  GdiplusRenderBitmap::GetWidth()
{
	if( NULL == m_pBitmap )
		return 0;

	return m_pBitmap->GetWidth();
}
int  GdiplusRenderBitmap::GetHeight() 
{
	if( NULL == m_pBitmap )
		return 0;

	return m_pBitmap->GetHeight();
}


//
// WARNING: 不支持多线程使用,不支持嵌套使用
//
BYTE* GdiplusRenderBitmap::LockBits()
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
void  GdiplusRenderBitmap::UnlockBits()
{
	if( NULL != m_pBitmapData )
	{
		m_pBitmap->UnlockBits(m_pBitmapData);
		delete m_pBitmapData;
		m_pBitmapData = NULL;
	}
}

bool  GdiplusRenderBitmap::SaveBits( ImageData* pImageData )
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
bool  GdiplusRenderBitmap::ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag )
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
			BYTE R = pTemp[i];
			BYTE G = pTemp[i+1];
			BYTE B = pTemp[i+2];

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

			pNewImageBits[i]   = R;
			pNewImageBits[i+1] = G;
			pNewImageBits[i+2] = B;
		}

		pNewImageBits += pBitmapData->Stride;
		pTemp += pOriginImageData->m_nStride;
	}

	//////////////////////////////////////////////////////////////////////////

	m_pBitmap->UnlockBits(pBitmapData);
	SAFE_DELETE(pBitmapData);

	return true;
}
//
//	Remark:
//		使用Gdiplus加载出来的ICON会丢失透明阴影，在这里对ICON进行了特殊处理
//		因此在这里先使用GDI DrawIcon获取一次完整的数据
//		TODO: 这里只默认支持16*16大小的，ico中其它大小的图标暂时没有增加接口分别加载
//
bool GdiplusRenderBitmap::LoadFromFile( const String& strPath )
{
	SAFE_DELETE(m_pBitmap);
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);

	String strExt = strPath.substr(strPath.length()-4, 4);
	if( 0 == _tcsicmp(strExt.c_str(), _T(".ico")) )
	{
		const int ICON_SIZE = 16;
		HICON hIcon = (HICON)::LoadImage ( NULL, strPath.c_str(), IMAGE_ICON,ICON_SIZE,ICON_SIZE, LR_LOADFROMFILE );
		HDC hMemDC = UI_GetCacheDC();
		
		Image image;
		image.Create( ICON_SIZE, ICON_SIZE, 32, Image::createAlphaChannel );
		HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)image );

		::DrawIconEx( hMemDC, 0,0, hIcon, ICON_SIZE, ICON_SIZE, 0, NULL, DI_NORMAL );
		::SelectObject(hMemDC, hOldBmp);
		::UI_ReleaseCacheDC(hMemDC);
		::DestroyIcon(hIcon);

		m_pBitmap = new Gdiplus::Bitmap(
						ICON_SIZE,
						ICON_SIZE,
						image.GetPitch(),
						PixelFormat32bppARGB,
						(BYTE*)image.GetBits() );
		m_hBitmapToFixIcon = image.Detach();    // Bitmap不负责保存bits数据，因此image.m_hBitmap不能提前释放，需要增加一个成员变量保存该句柄
	}
	else
	{
		m_pBitmap = Gdiplus::Bitmap::FromFile(strPath.c_str());
	}

	if( NULL == m_pBitmap )
		return false;
	else
		return true;
}

bool GdiplusRenderBitmap::Create( int nWidth, int nHeight )
{
	SAFE_DELETE(m_pBitmap);

	m_pBitmap = new Gdiplus::Bitmap(nWidth, nHeight);

	if( NULL == m_pBitmap )
		return false;
	else
		return true;
}
//////////////////////////////////////////////////////////////////////////

GdiplusRenderFont::GdiplusRenderFont(IRenderFont** ppOutRef) : IRenderFont((IRenderResource**)ppOutRef)
{
	m_pFont = NULL;
	m_hFontAttach = NULL;
	m_hFontForGDI = NULL;
}
GdiplusRenderFont::~GdiplusRenderFont()
{
	UI_LOG_DEBUG(_T("GdiplusRenderFont Delete. ptr=0x%08X"), this);

	SAFE_DELETE(m_pFont);
	SAFE_DELETE_GDIOBJECT(m_hFontForGDI);
	m_hFontAttach = NULL;
}
void  GdiplusRenderFont::CreateInstance( IRenderFont** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if(NULL == ppOutRef)
		return ;

	GdiplusRenderFont* p = new GdiplusRenderFont(ppOutRef);
	*ppOutRef = p;
}

bool  GdiplusRenderFont::Load( LOGFONT* plogfont )
{
	if( NULL == plogfont )
		return false;

	SAFE_DELETE(m_pFont);

	INT  nSize = abs(Util::FontHeight2Size(plogfont->lfHeight));
	//Gdiplus::FontStyle style = Gdiplus::FontStyleRegular;   // FontStyle不支持|=，改用int代替
	int  nStyle = Gdiplus::FontStyleRegular;

	if(plogfont->lfWeight >= FW_BOLD)
		nStyle |= (int)Gdiplus::FontStyleBold;
	if(plogfont->lfItalic)
		nStyle |= (int)Gdiplus::FontStyleItalic;
	if(plogfont->lfStrikeOut)
		nStyle |= (int)Gdiplus::FontStyleStrikeout;
	if(plogfont->lfUnderline)
		nStyle |= (int)Gdiplus::FontStyleUnderline;

	m_pFont = new Gdiplus::Font(plogfont->lfFaceName, Gdiplus::REAL(nSize), nStyle); 
	return true;
}
bool  GdiplusRenderFont::ModifyFont(LOGFONT* plogfont)
{
	return this->Load(plogfont);
}

void  GdiplusRenderFont::Attach(HFONT hFont)
{
	if( NULL == hFont )
		return;

	m_hFontAttach = hFont;
	
	LOGFONT lf;
	::GetObject(hFont, sizeof(LOGFONT),&lf);
	this->Load(&lf);
}
HFONT GdiplusRenderFont::Detach()
{
	HFONT hSave = m_hFontAttach;
	m_hFontAttach = NULL;
	SAFE_DELETE(m_pFont);

	return hSave;

}


SIZE GdiplusRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);

	if(  NULL == m_pFont )
		return sizeText;

	HDC hDC = UI_GetCacheDC();
	Gdiplus::Graphics  g(hDC);

	if( -1 != nLimitWidth )
	{
		Gdiplus::RectF  layoutRect((Gdiplus::REAL)0,(Gdiplus::REAL)0, (Gdiplus::REAL)nLimitWidth, (Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, layoutRect, &boundingBox );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}
	else
	{
		Gdiplus::PointF origin((Gdiplus::REAL)0,(Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, origin, &boundingBox );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}

	UI_ReleaseCacheDC(hDC);
	return sizeText;
}


UINT GdiplusRenderFont::GetTextMetricsHeight( )
{
	if( NULL == m_pFont )
		return 0;

	HDC hDC = ::UI_GetCacheDC();
	Gdiplus::Graphics g(hDC);
	UINT nRet = (UINT)m_pFont->GetHeight(&g);
	::UI_ReleaseCacheDC(hDC);

	return nRet;
}


HFONT GdiplusRenderFont::GetHFONT()
{
	HDC hCacheDC = ::UI_GetCacheDC();
	Gdiplus::Graphics g(hCacheDC);

	HFONT hFont = m_hFontAttach;
	if( NULL == hFont )  
	{
		if( NULL == m_hFontForGDI )
		{
			LOGFONT lf;
	#ifdef _UNICODE
			m_pFont->GetLogFontW(&g,&lf);	
	#else
			m_pFont->GetLogFontA(&g,&lf);
	#endif
			m_hFontForGDI = ::CreateFontIndirect(&lf);  // 析构时，释放
		}
		hFont = m_hFontForGDI;
	}
	::UI_ReleaseCacheDC(hCacheDC);
	return hFont;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//		GdiplusRenderDC
//
GdiplusRenderDC::GdiplusRenderDC()
{
	m_pGraphics = NULL;
	this->Init();
}

GdiplusRenderDC::GdiplusRenderDC(HDC hDC):IRenderDC(hDC)
{
	m_pGraphics = Gdiplus::Graphics::FromHDC(hDC);
	UIASSERT(NULL != m_pGraphics);

	this->Init();
}
GdiplusRenderDC::GdiplusRenderDC(HWND hWnd):IRenderDC(hWnd)
{
	m_pGraphics = Gdiplus::Graphics::FromHWND(hWnd);
	UIASSERT(NULL != m_pGraphics);

	this->Init();
}
GdiplusRenderDC::~GdiplusRenderDC()
{
	if( NULL != m_pGraphics )
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
//	m_pFont = NULL;
// 	if( NULL != m_hFont_GetHDC )
// 	{
// 		::DeleteObject(m_hFont_GetHDC);
// 		m_hFont_GetHDC = NULL;
// 	}
}


void GdiplusRenderDC::Init()
{
//	m_colorText.SetValue(Gdiplus::Color::MakeARGB(254,0,0,0));

//	m_pFont = NULL;
//	m_hFont_GetHDC = NULL;
}

HRDC GdiplusRenderDC::CreateCompatibleHRDC( int nWidth, int nHeight )
{
	return (HRDC)new GdiplusMemRenderDC( nWidth, nHeight );
}

//
//	Remark
//		调用Graphics的GetHDC，获取到的HDC不会继续Graphics的任何属性，因此需要我们自己再设置一次属性
//		由于FONT、text color属性移到DrawString中当场设置，因此这两个属性没有设置上
//
//		MSDN:
//			Any state changes you make to the device context between GetHDC and ReleaseHDC will 
//			be ignored by GDI+ and will not be reflected in rendering done by GDI+.
//
HDC GdiplusRenderDC::GetHDC()
{
	if( NULL == m_pGraphics )
		return NULL;

	POINT  p = {0,0};
	this->GetViewportOrgEx(&p);

	Gdiplus::Region r;
	m_pGraphics->GetClip(&r);
	HRGN hRgn = r.GetHRGN(m_pGraphics);  // 必须在GetHDC前面获取，否则graphics将处于busy状态

	HDC hDC = m_pGraphics->GetHDC();

	::SetViewportOrgEx( hDC, p.x, p.y, NULL );
	::SelectClipRgn(hDC, hRgn );
	::DeleteObject(hRgn);

//	::SetTextColor( hDC, m_colorText.ToCOLORREF() );
//
// 	if( NULL != m_pFont )
// 	{
// 		Gdiplus::Font* pGdiplusFont = m_pFont->GetFont();
// 		if( NULL != pGdiplusFont )
// 		{
// 			LOGFONT lf;
// 	#ifdef _UNICODE
// 			pGdiplusFont->GetLogFontW(m_pGraphics,&lf);	
// 	#else
// 			pGdiplusFont->GetLogFontA(m_pGraphics,&lf);
// 	#endif
// 			
// 			if( NULL != m_hFont_GetHDC )
// 			{
// 				::DeleteObject(m_hFont_GetHDC);
// 				m_hFont_GetHDC = NULL;
// 			}
// 			m_hFont_GetHDC = ::CreateFontIndirect(&lf);
// 			::SelectObject( hDC, m_hFont_GetHDC );
//		}
//	}
	return hDC;
}
void GdiplusRenderDC::ReleaseHDC( HDC hDC )
{
	m_pGraphics->ReleaseHDC(hDC);
// 	if( NULL != m_hFont_GetHDC )
// 	{
// 		::DeleteObject(m_hFont_GetHDC);
// 		m_hFont_GetHDC = NULL;
// 	}
}
// HRFONT GdiplusRenderDC::SelectFont( HRFONT hRFont ) 
// {
// 	if( NULL != hRFont )
// 	{
// 		if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
// 		{
// 			hRFont = NULL;
// 		}
// 	}
// 
// 	HRFONT hOldFont = m_pFont;
// 	m_pFont = (GdiplusRenderFont*)hRFont;
// 	return hOldFont;
// }
// HRFONT GdiplusRenderDC::GetFont()
// {
// 	return m_pFont;
// }
HRGN GdiplusRenderDC::GetClipRgn()
{
	Gdiplus::Region region;
	m_pGraphics->GetClip(&region);

	return region.GetHRGN(m_pGraphics);
}
int GdiplusRenderDC::SelectClipRgn( HRGN hRgn, int nMode )
{
	if( NULL == hRgn )
	{
		Gdiplus::Status s = m_pGraphics->ResetClip();
		return (int)s;
	}

// 	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);   // 注意：在这里千万不要使用Region对象进行setclip，否则会导致当前的clip跟随vieworg而偏移
// 	if( NULL == pRegion )
// 		return ERROR;

	Gdiplus::CombineMode mode = Gdiplus::CombineModeReplace;
	switch( nMode )
	{
	case RGN_AND:
		mode = Gdiplus::CombineModeIntersect;
		break;

	case RGN_COPY:
		mode = Gdiplus::CombineModeReplace;
		break;

	case RGN_DIFF:
		mode = Gdiplus::CombineModeExclude; // CombineModeComplement
		break;

	case RGN_OR:
		mode = Gdiplus::CombineModeUnion;
		break;

	case RGN_XOR:
		mode = Gdiplus::CombineModeXor;
		break;
	}

	//Gdiplus::Status s = m_pGraphics->SetClip(pRegion, mode);
	Gdiplus::Status s = m_pGraphics->SetClip(hRgn,mode);

//	delete pRegion;
//	pRegion = NULL;

	return (int)s;
}
BOOL GdiplusRenderDC::GetViewportOrgEx( LPPOINT lpPoint )
{
	if( NULL == lpPoint )
	{
		return FALSE;
	}

	Gdiplus::Matrix  m;
	m_pGraphics->GetTransform(&m);

	lpPoint->x = (int)m.OffsetX();
	lpPoint->y = (int)m.OffsetY();

	return TRUE;
}
BOOL GdiplusRenderDC::OffsetViewportOrgEx( int xOffset, int yOffset, LPPOINT lpPoint ) 
{
	if( NULL != lpPoint )
	{
		Gdiplus::Matrix  m;
		m_pGraphics->GetTransform(&m);

		lpPoint->x = (int)m.OffsetX();
		lpPoint->y = (int)m.OffsetY();
	}
	if( Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)xOffset,(Gdiplus::REAL)yOffset) )
		return TRUE;
	else
		return FALSE;
}
BOOL GdiplusRenderDC::SetViewportOrgEx( int x, int y, LPPOINT lpPoint )
{
	if( NULL != lpPoint )
	{
		Gdiplus::Matrix  m;
		m_pGraphics->GetTransform(&m);
		lpPoint->x = (int)m.OffsetX();
		lpPoint->y = (int)m.OffsetY();
	}

	m_pGraphics->ResetTransform();
	if( Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)x,(Gdiplus::REAL)y) )
		return TRUE;
	else
		return FALSE;
}


// COLORREF GdiplusRenderDC::SetTextColor( COLORREF color, byte Alpha )
// {
// 	Gdiplus::Color old = m_colorText;
// 	m_colorText.SetValue(Gdiplus::Color::MakeARGB(Alpha,GetRValue(color),GetGValue(color),GetBValue(color))) ;
// 	return old.ToCOLORREF();
// }
// COLORREF GdiplusRenderDC::GetTextColor( )
// {
// 	return m_colorText.ToCOLORREF();
// }


int GdiplusRenderDC::DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == m_pGraphics )
		return 0;

	if( NULL == hRFont )
	{
		UI_LOG_WARN(_T("GdiplusRenderDC::DrawString hRFont == NULL"));
		return -1;
	}

	if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
	{
		UI_LOG_WARN(_T("GdiplusRenderDC::DrawString hRFont render type != GRAPHICS_RENDER_TYPE_GDIPLUS"));
		return -1;
	}

	Gdiplus::Font* pFont = ((GdiplusRenderFont*)hRFont)->GetFont();
	if( NULL == pFont )
		return -1;

	Gdiplus::RectF region(
		(Gdiplus::REAL)(lpRect->left),
		(Gdiplus::REAL)(lpRect->top), 
		(Gdiplus::REAL)(lpRect->Width()), 
		(Gdiplus::REAL)(lpRect->Height()) );

	Gdiplus::Color colText;
	colText.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col), GetGValue(col), GetBValue(col)));
	Gdiplus::SolidBrush textBrush(colText);

	Gdiplus::StringFormat format;
	if( nFormat & DT_CENTER )
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if( nFormat & DT_RIGHT )
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if( nFormat & DT_VCENTER )
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if( nFormat & DT_BOTTOM )
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentFar);
	}

	// Draw string.
	Gdiplus::Status s = m_pGraphics->DrawString(
		szText,
		-1/*_tcslen(szText)*/,
		pFont,
		region,
		&format,
		&textBrush);

	return 0;
}

void GdiplusRenderDC::FillRgn( HRGN hRgn, COLORREF col )
{
	if( NULL == hRgn )
		return;

	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);
	if( NULL == pRegion )
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col),GetGValue(col),GetBValue(col))) ;
	Gdiplus::SolidBrush brush(color);

	m_pGraphics->FillRegion(&brush, pRegion);

	delete pRegion;
	pRegion = NULL;
}

void GdiplusRenderDC::FillRect(const CRect* lprc, COLORREF col)
{
	if( NULL == lprc )
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col),GetGValue(col),GetBValue(col))) ;
	Gdiplus::SolidBrush brush(color);

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderDC::TileRect( const CRect* lprc, HRBITMAP hBitmap )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if( NULL == pRenderBitmap )
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if( NULL == pBitmap )
		return;

	Gdiplus::TextureBrush brush(pBitmap);
	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderDC::Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
{
	if( NULL == m_pGraphics )
		return;

	if( false == bNullBack )
	{
		this->FillRect(lprc,colBack);
	}

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colBorder),GetGValue(colBorder),GetBValue(colBorder))) ;
	Gdiplus::Pen pen(color, (Gdiplus::REAL)nBorder);

	m_pGraphics->DrawRectangle(&pen,   
		lprc->left, lprc->top, 
		lprc->Width()-1, lprc->Height()-1   // Remark:在这里如果不减1的话，会导致右、下的边框显示不全，原因未知
		);  

}

void GdiplusRenderDC::DrawFocusRect( const CRect* lprc )
{
	if( NULL == m_pGraphics )
		return;

	Gdiplus::Pen p( Gdiplus::Color(254,0,0,0) );
	p.SetDashStyle( Gdiplus::DashStyleDot );
	m_pGraphics->DrawRectangle(&p,   
		lprc->left, lprc->top, 
		lprc->Width()-1, lprc->Height()-1   // Remark:在这里如果不减1的话，会导致右、下的边框显示不全，原因未知
		);  
}

void GdiplusRenderDC::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height() );
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeHorizontal );

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderDC::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height() );
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeVertical  );

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderDC::BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderDC* pSrcHDC, int xSrc, int ySrc, DWORD dwRop )
{
	if( NULL == pSrcHDC )
		return;
	if( pSrcHDC->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusMemRenderDC* pMemRDC = (GdiplusMemRenderDC*)pSrcHDC;
	m_pGraphics->DrawImage(pMemRDC->GetMemBitmap(), xDest,yDest,xSrc,ySrc, wDest,hDest, Gdiplus::UnitPixel );
}

void GdiplusRenderDC::DrawBitmap( HRBITMAP hBitmap, int x, int y)
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if( NULL == pRenderBitmap )
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if( NULL == pBitmap )
		return;

	m_pGraphics->DrawImage( pBitmap,x, y, pBitmap->GetWidth(), pBitmap->GetHeight() );
}

void GdiplusRenderDC::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if( NULL == pRenderBitmap )
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if( NULL == pBitmap )
		return;

	Gdiplus::RectF rcDst( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, (Gdiplus::REAL)nDestWidth, (Gdiplus::REAL)nDestHeight );
	m_pGraphics->DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)nSrcWidth, (Gdiplus::REAL)nSrcHeight, Gdiplus::UnitPixel );
}

void GdiplusRenderDC::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					Image9Region* pImage9Region )
{
	if( NULL == hBitmap )
		return;

	if( NULL == pImage9Region )
		return this->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if( NULL == pRenderBitmap )
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if( NULL == pBitmap )
		return;


	// 1. topleft
	Gdiplus::RectF rcDst1( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)pImage9Region->topleft, (Gdiplus::REAL)pImage9Region->top );
	m_pGraphics->DrawImage( pBitmap, rcDst1, 

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)ySrc,
		(Gdiplus::REAL)pImage9Region->topleft,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel
		);

	// 2. top
	Gdiplus::RectF rcDst2( (Gdiplus::REAL)(xDest + pImage9Region->topleft), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->topleft - pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top );
	m_pGraphics->DrawImage( pBitmap, rcDst2,

		(Gdiplus::REAL)(xSrc + pImage9Region->topleft),                                         // xSrc
		(Gdiplus::REAL)ySrc,                                                               // ySrc
		(Gdiplus::REAL)(nSrcWidth - pImage9Region->topleft - pImage9Region->topright),       // nSrcWidth
		(Gdiplus::REAL)pImage9Region->top ,

		Gdiplus::UnitPixel
		);                                                 

	// 3. topright
	Gdiplus::RectF rcDst3( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->topright), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top );
	m_pGraphics->DrawImage( pBitmap, rcDst3,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->topright),
		(Gdiplus::REAL)ySrc, 
		(Gdiplus::REAL)pImage9Region->topright,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel
		);

	// 4. left
	Gdiplus::RectF rcDst4( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->left), (Gdiplus::REAL)(nDestHeight - pImage9Region->top- pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst4,

		(Gdiplus::REAL)xSrc ,                                                        // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)pImage9Region->left,                                         // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),      // nSrcHeight

		Gdiplus::UnitPixel
		);

	// 5. center

	Gdiplus::RectF rcDst5( (Gdiplus::REAL)(xDest + pImage9Region->left), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(nDestWidth  - pImage9Region->left - pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top  - pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst5,

		(Gdiplus::REAL)(xSrc + pImage9Region->left),                                         // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)(nSrcWidth  - pImage9Region->left - pImage9Region->right),           // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top  - pImage9Region->bottom),       // nSrcHeight

		Gdiplus::UnitPixel
		); 

	// 6. right
	Gdiplus::RectF rcDst6( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->right), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top - pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst6,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->right),                            // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)(pImage9Region->right),                                        // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),        // nSrcHeight

		Gdiplus::UnitPixel
		);

	// 7. bottomleft
	Gdiplus::RectF rcDst7( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomleft), (Gdiplus::REAL)(pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst7,

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom),
		(Gdiplus::REAL)pImage9Region->bottomleft, 
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel
		);

	// 8. bottom
	Gdiplus::RectF rcDst8( (Gdiplus::REAL)(xDest + pImage9Region->bottomleft), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->bottomleft - pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst8,

		(Gdiplus::REAL)xSrc + pImage9Region->bottomleft,                                   // xSrc
		(Gdiplus::REAL)ySrc + nSrcHeight - pImage9Region->bottom,                          // ySrc
		(Gdiplus::REAL)nSrcWidth - pImage9Region->bottomleft - pImage9Region->bottomright, // nSrcWidth
		(Gdiplus::REAL)pImage9Region->bottom,                                         // nSrcHeight

		Gdiplus::UnitPixel
		);

	// 9. bottomright
	Gdiplus::RectF rcDst9( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->bottomright), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom) );
	m_pGraphics->DrawImage( pBitmap, rcDst9,

		(Gdiplus::REAL)(xSrc + nSrcWidth  - pImage9Region->bottomright),
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom), 
		(Gdiplus::REAL)pImage9Region->bottomright,
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel
		);
}
void GdiplusRenderDC::ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if( NULL == pRenderBitmap )
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if( NULL == pBitmap )
		return;


	int xSrc = col * cx;
	int ySrc = row * cy;

// 	if( xSrc > pBitmap->GetWidth() )
// 		return false;
// 	if( ySrc > pBitmap->GetHeight() )
// 		return false;

	//	m_pGraphics->DrawImage(pBitmap, x,y,xSrc,ySrc, cx,cy, Gdiplus::UnitPixel );  <-- 使用这个版本会导致图片被放大...原因未知
	Gdiplus::RectF rcDst( (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy );
	m_pGraphics->DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy, Gdiplus::UnitPixel );

}

//////////////////////////////////////////////////////////////////////////
//


GdiplusMemRenderDC::GdiplusMemRenderDC(int nWidth, int nHeight)
{
	GdiplusRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, nHeight);
	m_pGraphics = new Gdiplus::Graphics(m_pMemBitmap->GetBitmap());
}
GdiplusMemRenderDC::GdiplusMemRenderDC(HWND hWnd, int nWidth, int nHeight)
{
	m_hWnd = hWnd;
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	GdiplusRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, nHeight);
	m_pGraphics = NULL;
	m_pWndGraphics = NULL;
}
GdiplusMemRenderDC::~GdiplusMemRenderDC()
{
	if( NULL != m_pMemBitmap )
	{
		delete m_pMemBitmap;
		m_pMemBitmap = NULL;
	}
}

Gdiplus::Bitmap*  GdiplusMemRenderDC::GetMemBitmap() 
{ 
	if( NULL == m_pMemBitmap )
		return NULL;

	return m_pMemBitmap->GetBitmap(); 
}

bool GdiplusMemRenderDC::BeginDraw( HDC hDC )
{
	if (NULL != m_pGraphics)
	{
		UI_LOG_WARN(_T("%s pre invoke has not been finish."),FUNC_NAME);
		return false;
	}

	if (NULL == m_pMemBitmap)
	{
		UI_LOG_WARN(_T("GdiplusMemRenderDC::BeginDraw NULL == m_pMemBitmap"));
		return false;
	}

	if( NULL != m_pWndGraphics )
	{
		delete m_pWndGraphics;
		m_pWndGraphics = NULL;
	}
	if( NULL != m_pGraphics )
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}

	m_pGraphics = new Gdiplus::Graphics(m_pMemBitmap->GetBitmap());
	if( NULL != hDC )
	{
		m_pWndGraphics = new Gdiplus::Graphics(hDC);
	}
	else
	{
		m_pWndGraphics = Gdiplus::Graphics::FromHWND(m_hWnd);
	}
	return true;
}

void GdiplusMemRenderDC::EndDraw( )
{
	if( NULL == m_pMemBitmap || NULL == m_pWndGraphics || NULL == m_pGraphics )
		return;

	m_pWndGraphics->DrawImage(m_pMemBitmap->GetBitmap(),0,0,0,0,m_nWidth,m_nHeight,Gdiplus::UnitPixel );

	delete m_pWndGraphics;
	delete m_pGraphics;
	m_pGraphics = NULL;
	m_pWndGraphics = NULL;
}

void GdiplusMemRenderDC::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc )
{
	if( NULL == m_pMemBitmap )
		return;

	m_pWndGraphics->DrawImage(m_pMemBitmap->GetBitmap(),xDest,yDest,xSrc,ySrc,wDest,hDest,Gdiplus::UnitPixel );

	delete m_pWndGraphics;
	delete m_pGraphics;
	m_pGraphics = NULL;
	m_pWndGraphics = NULL;
}
void GdiplusMemRenderDC::ResizeRenderTarget( int nWidth, int nHeight )
{
	if( m_nWidth == nWidth && m_nHeight == nHeight )
		return;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	bool bNeedToCreateGraphics = false;
	if( NULL != m_pGraphics )
	{
		bNeedToCreateGraphics = true;
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
	if( NULL != m_pMemBitmap )
	{
		delete m_pMemBitmap;
		m_pMemBitmap = NULL;
	}
	
	GdiplusRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap); 
	m_pMemBitmap->Create(nWidth, nHeight);

	if( bNeedToCreateGraphics )
	{
		m_pGraphics = new Gdiplus::Graphics(m_pMemBitmap->GetBitmap());
	}
}
BYTE* GdiplusMemRenderDC::LockBits()
{
	if( NULL == m_pMemBitmap )
		return NULL;

	return m_pMemBitmap->LockBits();
}
void GdiplusMemRenderDC::UnlockBits()
{
	if( NULL == m_pMemBitmap )
		return ;

	return m_pMemBitmap->UnlockBits();
}





int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
void GdiplusMemRenderDC::Save( const String& strPath )
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);

	if( NULL != this->GetMemBitmap() )
	{
		this->GetMemBitmap()->Save(strPath.c_str(),&pngClsid, NULL);
	}
}