#pragma once

#include "atlimage.h"
//#include "atlcrack.h"

template<class T>   // T为CLayeredWindow的子类
class CLayeredWindow
{
public:
	CLayeredWindow()
	{
		m_hLayeredMemDC = NULL;
		m_hLayeredBitmap = NULL;
		m_nSourceConstantAlpha = 255;
	}
	~CLayeredWindow()
	{
		this->Release();
	}

	void Release()
	{
		if (NULL != m_hLayeredBitmap)
		{
			DeleteObject(m_hLayeredBitmap);
			m_hLayeredBitmap = NULL;
		}
		if (NULL != m_hLayeredMemDC)
		{
			DeleteObject(m_hLayeredMemDC);
			m_hLayeredMemDC = NULL;
		}
	}

	void ReSize()
	{
		Release();

		T* pThis = static_cast<T*>(this);

		RECT rcWindow;
		::GetWindowRect( pThis->m_hWnd, &rcWindow );
	
		m_hLayeredMemDC = ::CreateCompatibleDC(NULL);

		CImage image;
		image.Create( rcWindow.right-rcWindow.left, -(rcWindow.bottom-rcWindow.top), 32, CImage::createAlphaChannel );
		m_hLayeredBitmap = image.Detach();
		::SelectObject(m_hLayeredMemDC, m_hLayeredBitmap);
	}

	void InitLayered(int nAlpha = 255)
	{
		T* pThis = static_cast<T*>(this);

		pThis->ModifyStyle(WS_BORDER|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU, 0);
		pThis->ModifyStyleEx(WS_EX_WINDOWEDGE|WS_EX_DLGMODALFRAME, WS_EX_LAYERED);
		pThis->SetWindowPos(0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED|SWP_NOACTIVATE);

		ReSize();

		SetAlpha(nAlpha);
	}

	void Commit2LayeredWindow()
	{
		if (NULL == m_hLayeredMemDC)
			return;

		T* pThis = static_cast<T*>(this);

		RECT rc;
		::GetWindowRect( pThis->m_hWnd, &rc );

		POINT leftTop  = {rc.left, rc.top};     // 设置分层窗口坐标，可为NULL
		POINT ptMemDC  = {0,0};
		SIZE  size     = {rc.right-rc.left, rc.bottom-rc.top}; // 设置分层窗口的大小，可为NULL

		int   nFlag = ULW_ALPHA;
		BLENDFUNCTION  bf;
		bf.BlendOp     = AC_SRC_OVER ;
		bf.AlphaFormat = AC_SRC_ALPHA;        
		bf.BlendFlags  = 0;
		bf.SourceConstantAlpha = m_nSourceConstantAlpha;

		BOOL bRet = ::UpdateLayeredWindow(pThis->m_hWnd, NULL, &leftTop, &size, m_hLayeredMemDC, &ptMemDC, 0, &bf, nFlag ); 
	}

	void RedrawLayered()
	{
		if (NULL == m_hLayeredMemDC)
			return;

		T* pThis = static_cast<T*>(this);
		::SendMessage(pThis->m_hWnd, WM_ERASEBKGND, (WPARAM)m_hLayeredMemDC, 1 );
		::SendMessage(pThis->m_hWnd, WM_PAINT, (WPARAM)m_hLayeredMemDC, 1 );
		this->Commit2LayeredWindow();
	}

	void SetAlpha(int n)
	{
		if (n < 0 )
			n = 0;
		if (n > 255)
			n = 255;

		m_nSourceConstantAlpha = n;
	}

protected:
	HDC       m_hLayeredMemDC;     // 分层窗口需要自己维护一份图片,UpdateLayeredWindow的第二个HDC
	HBITMAP   m_hLayeredBitmap;    // 分层窗口需要自己维护一份图片
	int       m_nSourceConstantAlpha;
};
