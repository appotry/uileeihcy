#include "stdafx.h"

// 由于内存泄露检测机制的new重定义与Gdiplusbase中定义的new有冲突，因此在这里需要屏蔽该功能
// 屏蔽之后，虽然能检测出来内存泄露，但是无法显示代码行所在
#undef new

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


//
//	Remark
//
//		Q: 为什么用Gdiplus在透明窗口上面绘制文字不能用alpha 255?
//		A: (从网上抄的，不一定正确)  alpha值会由于Red值的进位导致255变成0
//

#pragma  region // Gdiplus bitmap 
GdiplusRenderBitmap::GdiplusRenderBitmap(IRenderBitmap** ppOutRef) : GdiplusRenderBitmapImpl<IRenderBitmap>(ppOutRef)
{
}

void GdiplusRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if(NULL == ppOutRef)
		return ;

	GdiplusRenderBitmap* p = new GdiplusRenderBitmap(ppOutRef);
	*ppOutRef = p;
}


GdiplusImageListRenderBitmap::GdiplusImageListRenderBitmap(IRenderBitmap** ppOutRef) : GdiplusRenderBitmapImpl<IImageListRenderBitmap>(ppOutRef)
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
void GdiplusImageListRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusImageListRenderBitmap* p = new GdiplusImageListRenderBitmap(ppOutRef);
	*ppOutRef = p;
}
void GdiplusImageListRenderBitmap::SetAttribute( const ATTRMAP& mapAttrib )
{
	__super::SetAttribute(mapAttrib);

	ATTRMAP::const_iterator iter = mapAttrib.find(XML_IMAGE_IMAGELIST_COUNT);
	if (mapAttrib.end() != iter)
	{
		m_nCount = _ttoi(iter->second.c_str());
	}

	iter = mapAttrib.find(XML_IMAGE_IMAGELIST_LAYOUT);
	if (mapAttrib.end() != iter)
	{
		const String& str = iter->second;
		if (XML_IMAGE_IMAGELIST_LAYOUT_H == str)
		{
			m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
		}
		else if (XML_IMAGE_IMAGELIST_LAYOUT_V == str)
		{
			m_eLayout = IMAGELIST_LAYOUT_TYPE_V;
		}
	}
}
int GdiplusImageListRenderBitmap::GetItemWidth()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_V:
		return GetWidth();

	case IMAGELIST_LAYOUT_TYPE_H:
		return GetWidth()/m_nCount;
	}

	return 0;
}
int GdiplusImageListRenderBitmap::GetItemHeight()
{
	if (0 == m_nCount)
		return 0;

	switch(m_eLayout)
	{
	case IMAGELIST_LAYOUT_TYPE_H:
		return GetHeight();

	case IMAGELIST_LAYOUT_TYPE_V:
		return GetHeight()/m_nCount;
	}

	return 0;
}
IMAGELIST_LAYOUT_TYPE GdiplusImageListRenderBitmap::GetLayoutType()
{
	return m_eLayout;
}
bool GdiplusImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
{
	if (NULL == pPoint)
		return false;

	pPoint->x = pPoint->y = 0;
	if (nIndex > m_nCount)
		return false;

	if (IMAGELIST_LAYOUT_TYPE_H == m_eLayout)
	{
		pPoint->x = nIndex*GetItemWidth();
		pPoint->y = 0;
	}
	else if (IMAGELIST_LAYOUT_TYPE_V == m_eLayout)
	{
		pPoint->x = 0;
		pPoint->y = nIndex*GetItemHeight();
	}
	else 
		return false;

	return true;
}
GdiplusIconRenderBitmap::GdiplusIconRenderBitmap(IRenderBitmap** ppOutRef) : GdiplusRenderBitmap(ppOutRef)
{
	m_nIconWidth = m_nIconHeight = 16;
	m_hBitmapToFixIcon = NULL;
}
GdiplusIconRenderBitmap::~GdiplusIconRenderBitmap()
{
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);
}
void GdiplusIconRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusIconRenderBitmap* p = new GdiplusIconRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

