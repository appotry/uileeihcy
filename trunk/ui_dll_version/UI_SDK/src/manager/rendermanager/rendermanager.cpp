#include "stdafx.h"

// pInitRect一般是取 window对象的 &m_rcParent.如果是只刷新一个控件，则直接指为NULL就行了，在begindraw中
// 会指定该对象的剪裁区域。
RenderOffsetClipHelper::RenderOffsetClipHelper(RECT* pInitRect)
{
	m_bUpdateClip = false;
	m_rcClip.SetRectEmpty();

	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

	if (NULL != pInitRect)
	{
		m_rcClip.CopyRect(pInitRect);
		m_bUpdateClip = true;
	}
}
void RenderOffsetClipHelper::Reset(IRenderTarget* pRenderTarget)
{
	pRenderTarget->SelectClipRgn(NULL);
	pRenderTarget->SetViewportOrgEx(0,0,NULL);
}
void RenderOffsetClipHelper::Update(IRenderTarget* pRenderTarget)
{
	pRenderTarget->SetViewportOrgEx(m_ptOffset.x, m_ptOffset.y, NULL);

	if (m_bUpdateClip)
	{
		HRGN hRgn = CreateRectRgnIndirect(&m_rcClip);
		SelectClipRgn(pRenderTarget, hRgn);
		SAFE_DELETE_GDIOBJECT(hRgn);
	}
}

//////////////////////////////////////////////////////////////////////////
// 返回false时，表示这个对象已完全超出当前剪裁区域了，不需要再绘制
bool RenderOffsetClipHelper::DrawChild(Object* pChild, IRenderTarget* pRenderTarget)
{
	if (NULL == pChild)
		return false;

	CRect rcWindow;
	pChild->GetWindowRect(&rcWindow);

	if (m_bUpdateClip)
	{
		CRect rcIntersect;
		if (!rcIntersect.IntersectRect(&m_rcClip, &rcWindow) )
			return false;

		m_rcClip = rcIntersect;
	}

	m_ptOffset.x = rcWindow.left;
	m_ptOffset.y = rcWindow.top;
	
	this->Update(pRenderTarget);
	return true;
}

void RenderOffsetClipHelper::Scroll(IRenderTarget* pRenderTarget, Object* pObjScroll, bool bUpdate)
{
	if (NULL == pObjScroll)
		return;
	
	int xOffset = 0, yOffset = 0;
	if (pObjScroll->GetScrollOffset(&xOffset, &yOffset))
	{
		m_ptOffset.x -= xOffset;
		m_ptOffset.y -= yOffset;

		if (bUpdate)
			this->Update(pRenderTarget);
	}
}

void RenderOffsetClipHelper::DrawClient(IRenderTarget* pRenderTarget, Object* pObject, bool bUpdate)
{
	if (NULL == pObject)
		return;

	CRegion4 r;
	pObject->GetNonClientRegion(&r);

	m_ptOffset.x += r.left;
	m_ptOffset.y += r.top;
	if (m_bUpdateClip)
	{
		Util::DeflatRect(&m_rcClip, &r);
	}

	if (bUpdate)
		this->Update(pRenderTarget);
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
IImageListRenderBitmap::IImageListRenderBitmap(IRenderResource**  ppOutRef) : IRenderBitmap(ppOutRef)
{

}
IRenderFont::IRenderFont(IRenderResource** ppOutRef) : IRenderResource(ppOutRef)
{
}

void RenderBitmapFactory::CreateInstance(IRenderBitmap** ppOut, GRAPHICS_RENDER_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, const String& strPath)
{
	if (NULL == ppOut)
		return;

	switch (eGraphicsRenderType)
	{	
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GDIIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GDIImageListRenderBitmap::CreateInstance(ppOut);
			}
// 			else if (eType == IMAGE_ITEM_TYPE_GIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
// 			else if (eType == IMAGE_ITEM_TYPE_PNGLISTGIF)
// 			{
// 				GDIGifRenderBitmap::CreateInstance(ppOut);
// 			}
			else 
			{
				GDIRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			if (eType == IMAGE_ITEM_TYPE_ICON)
			{
				GdiplusIconRenderBitmap::CreateInstance(ppOut);
			}
			else if (eType == IMAGE_ITEM_TYPE_IMAGE_LIST)
			{
				GdiplusImageListRenderBitmap::CreateInstance(ppOut);
			}
			else 
			{
				GdiplusRenderBitmap::CreateInstance(ppOut);
			}
		}
		break;

	case GRAPHICS_RENDER_TYPE_DIRECT2D:
		{
			Direct2DRenderBitmap::CreateInstance(ppOut);
		}
		break;
	}

// 	if (NULL != *ppOut)
// 	{
// 		(*ppOut)->SetAttribute(mapAttrib);
// 	} 
}
 

//////////////////////////////////////////////////////////////////////////

// IRenderTarget::IRenderTarget()
// {
// 	m_hWnd = NULL;
// }
// IRenderTarget::IRenderTarget(HDC hDC)
// {
// 	m_hWnd = NULL;
// }	
IRenderTarget::IRenderTarget(HWND hWnd)
{
	m_hWnd = hWnd;
}

