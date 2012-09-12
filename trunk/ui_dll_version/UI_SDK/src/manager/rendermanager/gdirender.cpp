#include "stdafx.h"

GDIRenderBitmap::GDIRenderBitmap(IRenderBitmap** ppOutRef) : IRenderBitmap((IRenderResource**)ppOutRef)
{

}
GDIRenderBitmap::~GDIRenderBitmap()
{
	UI_LOG_DEBUG(_T("GDIRenderBitmap Delete. ptr=0x%08X"), this);
}
void GDIRenderBitmap::CreateInstance( IRenderBitmap** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GDIRenderBitmap* p = new GDIRenderBitmap(ppOutRef);
	*ppOutRef = p;
}

int  GDIRenderBitmap::GetWidth()
{
	return m_image.GetWidth();
}
int  GDIRenderBitmap::GetHeight() 
{
	return m_image.GetHeight();
}

BYTE* GDIRenderBitmap::LockBits()
{
	return (BYTE*)m_image.GetBits();
}
void  GDIRenderBitmap::UnlockBits()
{
	// Nothing.
}

bool  GDIRenderBitmap::SaveBits( ImageData* pImageData )
{
	return m_image.SaveBits(pImageData);
}
bool  GDIRenderBitmap::ChangeHLS( const ImageData* pOriginImageData, short h, short l , short s, int nFlag)
{
	return m_image.ChangeHLS(pOriginImageData, h, l, s, nFlag );
}

//
//  ע������ʹ��gdiplusֱ�Ӽ���icon�ļ��ᶪʧalphaͨ���������������ʹ��GDI DrawIcon��ȡһ������������
//		TODO: ����ֻĬ��֧��16*16��С�ģ�ico��������С��ͼ����ʱû�����ӽӿڷֱ����
//
bool GDIRenderBitmap::LoadFromFile( const String& strPath )
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

bool GDIRenderBitmap::Create(int nWidth, int nHeight)
{
	UIASSERT(nHeight < 0);   // ʹ�÷����
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


//////////////////////////////////////////////////////////////////////////

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
//	����FONT�Ĵ�����ʽ���ͷ�����
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
		UI_LOG_WARN(_T("GDIRenderFont::Load plogfont == NULL"));
		return false;
	}

	this->DestroyFont();
	m_hFont = ::CreateFontIndirect(plogfont);
	if( NULL == m_hFont )
	{
		UI_LOG_WARN(_T("GDIRenderFont::Load CreateFontIndirect failed, facename=%s"), plogfont->lfFaceName );
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
		UI_LOG_WARN(_T("GDIRenderFont::ModifyFont Load failed."));
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
//////////////////////////////////////////////////////////////////////////
//
//

GDIRenderDC::GDIRenderDC()
{
	m_hDC = NULL;
}
GDIRenderDC::GDIRenderDC(HDC hDC):IRenderDC(hDC)
{
	m_hDC = hDC;
}
GDIRenderDC::GDIRenderDC(HWND hWnd):IRenderDC(hWnd)
{
	m_hDC = ::GetDC(hWnd);
	::SetBkMode(m_hDC, TRANSPARENT);   // ��ȥ֧�ִ����������֣������Ҫ��ʹ�ñ������
}
GDIRenderDC::~GDIRenderDC()
{
	if( NULL != m_hWnd && NULL != m_hDC)
	{
		::ReleaseDC(m_hWnd,m_hDC);
	}
	m_hDC = NULL;
}

HRDC GDIRenderDC::CreateCompatibleHRDC( int nWidth, int nHeight )
{
	return (HRDC)new GDIMemRenderDC(m_hDC, nWidth, nHeight);
}

HDC GDIRenderDC::GetHDC()
{
	return m_hDC;
}
void GDIRenderDC::ReleaseHDC( HDC hDC )
{
	return ;
}

// HRFONT GDIRenderDC::SelectFont( HRFONT hRFont ) 
// {
// 	if( NULL != hRFont )
// 	{
// 		if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
// 		{
// 			hRFont = NULL;
// 		}
// 	}
// 
// 	HRFONT hSave = m_hRFont;
// 	m_hRFont = hRFont;
// 
// 	HFONT hFont = NULL;
// 	if( NULL != hRFont )
// 	{
// 		hFont = ((GDIRenderFont*)hRFont)->GetHFONT();
// 	}
// 	::SelectObject(m_hDC, hFont);
// 
// 	return hSave;
// }
// HRFONT GDIRenderDC::GetFont()
// {
// 	return m_hRFont;
// }

HRGN GDIRenderDC::GetClipRgn()
{
	HRGN hRgn = ::CreateRectRgn(0,0,0,0);
	if( 1 != ::GetClipRgn(m_hDC,hRgn) )  // �ջ���ʧ��
	{
		::DeleteObject(hRgn);
		hRgn = NULL;
	}
	return hRgn;
}
int GDIRenderDC::SelectClipRgn( HRGN hRgn, int nMode )
{
	return ExtSelectClipRgn(m_hDC, hRgn, nMode);
}

BOOL GDIRenderDC::GetViewportOrgEx( LPPOINT lpPoint )
{
	if( NULL == lpPoint )
	{
		return FALSE;
	}

	::GetViewportOrgEx(m_hDC, lpPoint );
	return TRUE;
}
BOOL GDIRenderDC::SetViewportOrgEx( int x, int y, LPPOINT lpPoint ) 
{
	return ::SetViewportOrgEx( m_hDC, x, y, lpPoint);
}
BOOL GDIRenderDC::OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint )
{
	return ::OffsetViewportOrgEx( m_hDC, x, y, lpPoint );
}

