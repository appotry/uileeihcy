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
#define ANCHOR_CUSTOM      0x0100   // 发送UI_WM_SYNC_WINDOWPOSCHANGING消息给窗口，由窗口自行决定如何移动自己


enum SYNC_WINDOW_EVENT_TYPE
{
	ADD_SYNC_WINDOW,            // 通知HOST, 增加一个跟随者.LPARAM: SyncWindowData*
	MODIFY_SYNC_WINDOW,         // 通知HOST, 修改一个跟随者信息.LPARAM: SyncWindowData*
	REMOVE_SYNC_WINDOW,         // 通知HOST, 移除一个跟随者.LPARAM: SyncWindowData*

	ADD_SYNC_WINDOW_RESULT,     // 通知SITE，添加成功.LPARAM: host HWND
	MODIFY_SYNC_WINDOW_RESULT,  // 通知SITE，修改成功.LPARAM: host HWND
	REMOVE_SYNC_WINDOW_RESULT,  // 通知SITE，删除成功.LPARAM: host HWND

	HOST_WINDOW_DESTROYED,      // 要跟随的HOST销毁了，清空自己的HOST句柄. LPARAM: 无
	HOST_WINDOW_POSCHANGING,    // 发送给SITE窗口，标明下一次响应的WindowPosChaning消息是因为HOST移动产生的
};

struct  AnchorData
{
	int  xOffset;
	int  yOffset;
	
	union{
		int xOffset2;
		int Width;         // -1 表示不修改窗口宽度
	};
	union{
		int yOffset2;
		int Height;       // -1 表示不修改窗口高度
	};

	AnchorData()
	{
		xOffset = yOffset = xOffset2 = yOffset2 = 0;
	}
};

#define SWDS_MASK_ANCHORTYPE 0x0001
#define SWDS_MASK_ANCHORDATA 0x0002
#define SWDS_MASK_ANCHORON   0x0004
#define SWDS_MASK_ALL   (SWDS_MASK_ANCHORTYPE|SWDS_MASK_ANCHORDATA|SWDS_MASK_ANCHORON)
struct  SyncWindowData
{
	HWND         m_hWnd;           
	UINT         m_nMask;
	UINT         m_nAnchorType;   
	AnchorData   m_rcAnchorData;  // 当m_nAnchorType相应位有值时，m_rcAnchorData的该位有效
	bool         m_bAnchorOn;     // 跟随是否起作用。例如临时将窗口移出HOST窗口，不进行跟随，然后又贴到HOST窗口旁再次开启跟随。

