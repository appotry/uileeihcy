#include "stdafx.h"
#include "gdiplusrender.h"
#include "UISDK\Kernel\Src\UIEffect\高斯模糊\gaussblur.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdirender.h"

GdiplusRenderTarget::GdiplusRenderTarget(HWND hWnd):IRenderTarget(hWnd)
{
	m_pGraphics = NULL;
	m_hDC = NULL;
	m_hRgnMeta = NULL;
}
GdiplusRenderTarget::~GdiplusRenderTarget()
{
	SAFE_DELETE(m_pGraphics);
	SAFE_DELETE_GDIOBJECT(m_hRgnMeta);
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
	UI_LOG_DEBUG(_T("%s advise: 该函数效率太低，最好不要调用"), FUNC_NAME);

	if (NULL == m_pGraphics)
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

// 该HDC不需要释放 
HDC GdiplusRenderTarget::GetBindHDC()
{
	return m_hDC;
}
void GdiplusRenderTarget::ReleaseHDC( HDC hDC)
{
	if (NULL == hDC)
		return;
	m_pGraphics->ReleaseHDC(hDC);
}

HRGN GdiplusRenderTarget::GetClipRgn()
{
	Gdiplus::Region region;
	m_pGraphics->GetClip(&region);

	return region.GetHRGN(m_pGraphics);
}
int GdiplusRenderTarget::SelectClipRgn( HRGN hRgn, int nMode)
{
	if (m_hDC)
	{
		if (RGN_COPY == nMode && NULL != hRgn)
			::SelectClipRgn(m_hDC, hRgn);
		else
			::ExtSelectClipRgn(m_hDC, hRgn, nMode);  // 为gdi类型带alpha通道的位图绘制准备的
	}

	if (NULL == m_pGraphics)
		return 0;

	if(NULL == hRgn)
	{
		Gdiplus::Status s = m_pGraphics->ResetClip();
		return (int)s;
	}

// 	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);   // 注意：在这里千万不要使用Region对象进行setclip，否则会导致当前的clip跟随vieworg而偏移
// 	if (NULL == pRegion)
// 		return ERROR;

	HRGN hRgnTemp = NULL;
	if (m_hRgnMeta)
	{
		hRgnTemp = CreateRectRgn(0,0,0,0);
		CombineRgn(hRgnTemp, hRgn, m_hRgnMeta, RGN_AND);
		hRgn = hRgnTemp;
	}
	Gdiplus::CombineMode mode = Gdiplus::CombineModeReplace;
	switch( nMode)
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
	Gdiplus::Status s = Gdiplus::Ok;
	s = m_pGraphics->SetClip(hRgn,mode);
	SAFE_DELETE_GDIOBJECT(hRgnTemp);

//	delete pRegion;
//	pRegion = NULL;

	return (int)s;
}
BOOL GdiplusRenderTarget::GetViewportOrgEx( LPPOINT lpPoint)
{
	if (NULL == lpPoint)
	{
		return FALSE;
	}

	Gdiplus::Matrix  m;
	m_pGraphics->GetTransform(&m);

	lpPoint->x = (int)m.OffsetX();
	lpPoint->y = (int)m.OffsetY();

	return TRUE;
}
BOOL GdiplusRenderTarget::OffsetViewportOrgEx( int xOffset, int yOffset, LPPOINT lpPoint) 
{
	if (lpPoint)
	{
		Gdiplus::Matrix  m;
		m_pGraphics->GetTransform(&m);

		lpPoint->x = (int)m.OffsetX();
		lpPoint->y = (int)m.OffsetY();
	}

	::OffsetViewportOrgEx(m_hDC, xOffset, yOffset, lpPoint);  // 为gdi类型带alpha通道的位图绘制准备的

	if (Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)xOffset,(Gdiplus::REAL)yOffset))
		return TRUE;
	else
		return FALSE;
}
BOOL GdiplusRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint )
{
	if (lpPoint)
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
	if (Gdiplus::Ok == m_pGraphics->TranslateTransform((Gdiplus::REAL)x,(Gdiplus::REAL)y))
		return TRUE;
	else
		return FALSE;
}

