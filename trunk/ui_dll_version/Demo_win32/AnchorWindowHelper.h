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

#if 1
//
//	通过采用拦截WM_WINDOWPOSCHANGING消息的方法，屏蔽系统处理，由自己再去调用一次DeferWindowPos
//
//	优点:
//		1. 简单，只需要处理一个消息
//		2. 能处理各种窗口位置、大小变化的场景
//
//	缺点:
//		1. 处理不好容易产生死循环
//
template<class T>
class AnchorWindowHelper
{
public:
	AnchorWindowHelper()
	{
		m_bSendByDeferWindowPos = false;
	}

protected:

	void OnMouseMove(UINT nFlags, POINT point)
	{
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

	void __OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (m_bSendByDeferWindowPos)
		{
			m_bSendByDeferWindowPos = false;  // DeferWindowPos传递过来的消息，放行。
			return;
		}
		T* pThis = static_cast<T*>(this);

		// 计算当前窗口的新坐标大小
		int   x = 0, y = 0, w = 0, h = 0;
		CRect rc;
		::GetWindowRect(pThis->m_hWnd, &rc);
		if (lpWndPos->flags & SWP_NOMOVE)
		{
			x = rc.left;
			y = rc.top;
		}
		else
		{
			x = lpWndPos->x;
			y = lpWndPos->y;
		}
		if (lpWndPos->flags & SWP_NOSIZE)
		{
			w = rc.right - rc.left;
			h = rc.bottom - rc.top;
		}
		else
		{
			w = lpWndPos->cx;
			h = lpWndPos->cy;
		}
		rc.SetRect(x,y, x+w, y+h);

		do 
		{
			// 注：DeferWindowPos不支持SWP_NOSENDCHANGING，会报无效参数，因此在这里才增加了一个
			//     bool 变量来防止死循环

			// 计算需要同步移动的窗口数量 
			int nCount = this->GetVisibleHWNDCount()+1;  // +1 自己本身的m_hWnd
			HDWP hdwp = BeginDeferWindowPos(nCount);

			if (NULL == hdwp) break;

			UINT nFlag = lpWndPos->flags & ~0x01000000;  // ?? 不过滤掉该标记会导致DeferWindodwPos失败，不知道为什么
			hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, lpWndPos->hwndInsertAfter, lpWndPos->x, lpWndPos->y, lpWndPos->cx, lpWndPos->cy, nFlag/*|SWP_NOSENDCHANGING*/);
			if (NULL == hdwp) break;

			bool bLoopResult = true;
			vector<AnchorWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<AnchorWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
			for (; iter != iterEnd; iter++)
			{
				HWND hWnd = iter->m_hWnd;
				if (NULL != hWnd && IsWindowVisible(hWnd))
				{
					hdwp = HandleWindowAnchor(hdwp, rc, *iter);
					if (NULL == hdwp) 
					{
						bLoopResult = false;
						break;
					}
				}
			}
			if (false == bLoopResult)  break;

			m_bSendByDeferWindowPos = true;
			EndDeferWindowPos(hdwp);

			// 注：在这里不能添加SWP_NOZORDER，否则会导致窗口拖拽时Z次序不正确
			lpWndPos->flags |= SWP_NOMOVE|SWP_NOSIZE; // 拒绝本次修改，使用DeferWindowPos的消息

			return;
		} while (0);

		// 失败了，调用默认过程
		vector<AnchorWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<AnchorWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
		for (; iter != iterEnd; iter++)
		{
			HWND hWnd = iter->m_hWnd;
			if (NULL != hWnd && IsWindowVisible(hWnd))
			{
				HDWP hdwpNull = NULL;
				HandleWindowAnchor(hdwpNull, rc, *iter);
			}
		}
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
		if (NULL == hdwp)
		{
			::SetWindowPos( rData.m_hWnd, NULL, x,y,w,h, SWP_NOZORDER|SWP_NOACTIVATE );
			return NULL;
		}
		else
		{
			return ::DeferWindowPos( hdwp, rData.m_hWnd, NULL, x,y,w,h, nFlag );
		}
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
	vector<AnchorWindowData>   m_vecAnchorItems;
	bool   m_bSendByDeferWindowPos;  // WM_WINDOWPOSCHANGING消息的发送者
};
#else
//
// 采用拦截鼠标的按下拖拽消息，自己来实现窗口拖拽的效果
// 千千静听应该就是采用了这种方法来实现的
//
// 缺点：
//    1. 较为复杂
//    2. 只能拦截鼠标拖拽的情况，通过代码移动窗口的情况无法拦截，窗口最大化最小化不好处理
//
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
		return DeferWindowPos( hdwp, rData.m_hWnd, NULL, x,y,w,h, nFlag );
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
#endif

