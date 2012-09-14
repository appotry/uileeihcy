#include "stdafx.h"


RenderOffsetClipHelper::RenderOffsetClipHelper(Object* pObject)
{
	pObject->GetParentRect(&m_rcClip);
	m_ptOffset.x = m_rcClip.left;
	m_ptOffset.y = m_rcClip.top;
	::OffsetRect(&m_rcClip, -m_ptOffset.x, -m_ptOffset.y);
}
void RenderOffsetClipHelper::Reset(HRDC hRDC)
{
	SelectClipRgn(hRDC, NULL);
	SetViewportOrgEx(hRDC,0,0,NULL);
}
void RenderOffsetClipHelper::Update(HRDC hRDC)
{
	SetViewportOrgEx(hRDC, m_ptOffset.x, m_ptOffset.y, NULL);

	CRect rcDevice = m_rcClip;
	rcDevice.OffsetRect(m_ptOffset.x, m_ptOffset.y); // 转换为设备坐标参数传递给CreateRectRgnIndirect

	HRGN hRgn = CreateRectRgnIndirect(&rcDevice);
	SelectClipRgn(hRDC, hRgn);
	SAFE_DELETE_GDIOBJECT(hRgn);
}

//////////////////////////////////////////////////////////////////////////
// 返回false时，表示这个对象已完全超出当前剪裁区域了，不需要再绘制
bool RenderOffsetClipHelper::DrawChild(Object* pChild, HRDC hRDC)
{
	if (NULL == pChild)
		return false;

	CRect rcParent;
	pChild->GetParentRect(&rcParent);

	CRect rcIntersect;
	if (!rcIntersect.IntersectRect(&m_rcClip, &rcParent) )
		return false;

	m_rcClip = rcIntersect;
	m_rcClip.OffsetRect(-rcParent.left, -rcParent.top);

	m_ptOffset.x += rcParent.left;
	m_ptOffset.y += rcParent.top;
	
	this->Update(hRDC);

	return true;
}

void RenderOffsetClipHelper::Scroll(HRDC hRDC, Object* pObjScroll, bool bUpdate)
{
	if (NULL == pObjScroll)
		return;
	
	int xOffset = 0, yOffset = 0;
	if (pObjScroll->GetScrollOffset(&xOffset, &yOffset))
	{
		m_ptOffset.x -= xOffset;
		m_ptOffset.y -= yOffset;
		OffsetRect(&m_rcClip, xOffset, yOffset);

		if (bUpdate)
			this->Update(hRDC);
	}
}

void RenderOffsetClipHelper::DrawClient(HRDC hRDC, Object* pObject, bool bUpdate)
{
	if (NULL == pObject)
		return;

	m_ptOffset.x += pObject->GetNonClientL();
	m_ptOffset.y += pObject->GetNonClientT();

	m_rcClip.right -= pObject->GetNonClientW();
	m_rcClip.bottom -= pObject->GetNonClientH();

	if (bUpdate)
		this->Update(hRDC);
}


//////////////////////////////////////////////////////////////////////////

IRenderResource::IRenderResource(IRenderResource** ppOutRef)
{
	m_dwRef = 0;
	if( NULL != ppOutRef )
	{
		*ppOutRef = this;
	}
	m_ppOutRef = ppOutRef;
}

IRenderResource::~IRenderResource()
{
	if(m_ppOutRef!=NULL)
	{
		*m_ppOutRef = NULL;
		m_ppOutRef = NULL;
	}
}

long IRenderResource::AddRef()
{
	m_dwRef ++;
	return m_dwRef;
}

long IRenderResource::Release()
{
	m_dwRef--;
	if( 0 >= m_dwRef )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}

//////////////////////////////////////////////////////////////////////////

IRenderBitmap::IRenderBitmap(IRenderResource** ppOutRef) : IRenderResource(ppOutRef)
{
}
IRenderFont::IRenderFont(IRenderResource** ppOutRef) : IRenderResource(ppOutRef)
{
}
 

//////////////////////////////////////////////////////////////////////////

