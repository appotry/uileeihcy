#include "stdafx.h"


#pragma region // Gdi Bitmap
GDIRenderBitmap::GDIRenderBitmap(IRenderBitmap** ppOutRef) : GDIRenderBitmapImpl<IRenderBitmap>(ppOutRef)
{
}

void GDIRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIRenderBitmap* p = new GDIRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

GDIImageListRenderBitmap::GDIImageListRenderBitmap(IRenderBitmap** ppOutRef) : GDIRenderBitmapImpl<IImageListRenderBitmap>(ppOutRef)
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
void GDIImageListRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIImageListRenderBitmap* p = new GDIImageListRenderBitmap(ppOutRef);
	*ppOutRef = p;
}
void GDIImageListRenderBitmap::SetAttribute( const ATTRMAP& mapAttrib )
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
int GDIImageListRenderBitmap::GetItemWidth()
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
int GDIImageListRenderBitmap::GetItemHeight()
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
IMAGELIST_LAYOUT_TYPE GDIImageListRenderBitmap::GetLayoutType()
{
	return m_eLayout;
}
bool GDIImageListRenderBitmap::GetIndexPos(int nIndex, POINT* pPoint)
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
GDIIconRenderBitmap::GDIIconRenderBitmap(IRenderBitmap** ppOutRef) : GDIRenderBitmap(ppOutRef)
{
	m_nIconWidth = m_nIconHeight = 16;
}
void GDIIconRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIIconRenderBitmap* p = new GDIIconRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

void GDIIconRenderBitmap::SetAttribute( const ATTRMAP& mapAttrib )
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

bool GDIIconRenderBitmap::LoadFromFile( const String& strPath, const ATTRMAP& mapAttrib  )
{
	this->SetAttribute(mapAttrib);

	if (! m_image.IsNull())
		m_image.Destroy();

	HICON hIcon = (HICON)::LoadImage(NULL, strPath.c_str(), IMAGE_ICON, m_nIconWidth, m_nIconHeight, LR_LOADFROMFILE);
	if (NULL == hIcon)
		return false;

	HDC hMemDC = UI_GetCacheDC();

	m_image.Create( m_nIconHeight, m_nIconHeight, 32, Image::createAlphaChannel );
	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC, (HBITMAP)m_image );

	::DrawIconEx( hMemDC, 0,0, hIcon, m_nIconWidth, m_nIconHeight, 0, NULL, DI_NORMAL );
	::SelectObject(hMemDC, hOldBmp);
	::UI_ReleaseCacheDC(hMemDC);
	::DestroyIcon(hIcon);

	if( m_image.IsNull() )
		return false;
	else
		return true;
}
//////////////////////////////////////////////////////////////////////////

GDIGifRenderBitmap::GDIGifRenderBitmap(IRenderBitmap** ppOutRef):IRenderBitmap((IRenderResource**)ppOutRef)
{
	 m_pGifImage = NULL;
}
GDIGifRenderBitmap::~GDIGifRenderBitmap()
{
	if (NULL != m_pGifImage)
	{
		m_pGifImage->Destroy();
		SAFE_DELETE(m_pGifImage);
	}
}
void GDIGifRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GDIGifRenderBitmap* p = new GDIGifRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

bool GDIGifRenderBitmap::LoadFromFile(const String& strPath, const ATTRMAP& mapAttrib)
{
	if (NULL != m_pGifImage)
	{
		m_pGifImage->Destroy();
		SAFE_DELETE(m_pGifImage);
	}

	String strType;
	ATTRMAP::const_iterator iter = mapAttrib.find(XML_IMAGE_ITEM_TYPE);
	if (iter != mapAttrib.end())
	{
		strType = iter->second;
	}

	if (strType == XML_IMAGE_ITEM_TYPE_GIF)
	{
		m_pGifImage = new GifImage;
	}
	else
	{
		m_pGifImage = new PngListGifImage;
	}

	bool bRet = m_pGifImage->Load(strPath.c_str(), &mapAttrib);
	if (false == bRet)
	{
		UI_LOG_WARN(_T("%s load gif file failed. path=%s"), FUNC_NAME, strPath.c_str());
	}
	return bRet;
}
void GDIGifRenderBitmap::SetAttribute(const ATTRMAP& mapAttrib)
{
}