void GdiplusIconRenderBitmap::SetAttribute( const ATTRMAP& mapAttrib )
{
	__super::SetAttribute(mapAttrib);

	ATTRMAP::const_iterator iter = mapAttrib.find(XML_IMAGE_ICON_WIDTH);
	if (mapAttrib.end() != iter)
	{
		m_nIconWidth = _ttoi(iter->second.c_str());
	}
	iter = mapAttrib.find(XML_IMAGE_ICON_HEIGHT);
	if (mapAttrib.end() != iter)
	{
		m_nIconHeight = _ttoi(iter->second.c_str());
	}
}

bool GdiplusIconRenderBitmap::LoadFromFile( const String& strPath, const ATTRMAP& mapAttrib )
{
	this->SetAttribute(mapAttrib);
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);

	HICON hIcon = (HICON)::LoadImage ( NULL, strPath.c_str(), IMAGE_ICON,m_nIconWidth,m_nIconHeight, LR_LOADFROMFILE );
	if (NULL == hIcon)
		return false;

	HDC hMemDC = UI_GetCacheDC();

	Image image;
	image.Create( m_nIconWidth,m_nIconHeight, 32, Image::createAlphaChannel );
	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)image );

	::DrawIconEx( hMemDC, 0,0, hIcon, m_nIconWidth,m_nIconHeight, 0, NULL, DI_NORMAL );
	::SelectObject(hMemDC, hOldBmp);
	::UI_ReleaseCacheDC(hMemDC);
	::DestroyIcon(hIcon);

	m_pBitmap = new Gdiplus::Bitmap(
		m_nIconWidth,
		m_nIconHeight,
		image.GetPitch(),
		PixelFormat32bppARGB,
		(BYTE*)image.GetBits() );
	m_hBitmapToFixIcon = image.Detach();    // Bitmap不负责保存bits数据，因此image.m_hBitmap不能提前释放，需要增加一个成员变量保存该句柄
	return true;
}
#pragma  endregion

#pragma  region // Gdiplus Font 
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

	if (NULL == m_pFont)
		return sizeText;

	if (NULL == szText || _tcslen(szText)==0)
		return sizeText;

	HDC hDC = UI_GetCacheDC();
	Gdiplus::Graphics  g(hDC);

	const Gdiplus::StringFormat* pStringFormat = Gdiplus::StringFormat::GenericTypographic();  // 不计算GDIPLUS的左右间距
	if (-1 != nLimitWidth)
	{
		Gdiplus::RectF  layoutRect((Gdiplus::REAL)0,(Gdiplus::REAL)0, (Gdiplus::REAL)nLimitWidth, (Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, layoutRect, pStringFormat, &boundingBox, NULL,NULL );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}
	else
	{
		Gdiplus::PointF origin((Gdiplus::REAL)0,(Gdiplus::REAL)0 );
		Gdiplus::RectF  boundingBox;

		g.MeasureString( szText, -1/*_tcslen(szText)*/, m_pFont, origin, pStringFormat, &boundingBox );

		sizeText.cx = (int)boundingBox.Width+1;
		sizeText.cy = (int)boundingBox.Height+1;
	}

	UI_ReleaseCacheDC(hDC);
	return sizeText;
}


UINT GdiplusRenderFont::GetTextMetricsHeight( )
{
	if (NULL == m_pFont)
		return 0;

	HDC hDC = ::UI_GetCacheDC();
	Gdiplus::Graphics g(hDC);
	UINT nRet = (UINT)m_pFont->GetHeight(&g);
	::UI_ReleaseCacheDC(hDC);

	return nRet;
}


HFONT GdiplusRenderFont::GetHFONT()
{
	HFONT hFont = m_hFontAttach;
	if (NULL == hFont)  
	{
		if (NULL == m_hFontForGDI)  // TODO: 如果m_pFont属性改变，那m_hFontForGDI也会一起改变吗？是否是每次获取前都将m_hFontForGDI干掉？
		{
			LOGFONT lf;
			this->GetLogFont(&lf);
			m_hFontForGDI = ::CreateFontIndirect(&lf);  // 析构时，释放
		}
		hFont = m_hFontForGDI;
	}
	return hFont;
}