	SyncWindowData()
	{
		m_hWnd = NULL;
		m_nAnchorType = ANCHOR_NONE;
		m_bAnchorOn = true;
		m_nMask = 0;
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
class SyncWindowHelper
{
public:
	SyncWindowHelper()
	{
		m_bSendByDeferWindowPos = false;
		m_hHostWnd = NULL;

		m_nAnchorMagnetCapability = 10;
		m_bAnchorToScreenEdge = false;
		m_bAnchorToOtherSize = false;
		m_bSizeMove = false;
	}

protected:

	void   _OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		// 先判断自己当前是否跟随于其它窗口，然后再处理自己的移动。
		if (NULL != m_hHostWnd)
		{
			if (this->OnSiteWindowPosChanging(lpWndPos))
				return;
		}

		this->OnHostWindowPosChanging(lpWndPos);
	}

	void   OnHostWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (0 == m_vecAnchorItems.size())
			return;

		if (m_bSendByDeferWindowPos)
		{
			m_bSendByDeferWindowPos = false;  // DeferWindowPos传递过来的消息，放行。
			return;
		}
		T* pThis = static_cast<T*>(this);

		// 计算当前窗口的新坐标大小
		CRect rc;
		this->GetRectByWNDPOS(lpWndPos, &rc);

		do 
		{
			// 注：DeferWindowPos不支持SWP_NOSENDCHANGING，会报无效参数，因此在这里才增加了一个
			//     bool 变量来防止死循环

			// 计算需要同步移动的窗口数量 
			int nCount = this->GetAvailableHWNDCount()+1;  // +1 自己本身的m_hWnd
			HDWP hdwp = BeginDeferWindowPos(nCount);

			if (NULL == hdwp) break;

			UINT nFlag = lpWndPos->flags & ~0x01000000;  // ?? 不过滤掉该标记会导致DeferWindodwPos失败，不知道为什么
			hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, lpWndPos->hwndInsertAfter, lpWndPos->x, lpWndPos->y, lpWndPos->cx, lpWndPos->cy, nFlag/*|SWP_NOSENDCHANGING*/);
			if (NULL == hdwp) break;

			bool bLoopResult = true;
			vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
			for (; iter != iterEnd; iter++)
			{
				HWND hWnd = iter->m_hWnd;
				if (NULL != hWnd /*&& IsWindowVisible(hWnd)*/ && iter->m_bAnchorOn)
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
		vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
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


	bool   OnSiteWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (m_bSendByDeferWindowPos)  // 本次的移动是因为HOST调用deferwindowpos产生的，忽略
		{
			m_bSendByDeferWindowPos = false;
			return false;
		}
		
		if (m_bSizeMove)   // 如果不是真的在用鼠标拖拽拉伸，则不处理，例如窗口刚创建，
		{
			bool bSize = (lpWndPos->flags & SWP_NOSIZE)?false:true;
			bool bMove = (lpWndPos->flags & SWP_NOMOVE)?false:true;
			if (!bSize && !bMove)
				return false;

			CRect rcHost, rcSite;
			::GetWindowRect(m_hHostWnd, &rcHost);
			this->GetRectByWNDPOS(lpWndPos, &rcSite);

			SyncWindowData data;
			data.m_hWnd = GetMyHWND();
			bool bRet = this->CheckRectAnchor( &rcHost, &rcSite, bSize, &data );
			
			if (bRet)  // 粘上了，修改窗口的位置
			{
				lpWndPos->x = rcSite.left;
				lpWndPos->y = rcSite.top;
				lpWndPos->cx = rcSite.Width();
				lpWndPos->cy = rcSite.Height();
			}

			SendMessage(m_hHostWnd, UI_WM_SYNC_WINDOW, MODIFY_SYNC_WINDOW, (LPARAM)&data);
		}
		return false;
	}

	//
	// 判断两个区域的粘合关系，如果粘上，则修改prcSize的值
	//
	// Return
	//    返回两个区域是否粘合上
	//
	// Remark
	//  Q1. 如果区分本次是MOVE，还是SIZE？检查LPWINDOWPOS->flags中的SWP_NOSIZE
	//
	bool   CheckRectAnchor(const CRect* prcHost, CRect* prcSite, bool bChangeSize, SyncWindowData* pData)
	{
		bChangeSize = true;

		bool bLeftLeft = (abs(prcSite->left - prcHost->left) < m_nAnchorMagnetCapability);
		bool bLeftRight = (abs(prcSite->left - prcHost->right)< m_nAnchorMagnetCapability);
		bool bRightLeft = (abs(prcSite->right - prcHost->left) < m_nAnchorMagnetCapability);
		bool bRightRight = (abs(prcSite->right - prcHost->right) < m_nAnchorMagnetCapability);
		bool bX = bLeftLeft || bLeftRight || bRightRight || bRightLeft || 
			(prcSite->left>prcHost->left && prcSite->left<prcHost->right) || 
			(prcSite->right>prcHost->left && prcSite->right<prcHost->right);

		bool bTopTop = (abs(prcHost->top - prcSite->top) < m_nAnchorMagnetCapability);
		bool bTopBottom = (abs(prcSite->top - prcHost->bottom)< m_nAnchorMagnetCapability);
		bool bBottomTop = (abs(prcSite->bottom - prcHost->top) < m_nAnchorMagnetCapability);
		bool bBottomBottom = (abs(prcSite->bottom - prcHost->bottom) < m_nAnchorMagnetCapability);
		bool bY = bTopTop || bTopBottom || bBottomTop || bBottomBottom ||
			(prcSite->top>prcHost->top && prcSite->top<prcHost->bottom) ||
			(prcSite->bottom>prcHost->top && prcSite->bottom<prcHost->bottom);

		bool bXAnchored = false;
		bool bYAnchored = false;
		int  nAnchorType = ANCHOR_NONE;
		if (bY)
		{
			if (bLeftLeft)
			{
	 			if (bChangeSize)
	 			{
	 				prcSite->left = prcHost->left;
	 			}
	 			else
				{
					int W = prcSite->Width();
					prcSite->left = prcHost->left;
					prcSite->right = prcSite->left + W;
				}
				bXAnchored = true;
				nAnchorType |= ANCHOR_LEFT;
			}
			else if (bLeftRight)
			{
				if (bChangeSize)
				{
					prcSite->left = prcHost->right;
				}
				else
				{
					int W = prcSite->Width();
					prcSite->left = prcHost->right;
					prcSite->right = prcSite->left + W;
				}
				bXAnchored = true;
				nAnchorType |= ANCHOR_OUT_RIGHT;
			}
			else if (bRightLeft)
			{
				if (bChangeSize)
				{
					prcSite->right = prcHost->left;
				}
				else
				{
					int W = prcSite->Width();
					prcSite->right = prcHost->left;
					prcSite->left = prcSite->right - W;
				}
				bXAnchored = true;
				nAnchorType |= ANCHOR_OUT_LEFT;
			}
			else if (bRightRight)
			{
				if (bChangeSize)
				{
					prcSite->right = prcHost->right;
				}
				else
				{
					int W = prcSite->Width();
					prcSite->right = prcHost->right;
					prcSite->left = prcSite->right - W;
				}
				bXAnchored = true;
				nAnchorType |= ANCHOR_RIGHT;
			}
		}

		if (bX)
		{
			if (bTopTop)
			{
				if (bChangeSize)
				{
					prcSite->top = prcHost->top;
				}
				else
				{
					int H = prcSite->Height();
					prcSite->top = prcHost->top;
					prcSite->bottom = prcSite->top + H;
				}
				bYAnchored = true;
				nAnchorType |= ANCHOR_TOP;
			}
			else if (bTopBottom)
			{
				if (bChangeSize)
				{
					prcSite->top = prcHost->bottom;
				}
				else
				{
					int H = prcSite->Height();
					prcSite->top = prcHost->bottom;
					prcSite->bottom = prcSite->top + H;
				}
				bYAnchored = true;
				nAnchorType |= ANCHOR_OUT_BOTTOM;
			}
			else if (bBottomTop)
			{
				if (bChangeSize)
				{
					prcSite->bottom = prcHost->top;
				}
				else
				{
					int H = prcSite->Height();
					prcSite->bottom = prcHost->top;
					prcSite->top = prcSite->bottom - H;
				}
				bYAnchored = true;
				nAnchorType |= ANCHOR_OUT_TOP;
			}
			else if (bBottomBottom)
			{
				if (bChangeSize)
				{
					prcSite->bottom = prcHost->bottom;
				}
				else
				{
					int H = prcSite->Height();
					prcSite->bottom = prcHost->bottom;
					prcSite->top = prcSite->bottom - H;
				}
				bYAnchored = true;
				nAnchorType |= ANCHOR_BOTTOM;
			}
		}

		// 修正nAnchorType只停靠了一个方向，另一个方向没有赋值的问题。
		if (0 != nAnchorType)
		{
			pData->m_nMask |= SWDS_MASK_ANCHORTYPE|SWDS_MASK_ANCHORON|SWDS_MASK_ANCHORDATA;
			pData->m_nAnchorType = nAnchorType;
			pData->m_bAnchorOn = true;
			pData->m_rcAnchorData.Width = -1;
			pData->m_rcAnchorData.Height = -1;
			if (false == bXAnchored)
			{
				pData->m_nAnchorType |= ANCHOR_LEFT;
				pData->m_rcAnchorData.xOffset = prcSite->left - prcHost->left;
			}
			else if (false == bYAnchored)
			{
				pData->m_nAnchorType |= ANCHOR_TOP;
				pData->m_rcAnchorData.yOffset = prcSite->top-prcHost->top;
			}

			return true;
		}
		else
		{
			pData->m_nMask |= SWDS_MASK_ANCHORON;
			pData->m_bAnchorOn = false;

			return false;
		}

		return false;
	}

	void   GetRectByWNDPOS(LPWINDOWPOS lpWndPos, CRect* prc)
	{
		int   x = 0, y = 0, w = 0, h = 0;
		::GetWindowRect(GetMyHWND(), prc);
		if (lpWndPos->flags & SWP_NOMOVE)
		{
			x = prc->left;
			y = prc->top;
		}
		else
		{
			x = lpWndPos->x;
			y = lpWndPos->y;
		}
		if (lpWndPos->flags & SWP_NOSIZE)
		{
			w = prc->right - prc->left;
			h = prc->bottom - prc->top;
		}
		else
		{
			w = lpWndPos->cx;
			h = lpWndPos->cy;
		}
		prc->SetRect(x,y, x+w, y+h);
	}

	HDWP   HandleWindowAnchor(HDWP& hdwp, const CRect& rcSrcWindow, const SyncWindowData& rData)
	{
		if (rData.m_nAnchorType == ANCHOR_CUSTOM)
		{
			CRect rcCopy = rcSrcWindow;
			return (HDWP)::SendMessage(rData.m_hWnd, UI_WM_SYNC_WINDOWPOSCHANING, (WPARAM)hdwp, (LPARAM)&rcCopy);
		}

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
				x = rcThisWindow.left;
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
				y = rcSrcWindow.bottom - rData.m_rcAnchorData.yOffset - h;
			else if (bOutTop)
				y = rcSrcWindow.top - rData.m_rcAnchorData.yOffset - h;
			else if (bOutBottom)
				y = rcSrcWindow.bottom + rData.m_rcAnchorData.yOffset;
			else
				y = rcThisWindow.top;
		}
		if (NULL == hdwp)
		{
			::SetWindowPos( rData.m_hWnd, NULL, x,y,w,h, SWP_NOZORDER|SWP_NOACTIVATE );
			return NULL;
		}
		else
		{
			SendMessage(rData.m_hWnd, UI_WM_SYNC_WINDOW, HOST_WINDOW_POSCHANGING, 0);
			return ::DeferWindowPos( hdwp, rData.m_hWnd, NULL, x,y,w,h, nFlag );
		}
	}

