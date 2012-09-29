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
#endif
}
}