bool GdiplusRenderFont::GetLogFont(LOGFONT* plf)
{
	if (NULL == plf || NULL == m_pFont)
		return false;

	HDC hCacheDC = ::UI_GetCacheDC();
	Gdiplus::Graphics g(hCacheDC);

#ifdef _UNICODE
	m_pFont->GetLogFontW(&g, plf);	
#else
	m_pFont->GetLogFontA(&g, plf);
#endif

	::UI_ReleaseCacheDC(hCacheDC);
	return true;
}

#pragma  endregion

#pragma  region // Gdiplus DC
//////////////////////////////////////////////////////////////////////////
//
//		GdiplusRenderTarget
//
// GdiplusRenderTarget::GdiplusRenderTarget()
// {
// 	m_pGraphics = NULL;
// 	m_hDC = NULL;
// 	this->Init();
// }

// GdiplusRenderTarget::GdiplusRenderTarget(HDC hDC):IRenderTarget(hDC)
// {
// 	m_pGraphics = Gdiplus::Graphics::FromHDC(hDC);
// 	UIASSERT(NULL != m_pGraphics);
// 
// 	this->Init();
// }
GdiplusRenderTarget::GdiplusRenderTarget(HWND hWnd):IRenderTarget(hWnd)
{
	m_pGraphics = NULL;
	m_hDC = NULL;
}
GdiplusRenderTarget::~GdiplusRenderTarget()
{
	SAFE_DELETE(m_pGraphics);
	m_hDC = NULL;
}

//
//	Remark
//		调用Graphics的GetHDC，获取到的HDC不会继承Graphics的任何属性，因此需要我们自己再设置一次属性
//		由于FONT、text color属性移到DrawString中当场设置，因此这两个属性没有设置
//
//		MSDN:
//			Any state changes you make to the device context between GetHDC and ReleaseHDC will 
//			be ignored by GDI+ and will not be reflected in rendering done by GDI+.
//
HDC GdiplusRenderTarget::GetHDC()
{
	if( NULL == m_pGraphics )
		return NULL;

	POINT  p = {0,0};
	this->GetViewportOrgEx(&p);

	Gdiplus::Region r;
	m_pGraphics->GetClip(&r);
	HRGN hRgn = r.GetHRGN(m_pGraphics);  // 必须在GetHDC前面获取，否则graphics将处于busy状态

	HDC hDC = m_pGraphics->GetHDC();

	::SetViewportOrgEx(hDC, p.x, p.y, NULL);
	::SelectClipRgn(hDC, hRgn);
	::DeleteObject(hRgn);
	::SetBkMode(hDC, TRANSPARENT);

	return hDC;
}
void GdiplusRenderTarget::ReleaseHDC( HDC hDC )
{
	m_pGraphics->ReleaseHDC(hDC);
}

HRGN GdiplusRenderTarget::GetClipRgn()
{
	Gdiplus::Region region;
	m_pGraphics->GetClip(&region);

	return region.GetHRGN(m_pGraphics);
}
int GdiplusRenderTarget::SelectClipRgn( HRGN hRgn, int nMode )
{
	if (RGN_COPY == nMode && NULL != hRgn)
		::SelectClipRgn(m_hDC, hRgn);
	else
		::ExtSelectClipRgn(m_hDC, hRgn, nMode);  // 为gdi类型带alpha通道的位图绘制准备的

	if (NULL == m_pGraphics)
		return 0;

	if(NULL == hRgn)
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
BOOL GdiplusRenderTarget::GetViewportOrgEx( LPPOINT lpPoint )
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
BOOL GdiplusRenderTarget::OffsetViewportOrgEx( int xOffset, int yOffset, LPPOINT lpPoint ) 
{
	if( NULL != lpPoint )
	{
		Gdiplus::Matrix  m;
		m_pGraphics->GetTransform(&m);

		lpPoint->x = (int)m.OffsetX();
		lpPoint->y = (int)m.OffsetY();
	}

	::OffsetViewportOrgEx(m_hDC, xOffset, yOffset, lpPoint );  // 为gdi类型带alpha通道的位图绘制准备的

	if( Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)xOffset,(Gdiplus::REAL)yOffset) )
		return TRUE;
	else
		return FALSE;
}
BOOL GdiplusRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint )
{
	if( NULL != lpPoint )
	{
		Gdiplus::Matrix  m;
		m_pGraphics->GetTransform(&m);
		lpPoint->x = (int)m.OffsetX();
		lpPoint->y = (int)m.OffsetY();
	}

	::SetViewportOrgEx(m_hDC, x, y, lpPoint);  // 为gdi类型带alpha通道的位图绘制准备的

	if (NULL == m_pGraphics)
		return TRUE;
	
	m_pGraphics->ResetTransform();
	if( Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)x,(Gdiplus::REAL)y) )
		return TRUE;
	else
		return FALSE;
}

