#include "stdafx.h"
#include "gdiplusbitmap.h"

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


//
//	Remark
//
//		Q: 为什么用Gdiplus在透明窗口上面绘制文字不能用alpha 255?
//		A: (从网上抄的，不一定正确)  alpha值会由于Red值的进位导致255变成0
//


void GdiplusRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return ;

	GdiplusRenderBitmap* p = new GdiplusRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

GdiplusImageListRenderBitmap::GdiplusImageListRenderBitmap()
{
	m_nCount = 0;
	m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
}
GdiplusImageListRenderBitmap::~GdiplusImageListRenderBitmap()
{

}
void GdiplusImageListRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusImageListRenderBitmap* p = new GdiplusImageListRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}
void GdiplusImageListRenderBitmap::SetAttribute(IMapAttribute* pMapAttrib)
{
	__super::SetAttribute(pMapAttrib);

    pMapAttrib->GetAttr_int(XML_IMAGE_IMAGELIST_COUNT, false, &m_nCount);

    const TCHAR* szText = pMapAttrib->GetAttr(XML_IMAGE_IMAGELIST_LAYOUT, false);
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_IMAGE_IMAGELIST_LAYOUT_H))
		{
			m_eLayout = IMAGELIST_LAYOUT_TYPE_H;
		}
		else if (0 == _tcscmp(szText, XML_IMAGE_IMAGELIST_LAYOUT_V))
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
int GdiplusImageListRenderBitmap::GetItemCount()
{
	return m_nCount;
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
GdiplusIconRenderBitmap::GdiplusIconRenderBitmap()
{
	m_nIconWidth = m_nIconHeight = 16;
	m_hBitmapToFixIcon = NULL;
}
GdiplusIconRenderBitmap::~GdiplusIconRenderBitmap()
{
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);
}
void GdiplusIconRenderBitmap::CreateInstance(IRenderBitmap** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	GdiplusIconRenderBitmap* p = new GdiplusIconRenderBitmap();
    p->AddRef();
	*ppOutRef = p;
}

void GdiplusIconRenderBitmap::SetAttribute(IMapAttribute* pMapAttrib)
{
	__super::SetAttribute(pMapAttrib);

    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_WIDTH,  false, &m_nIconWidth);
    pMapAttrib->GetAttr_int(XML_IMAGE_ICON_HEIGHT, false, &m_nIconHeight);
}

// 强制加载为支持32位alpha的
bool GdiplusIconRenderBitmap::LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel)
{
	SAFE_DELETE_GDIOBJECT(m_hBitmapToFixIcon);

	HICON hIcon = (HICON)::LoadImage (NULL, szPath, IMAGE_ICON,m_nIconWidth,m_nIconHeight, LR_LOADFROMFILE);
	if (NULL == hIcon)
		return false;
#if 1

#else
	HDC hMemDC = UI_GetCacheDC();

	Image image;
	image.Create(m_nIconWidth,m_nIconHeight, 32, Image::createAlphaChannel);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, (HBITMAP)image);

	::DrawIconEx(hMemDC, 0,0, hIcon, m_nIconWidth,m_nIconHeight, 0, NULL, DI_NORMAL);
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
#endif
	return true;
}