// COLORREF GDIRenderDC::SetTextColor( COLORREF color, byte Alpha )
// {
// 	return ::SetTextColor(m_hDC, color);
// }
// COLORREF GDIRenderDC::GetTextColor( )
// {
// 	return ::GetTextColor(m_hDC);
// }


int GDIRenderDC::DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == hRFont )
	{
		UI_LOG_WARN(_T("GDIRenderDC::DrawString hRFont == NULL"));
		return -1;
	}

	if( ((IRenderFont*)hRFont)->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
	{
		UI_LOG_WARN(_T("GDIRenderDC::DrawString hRFont render type != GRAPHICS_RENDER_TYPE_GDI"));
		return -1;
	}

	HFONT hOldFont = (HFONT)::SelectObject(m_hDC, ((GDIRenderFont*)hRFont)->GetHFONT());
	COLORREF oldCol = ::SetTextColor(m_hDC, col);

	int nRet = ::DrawText(m_hDC, szText, _tcslen(szText), (RECT*)lpRect, nFormat);

	::SetTextColor(m_hDC,oldCol);
	::SelectObject(m_hDC, hOldFont);

	return nRet;
}


void GDIRenderDC::FillRgn( HRGN hRgn, COLORREF col )
{
	HBRUSH hBrush = ::CreateSolidBrush(col);
	::FillRgn( m_hDC, hRgn, hBrush );
	::DeleteObject(hBrush);
}

void GDIRenderDC::FillRect( const CRect* lprc, COLORREF col )
{
	HBRUSH hBrush = ::CreateSolidBrush(col);
	::FillRect( m_hDC, lprc, hBrush );
	::DeleteObject(hBrush);
}

void GDIRenderDC::TileRect( const CRect* lprc, HRBITMAP hBitmap )
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
// ע�����ڿؼ�����ʱ�Ѿ����ü��������⽫����ʹ��pen���Ʊ߿�ʱ����һ���ֻ��Ƴ�����
//     �߿�λ�ڿؼ�����������á�
//
void GDIRenderDC::Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
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

	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDC, hBrush);

	::Rectangle(m_hDC, lprc->left, lprc->top, lprc->right, lprc->bottom );

	::SelectObject(m_hDC, hOldPen);
	::SelectObject(m_hDC, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void GDIRenderDC::DrawFocusRect( const CRect* lprc )
{
	::DrawFocusRect( m_hDC, lprc );
}

void GDIRenderDC::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillH(m_hDC, lprc, colFrom, colTo );
}
void GDIRenderDC::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillV(m_hDC, lprc, colFrom, colTo );
}