int GDIGifRenderBitmap::GetWidth()
{
	if (NULL == m_pGifImage)
		return 0;

	return m_pGifImage->GetWidth();
}
int GDIGifRenderBitmap::GetHeight()
{
	if (NULL == m_pGifImage)
		return 0;

	return m_pGifImage->GetHeight();
}

BYTE* GDIGifRenderBitmap::LockBits()
{
	UIASSERT(0);
	return NULL;
}
void GDIGifRenderBitmap::UnlockBits()
{
	UIASSERT(0);
}

bool GDIGifRenderBitmap::SaveBits(ImageData* pImageData)
{
	UIASSERT(0);
	return false;
}
bool GDIGifRenderBitmap::ChangeHLS(const ImageData* pOriginImageData, short h, short l, short s, int nFlag)
{
	UIASSERT(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
#pragma  endregion

#pragma  region  // Gdi Font
GDIRenderFont::GDIRenderFont(IRenderFont** ppOutRef) : IRenderFont((IRenderResource**)ppOutRef)
{
	m_hFont = NULL;
	m_bCreateOrAttach = true;
}
GDIRenderFont::~GDIRenderFont()
{
	UI_LOG_DEBUG(_T("GDIRenderFont Delete. ptr=0x%08X"), this);
	this->DestroyFont();
}
//
//	根据FONT的创建方式来释放字体
//
void  GDIRenderFont::DestroyFont()
{
	if(m_bCreateOrAttach)
	{
		SAFE_DELETE_GDIOBJECT(m_hFont);
	}
	m_hFont = NULL;
	m_bCreateOrAttach = true;
}
void  GDIRenderFont::CreateInstance( IRenderFont** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIRenderFont* p = new GDIRenderFont(ppOutRef);
	*ppOutRef = p;
}

bool GDIRenderFont::Load( LOGFONT* plogfont )
{
	if( NULL == plogfont )
	{
		UI_LOG_WARN(_T("%s plogfont == NULL"), FUNC_NAME);
		return false;
	}

	this->DestroyFont();
	m_hFont = ::CreateFontIndirect(plogfont);
	if( NULL == m_hFont )
	{
		UI_LOG_WARN(_T("%s CreateFontIndirect failed, facename=%s"), FUNC_NAME, plogfont->lfFaceName );
		return false;
	}
	m_bCreateOrAttach = true;
	return true;
}
bool GDIRenderFont::ModifyFont(LOGFONT* plogfont)
{
	bool bRet = this->Load(plogfont);
	if( false == bRet )
	{
		UI_LOG_WARN(_T("%s Load failed."), FUNC_NAME);
	}
	return bRet;
}

void  GDIRenderFont::Attach(HFONT hFont)
{
	this->DestroyFont();
	m_hFont = hFont;
	m_bCreateOrAttach = false;
}
HFONT GDIRenderFont::Detach()
{
	HFONT hSave = m_hFont;
	m_hFont = NULL;
	m_bCreateOrAttach = true;
	
	return hSave;
}
SIZE GDIRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);

	HDC hDC = UI_GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	if ( NULL == _tcsrchr( szText,_T('\n')) )
	{
		::GetTextExtentPoint32( hDC, szText, _tcslen(szText), &sizeText );
	}
	else
	{
		RECT rcText = {0,0,0,0};
		if( -1 != nLimitWidth )
		{
			rcText.right = nLimitWidth;
		}
		::DrawText(hDC, szText, _tcslen(szText), &rcText, DT_CALCRECT );
		sizeText.cx = rcText.right;
		sizeText.cy = rcText.bottom;
	}
	::SelectObject(hDC, hOldFont);
	UI_ReleaseCacheDC(hDC);

	return sizeText;
}
UINT GDIRenderFont::GetTextMetricsHeight()
{
	HDC hDC = UI_GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	TEXTMETRIC  t;
	GetTextMetrics( hDC, &t );
	
	::SelectObject(hDC, hOldFont);
	UI_ReleaseCacheDC(hDC);
	return t.tmHeight;
}