void GdiplusRenderTarget::BindHDC(HDC hDC)
{
	if (NULL == hDC)
		return;

	m_hDC = hDC;

// 	HBITMAP hMemBitmap = (HBITMAP) GetCurrentObject(m_hDC, OBJ_BITMAP);
// 	//	m_pGdiMemBitmap = new Gdiplus::Bitmap(hMemBitmap,NULL);  // 这种方式由于是重新创建一张位图，会大量消耗内存。
// 
// 	DIBSECTION  dibSection;
// 	GetObject(hMemBitmap, sizeof(DIBSECTION), &dibSection);
// 
// 	BYTE* pBits = (BYTE*)dibSection.dsBm.bmBits;
// 	pBits += (dibSection.dsBm.bmHeight-1)*dibSection.dsBm.bmWidthBytes;  // 将指针移到第一行数据位置
// 	m_pGdiMemBitmap = new Gdiplus::Bitmap(dibSection.dsBm.bmWidth, dibSection.dsBm.bmHeight, -dibSection.dsBm.bmWidthBytes, PixelFormat32bppARGB, (BYTE*)pBits);

}
bool  GdiplusRenderTarget::BeginDraw(RECT* prcArray, int nrcCount, bool bClear)
{
	bool bIsDrawing = NULL != m_pGraphics? true:false;

	if (false == bIsDrawing)
	{
		//	m_pGraphics = Gdiplus::Graphics::FromHDC(hDC);   // 采用这种方式创建出来的graphics，再调用gethdc，返回的就是原始的hDC，无法使用alpha通道
		m_pGraphics = new Gdiplus::Graphics(m_hDC/*m_pGdiMemBitmap*/);  // 使用内存图片创建出来的graphics，再调用gethdc，就能让HDC使用alpha通道 <-- 放弃！效率太低。直接使用HDC创建吧

        // 启用GDIPLUS的cleartype功能
        // m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        // 消除锯齿模式
        // m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		// 判断HDC是否被设置过了MetaRgn . ps. removed 20130206 在显示flash时，会频繁调用BeginDraw绘制，测试中发现
		// SelectClipRgn中的SetClip会导致效率大大的下降。因为设置meta rgn是为了加快速度的，结果却变的更慢了
		HRGN hRgnMeta = CreateRectRgn(0,0,0,0);
		if (::GetMetaRgn(m_hDC, hRgnMeta))
		{
			SelectClipRgn(hRgnMeta);
			m_hRgnMeta = hRgnMeta;
		}
		else
		{
			SAFE_DELETE_GDIOBJECT(hRgnMeta);
		}
	}
#if 1
	if (false == bIsDrawing)
	{
		if (nrcCount > 0 && NULL != prcArray)
		{
			RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT) ];
			pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
			pRgnData->rdh.iType      = RDH_RECTANGLES;
			pRgnData->rdh.nCount     = nrcCount;
			pRgnData->rdh.nRgnSize   = nrcCount*sizeof(RECT);
			for (int k = 0; k < nrcCount; k++)
			{
				RECT* prc = (RECT*)pRgnData->Buffer;
				prc[k] = prcArray[k];
			}

			HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nrcCount*sizeof(RECT), pRgnData);
			SAFE_ARRAY_DELETE(pRgnData);

			this->SelectClipRgn(hRgn, RGN_COPY);
			SAFE_DELETE_GDIOBJECT(hRgn);
		}
	}
#else
	if (NULL == prc && NULL == prc2)
	{
	}
	else if (prc && NULL != prc2)
	{
		HRGN hRgn = CreateRectRgnIndirect(prc);
		if (prc2)
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
#endif
	if (bClear && nrcCount > 0)
	{
		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		for (int i = 0; i < nrcCount; i++)
		{
			::FillRect(m_hDC, &(prcArray[i]), hBrush);
		}
	}
	return !bIsDrawing;
}
void GdiplusRenderTarget::EndDraw( )
{
// 	SAFE_DELETE(m_pGdiMemBitmap);
 	SAFE_DELETE(m_pGraphics);

	this->SetViewportOrgEx(0,0);
	this->SelectClipRgn(NULL);
	SAFE_DELETE_GDIOBJECT(m_hRgnMeta);
}

