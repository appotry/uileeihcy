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

	delete this;  // ͨ���๤����new�����ģ��������ͷ��Լ� 
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

	// �����ֲ㴰�ڣ�����ʵ�ֶ���
	m_hWinLayered = ::CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		WND_ANIMATE_CLASS_NAME, _T(""), WS_OVERLAPPEDWINDOW, 0,0,2000,2000,NULL,0,0,0);

	::ShowWindow( m_hWinLayered, SW_SHOW );
	::SetWindowPos( m_hWinLayered, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE );

	// ��ȡ��ǰ���ڽ���ͼƬ
	HDC hWindowDC = ::GetWindowDC( m_hTargetWnd );
	HDC hMemDC    = ::CreateCompatibleDC( hWindowDC );
//	m_hScreenDC   = ::CreateDC(_T("DISPLAY"),0,0,0 );

	HBITMAP hMemBmp = ::CreateCompatibleBitmap( hWindowDC, nW, nH );
	HBITMAP hOldBmp = (HBITMAP)::SelectObject( hMemDC,hMemBmp );
	//::BitBlt( hMemDC, 0,0, rc.right-rc.left,rc.bottom-rc.top, hWindowDC, 0,0, SRCCOPY ); // <-- �����ڱ��ڸ�ʱ�޷���ͼ
#if(_WIN32_WINNT >= 0x0501)
	::PrintWindow(m_hTargetWnd,hMemDC,0);
#else
	UIASSERT(0);
#endif
	::ReleaseDC( m_hTargetWnd,hWindowDC );

	// ��ȡ��ǰ���ڵ������������ڻ��Ʒֲ㴰��ʱ��͸��
	HRGN hRgnNone   = ::CreateRectRgn(0,0,nW,nH);
	HRGN hRgnWindow = ::CreateRectRgn(0,0,0,0);
	
	GetWindowRgn(m_hTargetWnd, hRgnWindow);
	::CombineRgn(hRgnNone,hRgnNone,hRgnWindow,RGN_DIFF);
	::DeleteObject(hRgnWindow);

	// ����λͼ��Ϣ  
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

	// ��ȡλͼ����   
	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];   
	::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);   
	if (!::GetDIBits( hMemDC, hMemBmp, 0, bm.bmHeight, pBits, &bi, DIB_RGB_COLORS))   
	{   
		delete[] pBits;   
		pBits = NULL;   
	}  

	// ���´���һ��DIB��ͨ����ͼƬ����Image�����У�����������Ӧ���ʹ�����
	m_layeredImage.Create( bi.bmiHeader.biWidth,bi.bmiHeader.biHeight, 32, Image::createAlphaChannel );
	m_layeredImage.ForceUseAlpha();  // ��create������imageĬ���ǽ�ͨ����Ϊfalse�����º��治��ʹ��alphablend

	LPBYTE lpbits = (LPBYTE)m_layeredImage.GetBits();
	memcpy( lpbits, pBits, bi.bmiHeader.biSizeImage );
		
	// ͨ����������ͼƬ���أ���λ�ڴ��������������alphaֵ��Ϊȫ͸��
	for (LONG i = bm.bmHeight; i > 0 ; i--)   
	{
		for (LONG j = 0; j < bm.bmWidth; j++)      
		{
			POINT pt = { j, bm.bmHeight-i }; 

			BYTE alpha = 0;
			if (::PtInRegion(hRgnNone,pt.x,pt.y))
			{
				*(lpbits++) = 0;  // ��win7��Ĭ����255�����²���͸�������⣻xp��Ĭ����0
				*(lpbits++) = 0;
				*(lpbits++) = 0;
				*(lpbits++) = 0;  // ��ȫ͸��
			}
			else
			{
				lpbits += 3;
				*(lpbits++) = 255; // ��͸��
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

	// ���㵱ǰ�ƶ�λ��
	RECT rc;
	GetWindowRect( m_hTargetWnd, &rc );

	POINT leftTop  = {ptStart.x - nAdd, ptStart.y - nAdd};                   // ���÷ֲ㴰�����꣬��ΪNULL
	POINT ptMemDC  = {0,0};
	SIZE  size     = { rc.right-rc.left+ 2*nAdd, rc.bottom-rc.top + 2*nAdd}; // ���÷ֲ㴰�ڵĴ�С����ΪNULL

	// ���Ƶ�ǰ��ʾ����
	HDC hDC    = ::GetDC(m_hWinLayered);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hMemBitmap = ::CreateCompatibleBitmap(hDC,size.cx,size.cy);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hMemDC, hMemBitmap );
	::SetStretchBltMode( hMemDC, HALFTONE);
	m_layeredImage.Draw( hMemDC, 0,0,size.cx,size.cy, 0,0,m_layeredImage.GetWidth(),m_layeredImage.GetHeight() );

	BLENDFUNCTION bf;
	bf.BlendOp     = AC_SRC_OVER ;
	bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA �ᵼ�´��ڱ�§��,AC_SRC_OVER��ʹ��͸��
	bf.BlendFlags  = 0;                   // Must be zero. 
	bf.SourceConstantAlpha = nAlpha;      // 0~255 ͸����

	// 	Gdiplus::Image* pImage = Gdiplus::Image::FromFile(_T("D:\\͸������\\TransparentWnd\\res\\6.PNG"));
	// 	Gdiplus::Graphics g(hMemDC);
	// 	g.DrawImage( pImage, 0, 0);

	::UpdateLayeredWindow( m_hWinLayered, NULL/*m_hScreenDC*/, &leftTop, &size, hMemDC, &ptMemDC, 0, &bf, ULW_ALPHA ); 

	// ����
	::SelectObject( hMemDC, hOldBitmap );
	::DeleteDC( hMemDC );
	::DeleteObject( hMemBitmap );
	::ReleaseDC( m_hWinLayered, hDC );

	if( nFrame == 0 )
	{
		::ShowWindow(m_hTargetWnd,SW_HIDE);  // �ڻ������һ֡���ٽ�ԭ�������أ���������������ʾ�����µ���˸
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