IRenderDC::IRenderDC()
{
	m_hWnd = NULL;
}
IRenderDC::IRenderDC(HDC hDC)
{
	m_hWnd = NULL;
}	
IRenderDC::IRenderDC(HWND hWnd)
{
	m_hWnd = hWnd;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{

}

GRAPHICS_RENDER_TYPE GetGraphicsRenderType(HWND hWnd)
{
	if( NULL == hWnd )
		return GRAPHICS_RENDER_TYPE_GDI;

	GRAPHICS_RENDER_TYPE e = (GRAPHICS_RENDER_TYPE)::SendMessage( hWnd, UI_WM_GETRENDERTYPE, 0, 0 );
	if( GRAPHICS_RENDER_TYPE_AUTO == e )
	{
		if( WS_EX_LAYERED & ::GetWindowLong(hWnd, GWL_EXSTYLE) )
		{
			e = GRAPHICS_RENDER_TYPE_GDIPLUS;
		}
		else
		{
			e = GRAPHICS_RENDER_TYPE_GDI;
		}
	}

	return e;
}

GRAPHICS_RENDER_TYPE GetGraphicsRenderType(Object* pObj)
{
	if( NULL == pObj ) 
		return GRAPHICS_RENDER_TYPE_GDI;

	WindowBase* pWindow = pObj->GetWindowObject();
	if( NULL == pWindow )
		return GRAPHICS_RENDER_TYPE_GDI;
	
	GRAPHICS_RENDER_TYPE e = (GRAPHICS_RENDER_TYPE)UISendMessage( pWindow, UI_WM_GETRENDERTYPE );
	if( GRAPHICS_RENDER_TYPE_AUTO == e )
	{
		if( WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd, GWL_EXSTYLE) )
		{
			e = GRAPHICS_RENDER_TYPE_GDIPLUS;
		}
		else
		{
			e = GRAPHICS_RENDER_TYPE_GDI;
		}
	}

	return e;
}
//
//	因为需要根据窗口的类型来获取相应的DC，因此这里
//	也将HWND作为参数传递进来了。
//
HRDC RenderManager::GetHRDC( HDC hDC, HWND hWnd )
{
	UIASSERT(NULL!=hWnd);
	switch(GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		return (HRDC)new GDIRenderDC(hDC);

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		return (HRDC)new GdiplusRenderDC(hDC);
	}
	return NULL;
}
HRDC RenderManager::GetHRDC( HWND hWnd )
{
	switch(GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		return (HRDC)new GDIRenderDC(hWnd);
		
	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		return (HRDC)new GdiplusRenderDC(hWnd);

	}
	return NULL;
}

void RenderManager::ReleaseHRDC( HRDC hRDC )
{
	if( NULL != hRDC )
	{
		delete (IRenderDC*)hRDC;
	}
}

HRDC RenderManager::CreateRenderTarget( HWND hWnd, int nWidth, int nHeight )
{
	switch(GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			GDIMemRenderDC*  pMemDC = new GDIMemRenderDC(hWnd, nWidth, nHeight);
			return (HRDC)pMemDC;
		}
		break;

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			GdiplusMemRenderDC*  pMemDC = new GdiplusMemRenderDC(hWnd, nWidth, nHeight);
			return (HRDC)pMemDC;
		}
		break;
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
//
//		Render DC Api

HRDC GetHRDC(HDC hDC, HWND hWnd)
{
	return g_pUIApplication->m_RenderMgr.GetHRDC(hDC,hWnd);
}
HRDC GetHRDC(HWND hWnd)
{
	return g_pUIApplication->m_RenderMgr.GetHRDC(hWnd);
}
void ReleaseHRDC(HRDC hRDC)
{
	g_pUIApplication->m_RenderMgr.ReleaseHRDC(hRDC);
}
HRDC CreateCompatibleHRDC( HRDC hRDC, int nWidth, int nHeight )
{
	::OutputDebugString( _T("TODO: CreateCompatibleHRDC 删除该函数，不要再调用了。改用CreateRenderTarget\r\n"));
	if( NULL == hRDC )
	{
		return NULL;
	}
	return ((IRenderDC*)hRDC)->CreateCompatibleHRDC( nWidth, nHeight );
}


//
//	创建一个双缓冲DC
//
//	对于GDI/GDIPLUS，仅先创建BITMAP，在BeginDrag的时候再创建DC/Graphics
//	对于Direct2d，创建RenderTarget
//
HRDC CreateRenderTarget( HWND hWnd, int nWidth, int nHeight )
{
	return g_pUIApplication->m_RenderMgr.CreateRenderTarget( hWnd, nWidth, nHeight );
}

//
//	GDI: 保存hDC，用于Bitblt，创建CompatibleDC
//	GDIPLUS，创建Graphics::FromBitmap,创建Graphic for draw
//	Direct2d，不需要参数
//
bool BeginDraw(HRDC hRDC, HDC hDC)
{
	if( NULL == hRDC )
	{
		return false;
	}
	return ((IRenderDC*)hRDC)->BeginDraw(hDC);
}

void EndDraw(HRDC hRDC)
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->EndDraw();
}
void EndDraw( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->EndDraw(xDest,yDest,wDest,hDest,xSrc,ySrc);
}
void ResizeRenderTarget( HRDC hRDC, int nWidth, int nHeight )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->ResizeRenderTarget( nWidth, nHeight );
}

HDC GetHDC(HRDC hRDC)
{
	if( NULL == hRDC )
	{
		return NULL;
	}
	return ((IRenderDC*)hRDC)->GetHDC();
}
void ReleaseHDC( HRDC hRDC, HDC hDC )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->ReleaseHDC(hDC);
}