void GdiplusRenderTarget::Clear()
{
	if (m_pGraphics)
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
#if 0
int GdiplusRenderTarget::DrawString(const TCHAR* szText, const CRect* lpRect, UINT nFormat, IRenderFont* pRenderFont, Color col)
{
	if (NULL == lpRect || lpRect->Width() <= 0 || lpRect->Height() <= 0)
		return 0;

	if (NULL == m_pGraphics)
		return 0;

	if (NULL == pRenderFont)
	{
		UI_LOG_WARN(_T("%s pRenderFont == NULL"), FUNC_NAME);
		return -1;
	}

	if (((IRenderFont*)pRenderFont)->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
	{
		UI_LOG_WARN(_T("%s pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS"), FUNC_NAME);
		return -1;
	}

	Gdiplus::Font* pFont = ((GdiplusRenderFont*)pRenderFont)->GetFont();
	if (NULL == pFont)
	{
		UI_LOG_WARN(_T("%s NULL == pFont"), FUNC_NAME);
		return -1;
	}

	Gdiplus::RectF region(
		(Gdiplus::REAL)(lpRect->left),
		(Gdiplus::REAL)(lpRect->top), 
		(Gdiplus::REAL)(lpRect->Width()), 
		(Gdiplus::REAL)(lpRect->Height()));

	Gdiplus::Color colText;
	colText.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(col), GetGValue(col), GetBValue(col)));
	Gdiplus::SolidBrush textBrush(colText);

    // 启用GDIPLUS的cleartype功能
    // m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    // 消除锯齿模式
    // m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
	if (nFormat & DT_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if (nFormat & DT_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if (nFormat & DT_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	else if (nFormat & DT_BOTTOM)
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
#endif
void  GdiplusRenderTarget::DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam)
{
    this->DrawStringEx(m_pGraphics, m_hDC, pFont, pParam);
}

// 	m_pGraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
// 	m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeInvalid);

// Gdiplus 文字特效 http://www.codeproject.com/Articles/42529/Outline-Text
/*static*/
void  GdiplusRenderTarget::DrawStringEx(Gdiplus::Graphics* pGraphics, HDC hBindDC, IRenderFont* pRenderFont, DRAWTEXTPARAM* pParam)
{
    if (NULL == pGraphics || NULL == pRenderFont || NULL == pParam)
        return;

    if (pParam->prc->Width() <= 0 || pParam->prc->Height() <= 0)
        return;

    if (((IRenderFont*)pRenderFont)->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
    {
        UI_LOG_WARN(_T("%s pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS"), FUNC_NAME);
        return;
    }

    Gdiplus::Font* pFont = ((GdiplusRenderFont*)pRenderFont)->GetFont();
    if (NULL == pFont)
    {
        UI_LOG_WARN(_T("%s NULL == pFont"), FUNC_NAME);
        return;
    }

    Gdiplus::RectF region(
        (Gdiplus::REAL)(pParam->prc->left),
        (Gdiplus::REAL)(pParam->prc->top), 
        (Gdiplus::REAL)(pParam->prc->Width()), 
        (Gdiplus::REAL)(pParam->prc->Height()));

    Gdiplus::Color colText;
    int a = pParam->color.a;
    if (a >= 254)
        a = 254;  // 防止穿透

    colText.SetValue(Gdiplus::Color::MakeARGB(a, pParam->color.r, pParam->color.g, pParam->color.b));
    Gdiplus::SolidBrush textBrush(colText);

    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    GetStringFormatByGdiFormat(pParam->nFormatFlag, &format);

    if (TEXT_EFFECT_NONE == pParam->nEffectFlag)
    {
        Gdiplus::TextRenderingHint eOldHint = pGraphics->GetTextRenderingHint();
        pGraphics->SetTextRenderingHint(((GdiplusRenderFont*)pRenderFont)->GetTextRenderingHint());

        // Draw string.
       /* Gdiplus::Status s = */pGraphics->DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            region,
            &format,
            &textBrush);

       pGraphics->SetTextRenderingHint(eOldHint);
    }
    else if (TEXT_EFFECT_HALO == pParam->nEffectFlag)
    {
#if 0
        RECT rcRealInMemBmp = *(pParam->prc); // 不考虑缩放，仅考虑偏移
        ::LPtoDP(hBindDC, (LPPOINT)&rcRealInMemBmp, 2);
        HBITMAP hMemBmp = (HBITMAP)::GetCurrentObject(hBindDC, OBJ_BITMAP);

        Gdiplus::TextRenderingHint eOldHint = pGraphics->GetTextRenderingHint();
        pGraphics->SetTextRenderingHint(((GdiplusRenderFont*)pRenderFont)->GetTextRenderingHint());

        Gdiplus::Color colorTextShadow;
        int a = pParam->bkcolor.a;
        if (a >= 254)
            a = 254;  // 防止穿透
        colorTextShadow.SetValue(Gdiplus::Color::MakeARGB(a, pParam->bkcolor.r, pParam->bkcolor.g, pParam->bkcolor.b));
        Gdiplus::SolidBrush textShadowBrush(colorTextShadow);

        // 阴影
        pGraphics->DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            region,
            &format,
            &textShadowBrush);

        // 模糊
        recursive_blur blur;
        blur.blur(hMemBmp, (double)pParam->wParam, (LPRECT)&rcRealInMemBmp);

        // 文字
        pGraphics->DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            region,
            &format,
            &textBrush);

        pGraphics->SetTextRenderingHint(eOldHint);
#else
        HDC hMemDC = CreateCompatibleDC(NULL);
        SetBkMode(hMemDC, TRANSPARENT);

        int nWidth = pParam->prc->Width();
        int nHeight = pParam->prc->Height();
        RECT rcMem = { 0, 0, pParam->prc->Width(), pParam->prc->Height() };
        Gdiplus::RectF regionMem(
            (Gdiplus::REAL)0,
            (Gdiplus::REAL)0, 
            (Gdiplus::REAL)nWidth, 
            (Gdiplus::REAL)nHeight);

        HBITMAP hMemBmp = CacheBitmap::GetInstance()->Create(nWidth, nHeight);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);
        CacheBitmap::GetInstance()->Clear(0 , &rcMem);

        Gdiplus::Graphics g(hMemDC);
        Gdiplus::TextRenderingHint eOldHint = pGraphics->GetTextRenderingHint();
        g.SetTextRenderingHint(((GdiplusRenderFont*)pRenderFont)->GetTextRenderingHint());

        Gdiplus::Color colorTextShadow;
        int a = pParam->bkcolor.a;
        if (a >= 254)
            a = 254;  // 防止穿透
        colorTextShadow.SetValue(Gdiplus::Color::MakeARGB(a, pParam->bkcolor.r, pParam->bkcolor.g, pParam->bkcolor.b));
        Gdiplus::SolidBrush textShadowBrush(colorTextShadow);

        // 阴影
        g.DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            regionMem,
            &format,
            &textShadowBrush);

        // 模糊
        recursive_blur<recursive_blur_calc_rgba<> > blur;
        blur.blur(hMemBmp, (double)pParam->wParam, (LPRECT)&rcMem, 0);

        // 文字
        g.DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            regionMem,
            &format,
            &textBrush);

        g.SetTextRenderingHint(eOldHint);
    
       
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        ::AlphaBlend(hBindDC, pParam->prc->left, pParam->prc->top, nWidth, nHeight, hMemDC, 0, 0, nWidth, nHeight, bf);
        ::SelectObject(hMemDC, hOldBmp);
        ::DeleteDC(hMemDC);