HFONT GDIRenderFont::GetHFONT()
{
	return m_hFont;
}

bool GDIRenderFont::GetLogFont(LOGFONT* plf)
{
	if (NULL == plf || NULL == m_hFont)
		return false;

	GetObject(m_hFont, sizeof(LOGFONT), plf);
	return true;
}
//////////////////////////////////////////////////////////////////////////
#pragma  endregion

#pragma  region  // gdi rendertarget
// GdiRenderTarget::GdiRenderTarget()
// {
// 	m_hDC = NULL;
// }
// GdiRenderTarget::GdiRenderTarget(HDC hDC):IRenderTarget(hDC)
// {
// 	UIASSERT(0);  // 废弃
// 	m_hDC = hDC;
// }
GdiRenderTarget::GdiRenderTarget(HWND hWnd):IRenderTarget(hWnd)
{
	m_hDC = NULL;
}
GdiRenderTarget::~GdiRenderTarget()
{
// 	if( NULL != m_hWnd && NULL != m_hDC)
// 	{
// 		::ReleaseDC(m_hWnd,m_hDC);
// 	}
 	m_hDC = NULL;
}

// HRDC GdiRenderTarget::CreateCompatibleHRDC( int nWidth, int nHeight )
// {
// 	UIASSERT(0);  // TODO: 该函数有用吗
// 	return (HRDC)new GDIMemRenderDC(m_hDC, nWidth, nHeight);
// }

HDC GdiRenderTarget::GetHDC()
{
	return m_hDC;
}
void GdiRenderTarget::ReleaseHDC( HDC hDC )
{
	return ;
}

HRGN GdiRenderTarget::GetClipRgn()
{
	HRGN hRgn = ::CreateRectRgn(0,0,0,0);
	if( 1 != ::GetClipRgn(m_hDC,hRgn) )  // 空或者失败
	{
		::DeleteObject(hRgn);
		hRgn = NULL;
	}
	return hRgn;
}
int GdiRenderTarget::SelectClipRgn( HRGN hRgn, int nMode )
{
	if (RGN_COPY == nMode && NULL != hRgn)
	{
		return ::SelectClipRgn(m_hDC, hRgn);
	}

	int nRet = ExtSelectClipRgn(m_hDC, hRgn, nMode);
	return nRet;
}

BOOL GdiRenderTarget::GetViewportOrgEx( LPPOINT lpPoint )
{
	if( NULL == lpPoint )
	{
		return FALSE;
	}

	::GetViewportOrgEx(m_hDC, lpPoint );
	return TRUE;
}
BOOL GdiRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint ) 
{
	BOOL bRet1 = ::SetViewportOrgEx( m_hDC, x, y, lpPoint);
	return bRet1;
}
BOOL GdiRenderTarget::OffsetViewportOrgEx(int x, int y, LPPOINT lpPoint)
{
	BOOL bRet1 = ::OffsetViewportOrgEx( m_hDC, x, y, lpPoint );
	return bRet1;
}

//
// 如果需要同时绘制两个item项，则可以提供两个RECT进行裁剪
//
// bClear主要是用于分层窗口中的透明背景，目前没有发现GDI有需要重刷背景的地方
//
bool GdiRenderTarget::BeginDraw(HDC hDC, RECT* prc, RECT* prc2, bool bClear)
{

	if (NULL != m_hDC)
		return false;

	if (NULL == hDC)
		return false;

	m_hDC = hDC;
	
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
	return true;
}
void GdiRenderTarget::EndDraw()
{
	this->SetViewportOrgEx(0,0);
	this->SelectClipRgn(NULL);

 	m_hDC = NULL;
}

