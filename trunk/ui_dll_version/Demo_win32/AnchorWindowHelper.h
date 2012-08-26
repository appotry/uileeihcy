#pragma once

//////////////////////////////////////////////////////////////////////////
//
//                          联合移动的窗口
//
//////////////////////////////////////////////////////////////////////////

#define ANCHOR_NONE        0
#define ANCHOR_LEFT        0x0001
#define ANCHOR_RIGHT       0x0002
#define ANCHOR_TOP         0x0004
#define ANCHOR_BOTTOM      0x0008
#define ANCHOR_OUT_LEFT    0x0010
#define ANCHOR_OUT_RIGHT   0x0020
#define ANCHOR_OUT_TOP     0x0040
#define ANCHOR_OUT_BOTTOM  0x0080

struct  AnchorData
{
	int  xOffset;
	int  yOffset;
	
	union{
		int xOffset2;
		int Width;
	};
	union{
		int yOffset2;
		int Height;
	};

	AnchorData()
	{
		xOffset = yOffset = xOffset2 = yOffset2 = 0;
	}
};
struct  AnchorWindowData
{
	HWND         m_hWnd;           
	UINT         m_nAnchorType;   
	AnchorData   m_rcAnchorData;  // 当m_nAnchorType相应位有值时，m_rcAnchorData的该位有效

	AnchorWindowData()
	{
		m_hWnd = NULL;
		m_nAnchorType = ANCHOR_NONE;
	}
};

template<class T>
class AnchorWindowHelper
{
public:
	AnchorWindowHelper()
	{
		m_lSizeMove = 0;
		m_ptCursorSizeMove.x = m_ptCursorSizeMove.y = 0;
		m_ptWndPosSizeMove.x = m_ptWndPosSizeMove.y = 0;
	}

protected:
	void OnLButtonDown(UINT nFlags, POINT point)
	{
		m_lSizeMove = 1;
		m_ptCursorSizeMove = point;
		
		T* pThis = static_cast<T*>(this);
		::SetCapture(pThis->m_hWnd);
	}
	void OnLButtonUp(UINT nFlags, POINT point)
	{
		if (2 == m_lSizeMove)
		{
			this->OnExitSizeMove();
		}
		m_lSizeMove = 0;
		::ReleaseCapture();
	}

	void OnMouseMove(UINT nFlags, POINT point)
	{
		if (0 != m_lSizeMove)
		{
			if (1 == m_lSizeMove)
			{
				this->OnEnterSizeMove();
			}

			// 计算新的窗口位置
			POINT ptCursor;
			::GetCursorPos(&ptCursor);

			POINT ptNew = { m_ptWndPosSizeMove.x + ptCursor.x - m_ptCursorSizeMove.x,
				m_ptWndPosSizeMove.y + ptCursor.y - m_ptCursorSizeMove.y };

			// 计算需要同步移动的窗口数量 
			T* pThis = static_cast<T*>(this);
			int nCount = this->GetVisibleHWNDCount()+1;
			HDWP hdwp = BeginDeferWindowPos(nCount);

			CRect  rcSourceWnd;
			::GetWindowRect(pThis->m_hWnd, &rcSourceWnd);
			hdwp = DeferWindowPos(hdwp, pThis->m_hWnd, NULL, ptNew.x, ptNew.y, 0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
			
			// 需要将最新的源窗口RECT传递进去，否则直接调用GetWindowRect得到的值将是旧的
			rcSourceWnd = CRect(ptNew.x, ptNew.y, ptNew.x+rcSourceWnd.Width(), ptNew.y+rcSourceWnd.Height());

			vector<AnchorWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<AnchorWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
			for (; iter != iterEnd; iter++)
			{
				HWND hWnd = iter->m_hWnd;
				if (NULL != hWnd && IsWindowVisible(hWnd))
				{
					hdwp = HandleWindowAnchor(hdwp, rcSourceWnd, *iter);
				}
			}
			
			EndDeferWindowPos(hdwp);
		}
	}


	void   OnEnterSizeMove()
	{
		T* pThis = static_cast<T*>(this);

		m_lSizeMove = 2;

		RECT rc;
		::GetWindowRect(pThis->m_hWnd, &rc);
		m_ptWndPosSizeMove.x = rc.left;
		m_ptWndPosSizeMove.y = rc.top;

		::MapWindowPoints(pThis->m_hWnd, NULL, &m_ptCursorSizeMove, 1);

		RECT rcWorkArea;
		::SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rcWorkArea, 0);
		ClipCursor(&rcWorkArea);
	}

	void   OnExitSizeMove()
	{
		m_lSizeMove = 0;
		::ClipCursor(NULL);
	}
	void   OnCancelMode()
	{
		this->OnExitSizeMove();
	}