#endif
#if 0
        // 准备一幅小图，用于将该图片放大以产生插值拉伸效果（即光晕）

        int nLevel = pParam->lParam;
        if (nLevel <= 0 || nLevel > 20)
            nLevel = 5;

        Gdiplus::Bitmap  smallBitmap(pParam->prc->Width()/nLevel, pParam->prc->Height()/nLevel);
        Gdiplus::Graphics*  pSmallGraphics = Gdiplus::Graphics::FromImage(&smallBitmap);

        Gdiplus::GraphicsPath path;
        Gdiplus::FontFamily fontFamily;
        pFont->GetFamily(&fontFamily);

        Gdiplus::RectF  rcPath = region;
        rcPath.Offset(-region.X, -region.Y);
        path.AddString(pParam->szText, -1, &fontFamily, pFont->GetStyle(), (Gdiplus::REAL)-Util::FontSize2Height((int)pFont->GetSize()), rcPath, &format);

        // 用一个matrix实现使用现有的字体大小绘制缩小版的文字
        Gdiplus::Matrix  matrix(1.0f/nLevel, 0, 0, 1.0f/nLevel, -(1.0f/nLevel),-(1.0f/nLevel));
        pSmallGraphics->SetTransform(&matrix);
        pSmallGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        Gdiplus::Pen pen(Gdiplus::Color(64, 255, 255, 255), 1);
        pSmallGraphics->DrawPath(&pen, &path);
        pSmallGraphics->FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(64, 255, 255, 255)), &path);

// 
//         Gdiplus::RectF  smallRect(0, 0,
//             (Gdiplus::REAL)smallBitmap.GetWidth(), 
//             (Gdiplus::REAL)smallBitmap.GetHeight());

//         pSmallGraphics->Clear(Gdiplus::Color::Black);
//         pSmallGraphics->DrawString(
//             pParam->szText,
//             -1/*_tcslen(szText)*/,
//             pFont,
//             smallRect,
//             &format,
//             &textBrush);

        // 提前到真正的graphics上

        pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        pGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

        pGraphics->DrawImage(
            &smallBitmap, 
            region, 
            0, 0, 
            (Gdiplus::REAL)smallBitmap.GetWidth(), 
            (Gdiplus::REAL)smallBitmap.GetHeight(), 
            Gdiplus::UnitPixel);

//        pGraphics->FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::Black), &path);
        pGraphics->DrawString(
            pParam->szText,
            -1/*_tcslen(szText)*/,
            pFont,
            region,
            &format,
            &textBrush);
        SAFE_DELETE(pSmallGraphics);
#endif
    }
}

void  GdiplusRenderTarget::GetStringFormatByGdiFormat(UINT nFormatFlag, Gdiplus::StringFormat* p)
{
    if (nFormatFlag & DT_CENTER)
    {
        p->SetAlignment(Gdiplus::StringAlignmentCenter);
    }
    else if (nFormatFlag & DT_RIGHT)
    {
        p->SetAlignment(Gdiplus::StringAlignmentFar);
    }
    if (nFormatFlag & DT_VCENTER)
    {
        p->SetLineAlignment(Gdiplus::StringAlignmentCenter);
    }
    else if (nFormatFlag & DT_BOTTOM)
    {
        p->SetLineAlignment(Gdiplus::StringAlignmentFar);
    }
    if (nFormatFlag & DT_SINGLELINE)
    {
        p->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
    }
    if (nFormatFlag & DT_END_ELLIPSIS)
    {
        p->SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    }
}


IRenderPen* GdiplusRenderTarget::CreateSolidPen(int nWidth, Color* pColor)
{
    IRenderPen* p = NULL;
    GdiplusPen::CreateInstance(&p);

    if (p)
    {
        p->CreateSolidPen(nWidth, pColor);
    }
    return p;
}
IRenderPen* GdiplusRenderTarget::CreateDotPen(int nWidth, Color* pColor)
{
    IRenderPen* p = NULL;
    GdiplusPen::CreateInstance(&p);

    if (p)
    {
        p->CreateDotPen(nWidth, pColor);
    }
    return p;
}
IRenderBrush*  GdiplusRenderTarget::CreateSolidBrush(Color* pColor)
{
    UIASSERT(0);
    return NULL;
}