void GdiRenderTarget::Clear()
{
	if (NULL == m_hDC)
		return;

	BITMAP  bm;
	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(m_hDC, OBJ_BITMAP);
	if (NULL == hBitmap)
		return;

	::GetObject(hBitmap, sizeof(bm), &bm);
	RECT  rc = {0,0, bm.bmWidth, bm.bmHeight};
	::FillRect(m_hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
}

int GdiRenderTarget::DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == hRFont )
	{
		UI_LOG_WARN(_T("GdiRenderTarget::DrawString hRFont == NULL"));
		return -1;
	}

	if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
	{
		UI_LOG_WARN(_T("GdiRenderTarget::DrawString hRFont render type != GRAPHICS_RENDER_TYPE_GDI"));
		return -1;
	}

	HDC hDC = m_hDC;

	HFONT hOldFont = (HFONT)::SelectObject(hDC, ((GDIRenderFont*)hRFont)->GetHFONT());
	COLORREF oldCol = ::SetTextColor(hDC, col);

	int nRet = 0;
	
	::DrawText(hDC, szText, _tcslen(szText), (RECT*)lpRect, nFormat);

	::SetTextColor(hDC,oldCol);
	::SelectObject(hDC, hOldFont);

	return nRet;
}


void GdiRenderTarget::FillRgn( HRGN hRgn, COLORREF col )
{
	HBRUSH hBrush = ::CreateSolidBrush(col);
	::FillRgn(m_hDC, hRgn, hBrush );
	::DeleteObject(hBrush);
}

void GdiRenderTarget::FillRect( const CRect* lprc, COLORREF col )
{
	HBRUSH hBrush = ::CreateSolidBrush(col);
	int n = ::FillRect(m_hDC, lprc, hBrush );
	::DeleteObject(hBrush);
}

void GdiRenderTarget::TileRect( const CRect* lprc, HRBITMAP hBitmap )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	HBRUSH hBrush = ::CreatePatternBrush(pBitmap->GetBitmap()->operator HBITMAP());
	::FillRect(m_hDC, lprc, hBrush);
	::DeleteObject(hBrush);
}

//
// 注：由于控件绘制时已经被裁剪了区域，这将导致使用pen绘制边框时，有一部分绘制出来的
//     边框位于控件外面而被剪裁。
//
void GdiRenderTarget::Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
{
	HPEN hPen = ::CreatePen(PS_SOLID, nBorder, colBorder );
	HBRUSH hBrush = NULL;
	if (bNullBack)
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	else
	{
		hBrush = ::CreateSolidBrush(colBack);
	}

	HDC hDC = m_hDC;

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);

	::Rectangle(hDC, lprc->left, lprc->top, lprc->right, lprc->bottom );

	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::DrawFocusRect( const CRect* lprc )
{
	::DrawFocusRect(m_hDC, lprc);
}

void GdiRenderTarget::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillH(m_hDC, lprc, colFrom, colTo );
}
void GdiRenderTarget::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillV(m_hDC, lprc, colFrom, colTo );
}

void GdiRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop)
{
	if (NULL == pSrcHDC)
		return;
	if (pSrcHDC->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI)
		return;

	::BitBlt(m_hDC, xDest,yDest,wDest,hDest, pSrcHDC->GetHDC(),xSrc,ySrc,dwRop );
}

void GdiRenderTarget::DrawBitmap( HRBITMAP hBitmap, int x, int y)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI)
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, x,y);
}
void GdiRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if (NULL == pBitmap)
		return;

	if (pBitmap->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI)
		return;

	GDIRenderBitmap* pRenderBitmap = static_cast<GDIRenderBitmap*>(pBitmap);
	if (NULL == pBitmap)
		return;

	Image* pImage = pRenderBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest,yDest, wDest,hDest, xSrc, ySrc, wDest, hDest);
}
void GdiRenderTarget::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}

void GdiRenderTarget::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					Image9Region* p9Region )
{
	if( NULL == hBitmap )
		return;

	if( NULL == p9Region )
		return this->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}

void GdiRenderTarget::ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->ImageList_Draw(m_hDC, x,y,col,row,cx,cy);
}