bool GdiplusRenderTarget::BeginDraw(HDC hDC, RECT* prc, RECT* prc2, bool bClear)
{
	if (NULL != m_hDC)
		return false;

	if (NULL == hDC)
		return false;

	m_hDC = hDC;

	HBITMAP hMemBitmap = (HBITMAP) GetCurrentObject(m_hDC, OBJ_BITMAP);
//	m_pGdiMemBitmap = new Gdiplus::Bitmap(hMemBitmap,NULL);  // 这种方式由于是重新创建一张位图，会大量消耗内存。

	DIBSECTION  dibSection;
	GetObject(hMemBitmap, sizeof(DIBSECTION), &dibSection);

	BYTE* pBits = (BYTE*)dibSection.dsBm.bmBits;
	pBits += (dibSection.dsBm.bmHeight-1)*dibSection.dsBm.bmWidthBytes;  // 将指针移到第一行数据位置
	m_pGdiMemBitmap = new Gdiplus::Bitmap(dibSection.dsBm.bmWidth, dibSection.dsBm.bmHeight, -dibSection.dsBm.bmWidthBytes, PixelFormat32bppARGB, (BYTE*)pBits);

//	m_pGraphics = Gdiplus::Graphics::FromHDC(hDC);   // 采用这种方式创建出来的graphics，再调用gethdc，返回的就是原始的hDC，无法使用alpha通道
	m_pGraphics = new Gdiplus::Graphics(m_pGdiMemBitmap);  // 使用内存图片创建出来的graphics，再调用gethdc，就能让HDC使用alpha通道

	if (NULL == prc && NULL == prc2)
	{
	}
	else if (NULL != prc && NULL != prc2)
	{
		HRGN hRgn = CreateRectRgnIndirect(prc);
		if (NULL != prc2)
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

	if (bClear)
	{
		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		if (NULL != prc)
			::FillRect(m_hDC, prc, hBrush);
		if (NULL != prc2)
			::FillRect(m_hDC, prc2, hBrush);
	}
	return true;
}
void GdiplusRenderTarget::EndDraw( )
{
	delete m_pGdiMemBitmap;
	SAFE_DELETE(m_pGraphics);


	this->SetViewportOrgEx(0,0);
	this->SelectClipRgn(NULL);

// 	Gdiplus::Graphics g(m_hDC);
// 	g.DrawImage(m_pGdiMemBitmap,0,0,m_pGdiMemBitmap->GetWidth(),m_pGdiMemBitmap->GetHeight());


	m_hDC = NULL;
}

void GdiplusRenderTarget::Clear()
{
	if (NULL != m_pGraphics)
	{
		m_pGraphics->Clear(Gdiplus::Color::MakeARGB(0,0,0,0));
	}
}
BYTE* GdiplusRenderTarget::LockBits()
{
	if (NULL == m_hDC)
		return NULL;

	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP);
	if (NULL == hBitmap)
		return NULL;

	DIBSECTION dib;
	::GetObject(hBitmap, sizeof(DIBSECTION), &dib);
	return (BYTE*)dib.dsBm.bmBits;
}
void GdiplusRenderTarget::UnlockBits()
{
	
}