void GdiplusRenderTarget::FillRgn(HRGN hRgn, UI::Color* pColor)
{
	if (NULL == hRgn || NULL == pColor)
		return;

	Gdiplus::Region* pRegion = Gdiplus::Region::FromHRGN(hRgn);
	if (NULL == pRegion)
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(pColor->a, pColor->r, pColor->g, pColor->b)) ;
	Gdiplus::SolidBrush brush(color);

	m_pGraphics->FillRegion(&brush, pRegion);

	delete pRegion;
	pRegion = NULL;
}


// 在给gdiplus设置了消除锯齿模式后，会导致绘制出来的rect范围不精确，有虚边效果。
// m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
// 例如一个10*10的矩形，就画出11*11的效果，并且四周有1px的虚边用于抗锯齿
void GdiplusRenderTarget::FillRect(const CRect* lprc, UI::Color* pColor)
{
	if (NULL == lprc || NULL == pColor)
		return;

	Gdiplus::Color color;
	color.SetValue(Gdiplus::Color::MakeARGB(pColor->a, pColor->r, pColor->g, pColor->b));
	Gdiplus::SolidBrush brush(color);

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height());
}

void GdiplusRenderTarget::TileRect(const CRect* lprc, IRenderBitmap* hBitmap)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	Gdiplus::TextureBrush brush(pBitmap);
	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height());
}

void GdiplusRenderTarget::Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack)
{
	if (NULL == m_pGraphics)
		return;

    CRect rc(lprc);
	if (false == bNullBack)
	{
		this->FillRect(&rc, pColBack);
	}

    if (pColBorder)
    {
	    Gdiplus::Color color;
	    color.SetValue(Gdiplus::Color::MakeARGB(pColBorder->a, pColBorder->r, pColBorder->g, pColBorder->b)) ;
	    Gdiplus::Pen pen(color, (Gdiplus::REAL)nBorder);

	    m_pGraphics->DrawRectangle(&pen,   
		    rc.left, rc.top, 
		    rc.Width(), rc.Height()  
		    );  
    }
}

void GdiplusRenderTarget::DrawFocusRect(const CRect* lprc)
{
	if (NULL == m_pGraphics)
		return;

	Gdiplus::Pen p( Gdiplus::Color(254,0,0,0));
	p.SetDashStyle( Gdiplus::DashStyleDot);
	m_pGraphics->DrawRectangle(&p,   
		lprc->left, lprc->top, 
		lprc->Width()-1, lprc->Height()-1   // Remark:在这里如果不减1的话，会导致右、下的边框显示不全，原因未知
		);  
}

void GdiplusRenderTarget::DrawLine(int x1, int y1, int x2, int y2, IRenderPen* pPen)
{
	if (NULL == pPen)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusPen* pGdiplusPen = (GdiplusPen*)pPen;

	Gdiplus::Point p1(x1, y1);
	Gdiplus::Point p2(x2, y2);
	m_pGraphics->DrawLine(pGdiplusPen->m_pPen, p1, p2);
}
void GdiplusRenderTarget::DrawPolyline(POINT* lppt, int nCount, IRenderPen* pPen)
{
	if (NULL == pPen || NULL == lppt)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusPen* pGdiplusPen = (GdiplusPen*)pPen;
	Gdiplus::Point *p = new Gdiplus::Point[nCount];
	for (int i = 0; i < nCount; i++)
	{
		p[i].X = lppt[i].x;
		p[i].Y = lppt[i].y;
	}
	m_pGraphics->DrawLines(pGdiplusPen->m_pPen, p, nCount);
	SAFE_ARRAY_DELETE(p);
}

void GdiplusRenderTarget::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo)
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height());
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeHorizontal);

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height());
}

void GdiplusRenderTarget::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo)
{
	Gdiplus::Color colorFrom, colorTo;
	colorFrom.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colFrom),GetGValue(colFrom),GetBValue(colFrom))) ;
	colorTo.SetValue(Gdiplus::Color::MakeARGB(254,GetRValue(colTo),GetGValue(colTo),GetBValue(colTo))) ;

	Gdiplus::Rect rect( lprc->left, lprc->top, lprc->Width(), lprc->Height());
	Gdiplus::LinearGradientBrush  brush(rect, colorFrom, colorTo, Gdiplus::LinearGradientModeVertical);

	m_pGraphics->FillRectangle(&brush, lprc->left, lprc->top, lprc->Width(), lprc->Height());
}

void GdiplusRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop)
{
	UIASSERT(0);
	// TODO: 该函数还会被调用吗

// 	if (NULL == pSrcHDC)
// 		return;
// 	if (pSrcHDC->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
// 		return;
// 
// 	GdiplusMemRenderDC* pMemRDC = (GdiplusMemRenderDC*)pSrcHDC;
// 	m_pGraphics->DrawImage(pMemRDC->GetMemBitmap(), xDest,yDest,xSrc,ySrc, wDest,hDest, Gdiplus::UnitPixel);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int x, int y)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	pGraphics->DrawImage( pBitmap,x, y, pBitmap->GetWidth(), pBitmap->GetHeight());
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if (NULL == pBitmap)
		return;

	if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = static_cast<GdiplusRenderBitmap*>(pBitmap);
	
	Gdiplus::Bitmap* p = pRenderBitmap->GetBitmap();
	if (NULL == p)
		return;

	pGraphics->DrawImage( p, xDest, yDest, xSrc, ySrc, pBitmap->GetWidth(), pBitmap->GetHeight(), Gdiplus::UnitPixel);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, Gdiplus::ImageAttributes* pAttr)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	Gdiplus::RectF rcDst( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, (Gdiplus::REAL)nDestWidth, (Gdiplus::REAL)nDestHeight);
	pGraphics->DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)nSrcWidth, (Gdiplus::REAL)nSrcHeight, Gdiplus::UnitPixel, pAttr);
}

void GdiplusRenderTarget::DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					Image9Region* pImage9Region, bool bDrawCenter, Gdiplus::ImageAttributes* pAttr)
{
	if (NULL == hBitmap)
		return;

	if (NULL == pImage9Region)
		return GdiplusRenderTarget::DrawBitmap(pGraphics, hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, pAttr);

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;


	// 1. topleft
	Gdiplus::RectF rcDst1( (Gdiplus::REAL)xDest, (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)pImage9Region->topleft, (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst1, 

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)ySrc,
		(Gdiplus::REAL)pImage9Region->topleft,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel, pAttr
		);

	// 2. top
	Gdiplus::RectF rcDst2( (Gdiplus::REAL)(xDest + pImage9Region->topleft), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->topleft - pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst2,

		(Gdiplus::REAL)(xSrc + pImage9Region->topleft),                                         // xSrc
		(Gdiplus::REAL)ySrc,                                                               // ySrc
		(Gdiplus::REAL)(nSrcWidth - pImage9Region->topleft - pImage9Region->topright),       // nSrcWidth
		(Gdiplus::REAL)pImage9Region->top ,

		Gdiplus::UnitPixel, pAttr
		);                                                 

	// 3. topright
	Gdiplus::RectF rcDst3( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->topright), (Gdiplus::REAL)yDest, 
							(Gdiplus::REAL)(pImage9Region->topright), (Gdiplus::REAL)pImage9Region->top);
	pGraphics->DrawImage( pBitmap, rcDst3,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->topright),
		(Gdiplus::REAL)ySrc, 
		(Gdiplus::REAL)pImage9Region->topright,
		(Gdiplus::REAL)pImage9Region->top,

		Gdiplus::UnitPixel, pAttr
		);

	// 4. left
	Gdiplus::RectF rcDst4( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->left), (Gdiplus::REAL)(nDestHeight - pImage9Region->top- pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst4,

		(Gdiplus::REAL)xSrc ,                                                        // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)pImage9Region->left,                                         // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),      // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 5. center

	if (bDrawCenter)
	{
		Gdiplus::RectF rcDst5( (Gdiplus::REAL)(xDest + pImage9Region->left), (Gdiplus::REAL)(yDest + pImage9Region->top), 
								(Gdiplus::REAL)(nDestWidth  - pImage9Region->left - pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top  - pImage9Region->bottom));
		pGraphics->DrawImage( pBitmap, rcDst5,

			(Gdiplus::REAL)(xSrc + pImage9Region->left),                                         // xSrc
			(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
			(Gdiplus::REAL)(nSrcWidth  - pImage9Region->left - pImage9Region->right),           // nSrcWidth
			(Gdiplus::REAL)(nSrcHeight - pImage9Region->top  - pImage9Region->bottom),       // nSrcHeight

			Gdiplus::UnitPixel, pAttr
			); 
	}

	// 6. right
	Gdiplus::RectF rcDst6( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->right), (Gdiplus::REAL)(yDest + pImage9Region->top), 
							(Gdiplus::REAL)(pImage9Region->right), (Gdiplus::REAL)(nDestHeight - pImage9Region->top - pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst6,

		(Gdiplus::REAL)(xSrc + nSrcWidth - pImage9Region->right),                            // xSrc
		(Gdiplus::REAL)(ySrc + pImage9Region->top),                                          // ySrc
		(Gdiplus::REAL)(pImage9Region->right),                                        // nSrcWidth
		(Gdiplus::REAL)(nSrcHeight - pImage9Region->top - pImage9Region->bottom),        // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 7. bottomleft
	Gdiplus::RectF rcDst7( (Gdiplus::REAL)(xDest), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomleft), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst7,

		(Gdiplus::REAL)xSrc, 
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom),
		(Gdiplus::REAL)pImage9Region->bottomleft, 
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel, pAttr
		);

	// 8. bottom
	Gdiplus::RectF rcDst8( (Gdiplus::REAL)(xDest + pImage9Region->bottomleft), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(nDestWidth - pImage9Region->bottomleft - pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst8,

		(Gdiplus::REAL)xSrc + pImage9Region->bottomleft,                                   // xSrc
		(Gdiplus::REAL)ySrc + nSrcHeight - pImage9Region->bottom,                          // ySrc
		(Gdiplus::REAL)nSrcWidth - pImage9Region->bottomleft - pImage9Region->bottomright, // nSrcWidth
		(Gdiplus::REAL)pImage9Region->bottom,                                         // nSrcHeight

		Gdiplus::UnitPixel, pAttr
		);

	// 9. bottomright
	Gdiplus::RectF rcDst9( (Gdiplus::REAL)(xDest + nDestWidth - pImage9Region->bottomright), (Gdiplus::REAL)(yDest + nDestHeight - pImage9Region->bottom), 
							(Gdiplus::REAL)(pImage9Region->bottomright), (Gdiplus::REAL)(pImage9Region->bottom));
	pGraphics->DrawImage( pBitmap, rcDst9,

		(Gdiplus::REAL)(xSrc + nSrcWidth  - pImage9Region->bottomright),
		(Gdiplus::REAL)(ySrc + nSrcHeight - pImage9Region->bottom), 
		(Gdiplus::REAL)pImage9Region->bottomright,
		(Gdiplus::REAL)pImage9Region->bottom,

		Gdiplus::UnitPixel, pAttr
		);
}
void GdiplusRenderTarget::ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	GdiplusRenderBitmap* pRenderBitmap = (GdiplusRenderBitmap*)p;
	if (NULL == pRenderBitmap)
		return;

	Gdiplus::Bitmap* pBitmap = pRenderBitmap->GetBitmap();
	if (NULL == pBitmap)
		return;

	int xSrc = col * cx;
	int ySrc = row * cy;

// 	if (xSrc > pBitmap->GetWidth())
// 		return false;
// 	if (ySrc > pBitmap->GetHeight())
// 		return false;

	//	m_pGraphics->DrawImage(pBitmap, x,y,xSrc,ySrc, cx,cy, Gdiplus::UnitPixel);  <-- 使用这个版本会导致图片被放大...原因未知
	Gdiplus::RectF rcDst( (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy);
	m_pGraphics->DrawImage( pBitmap, rcDst, (Gdiplus::REAL)xSrc, (Gdiplus::REAL)ySrc, (Gdiplus::REAL)cx, (Gdiplus::REAL)cy, Gdiplus::UnitPixel);
}


void GdiplusRenderTarget::DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == hBitmap || NULL == pParam)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;

	if (p->GetGraphicsRenderLibraryType() == GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
		GdiRenderTarget::DrawBitmapEx(m_hDC, hBitmap, pParam);
		return;
	}

	GdiplusRenderTarget::DrawBitmapEx(m_pGraphics, m_hDC, p, pParam);
}