	void   _OnNcDestroy()
	{
		// 自己被销毁时，检查跟随情况
		if (NULL != m_hHostWnd)  // 通知宿主窗口移除自己
		{
			SyncWindowData data;
			data.m_hWnd = GetMyHWND();
			::SendMessage(m_hHostWnd, UI_WM_SYNC_WINDOW, REMOVE_SYNC_WINDOW, (LPARAM)&data);
		}

		// 通知其它跟随自己的窗口，宿主窗口销毁了
		this->ClearAnchorItem();
	}
	HWND   GetMyHWND()
	{
		T* pThis = static_cast<T*>(this);
		return pThis->m_hWnd;
	}
	void   _OnEnterSizeMove()
	{
		m_bSizeMove = true;
	}
	void   _OnExitSizeMove()
	{
		m_bSizeMove = false;
	}
public:
	bool   AddAnchorItem(const SyncWindowData& data)
	{
		if (NULL == data.m_hWnd)
			return false;

		if (GetWindowLong(data.m_hWnd, GWL_STYLE) & WS_CHILD)
			return false;

		if (-1 != this->FindAnchorItem(data.m_hWnd))
			return false;

		m_vecAnchorItems.push_back(data);
		::SendMessage(data.m_hWnd, UI_WM_SYNC_WINDOW, ADD_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
		return true;
	}
	bool   RemoveAnchorItem(HWND hWnd)
	{
		vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
		for (; iter!=iterEnd; iter++)
		{
			if (iter->m_hWnd == hWnd)
			{
				m_vecAnchorItems.erase(iter);
				return true;
			}
		}
		::SendMessage(hWnd, UI_WM_SYNC_WINDOW, REMOVE_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
		return false;
	}
	bool   ModifyAnchorItem(const SyncWindowData& data)
	{
		int nIndex = this->FindAnchorItem(data.m_hWnd);
		if (-1 == nIndex)
			return false;
	
		if (data.m_nMask & SWDS_MASK_ANCHORON)
			m_vecAnchorItems[nIndex].m_bAnchorOn = data.m_bAnchorOn;
		if (data.m_nMask & SWDS_MASK_ANCHORTYPE)
			m_vecAnchorItems[nIndex].m_nAnchorType = data.m_nAnchorType;
		if (data.m_nMask & SWDS_MASK_ANCHORDATA)
			memcpy(&m_vecAnchorItems[nIndex].m_rcAnchorData, &data.m_rcAnchorData, sizeof(AnchorData));

		::SendMessage(data.m_hWnd, UI_WM_SYNC_WINDOW, MODIFY_SYNC_WINDOW_RESULT, (LPARAM)GetMyHWND());
		return true;
	}
	void   OnAddAnchorItem(HWND hHostWnd)     // 自己作为跟随者
	{
		UIASSERT(NULL == m_hHostWnd);
		m_hHostWnd = hHostWnd;
	}
	void   OnModifyAnchorItem(HWND hHostWnd)  // 自己作为跟随者
	{
		UIASSERT(NULL != m_hHostWnd);
	}
	void   OnRemoveAnchorItem(HWND hHostWnd)  // 自己作为跟随者
	{
		UIASSERT(hHostWnd == m_hHostWnd);
		m_hHostWnd = NULL;
	}
	void   OnHostWindowPosChanging()
	{
		UIASSERT(NULL != m_hHostWnd);
		m_bSendByDeferWindowPos = true;
	}
	bool   ClearAnchorItem()
	{
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			::SendMessage(m_vecAnchorItems[i].m_hWnd, UI_WM_SYNC_WINDOW, HOST_WINDOW_DESTROYED, (LPARAM)GetMyHWND());
		}
		m_vecAnchorItems.clear();
		return true;
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

	int    GetAvailableHWNDCount()  // 放弃判断是否可见。有时不可见也需要调整，要不然再显示时，位置就不正确了
	{
		int nCount = 0;
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			HWND hWnd = m_vecAnchorItems[i].m_hWnd;
			if (NULL != hWnd /*&& ::IsWindowVisible(hWnd)*/ && m_vecAnchorItems[i].m_bAnchorOn)
			{
				nCount++;
			}
		}
		return nCount;
	}

	void   HideAllAnchorItem()
	{
		int nSize = (int)m_vecAnchorItems.size()+1;
		HDWP hdwp = ::BeginDeferWindowPos(nSize);

		T* pThis = static_cast<T*>(this);
		hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, NULL, 0,0,0,0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW );

		vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
		for (; iter!=iterEnd; iter++)
		{
			hdwp = ::DeferWindowPos(hdwp, iter->m_hWnd, NULL, 0,0,0,0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW );
		}

		::EndDeferWindowPos(hdwp);
	}



private:
	vector<SyncWindowData>   m_vecAnchorItems;
	HWND   m_hHostWnd;                 // 表示自己当前要跟随哪个窗口进行移动。如果为NULL则表示自己不跟随其它窗口
	bool   m_bSendByDeferWindowPos;    // WM_WINDOWPOSCHANGING消息的发送者
	byte   m_nAnchorMagnetCapability;  // 停靠的磁性能力。当小于多少px时自动粘合
	bool   m_bAnchorToScreenEdge;      // 是否计算停靠到屏幕边缘
	bool   m_bAnchorToOtherSize;       // 是否计算依靠到其它跟随者
	bool   m_bSizeMove;                // 标明当前是否正在拖拽窗口
};
#else
//
// 采用拦截鼠标的按下拖拽消息，自己来实现窗口拖拽的效果
//
// 缺点：
//    1. 较为复杂
//    2. 只能拦截鼠标拖拽的情况，通过代码移动窗口的情况无法拦截，窗口最大化最小化不好处理
//
template<class T>
class SyncWindowHelper
{
public:
	SyncWindowHelper()
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

			vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
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

	HDWP   HandleWindowAnchor(HDWP& hdwp, const CRect& rcSrcWindow, const SyncWindowData& rData)
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
	bool   AddAnchorItem(const SyncWindowData& data)
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
		vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
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
	bool   ModifyAnchorItem(const SyncWindowData& data)
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

	void   HideAllAnchorItem()
	{
		int nSize = m_vecAnchorItems.size()+1;
		HDWP hdwp = ::BeginDeferWindowPos(nSize);

		T* pThis = static_cast<T*>(this);
		hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, NULL, 0,0,0,0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW );
	
		vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
		vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
		for (; iter!=iterEnd; iter++)
		{
			hdwp = ::DeferWindowPos(hdwp, iter->m_hWnd, NULL, 0,0,0,0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW );
		}

		::EndDeferWindowPos(hdwp);
	}


private:
	long     m_lSizeMove;    // 0 没开始，1鼠标左键已按下，2鼠标开始拖拽
	POINT    m_ptCursorSizeMove;
	POINT    m_ptWndPosSizeMove;

	vector<SyncWindowData>   m_vecAnchorItems;
};
#endif