int GdiplusRenderTarget::DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == m_pGraphics )
		return 0;

	if( NULL == hRFont )
	{
		UI_LOG_WARN(_T("%s hRFont == NULL"), FUNC_NAME);
		return -1;
	}

	if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
	{
		UI_LOG_WARN(_T("%s hRFont render type != GRAPHICS_RENDER_TYPE_GDIPLUS"), FUNC_NAME);
		return -1;
	}

	Gdiplus::Font* pFont = ((GdiplusRenderFont*)hRFont)->GetFont();
	if( NULL == pFont )
	{
		UI_LOG_WARN(_T("%s NULL == pFont"), FUNC_NAME);
		return -1;
	}

	Gdiplus::RectF region(
		(Gdiplus::REAL)(lpRect->left),
		(Gdiplus::REAL)(lpRect->top), 
		(Gdiplus::REAL)(lpRect->Width()), 
		(Gdiplus::REAL)(lpRect->Height()) );

	Gdiplus::Color colText;
	colText.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col), GetGValue(col), GetBValue(col)));
	Gdiplus::SolidBrush textBrush(colText);

// 	m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
// 	m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeInvalid );
	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
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

void GdiplusRenderTarget::FillRgn( HRGN hRgn, COLORREF col )
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

void GdiplusRenderTarget::FillRect(const CRect* lprc, COLORREF col)
{
	if( NULL == lprc )
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col),GetGValue(col),GetBValue(col))) ;
	Gdiplus::SolidBrush brush(color);

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderTarget::TileRect( const CRect* lprc, HRBITMAP hBitmap )
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

void GdiplusRenderTarget::Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
{
	if( NULL == m_pGraphics )
		return;

	 // Remark:在这里如果不减1的话，会导致右、下的边框显示不全，原因未知
	CRect rc(lprc);
	rc.bottom--;
	rc.right--;

	if( false == bNullBack )
	{
		this->FillRect(&rc,colBack);
	}

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colBorder),GetGValue(colBorder),GetBValue(colBorder))) ;
	Gdiplus::Pen pen(color, (Gdiplus::REAL)nBorder);

	m_pGraphics->DrawRectangle(&pen,   
		rc.left, rc.top, 
		rc.Width(), rc.Height()  
		);  
}

void GdiplusRenderTarget::DrawFocusRect( const CRect* lprc )
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

void GdiplusRenderTarget::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height() );
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeHorizontal );

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderTarget::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height() );
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeVertical  );

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height() );
}

void GdiplusRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop )
{
	UIASSERT(0);
	// TODO: 该函数还会被调用吗

// 	if( NULL == pSrcHDC )
// 		return;
// 	if( pSrcHDC->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
// 		return;
// 
// 	GdiplusMemRenderDC* pMemRDC = (GdiplusMemRenderDC*)pSrcHDC;
// 	m_pGraphics->DrawImage(pMemRDC->GetMemBitmap(), xDest,yDest,xSrc,ySrc, wDest,hDest, Gdiplus::UnitPixel );
}

void GdiplusRenderTarget::DrawBitmap( HRBITMAP hBitmap, int x, int y)
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

void GdiplusRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if( NULL == pBitmap )
		return;

	if( pBitmap->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS )
		return;

	GdiplusRenderBitmap* pRenderBitmap = static_cast<GdiplusRenderBitmap*>(pBitmap);
	
	Gdiplus::Bitmap* p = pRenderBitmap->GetBitmap();
	if( NULL == p )
		return;

	m_pGraphics->DrawImage( p, xDest, yDest, xSrc, ySrc, pBitmap->GetWidth(), pBitmap->GetHeight(), Gdiplus::UnitPixel );
}

void GdiplusRenderTarget::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
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

void GdiplusRenderTarget::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
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
void GdiplusRenderTarget::ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
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