// [注]:这里的alpha绘制效率很低，特别是在实现动画渐变效果时，效果很不好，最好还是用gdi的alpha blend
void GdiplusRenderTarget::DrawBitmapEx(Gdiplus::Graphics* pGraphics, HDC hBindDC, IRenderBitmap* p, DRAWBITMAPPARAM* pParam)
{
	if (NULL == pGraphics || p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
		return;

	Gdiplus::Bitmap* pBitmap = ((GdiplusRenderBitmap*)p)->GetBitmap();
	if (NULL == pBitmap)
		return;

	// 利用颜色矩阵来直接绘制灰度图
	Gdiplus::ImageAttributes* pImageAttribute = NULL;
	Gdiplus::ImageAttributes  imageAttribute;

	float fAlphaBlend = 1.0f;
	if (pParam->nAlpha != 255)
		fAlphaBlend = pParam->nAlpha / 255.0f;

	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		const Gdiplus::REAL r = (Gdiplus::REAL)0.3;
		const Gdiplus::REAL g = (Gdiplus::REAL)0.59;
		const Gdiplus::REAL b = (Gdiplus::REAL)0.11;
		const Gdiplus::ColorMatrix matrix =
				{r,    r,   r,     0.0f,  0.0f,      // Red
				 g,    g,    g,    0.0f,  0.0f,      // Green
				 b,    b,    b,    0.0f,  0.0f,      // Blue
				 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
				 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};     
		imageAttribute.SetColorMatrix(&matrix);
		pImageAttribute = &imageAttribute;
	}
	else if (pParam->nAlpha != 255)
	{
		const Gdiplus::ColorMatrix matrix =
				{1.0f, 0.0f, 0.0f,  0.0f,  0.0f,      // Red
				 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,      // Green
				 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,      // Blue
				 0.0f, 0.0f, 0.0f, fAlphaBlend,  0.0f,      // Alpha
				 0.0f, 0.0f, 0.0f, 0.0f,  1.0f};     
		imageAttribute.SetColorMatrix(&matrix);
		pImageAttribute = &imageAttribute;
	}

	if (pParam->nFlag & DRAW_BITMAP_BITBLT)
	{
		Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
		Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);
		Gdiplus::RectF destRect((Gdiplus::REAL)pParam->xDest, (Gdiplus::REAL)pParam->yDest, nW, nH);
		pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+(int)nW, pParam->yDest+(int)nH);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		GdiplusRenderTarget::DrawBitmap(pGraphics, p, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest,
			pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
	{
		GdiplusRenderTarget::DrawBitmap(pGraphics, p, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest,
			pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, false, pImageAttribute);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		Gdiplus::TextureBrush brush(pBitmap);
		pGraphics->FillRectangle(&brush, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		Gdiplus::RectF destRect((Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
		pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(x, y, x+pParam->wSrc, y+pParam->hSrc);
        }
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

			if (tan_x_y_image > tan_x_y_dest) // 横向占满
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

		if (bNeedToStretch)
		{
			GdiplusRenderTarget::DrawBitmap(pGraphics, p, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);
		}
		else
		{
			Gdiplus::RectF destRect((Gdiplus::REAL)xDisplayPos, (Gdiplus::REAL)yDisplayPos, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc);
			pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, (Gdiplus::REAL)pParam->wSrc, (Gdiplus::REAL)pParam->hSrc, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);
		}

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDisplayPos, yDisplayPos, xDisplayPos+wImage, yDisplayPos+hImage);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT_RIGHTTOP)
    {
        Gdiplus::REAL nW = (Gdiplus::REAL)min(pParam->wSrc,pParam->wDest);
        Gdiplus::REAL nH = (Gdiplus::REAL)min(pParam->hSrc,pParam->hDest);

        int xDest = pParam->xDest + (pParam->wDest-(int)nW);
        int xSrc = pParam->xSrc+ (pParam->wSrc-(int)nW);

        Gdiplus::RectF destRect((Gdiplus::REAL)xDest, (Gdiplus::REAL)pParam->yDest, nW, nH);
        pGraphics->DrawImage(pBitmap, destRect, (Gdiplus::REAL)pParam->xSrc, (Gdiplus::REAL)pParam->ySrc, nW, nH, Gdiplus::UnitPixel, pImageAttribute, NULL, NULL);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDest, pParam->yDest, xDest+(int)nW, pParam->yDest+(int)nH);
        }
    }
    else if (pParam->nFlag & DRAW_BITMAP_STRETCH_ROUNDCORNER)
    {
        HDC hDC = Image::GetCacheDC();
        HBITMAP hCacheBitmap = CacheBitmap::GetInstance()->Create(pParam->wDest, pParam->hDest);
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hCacheBitmap);

        // 绘制到临时位置
        {
            Gdiplus::Graphics g(hDC);
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);  // 抗锯齿

            Gdiplus::ImageAttributes attr;
            if (!pImageAttribute)
                pImageAttribute = &attr;
            pImageAttribute->SetWrapMode(Gdiplus::WrapModeTileFlipXY);   // 不加这句边缘绘制出来有问题

            GdiplusRenderTarget::DrawBitmap(&g, p, 0, 0, pParam->wDest, pParam->hDest,
                pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pImageAttribute);
        }

        // 圆角处理
        int& w = pParam->wDest;
        int& h = pParam->hDest;
        if (w > 8 && h > 8)
        {
            BITMAP bm = {0};
            ::GetObject(hCacheBitmap, sizeof(bm), &bm);
            byte* pBits = (byte*)bm.bmBits;
            int   nPitch = bm.bmWidthBytes;

            // 25%透
            POINT ptAlpha0[4] = { {0, 0}, {w-1, 0}, {0, h-1}, {w-1, h-1} };
            for (int i = 0; i < 4; i++)
            {
                byte* pAlpha = pBits + (ptAlpha0[i].y*nPitch + (ptAlpha0[i].x<<2));
                byte a = ((int)*(pAlpha+3)) >> 4;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = a;
            }

            // 50%透
            POINT ptAlpha1[8] = { {1, 0}, {w-2, 0}, {0, 1}, {w-1,1},  {0, h-2}, {w-1, h-2}, {1, h-1}, {w-2, h-1} };
            for (int i = 0; i < 8; i++)
            {
                byte* pAlpha = pBits + (ptAlpha1[i].y*nPitch + (ptAlpha1[i].x<<2));
                byte a = ((int)*(pAlpha+3)) >> 2;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = a;
            }

            // 75%
            POINT ptAlpha2[8] = { {2, 0}, {w-3, 0}, {0, 2}, {w-1,2},  {0, h-3}, {w-1, h-3}, {2, h-1}, {w-3, h-1} };
            for (int i = 0; i < 8; i++)
            {
                byte* pAlpha = pBits + (ptAlpha2[i].y*nPitch + (ptAlpha2[i].x<<2));
                byte a = ((int)*(pAlpha+3)) * 3 >> 2;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = (*pAlpha) * a >> 8;
                pAlpha++;
                *pAlpha = a;
            }
        }

        // 提交
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, pParam->nAlpha, AC_SRC_ALPHA};
        AlphaBlend(hBindDC, pParam->xDest, pParam->yDest, w, h, hDC, 0, 0, w, h, bf);

        ::SelectObject(hDC, hOldBitmap);
        Image::ReleaseCacheDC(hDC);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
    }
}

void GdiplusRenderTarget::DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam)
{
	UIASSERT(0 && _T("未实现"));
}
#pragma endregion