void IRenderTarget::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

GRAPHICS_RENDER_TYPE GetGraphicsRenderType(HWND hWnd)
{
// #ifdef _DEBUG
// 	return GRAPHICS_RENDER_TYPE_GDIPLUS;
// #endif

	if (NULL == hWnd)
		return GRAPHICS_RENDER_TYPE_GDI;

	GRAPHICS_RENDER_TYPE e = (GRAPHICS_RENDER_TYPE)::SendMessage(hWnd, UI_WM_GETRENDERTYPE, 0, 0);
	if (GRAPHICS_RENDER_TYPE_AUTO == e)
	{
		if (WS_EX_LAYERED & ::GetWindowLong(hWnd, GWL_EXSTYLE))
		{
		//	if (UI_IsUnderXpOS())
				e = GRAPHICS_RENDER_TYPE_GDIPLUS;
// 			else
// 				e = GRAPHICS_RENDER_TYPE_DIRECT2D;
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
// #ifdef _DEBUG
// 	return GRAPHICS_RENDER_TYPE_GDIPLUS;
// #endif

	if (NULL == pObj) 
		return GRAPHICS_RENDER_TYPE_GDI;

	WindowBase* pWindow = pObj->GetWindowObject();
	if (NULL == pWindow)
	{
		// 针对menu,listbox popup类型一开始没有窗口的控件，向控件本身发消息进行获取
		GRAPHICS_RENDER_TYPE e = (GRAPHICS_RENDER_TYPE)UISendMessage( pObj, UI_WM_GETRENDERTYPE );
		if (GRAPHICS_RENDER_TYPE_AUTO == e)
		{
			e = GRAPHICS_RENDER_TYPE_GDI;
		}
		return e;
	}
	
	GRAPHICS_RENDER_TYPE e = (GRAPHICS_RENDER_TYPE)UISendMessage( pWindow, UI_WM_GETRENDERTYPE );
	if (GRAPHICS_RENDER_TYPE_AUTO == e)
	{
		if (WS_EX_LAYERED & ::GetWindowLong(pWindow->m_hWnd, GWL_EXSTYLE))
		{
		//	if (UI_IsUnderXpOS())
				e = GRAPHICS_RENDER_TYPE_GDIPLUS;
// 			else
// 				e = GRAPHICS_RENDER_TYPE_DIRECT2D;
		}
		else
		{
			e = GRAPHICS_RENDER_TYPE_GDI;
		}
	}

	return e;
}

//////////////////////////////////////////////////////////////////////////
//
//		Render DC Api
 

//
//	因为需要根据窗口的类型来获取相应的DC，因此这里
//	也将HWND作为参数传递进来了。
//
HRDC GetHRDC(HDC hDC, HWND hWnd)
{
	UIASSERT(0);  // TODO: 还有人用吗
#if 0
	UIASSERT(NULL!=hWnd);
	switch(GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		return (HRDC)new GdiRenderTarget(hDC);

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		return (HRDC)new GdiplusRenderDC(hDC);
	}
#endif
	return NULL;
}
HRDC GetHRDC(HWND hWnd)
{
	UIASSERT(0);  // TODO: 还有人用吗
#if 0
	switch(GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		return (HRDC)new GdiRenderTarget(hWnd);

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		return (HRDC)new GdiplusRenderDC(hWnd);

	}
#endif
	return NULL;
}
void ReleaseHRDC(HRDC hRDC)
{
	if( NULL != hRDC )
	{
		delete (IRenderTarget*)hRDC;
	}
}


//
//	创建一个双缓冲DC
//
//	对于GDI/GDIPLUS，仅先创建BITMAP，在BeginDrag的时候再创建DC/Graphics
//	对于Direct2d，创建RenderTarget
//
// HRDC CreateRenderTarget( HWND hWnd, int nWidth, int nHeight )
// {
// 	switch(GetGraphicsRenderType(hWnd))
// 	{
// 	case GRAPHICS_RENDER_TYPE_GDI:
// 		{
// 			GDIMemRenderDC*  pMemDC = new GDIMemRenderDC(hWnd, nWidth, nHeight);
// 			return (HRDC)pMemDC;
// 		}
// 		break;
// 
// 	case GRAPHICS_RENDER_TYPE_GDIPLUS:
// 		{
// 			GdiplusMemRenderDC*  pMemDC = new GdiplusMemRenderDC(hWnd, nWidth, nHeight);
// 			return (HRDC)pMemDC;
// 		}
// 		break;
// 		
// 	case GRAPHICS_RENDER_TYPE_DIRECT2D:
// 		{
// 			Direct2DRenderTarget* pMemDC = new Direct2DRenderTarget(hWnd, nWidth, nHeight);
// 			return (IRenderTarget*)pMemDC;
// 		}
// 		break;
// 	}
// 	return NULL;
// }

IRenderTarget* CreateRenderTarget(HWND hWnd)
{
	IRenderTarget*   pRenderTarget = NULL;

	switch (GetGraphicsRenderType(hWnd))
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			pRenderTarget = new GdiRenderTarget(hWnd);
		}
		break;
	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			pRenderTarget = new GdiplusRenderTarget(hWnd);
		}
		break;
	default:
		UIASSERT(0);
		break;
	}
	return pRenderTarget;
}

