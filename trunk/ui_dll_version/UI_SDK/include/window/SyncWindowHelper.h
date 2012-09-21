#pragma once

//////////////////////////////////////////////////////////////////////////
//
//                          �����ƶ��Ĵ���
//
//////////////////////////////////////////////////////////////////////////

#define SYNC_NONE        0
#define SYNC_LEFT        0x0001
#define SYNC_RIGHT       0x0002
#define SYNC_TOP         0x0004
#define SYNC_BOTTOM      0x0008
#define SYNC_OUT_LEFT    0x0010
#define SYNC_OUT_RIGHT   0x0020
#define SYNC_OUT_TOP     0x0040
#define SYNC_OUT_BOTTOM  0x0080
#define SYNC_CUSTOM      0x0100   // ����UI_WM_SYNC_WINDOWPOSCHANGING��Ϣ�����ڣ��ɴ������о�������ƶ��Լ�

enum SYNC_WINDOW_EVENT_TYPE
{
	ADD_SYNC_WINDOW,
	MODIFY_SYNC_WINDOW,
	REMOVE_SYNC_WINDOW
};

struct  SyncData
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

	SyncData()
	{
		xOffset = yOffset = xOffset2 = yOffset2 = 0;
	}
};
struct  SyncWindowData
{
	HWND         m_hWnd;           
	UINT         m_nAnchorType;   
	SyncData     m_rcAnchorData;  // ��m_nAnchorType��Ӧλ��ֵʱ��m_rcAnchorData�ĸ�λ��Ч

	SyncWindowData()
	{
		m_hWnd = NULL;
		m_nAnchorType = SYNC_NONE;
	}
};

#if 1
//
//	ͨ����������WM_WINDOWPOSCHANGING��Ϣ�ķ���������ϵͳ�������Լ���ȥ����һ��DeferWindowPos
//
//	�ŵ�:
//		1. �򵥣�ֻ��Ҫ����һ����Ϣ
//		2. �ܴ�����ִ���λ�á���С�仯�ĳ���
//
//	ȱ��:
//		1. ���������ײ�����ѭ��
//
template<class T>
class SyncWindowHelper
{
public:
	SyncWindowHelper()
	{
		m_bSendByDeferWindowPos = false;
	}

protected:

	void _OnWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (0 == m_vecAnchorItems.size())
			return;

		if (m_bSendByDeferWindowPos)
		{
			m_bSendByDeferWindowPos = false;  // DeferWindowPos���ݹ�������Ϣ�����С�
			return;
		}
		T* pThis = static_cast<T*>(this);

		// ���㵱ǰ���ڵ��������С
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
			// ע��DeferWindowPos��֧��SWP_NOSENDCHANGING���ᱨ��Ч����������������������һ��
			//     bool ��������ֹ��ѭ��

			// ������Ҫͬ���ƶ��Ĵ������� 
			int nCount = this->GetVisibleHWNDCount()+1;  // +1 �Լ������m_hWnd
			HDWP hdwp = BeginDeferWindowPos(nCount);

			if (NULL == hdwp) break;