	HDWP   HandleWindowAnchor(HDWP& hdwp, const CRect& rcSrcWindow, const AnchorWindowData& rData)
	{
		int x=0, y=0, w=0, h=0;
		int nFlag = SWP_NOZORDER|SWP_NOACTIVATE;

		CRect  rcThisWindow;
		::GetWindowRect(rData.m_hWnd, &rcThisWindow);
		w = rcThisWindow.Width();
		h = rcThisWindow.Height();

		// 分析对齐方式
		bool bLeft   = rData.m_nAnchorType&ANCHOR_LEFT? true:false;
		bool bRight  = rData.m_nAnchorType&ANCHOR_RIGHT? true:false;
		bool bTop    = rData.m_nAnchorType&ANCHOR_TOP? true:false;
		bool bBottom = rData.m_nAnchorType&ANCHOR_BOTTOM? true:false;

		bool bOutLeft   = rData.m_nAnchorType&ANCHOR_OUT_LEFT? true:false;
		bool bOutRight  = rData.m_nAnchorType&ANCHOR_OUT_RIGHT? true:false;
		bool bOutTop    = rData.m_nAnchorType&ANCHOR_OUT_TOP? true:false;
		bool bOutBottom = rData.m_nAnchorType&ANCHOR_OUT_BOTTOM? true:false;

		if (bLeft && bRight)
		{
			x = rcSrcWindow.left + rData.m_rcAnchorData.xOffset;
			int xRight = rcSrcWindow.right - rData.m_rcAnchorData.xOffset2;
			w = xRight - x;
		}
		else 
		{
			if (-1 != rData.m_rcAnchorData.Width)
				w = rData.m_rcAnchorData.Width;

			if (bLeft)
				x = rcSrcWindow.left + rData.m_rcAnchorData.xOffset;
			else if (bRight)
				x = rcSrcWindow.right - rData.m_rcAnchorData.xOffset - w;
			else if (bOutLeft)
				x = rcSrcWindow.left - rData.m_rcAnchorData.xOffset - w;
			else if (bOutRight)
				x = rcSrcWindow.right + rData.m_rcAnchorData.xOffset;
			else
				x = rcSrcWindow.left + rData.m_rcAnchorData.xOffset;
		}

		if (bTop && bBottom)
		{
			y = rcSrcWindow.top + rData.m_rcAnchorData.yOffset;
			int yBottom = rcSrcWindow.bottom - rData.m_rcAnchorData.yOffset2;
			h = yBottom - y;
		}
		else 
		{
			if (-1 != rData.m_rcAnchorData.Height)
				h = rData.m_rcAnchorData.Height;

			if (bTop)
				y = rcSrcWindow.top + rData.m_rcAnchorData.yOffset;
			else if (bBottom)
				y = rcSrcWindow.bottom - rData.m_rcAnchorData.yOffset - w;
			else if (bOutTop)
				y = rcSrcWindow.top - rData.m_rcAnchorData.yOffset - w;
			else if (bOutBottom)
				y = rcSrcWindow.bottom + rData.m_rcAnchorData.yOffset;
			else
				y = rcSrcWindow.top + rData.m_rcAnchorData.yOffset;
		}
		return DeferWindowPos( hdwp, rData.m_hWnd, NULL, x,y,w,h, nFlag|SWP_NOSIZE );
	}

public:
	bool   AddAnchorItem(const AnchorWindowData& data)
	{
		if (NULL == data.m_hWnd)
			return false;

		if (GetWindowLong(data.m_hWnd, GWL_STYLE) & WS_CHILD)
			return false;

		if (-1 != this->FindAnchorItem(data.m_hWnd))
			return false;

		m_vecAnchorItems.push_back(data);
		return true;
	}
	bool   RemoveAnchorItem(HWND hWnd)
	{
		vector<AnchorWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<AnchorWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
		for (; iter!=iterEnd; iter++)
		{
			if (iter->m_hWnd == hWnd)
			{
				m_vecAnchorItems.erase(iter);
				return true;
			}
		}
		return false;
	}
	bool   ModifyAnchorItem(const AnchorWindowData& data)
	{
		if (-1 == this->FindAnchorItem(data.m_hWnd))
			return false;

		m_vecAnchorItems[i] = data;
		return true;
	}
	bool   ClearAnchorItem()
	{
		m_vecAnchorItems.clear();
	}

	int    FindAnchorItem(HWND hWnd)
	{
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			if (m_vecAnchorItems[i].m_hWnd == hWnd)
			{
				return i;
			}
		}
		return -1;
	}

	int    GetVisibleHWNDCount()
	{
		int nCount = 0;
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			HWND hWnd = m_vecAnchorItems[i].m_hWnd;
			if (NULL != hWnd && ::IsWindowVisible(hWnd))
			{
				nCount++;
			}
		}
		return nCount;
	}


private:
	long     m_lSizeMove;    // 0 没开始，1鼠标左键已按下，2鼠标开始拖拽
	POINT    m_ptCursorSizeMove;
	POINT    m_ptWndPosSizeMove;

	vector<AnchorWindowData>   m_vecAnchorItems;
};


