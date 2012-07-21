#include "stdafx.h"


namespace UI
{


DWORD __stdcall AnimateBase::AnimateThreadProc(LPVOID lp)
{
	AnimateBase* p = (AnimateBase*)lp;
	while(1)
	{
		p->OnTick();
		Sleep(40);

		if ( p->m_bTerminateTread )
			break;
	}


	return 0;
}

void AnimateBase1Windows::End()
{
	AnimateBase::End();
	::ShowWindow( m_hWinLayered,SW_HIDE);
	::DestroyWindow( m_hWinLayered );
//	::DeleteDC(m_hScreenDC);

	m_hWinLayered = NULL;
//	m_hScreenDC = NULL;

	delete this;  // 通过类工厂被new出来的，在这里释放自己 
};

void AnimateBase1Windows::Start(HWND hWnd)
{
	m_hTargetWnd = hWnd;

	RECT rc;
	GetWindowRect(m_hTargetWnd, &rc);
	ptStart.x = rc.left;
	ptStart.y = rc.top;

	const int nW = rc.right-rc.left;
	const int nH = rc.bottom-rc.top;

	// 创建分层窗口，用于实现动画
	m_hWinLayered = ::CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		WND_ANIMATE_CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW, 0,0,2000,2000,NULL,0,0,0);

	::ShowWindow( m_hWinLayered, SW_SHOW );
	::SetWindowPos( m_hWinLayered, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE );

	// 获取当前窗口界面图片
	HDC hWindowDC = ::GetWindowDC( m_hTargetWnd );
	HDC hMemDC    = ::CreateCompatibleDC( hWindowDC );
//	m_hScreenDC   = ::CreateDC(_T("DISPLAY"),0,0,0 );

	HBITMAP hMemBmp = ::CreateCompatibleBitmap( hWindowDC, nW, nH );
	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC,hMemBmp );
	//::BitBlt( hMemDC, 0,0, rc.right-rc.left,rc.bottom-rc.top, hWindowDC, 0,0, SRCCOPY ); // <-- 当窗口被遮盖时无法截图
#if(_WIN32_WINNT >= 0x0501)
	::PrintWindow(m_hTargetWnd,hMemDC,0);
#else
	UIASSERT(0);
#endif
	::ReleaseDC( m_hTargetWnd,hWindowDC );

	// 获取当前窗口的异形区域，用于绘制分层窗口时的透明
	HRGN hRgnNone   = ::CreateRectRgn(0,0,nW,nH);
	HRGN hRgnWindow = ::CreateRectRgn(0,0,0,0);
	
	GetWindowRgn(m_hTargetWnd, hRgnWindow);
	::CombineRgn(hRgnNone,hRgnNone,hRgnWindow,RGN_DIFF);
	::DeleteObject(hRgnWindow);

	// 定义位图信息  
	BITMAP bm;      
	::GetObject( hMemBmp, sizeof(BITMAP), (LPBYTE)&bm);   

	BITMAPINFO bi;   
	::ZeroMemory( &bi, sizeof(BITMAPINFO) );
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);   
	bi.bmiHeader.biWidth = bm.bmWidth;   
	bi.bmiHeader.biHeight = -bm.bmHeight;   
	bi.bmiHeader.biPlanes = 1;   
	bi.bmiHeader.biBitCount = 32;    
	bi.bmiHeader.biCompression = BI_RGB;    
	bi.bmiHeader.biSizeImage = bm.bmWidth * 4 * bm.bmHeight; // 32 bit   
	bi.bmiHeader.biClrUsed = 0;   
	bi.bmiHeader.biClrImportant = 0;   

	// 获取位图数据   
	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];   
	::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);   
	if (!::GetDIBits( hMemDC, hMemBmp, 0, bm.bmHeight, pBits, &bi, DIB_RGB_COLORS))   
	{   
		delete[] pBits;   
		pBits = NULL;   
	}  

	// 重新创建一张DIB带通道的图片放在Image对象中，这样就能适应异型窗口了
	m_layeredImage.Create( bi.bmiHeader.biWidth,bi.bmiHeader.biHeight, 32, Image::createAlphaChannel );
	m_layeredImage.ForceUseAlpha();  // 用create创建的image默认是将通道置为false，导致后面不能使用alphablend

	LPBYTE lpbits = (LPBYTE)m_layeredImage.GetBits();
	memcpy( lpbits, pBits, bi.bmiHeader.biSizeImage );
		
	// 通过遍历整个图片像素，将位于窗口区域外的像素alpha值设为全透明
	for (LONG i = bm.bmHeight; i > 0 ; i--)   
	{
		for (LONG j = 0; j < bm.bmWidth; j++)      
		{
			POINT pt = { j, bm.bmHeight-i }; 

			BYTE alpha = 0;
			if (::PtInRegion(hRgnNone,pt.x,pt.y))
			{
				*(lpbits++) = 0;  // 在win7下默认是255，导致不能透明的问题；xp上默认是0
				*(lpbits++) = 0;
				*(lpbits++) = 0;
				*(lpbits++) = 0;  // 完全透明
			}
			else
			{
				lpbits += 3;
				*(lpbits++) = 255; // 不透明
			}
			
			*(lpbits+3) = alpha;
		}
	}
	
	::DeleteObject( hRgnNone );
	::SelectObject( hMemDC, hOldBmp );
	::DeleteObject( hMemBmp );
	::DeleteDC( hMemDC );
	delete [] pBits;
	pBits = NULL;