void GdiRenderTarget::DrawBitmapEx(HDC hDC, HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam )
{
	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI)
		return;

	Image* pImage =  ((GDIRenderBitmap*)p)->GetBitmap();
	Image  imageDisable;
	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		imageDisable.CopyGrayImageFrom(pImage);
		pImage = &imageDisable;
	}

	if (pParam->nFlag & DRAW_BITMAP_BITBLT)
	{
		int nW = min(pParam->wDest, pParam->wSrc);
		int nH = min(pParam->hDest, pParam->hSrc);
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, nW, nH, pParam->xSrc, pParam->ySrc, nW, nH);
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion);
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		if( NULL == hBitmap )
			return;

		RECT rc = {pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest};
		HBRUSH hBrush = ::CreatePatternBrush(pImage->operator HBITMAP());
		::FillRect(hDC, &rc, hBrush);
		::DeleteObject(hBrush);

		// 			int yDest2 = (pParam->hDest+pParam->yDest);
		// 			int xDest2 = (pParam->wDest+pParam->xDest);
		// 			for (int y = pParam->yDest; y < yDest2; y += pImage->GetHeight())
		// 			{
		// 				int nH = yDest2-y;
		// 				if (nH > pParam->hSrc)
		// 					nH = pParam->hSrc;
		// 
		// 				for (int x = pParam->xDest; x < xDest2; x +=pImage->GetWidth())
		// 				{
		// 					int nW = xDest2-x;
		// 					if (nW > pParam->wSrc)
		// 						nW = pParam->wSrc;
		// 
		// 					pImage->Draw(m_hDC, x, y, nW, nH, pParam->xSrc, pParam->ySrc, nW, nH);
		// 				}
		// 			}

	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		pImage->Draw(hDC, x,y, pParam->wSrc, pParam->hSrc, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc);
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
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion);
		}
		else
		{
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, wImage, hImage);
		}
	}
}
void GdiRenderTarget::DrawBitmap(HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == hBitmap || NULL == pParam)
		return;

	GdiRenderTarget::DrawBitmapEx(m_hDC, hBitmap, pParam);
}

#pragma  endregion

#pragma  region  // Gdi mem RenderTarget
#if 0
GDIMemRenderDC::GDIMemRenderDC(HDC hDC, int nWidth, int nHeight ):GdiRenderTarget(NULL)
{
	UIASSERT(0);  // 不再使用该类
	::OutputDebugString(_T("TODO: GDIMemRenderDC::GDIMemRenderDC 删除该函数，不再调用。\r\n"));
	m_hDC = ::CreateCompatibleDC(hDC);

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );

	::SetBkMode(m_hDC, TRANSPARENT);   // 不去支持带背景的文字，如果需要就使用背景填充
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
}
GDIMemRenderDC::GDIMemRenderDC(HWND hWnd, int nWidth, int nHeight ) : GdiRenderTarget(hWnd) 
{
	UIASSERT(0);  // 不再使用该类

	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC    = NULL;
	m_hOldBitmap = NULL;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
}
GDIMemRenderDC::~GDIMemRenderDC()
{
	if (NULL != m_hOldBitmap)
	{
	    if (NULL != m_hDC)
		{
			::SelectObject(m_hDC, m_hOldBitmap);
			m_hOldBitmap = NULL;
		}
		else
		{
			UI_LOG_WARN(_T("%s old bitmap is not selected back into dc"),FUNC_NAME);
		}		
	}
	SAFE_DELETE(m_pMemBitmap);

	if( NULL != m_hDC )
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
	m_hWndDC = NULL;
}

bool GDIMemRenderDC::BeginDraw( HDC hDC )
{
	// 注：在这里排查了一天。有时候发现窗口变成空白了，没有内容。
	//     然后调试过程中，流程也正常，不知道是哪的问题，打印出来的图片显示mem bmp只显示了一部分控件，然后后面一直也没有再变化
	//     通过加了一句FillRect(RGB(0,0,0)),发现mem bmp的内容居然没有发生变化。因此可以推断是mem bmp没有被正确选入到m_hDC中。
	//     然后在BeginDraw中发现，出现问题的时候，selectobject返回为空，失败了。再观察堆栈，发现BeginDraw嵌套响应了两次，而
	//     EndDraw没有响应，导致第一次的m_hDC被重写，同时mem bmp也由于被第一次的m_hDC选入了而无法释放，因此也不能再被
	//     其它DC选入。所以才导致所有的绘制操作都无效了。
	//     
	if (NULL != m_hDC)  
	{
		UI_LOG_WARN(_T("%s pre invoke has not been finish."),FUNC_NAME);
		return false;
	}

	m_hOldWndDC = hDC;
	m_hWndDC = hDC == NULL ? ::GetDC(m_hWnd):hDC;
	m_hDC = ::CreateCompatibleDC(m_hWndDC);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
	UIASSERT(NULL != m_hOldBitmap);

	::SetBkMode(m_hDC, TRANSPARENT);
	::SetStretchBltMode(m_hDC, COLORONCOLOR);

	return true;
}