//
//	GDI: 保存hDC，用于Bitblt，创建CompatibleDC
//	GDIPLUS，创建Graphics::FromBitmap,创建Graphic for draw
//	Direct2d，不需要参数
//
// bool BeginDraw(HRDC hRDC, HDC hDC)
// {
// 	if( NULL == hRDC )
// 	{
// 		return false;
// 	}
// 	return ((IRenderTarget*)hRDC)->BeginDraw(hDC);
// }

// void EndDraw(HRDC hRDC)
// {
// 	if( NULL == hRDC )
// 	{
// 		return;
// 	}
// 	return ((IRenderTarget*)hRDC)->EndDraw();
// }
// void EndDraw( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish )
// {
// 	if( NULL == hRDC )
// 	{
// 		return;
// 	}
// 	return ((IRenderTarget*)hRDC)->EndDraw(xDest,yDest,wDest,hDest,xSrc,ySrc,bFinish);
// }
void ResizeRenderTarget( HRDC hRDC, int nWidth, int nHeight )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderTarget*)hRDC)->ResizeRenderTarget( nWidth, nHeight );
}

HDC GetHDC(HRDC hRDC)
{
	if( NULL == hRDC )
	{
		return NULL;
	}
	return ((IRenderTarget*)hRDC)->GetHDC();
}
void ReleaseHDC( HRDC hRDC, HDC hDC )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderTarget*)hRDC)->ReleaseHDC(hDC);
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
	return ((IRenderTarget*)hRDC)->GetClipRgn();
}
int SelectClipRgn( HRDC hRDC, HRGN hRgn, int nMode )
{
	if( NULL == hRDC )
	{
		return NULLREGION;
	}
	return ((IRenderTarget*)hRDC)->SelectClipRgn(hRgn, nMode);
}
BOOL GetViewportOrgEx( HRDC hRDC, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderTarget*)hRDC)->GetViewportOrgEx(lpPoint);
}
BOOL SetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderTarget*)hRDC)->SetViewportOrgEx(x, y, lpPoint);
}
BOOL OffsetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint )
{
	if( NULL == hRDC )
	{
		return FALSE;
	}
	return ((IRenderTarget*)hRDC)->OffsetViewportOrgEx(x, y, lpPoint);
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
	return ((IRenderTarget*)hRDC)->LockBits();
}
void  UnlockBits( HRDC hRDC )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderTarget*)hRDC)->UnlockBits();
}

void Save( HRDC hRDC, const String& strPath )
{
	if( NULL == hRDC )
	{
		return;
	}
	return ((IRenderTarget*)hRDC)->Save(strPath);
}

int  DrawString( HRDC hRDC, const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col )
{
	if( NULL == hRDC )
	{
		return 0;
	}
	return ((IRenderTarget*)hRDC)->DrawString( szText, lpRect, nFormat, hRFont, col );
}

void  FillRgn( HRDC hRDC, HRGN hRgn, COLORREF col )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->FillRgn( hRgn, col );
}

void FillRect( HRDC hRDC, const CRect* lprc, COLORREF col)
{
	if( NULL == hRDC )
	{
		return; 
	}
 	((IRenderTarget*)hRDC)->FillRect( lprc, col );
}
void TileRect( HRDC hRDC, const CRect* lprc, HRBITMAP hBitmap )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->TileRect( lprc, hBitmap );
}
void   Rectangle( HRDC hRDC, const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->Rectangle( lprc, colBorder, colBack, nBorder, bNullBack );
}
void   DrawFocusRect( HRDC hRDC, const CRect* lprc )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->DrawFocusRect( lprc );
}

void   GradientFillH( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->GradientFillH( lprc, colFrom, colTo );
}
void   GradientFillV( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == hRDC )
	{
		return;
	}
	((IRenderTarget*)hRDC)->GradientFillV( lprc, colFrom, colTo );
}
void BitBlt( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, HRDC hrdcSrc, int xSrc, int ySrc, DWORD dwRop )
{
	if( NULL == hRDC || NULL == hrdcSrc )
	{
		return;
	}
	IRenderTarget* pDestRenderDC = (IRenderTarget*)hRDC;
	IRenderTarget* pSrcRenderDC = (IRenderTarget*)hrdcSrc;
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
	((IRenderTarget*)hRDC)->DrawBitmap( hBitmap, x, y );
}
void DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						 int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderTarget*)hRDC)->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}


void DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						  int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						  Image9Region* p9Region )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderTarget*)hRDC)->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}
void ImageList_Draw(HRDC hRDC, HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hRDC || NULL == hBitmap )
	{
		return;
	}
	((IRenderTarget*)hRDC)->ImageList_Draw( hBitmap, x,y,col,row,cx,cy );
}