			UINT nFlag = lpWndPos->flags & ~0x01000000;  // ?? �����˵��ñ�ǻᵼ��DeferWindodwPosʧ�ܣ���֪��Ϊʲô
			hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, lpWndPos->hwndInsertAfter, lpWndPos->x, lpWndPos->y, lpWndPos->cx, lpWndPos->cy, nFlag/*|SWP_NOSENDCHANGING*/);
			if (NULL == hdwp) break;

			bool bLoopResult = true;
			vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
			for (; iter != iterEnd; iter++)
			{
				HWND hWnd = iter->m_hWnd;
				if (NULL != hWnd /*&& IsWindowVisible(hWnd)*/)
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

			// ע�������ﲻ�����SWP_NOZORDER������ᵼ�´�����קʱZ������ȷ
			lpWndPos->flags |= SWP_NOMOVE|SWP_NOSIZE; // �ܾ������޸ģ�ʹ��DeferWindowPos����Ϣ

			return;
		} while (0);

		// ʧ���ˣ�����Ĭ�Ϲ���
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


	HDWP   HandleWindowAnchor(HDWP& hdwp, const CRect& rcSrcWindow, const SyncWindowData& rData)
	{
		if (rData.m_nAnchorType == SYNC_CUSTOM)
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

		// �������뷽ʽ
		bool bLeft   = rData.m_nAnchorType&SYNC_LEFT? true:false;
		bool bRight  = rData.m_nAnchorType&SYNC_RIGHT? true:false;
		bool bTop    = rData.m_nAnchorType&SYNC_TOP? true:false;
		bool bBottom = rData.m_nAnchorType&SYNC_BOTTOM? true:false;

		bool bOutLeft   = rData.m_nAnchorType&SYNC_OUT_LEFT? true:false;
		bool bOutRight  = rData.m_nAnchorType&SYNC_OUT_RIGHT? true:false;
		bool bOutTop    = rData.m_nAnchorType&SYNC_OUT_TOP? true:false;
		bool bOutBottom = rData.m_nAnchorType&SYNC_OUT_BOTTOM? true:false;

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
		int nIndex = this->FindAnchorItem(data.m_hWnd);
		if (-1 == nIndex)
			return false;

		m_vecAnchorItems[nIndex] = data;
		return true;
	}
	bool   ClearAnchorItem()
	{
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

	int    GetVisibleHWNDCount()  // �����ж��Ƿ�ɼ�����ʱ���ɼ�Ҳ��Ҫ������Ҫ��Ȼ����ʾʱ��λ�þͲ���ȷ��
	{
		int nCount = 0;
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			HWND hWnd = m_vecAnchorItems[i].m_hWnd;
			if (NULL != hWnd /*&& ::IsWindowVisible(hWnd)*/)
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
	bool   m_bSendByDeferWindowPos;  // WM_WINDOWPOSCHANGING��Ϣ�ķ�����
};
#else
//
// �����������İ�����ק��Ϣ���Լ���ʵ�ִ�����ק��Ч��
// ǧǧ����Ӧ�þ��ǲ��������ַ�����ʵ�ֵ�
//
// ȱ�㣺
//    1. ��Ϊ����
//    2. ֻ�����������ק�������ͨ�������ƶ����ڵ�����޷����أ����������С�����ô���
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

			// �����µĴ���λ��
			POINT ptCursor;
			::GetCursorPos(&ptCursor);

			POINT ptNew = { m_ptWndPosSizeMove.x + ptCursor.x - m_ptCursorSizeMove.x,
				m_ptWndPosSizeMove.y + ptCursor.y - m_ptCursorSizeMove.y };

			// ������Ҫͬ���ƶ��Ĵ������� 
			T* pThis = static_cast<T*>(this);
			int nCount = this->GetVisibleHWNDCount()+1;
			HDWP hdwp = BeginDeferWindowPos(nCount);

			CRect  rcSourceWnd;
			::GetWindowRect(pThis->m_hWnd, &rcSourceWnd);
			hdwp = DeferWindowPos(hdwp, pThis->m_hWnd, NULL, ptNew.x, ptNew.y, 0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
			
			// ��Ҫ�����µ�Դ����RECT���ݽ�ȥ������ֱ�ӵ���GetWindowRect�õ���ֵ���Ǿɵ�
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

		// �������뷽ʽ
		bool bLeft   = rData.m_nAnchorType&SYNC_LEFT? true:false;
		bool bRight  = rData.m_nAnchorType&SYNC_RIGHT? true:false;
		bool bTop    = rData.m_nAnchorType&SYNC_TOP? true:false;
		bool bBottom = rData.m_nAnchorType&SYNC_BOTTOM? true:false;

		bool bOutLeft   = rData.m_nAnchorType&SYNC_OUT_LEFT? true:false;
		bool bOutRight  = rData.m_nAnchorType&SYNC_OUT_RIGHT? true:false;
		bool bOutTop    = rData.m_nAnchorType&SYNC_OUT_TOP? true:false;
		bool bOutBottom = rData.m_nAnchorType&SYNC_OUT_BOTTOM? true:false;

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
	long     m_lSizeMove;    // 0 û��ʼ��1�������Ѱ��£�2��꿪ʼ��ק
	POINT    m_ptCursorSizeMove;
	POINT    m_ptWndPosSizeMove;

	vector<SyncWindowData>   m_vecAnchorItems;
};
#endif