void GDIRenderDC::BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderDC* pSrcHDC, int xSrc, int ySrc, DWORD dwRop )
{
	if( NULL == pSrcHDC )
		return;
	if( pSrcHDC->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	::BitBlt(m_hDC, xDest,yDest,wDest,hDest, pSrcHDC->GetHDC(),xSrc,ySrc,dwRop );
}
void GDIRenderDC::DrawBitmap( HRBITMAP hBitmap, int x, int y)
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	pBitmap->GetBitmap()->Draw(m_hDC,x,y);
}

void GDIRenderDC::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	pBitmap->GetBitmap()->Draw( m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}

void GDIRenderDC::DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
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
	pBitmap->GetBitmap()->Draw( m_hDC, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}

void GDIRenderDC::ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetRenderType() != GRAPHICS_RENDER_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	pBitmap->GetBitmap()->ImageList_Draw(m_hDC, x,y,col,row,cx,cy);
}


GDIMemRenderDC::GDIMemRenderDC(HDC hDC, int nWidth, int nHeight )
{
	::OutputDebugString(_T("TODO: GDIMemRenderDC::GDIMemRenderDC ɾ���ú��������ٵ��á�\r\n"));
	m_hDC = ::CreateCompatibleDC(hDC);

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
	::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );

	::SetBkMode(m_hDC, TRANSPARENT);   // ��ȥ֧�ִ����������֣������Ҫ��ʹ�ñ������
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
}
GDIMemRenderDC::GDIMemRenderDC(HWND hWnd, int nWidth, int nHeight )
{
	m_hWnd   = hWnd;
	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC    = NULL;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pMemBitmap = NULL;
	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);
}
GDIMemRenderDC::~GDIMemRenderDC()
{
	SAFE_DELETE(m_pMemBitmap);

	if( NULL != m_hDC )
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
	m_hWndDC = NULL;
}

void GDIMemRenderDC::BeginDraw( HDC hDC )
{
	m_hOldWndDC = hDC;
	m_hWndDC = hDC == NULL ? ::GetDC(m_hWnd):hDC;
	m_hDC = ::CreateCompatibleDC(m_hWndDC);
	::SelectObject(m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
	::SetBkMode(m_hDC, TRANSPARENT);
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
}

void GDIMemRenderDC::EndDraw( )
{
	::BitBlt(m_hWndDC, 0,0, m_nWidth, m_nHeight, m_hDC, 0,0, SRCCOPY );

	::DeleteDC(m_hDC);
	if( NULL == m_hOldWndDC )
	{
		::ReleaseDC(m_hWnd, m_hWndDC);
	}
	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC = NULL;
}
void GDIMemRenderDC::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc )
{
	::BitBlt(m_hWndDC, xDest, yDest, wDest, hDest, m_hDC, xSrc,ySrc, SRCCOPY );

	::DeleteDC(m_hDC);
	if( NULL == m_hOldWndDC )
	{
		::ReleaseDC(m_hWnd, m_hWndDC);
	}
	m_hWndDC = NULL;
	m_hOldWndDC = NULL;
	m_hDC = NULL;
}

void GDIMemRenderDC::ResizeRenderTarget( int nWidth, int nHeight )
{
	if( m_nWidth == nWidth && m_nHeight == nHeight )
		return;

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	SAFE_DELETE(m_pMemBitmap);

	GDIRenderBitmap::CreateInstance((IRenderBitmap**)&m_pMemBitmap);
	m_pMemBitmap->Create(nWidth, -nHeight);

	if( NULL != m_hDC )
	{
		::SelectObject( m_hDC, m_pMemBitmap->GetBitmap()->GetHBITMAP() );
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
		m_pMemBitmap->GetBitmap()->Save( strPath.c_str(), Gdiplus::ImageFormatPNG);
	}
}