// HRFONT    SelectFont( HRDC hRDC, HRFONT hFont )
// {
// 	if( NULL == hRDC )
// 	{
// 		return NULL;
// 	}
// 	return ((IRenderDC*)hRDC)->SelectFont(hFont);
// }
// HRFONT    GetFont( HRDC hRDC )
// {
// 	if( NULL == hRDC )
// 	{
// 		return NULL;
// 	}
// 	return ((IRenderDC*)hRDC)->GetFont();
// }
HRGN GetClipRgn( HRDC hRDC )
{
	if( NULL == hRDC )
	{
		return NULL;
	}
	return ((IRenderDC*)hRDC)->GetClipRgn();
}
int SelectClipRgn( HRDC hRDC, HRGN hRgn, int nMode )
{
	if( NULL == hRDC )
	{
		return NULLREGION;
	}
	return ((IRenderDC*)hRDC)->SelectClipRgn(hRgn, nMode);
}
BOOL GetViewportOrgEx( HRDC hRDC, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderDC*)hRDC)->GetViewportOrgEx(lpPoint);
}
BOOL SetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderDC*)hRDC)->SetViewportOrgEx(x, y, lpPoint);
}
BOOL OffsetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderDC*)hRDC)->OffsetViewportOrgEx(x, y, lpPoint);
}

// COLORREF SetTextColor( HRDC hRDC, COLORREF color, byte Alpha)
// {
// 	if( NULL == hRDC )
// 	{
// 		return 0;
// 	}
// 	return ((IRenderDC*)hRDC)->SetTextColor(color, Alpha);
// }
// 
// COLORREF GetTextColor( HRDC hRDC )
// {
// 	if( NULL == hRDC )
// 	{
// 		return 0;
// 	}
// 	return ((IRenderDC*)hRDC)->GetTextColor( );
// }

UINT GetTextMetricsHeight( HRFONT hRFont )
{
	if( NULL == hRFont )
	{
		return 0;
	}

	return ((IRenderFont*)hRFont)->GetTextMetricsHeight( );
}

SIZE MeasureString( HRFONT hRFont, const TCHAR* szText, int nLimitWidth)
{
	if( NULL == hRFont )
	{
		SIZE s = {0,0};
		return s;
	}

	return ((IRenderFont*)hRFont)->MeasureString( szText, nLimitWidth );
}

BYTE* LockBits( HRDC hRDC )
{
	if( NULL == hRDC )
	{
		return NULL;
	}
	return ((IRenderDC*)hRDC)->LockBits();
}
void  UnlockBits( HRDC hRDC )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->UnlockBits();
}

void Save( HRDC hRDC, const String& strPath )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderDC*)hRDC)->Save(strPath);
}

int  DrawString( HRDC hRDC, const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == hRDC )
	{
		return 0;
	}
	return ((IRenderDC*)hRDC)->DrawString( szText, lpRect, nFormat, hRFont, col );
}

void  FillRgn( HRDC hRDC, HRGN hRgn, COLORREF col )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->FillRgn( hRgn, col );
}

void FillRect( HRDC hRDC, const CRect* lprc, COLORREF col)
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->FillRect( lprc, col );
}
void TileRect( HRDC hRDC, const CRect* lprc, HRBITMAP hBitmap )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->TileRect( lprc, hBitmap );
}
void   Rectangle( HRDC hRDC, const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->Rectangle( lprc, colBorder, colBack, nBorder, bNullBack );
}
void   DrawFocusRect( HRDC hRDC, const CRect* lprc )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->DrawFocusRect( lprc );
}

void   GradientFillH( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->GradientFillH( lprc, colFrom, colTo );
}
void   GradientFillV( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderDC*)hRDC)->GradientFillV( lprc, colFrom, colTo );
}
void BitBlt( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, HRDC hrdcSrc, int xSrc, int ySrc, DWORD dwRop )
{
	if( NULL == hRDC || NULL == hrdcSrc )
	{
		return;
	}
	IRenderDC* pDestRenderDC = (IRenderDC*)hRDC;
	IRenderDC* pSrcRenderDC = (IRenderDC*)hrdcSrc;
	if( pDestRenderDC->GetRenderType() != pSrcRenderDC->GetRenderType() )
		return;

	pDestRenderDC->BitBlt(xDest,yDest,wDest,hDest, pSrcRenderDC, xSrc,ySrc, dwRop );
}
void DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int x, int y )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderDC*)hRDC)->DrawBitmap( hBitmap, x, y );
}
void DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						 int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderDC*)hRDC)->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}


void DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						  int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						  Image9Region* p9Region )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderDC*)hRDC)->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}
void ImageList_Draw(HRDC hRDC, HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderDC*)hRDC)->ImageList_Draw( hBitmap, x,y,col,row,cx,cy );
}