void GDIMemRenderDC::EndDraw( )
{
    if (NULL == m_hDC)
		return;
		
	::BitBlt(m_hWndDC, 0,0, m_nWidth, m_nHeight, m_hDC, 0,0, SRCCOPY );

	if (NULL != m_hOldBitmap)
	{
		::SelectObject(m_hDC, m_hOldBitmap);
		m_hOldBitmap = NULL;
	}

	::DeleteDC(m_hDC);
	if( NULL == m_hOldWndDC )
	{
		::ReleaseDC(m_hWnd, m_hWndDC);
	}
	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC = NULL;
}
// void GDIMemRenderDC::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish )
// {
// 	if (NULL == m_hDC)
// 		return;
// 
// 	::BitBlt(m_hWndDC, xDest, yDest, wDest, hDest, m_hDC, xSrc,ySrc, SRCCOPY );
// 	if (!bFinish)
// 		return;
// 
// 	if (NULL != m_hOldBitmap)
// 	{
// 		::SelectObject(m_hDC, m_hOldBitmap);
// 		m_hOldBitmap = NULL;
// 	}
// 	::DeleteDC(m_hDC);
// 	if( NULL == m_hOldWndDC )
// 	{
// 		::ReleaseDC(m_hWnd, m_hWndDC);
// 	}
// 	m_hWndDC = NULL;
// 	m_hOldWndDC = NULL;
// 	m_hDC = NULL;
// }

void GDIMemRenderDC::ResizeRenderTarget( int nWidth, int nHeight )
{
	if( m_nWidth == nWidth && m_nHeight == nHeight )
		return;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	if (NULL != m_hDC && NULL != m_hOldBitmap)
	{
		SelectObject(m_hDC, m_hOldBitmap);
		m_hOldBitmap = NULL;
	}
	SAFE_DELETE(m_pMemBitmap);

	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);

	if( NULL != m_hDC )
	{
		m_hOldBitmap = (HBITMAP)::SelectObject( m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
	}
}
BYTE* GDIMemRenderDC::LockBits()
{
#if 0
	DIBSECTION dibsection;
	int nBytes;

	nBytes = ::GetObject( m_hMemBitmap, sizeof( DIBSECTION ), &dibsection );
	if( nBytes == sizeof( DIBSECTION ) )
	{
		return (BYTE*)dibsection.dsBm.bmBits;
	}
	else
	{
		return NULL;
	}
#else
	if( NULL == m_pMemBitmap )
		return NULL;

	return m_pMemBitmap->LockBits();
#endif
}
void GDIMemRenderDC::UnlockBits()
{
	if( NULL == m_pMemBitmap )
		return;

	m_pMemBitmap->UnlockBits();
}

void GDIMemRenderDC::Save( const String& strPath )
{
	if( m_pMemBitmap->GetBitmap() )
	{
		REFGUID guidType = Gdiplus::ImageFormatBMP;
		String strExt = strPath.substr(strPath.length()-3,3);
		if (0 == _tcsicmp(strExt.c_str(), _T("bmp")))
		{
		}
		else if (0 == _tcsicmp(strExt.c_str(), _T("png")))
		{
			memcpy((void*)&guidType, & Gdiplus::ImageFormatPNG, sizeof(GUID));
		}
		else if (0 == _tcsicmp(strExt.c_str(), _T("jpg")))
		{
			memcpy((void*)&guidType, & Gdiplus::ImageFormatJPEG, sizeof(GUID));
		}
		else
		{
			return ;
		}

		m_pMemBitmap->GetBitmap()->Save( strPath.c_str(), guidType);
	}
}

HBITMAP GDIMemRenderDC::CopyRect(RECT *prc)
{
	if (NULL == m_pMemBitmap)
		return NULL;

	return m_pMemBitmap->CopyRect(prc);
}
#endif
#pragma endregion