#ifdef _DEBUG
	//m_layeredImage.Save(L"d:\\aaa.png", Gdiplus::ImageFormatPNG);
#endif
	AnimateBase::Start(m_hTargetWnd);

	SetWindowLong(m_hTargetWnd, GWL_EXSTYLE, (GetWindowLong(m_hTargetWnd,GWL_EXSTYLE)&~WS_EX_LAYERED)&~WS_EX_TOOLWINDOW);
	//ShowWindow(m_hTargetWnd,SW_HIDE);
};



void Animate1W_Hide_Kuosan::OnTick()
{
	static int nFrame = 0;

	int nAdd = 0;
	int nAlpha = 255;
	switch(nFrame)
	{
	case 1:
		nAdd = -5;
		nAlpha = 200;
		break;
	case 2:
		nAdd = -10;
		nAlpha = 200;
		break;
	case 3:
		nAdd = -8;
		nAlpha = 150;
		break;
	case 4:
		nAdd = 5;
		nAlpha = 120;
		break;
	case 5:
		nAdd = 10;
		nAlpha = 100;
		break;
	case 6:
		nAdd = 20;
		nAlpha = 60;
		break;;
	}

	// 计算当前移动位置
	RECT rc;
	GetWindowRect( m_hTargetWnd, &rc );

	POINT leftTop  = {ptStart.x - nAdd, ptStart.y - nAdd};                   // 设置分层窗口坐标，可为NULL
	POINT ptMemDC  = {0,0};
	SIZE  size     = { rc.right-rc.left+ 2*nAdd, rc.bottom-rc.top + 2*nAdd}; // 设置分层窗口的大小，可为NULL

	// 绘制当前显示界面
	HDC hDC    = ::GetDC(m_hWinLayered);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDC,size.cx,size.cy);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hMemDC, hMemBitmap );
	::SetStretchBltMode( hMemDC, HALFTONE);
	m_layeredImage.Draw( hMemDC, 0,0,size.cx,size.cy, 0,0,m_layeredImage.GetWidth(),m_layeredImage.GetHeight() );

	BLENDFUNCTION bf;
	bf.BlendOp     = AC_SRC_OVER ;
	bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA 会导致窗口被搂空,AC_SRC_OVER不使用透明
	bf.BlendFlags  = 0;                   // Must be zero. 
	bf.SourceConstantAlpha = nAlpha;      // 0~255 透明度

	// 	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(_T("D:\\透明窗口\\TransparentWnd\\res\\6.PNG"));
	// 	Gdiplus::Graphics g(hMemDC);
	// 	g.DrawImage( pImage, 0, 0);

	::UpdateLayeredWindow( m_hWinLayered, NULL/*m_hScreenDC*/, &leftTop, &size, hMemDC, &ptMemDC, 0, &bf, ULW_ALPHA ); 

	// 清理
	::SelectObject( hMemDC, hOldBitmap );
	::DeleteDC( hMemDC );
	::DeleteObject( hMemBitmap );
	::ReleaseDC( m_hWinLayered, hDC );

	if( nFrame == 0 )
	{
		::ShowWindow(m_hTargetWnd,SW_HIDE);  // 在绘制完第一帧后再将原窗口隐藏，避免先隐藏再显示而导致的闪烁
	}
	if(nFrame == 6)
	{
		this->End();
		nFrame = -1;
	}

	nFrame++;
};






UIAPI void  UIAnimateWindow ( HWND  hWnd,  UINT nFlags, void* pParam )
{
	AnimateBase*  p = AnimateBase1WindowsFactory::Create(nFlags);
	if( NULL != p )
		p->Start(hWnd);
}

UIAPI void   UIAnimateWindow2( HWND  hWnd1, HWND hWnd2,  UINT nFlags, void* pParam )
{
	
}

}