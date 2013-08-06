#include "stdafx.h"
#include "h\util.h"

namespace UI { namespace Util
{
#ifdef UTIL_GDI
#pragma comment(lib,"Msimg32.lib")   // GradientFill

//
//	将字体的大小转换成CreateFont需要使用的字体高度
//
//	Parameter
//		nSize	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的height;
//
int FontSize2Height(int nSize)
{
	int nHeight = 0;
	HDC hDC = ::GetDC(NULL);
	nHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL,hDC);
	return nHeight;
}

//
//	将字体LOGFONT.lfHeight转换成字体大小
//
//	Parameter
//		nHeight	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的size;
//
int FontHeight2Size(int nHeight)
{
	int nSize = 0;
	HDC hDC = ::GetDC(NULL);
	nSize = -MulDiv( nHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY) );  // 96
	::ReleaseDC(NULL,hDC);
	return nSize;
}

namespace UI { namespace Util {
int CALLBACK IsFontExistEnumFontsProc(
						   CONST LOGFONT *lplf,     // logical-font data
						   CONST TEXTMETRIC *lptm,  // physical-font data
						   DWORD dwType,            // font type
						   LPARAM lpData            // application-defined data
						   )
{
	BOOL* pbFind = (BOOL*)lpData;
	if (pbFind)
	{
		*pbFind = TRUE;
	}
	return 0;
}
} }

//
//  判断指定的字体在该系统中是否存在
//
//	Parameter
//		pszFaceName
//			[in]	要检测的字体名称
//
BOOL IsFontExist(const TCHAR* pszFaceName)
{
	if (NULL == pszFaceName)
		return FALSE;

	HDC hDC = GetDC(NULL);
	BOOL bFind = FALSE;
	int nRet = EnumFonts(hDC, pszFaceName, UI::Util::IsFontExistEnumFontsProc, (LPARAM)&bFind);
	::ReleaseDC(NULL, hDC);
	
	return bFind;
}

BOOL GradientFillH( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == prc )
		return FALSE;

	if( colFrom == colTo )
	{
		HBRUSH hBrush = ::CreateSolidBrush(colFrom);
		::FillRect(hDC,prc,hBrush);
		::DeleteObject(hBrush);

		return TRUE;
	}

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = prc->left;
	vert [0] .y      = prc->top;
	vert [0] .Red    = GetRValue(colFrom) << 8;
	vert [0] .Green  = GetGValue(colFrom) << 8;
	vert [0] .Blue   = GetBValue(colFrom) << 8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = prc->right;
	vert [1] .y      = prc->bottom; 
	vert [1] .Red    = GetRValue(colTo) << 8;
	vert [1] .Green  = GetGValue(colTo) << 8;
	vert [1] .Blue   = GetBValue(colTo) << 8;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);

	return TRUE;
}
int GradientFillV( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == prc )
		return FALSE;

	if( colFrom == colTo )
	{
		HBRUSH hBrush = ::CreateSolidBrush(colFrom);
		::FillRect(hDC,prc,hBrush);
		::DeleteObject(hBrush);

		return TRUE;
	}

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = prc->left;
	vert [0] .y      = prc->top;
	vert [0] .Red    = GetRValue(colFrom) << 8;
	vert [0] .Green  = GetGValue(colFrom) << 8;
	vert [0] .Blue   = GetBValue(colFrom) << 8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = prc->right;
	vert [1] .y      = prc->bottom; 
	vert [1] .Red    = GetRValue(colTo) << 8;
	vert [1] .Green  = GetGValue(colTo) << 8;
	vert [1] .Blue   = GetBValue(colTo) << 8;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	return TRUE;
}

// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540
// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch) 
{	
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}
// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch) 
{	
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}
// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch) 
{ 
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch); 
}
// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch) 
{ 
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch); 
}
#endif

BOOL IsLayeredWindow(HWND hWnd)
{
	if (NULL == hWnd)
		return FALSE;

	LONG lStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (lStyle & WS_EX_LAYERED)
		return TRUE;

	return FALSE;
}


void  FixGdiAlpha(HDC hDC, RECT* lprc)
{
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);

    BITMAP  bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    RECT rcDest;
    RECT rcBitmap = {0, 0, bm.bmWidth, bm.bmHeight};
    if (FALSE == ::IntersectRect(&rcDest, &rcBitmap, lprc))
        return;

    BYTE* pBits = (BYTE*)bm.bmBits;
    int nPitch = -bm.bmWidthBytes;
    int n = 0;

    BYTE* p = NULL;
    BYTE* pEnd = NULL;

    pBits += (bm.bmHeight - rcDest.top - 1)*bm.bmWidthBytes;
    for (int y = rcDest.top ; y < rcDest.bottom; y++)
    {
        p    = pBits + (rcDest.left << 2) + 3;
        pEnd = pBits + (rcDest.right << 2) + 3;

        while (p < pEnd)  // TODO: 这种优化效果也不怎么样
        {
           if (0 == (*p))
           {
               *p = 0xFF;
           }
           p += 4;
        }

//         for (int x = rcDest.left; x < rcDest.right; x++)
//         {
//             n = (x<<2) + 3;
//             if (pBits[n])
//                 continue;
// 
//             pBits[n] = 255;
//         }
        pBits += nPitch;
    }
}

}
}
