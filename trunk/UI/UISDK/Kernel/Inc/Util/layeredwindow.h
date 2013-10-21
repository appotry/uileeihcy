#ifndef LAYEREDWINDOW_H_6F942580_4FD4_442f_BE87_DA7EFC738A58
#define LAYEREDWINDOW_H_6F942580_4FD4_442f_BE87_DA7EFC738A58

// 该类目前被用于实现光标窗口和动画窗口
class CLayeredWindow
{
public:
	CLayeredWindow()
	{
		m_hLayeredMemDC = NULL;
		m_hLayeredBitmap = NULL;
		m_nSourceConstantAlpha = 255;
		m_pBits = NULL;
		m_nPitch = 0;
		m_hHostWnd = NULL;
        m_nWidth = 0;
        m_nHeight = 0;
	}
	~CLayeredWindow()
	{
		this->Release();
	}

 	void Attach(HWND hWnd)
 	{
 		m_hHostWnd = hWnd;
 	}

	void Release()
	{
		if (m_hLayeredBitmap)
		{
			DeleteObject(m_hLayeredBitmap);
			m_hLayeredBitmap = NULL;
		}
		if (m_hLayeredMemDC)
		{
			DeleteObject(m_hLayeredMemDC);
			m_hLayeredMemDC = NULL;
		}
	}

	void ReSize()
	{
		Release();

		RECT rcWindow = {0};
		::GetWindowRect(m_hHostWnd, &rcWindow);
		int W = RECTW(rcWindow);
		int H = RECTH(rcWindow);
		if (W == 0 || H == 0)
			return;
	
        m_nWidth = W;
        m_nHeight = H;

		m_hLayeredMemDC = ::CreateCompatibleDC(NULL);
        m_hLayeredBitmap = UI::CreateMemBitmap(W,H, &m_nPitch, &m_pBits);

		::SelectObject(m_hLayeredMemDC, m_hLayeredBitmap);
	}

    void Move(POINT pt)
    {
//         POINT ptMemDC  = {0,0};
// 
//         int   nFlag = ULW_ALPHA;
//         BLENDFUNCTION  bf;
//         bf.BlendOp     = AC_SRC_OVER ;
//         bf.AlphaFormat = AC_SRC_ALPHA;        
//         bf.BlendFlags  = 0;
//         bf.SourceConstantAlpha = m_nSourceConstantAlpha;
// 
//         ::UpdateLayeredWindow(m_hHostWnd, NULL, &pt, NULL, m_hLayeredMemDC, &ptMemDC, 0, &bf, nFlag );

        ::UpdateLayeredWindow(m_hHostWnd, NULL, &pt, NULL, NULL, NULL, 0, NULL, 0); 
    }

	void InitLayered(int nAlpha = 255)
	{
		if (NULL == m_hHostWnd)
			return;

		::SetWindowLong(m_hHostWnd, GWL_STYLE, ::GetWindowLong(m_hHostWnd,GWL_STYLE)&~(WS_BORDER|WS_CAPTION|WS_THICKFRAME|WS_SYSMENU));
		::SetWindowLong(m_hHostWnd, GWL_EXSTYLE, WS_EX_LAYERED | ::GetWindowLong(m_hHostWnd,GWL_EXSTYLE)&~(WS_EX_WINDOWEDGE|WS_EX_DLGMODALFRAME));
		::SetWindowPos(m_hHostWnd, 0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED|SWP_NOACTIVATE);

		ReSize();
		SetAlpha(nAlpha);
	}

	void Commit2LayeredWindow()
	{
		if (NULL == m_hLayeredMemDC)
			return;

		RECT rc;
		::GetWindowRect(m_hHostWnd, &rc);

		POINT leftTop  = {rc.left, rc.top};     // 设置分层窗口坐标，可为NULL
		POINT ptMemDC  = {0,0};
		SIZE  size     = {rc.right-rc.left, rc.bottom-rc.top}; // 设置分层窗口的大小，可为NULL

		int   nFlag = ULW_ALPHA;
		BLENDFUNCTION  bf;
		bf.BlendOp     = AC_SRC_OVER ;
		bf.AlphaFormat = AC_SRC_ALPHA;        
		bf.BlendFlags  = 0;
		bf.SourceConstantAlpha = m_nSourceConstantAlpha;

		BOOL bRet = ::UpdateLayeredWindow(m_hHostWnd, NULL, &leftTop, &size, m_hLayeredMemDC, &ptMemDC, 0, &bf, nFlag ); 
	}

	void RedrawLayered()
	{
		if (NULL == m_hLayeredMemDC)
			return;

		::SendMessage(m_hHostWnd, WM_ERASEBKGND, (WPARAM)m_hLayeredMemDC, 1 );
		::SendMessage(m_hHostWnd, WM_PAINT, (WPARAM)m_hLayeredMemDC, 1 );
		this->Commit2LayeredWindow();
	}

	void SetAlpha(int n)
	{
		if (n < 0)
			n = 0;
		if (n > 255)
			n = 255;

		m_nSourceConstantAlpha = n;
	}

public:
	HDC       m_hLayeredMemDC;     // 分层窗口需要自己维护一份图片,UpdateLayeredWindow的第二个HDC
	HBITMAP   m_hLayeredBitmap;    // 分层窗口需要自己维护一份图片
	int       m_nSourceConstantAlpha;

	HWND      m_hHostWnd;
public:
	BYTE*     m_pBits;
	int       m_nPitch;
    int       m_nWidth;
    int       m_nHeight;
};


#endif // LAYEREDWINDOW_H_6F942580_4FD4_442f_BE87_DA7EFC738A58