void GdiplusRenderTarget::DrawBitmap(HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == hBitmap || NULL == pParam)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;

	if (p->GetRenderType() == GRAPHICS_RENDER_TYPE_GDI)
	{
		GdiRenderTarget::DrawBitmapEx(m_hDC, hBitmap, pParam);
		return;
	}
	else if (p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDIPLUS)
	{
		return;
	}

	Gdiplus::Bitmap* pBitmap = ((GdiplusRenderBitmap*)p)->GetBitmap();
	if( NULL == pBitmap )
		return;

	// 利用颜色矩阵来直接绘制灰度图
	Gdiplus::ImageAttributes* pImageAttribute = NULL;
	Gdiplus::ImageAttributes  imageAttribute;
	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		const Gdiplus::REAL r = (Gdiplus::REAL)0.3;
		const Gdiplus::REAL g = (Gdiplus::REAL)0.59;
		const Gdiplus::REAL b = (Gdiplus::REAL)0.11;
		const Gdiplus::ColorMatrix matrix =
				{r,    r,   r,     0.0f,  0.0f,      // Red
				 g,    g,    g,    0.0f,  0.0f,      // Green
				 b,    b,    b,    0.0f,  0.0f,      // Blue
				 0.0f, 0.0f, 0.0f, 1.0f,  0.0f,      // Alpha
				 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};     
		imageAttribute.SetColorMatrix(&matrix);
		pImageAttribute = &imageAttribute;
	}

	if (pParam->nFlag & DRAW_BITMAP_BITBLT)
	{
		Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
		Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);
		Gdiplus::RectF destRect((Gdiplus::REAL)pParam->xDest, (Gdiplus::REAL)pParam->yDest, nW, nH);
		m_pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		this->DrawBitmap(hBitmap, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest,
			pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion);
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		Gdiplus::TextureBrush brush(pBitmap);
		m_pGraphics->FillRectangle(&brush, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest);
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		Gdiplus::RectF destRect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
		m_pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
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

			if( tan_x_y_image > tan_x_y_dest ) // 横向占满
			{
				wImage = pParam->wDest;
				hImage = (int)((double)wImage/tan_x_y_image);
			}
			else   // 纵向占满
			{
				hImage = pParam->hDest;
				wImage = (int)(hImage*tan_x_y_image);
			}
		}

		// 计算图片显示位置
		int xDisplayPos = pParam->xDest + (pParam->wDest-wImage)/2;
		int yDisplayPos = pParam->yDest + (pParam->hDest-hImage)/2;

		if( bNeedToStretch )
		{
			this->DrawBitmap(hBitmap, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion);
		}
		else
		{
			Gdiplus::RectF destRect((Gdiplus::REAL)xDisplayPos, (Gdiplus::REAL)yDisplayPos, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
			m_pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
		}
	}
}

#pragma endregion

#pragma region // Gdiplus mem dc
#if 0
GdiplusMemRenderDC::GdiplusMemRenderDC(int nWidth, int nHeight):GdiplusRenderTarget(NULL)
{
	GdiplusRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, nHeight);
	m_pGraphics = new Gdiplus::Graphics(m_pMemBitmap->GetBitmap());
}
GdiplusMemRenderDC::GdiplusMemRenderDC(HWND hWnd, int nWidth, int nHeight):GdiplusRenderTarget(hWnd)
{
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

	// 启用GDIPLUS的cleartype功能
 	m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
	// 消除锯齿模式
 	m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias );
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

// void GdiplusMemRenderDC::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish )
// {
// 	if( NULL == m_pMemBitmap )
// 		return;
// 
// 	m_pWndGraphics->DrawImage(m_pMemBitmap->GetBitmap(),xDest,yDest,xSrc,ySrc,wDest,hDest,Gdiplus::UnitPixel );
// 	if (!bFinish)
// 		return;
// 
// 	delete m_pWndGraphics;
// 	delete m_pGraphics;
// 	m_pGraphics = NULL;
// 	m_pWndGraphics = NULL;
// }
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

//
// 用于窗口透明背景时避免本次背景贴在上一次的背景上造成alpha叠加
//
void GdiplusMemRenderDC::Clear()
{
	if (NULL == m_pMemBitmap)
		return;

	//m_pMemBitmap->Clear();
	m_pGraphics->Clear(Gdiplus::Color::MakeARGB(0,0,0,0));
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

HBITMAP GdiplusMemRenderDC::CopyRect(RECT *prc)
{
	if (NULL == m_pMemBitmap || NULL == prc)
		return NULL;

	return m_pMemBitmap->CopyRect(prc);
}
#endif
#pragma endregion