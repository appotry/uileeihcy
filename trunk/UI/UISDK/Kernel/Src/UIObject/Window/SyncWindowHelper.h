#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

//////////////////////////////////////////////////////////////////////////
//
//                          �����ƶ��Ĵ���
//
//////////////////////////////////////////////////////////////////////////

// TODO: 
//		1. ���ڶ���Ļ��Ե��ճ��
//		2. ���ڶ�����site window��ճ�������ճ��
//		3. host �����ƶ���site window��ճ��

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
		m_hHostWnd = NULL;

		m_nAnchorMagnetCapability = 10;
		m_bAnchorToScreenEdge = false;
		m_bAnchorToOtherSize = false;
		m_bSizeMove = false;
	}

protected:

	void   _OnWindowPosChanging(LPWINDOWPOS lpWndPos, BOOL& bHandled)
	{
        if (lpWndPos->flags & SWP_NOMOVE && lpWndPos->flags & SWP_NOSIZE) 
        {
            // ������������жϻᵼ��HostWindow�ڴ���С���ָ�ʱ��SiteWindow������һ��ָ�
            bHandled = FALSE;
            return;
        }

        bHandled = TRUE;
		// ���ж��Լ���ǰ�Ƿ�������������ڣ�Ȼ���ٴ����Լ����ƶ���
		if (m_hHostWnd)
		{
			if (this->OnSiteWindowPosChanging(lpWndPos))
				return;
		}

        if (m_vecAnchorItems.size() > 0)
        {
		    this->OnHostWindowPosChanging(lpWndPos);
        }
        else
        {
            bHandled = FALSE;
        }
	}

	void   OnHostWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (m_bSendByDeferWindowPos)
		{
			m_bSendByDeferWindowPos = false;  // DeferWindowPos���ݹ�������Ϣ�����С�
			return;
		}
		T* pThis = static_cast<T*>(this);

		// ���㵱ǰ���ڵ��������С
		CRect rc;
		this->GetRectByWNDPOS(lpWndPos, &rc);

		do 
		{
			// ע��DeferWindowPos��֧��SWP_NOSENDCHANGING���ᱨ��Ч����������������������һ��
			//     bool ��������ֹ��ѭ��

			// ������Ҫͬ���ƶ��Ĵ������� 
			int nCount = this->GetAvailableHWNDCount();

			// +1 �Լ������m_hWnd.���ֲ㴰���Լ�ά���Լ��Ĵ�С�ı䣩
			bool IsLayeredWindow = lpWndPos->flags & SWP_LAYEREDWINDOW_SIZEMOVE?true:false;
			if (!IsLayeredWindow)
				nCount += 1;
			
			HDWP hdwp = BeginDeferWindowPos(nCount);
			if (NULL == hdwp) break;

			if (!IsLayeredWindow)
			{
				UINT nFlag = lpWndPos->flags & ~0x01000000;  // ?? �����˵��ñ�ǻᵼ��DeferWindodwPosʧ�ܣ���֪��Ϊʲô
				hdwp = ::DeferWindowPos(hdwp, pThis->m_hWnd, lpWndPos->hwndInsertAfter, lpWndPos->x, lpWndPos->y, lpWndPos->cx, lpWndPos->cy, nFlag/*|SWP_NOSENDCHANGING*/);
				if (NULL == hdwp) break;
			}

			bool bLoopResult = true;
			vector<SyncWindowData>::iterator  iter = m_vecAnchorItems.begin();
			vector<SyncWindowData>::iterator  iterEnd = m_vecAnchorItems.end();
			for (; iter != iterEnd; iter++)
			{
				HWND hWnd = iter->m_hWnd;
				if (hWnd /*&& IsWindowVisible(hWnd)*/ && iter->m_bAnchorOn)
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

			if (!IsLayeredWindow)
			{
				m_bSendByDeferWindowPos = true;
			}
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
			if (hWnd && IsWindowVisible(hWnd))
			{
				HDWP hdwpNull = NULL;
				HandleWindowAnchor(hdwpNull, rc, *iter);
			}
		}
	}


	bool   OnSiteWindowPosChanging(LPWINDOWPOS lpWndPos)
	{
		if (m_bSendByDeferWindowPos)  // ���ε��ƶ�����ΪHOST����deferwindowpos�����ģ�����
		{
			m_bSendByDeferWindowPos = false;
			return false;
		}
		
		// �������������������ק���죬�򲻴������細�ڸմ�����
		// ����ֲ㴰������ק�ı��Сʱ��Ŀǰ������ENTERSIZEMOVE��Ϣ������ֱ�Ӵ���
		if (m_bSizeMove || lpWndPos->flags & SWP_LAYEREDWINDOW_SIZEMOVE)   
		{
			bool bSize = (lpWndPos->flags & SWP_NOSIZE)?false:true;   // ���ֶβ���ȷ����ʹֻ�϶����ڣ�Ҳû��SWP_NOSIZE��־
			bool bMove = (lpWndPos->flags & SWP_NOMOVE)?false:true;
			if (!bSize && !bMove)
				return false;

			SyncWindowData data;
			data.m_hWnd = GetMyHWND();

			CRect rcHost, rcSite, rcSiteOld;
			::GetWindowRect(m_hHostWnd, &rcHost);
			::GetWindowRect(data.m_hWnd, &rcSiteOld);
			this->GetRectByWNDPOS(lpWndPos, &rcSite);

			if (rcSiteOld.Width() != lpWndPos->cx || rcSiteOld.Height() != lpWndPos->cy)
			{
				bSize = true;
			}
			else
			{
				bSize = false;
			}
			bool bRet = this->CheckRectAnchor( &rcHost, &rcSite, bSize, &data );
			
			if (bRet)  // ճ���ˣ��޸Ĵ��ڵ�λ��
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
	// �ж����������ճ�Ϲ�ϵ�����ճ�ϣ����޸�prcSize��ֵ
	//
	// Return
	//    �������������Ƿ�ճ����
	//
	// Remark
	//  Q1. ������ֱ�����MOVE������SIZE����ǰ�������ڵĸߺͿ��Ա�lpWindowPos�еĸߺͿ������������Ƿ�����change size.
	//      TODO: ���ܻ᲻׼ȷ��...
	//
	bool   CheckRectAnchor(const CRect* prcHost, CRect* prcSite, bool bChangeSize, SyncWindowData* pData)
	{
		bool bLeftLeft = (abs(prcSite->left - prcHost->left) < m_nAnchorMagnetCapability);
		bool bLeftRight = (abs(prcSite->left - prcHost->right)< m_nAnchorMagnetCapability);
		bool bRightLeft = (abs(prcSite->right - prcHost->left) < m_nAnchorMagnetCapability);
		bool bRightRight = (abs(prcSite->right - prcHost->right) < m_nAnchorMagnetCapability);
		bool bX = bLeftLeft || bLeftRight || bRightRight || bRightLeft || 
			(prcSite->left>prcHost->left && prcSite->left<prcHost->right) || 
			(prcSite->right>prcHost->left && prcSite->right<prcHost->right) ||
			(prcHost->left>prcSite->left && prcHost->left<prcSite->right) ||
			(prcHost->right>prcSite->left && prcHost->right<prcSite->right);

		bool bTopTop = (abs(prcHost->top - prcSite->top) < m_nAnchorMagnetCapability);
		bool bTopBottom = (abs(prcSite->top - prcHost->bottom)< m_nAnchorMagnetCapability);
		bool bBottomTop = (abs(prcSite->bottom - prcHost->top) < m_nAnchorMagnetCapability);
		bool bBottomBottom = (abs(prcSite->bottom - prcHost->bottom) < m_nAnchorMagnetCapability);
		bool bY = bTopTop || bTopBottom || bBottomTop || bBottomBottom ||
			(prcSite->top>prcHost->top && prcSite->top<prcHost->bottom) ||
			(prcSite->bottom>prcHost->top && prcSite->bottom<prcHost->bottom) ||
			(prcHost->top>prcSite->top && prcHost->top<prcSite->bottom) ||
			(prcHost->bottom>prcSite->top && prcHost->bottom<prcSite->bottom);

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

		// ����nAnchorTypeֻͣ����һ��������һ������û�и�ֵ�����⡣
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

		// �������뷽ʽ
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
		// �Լ�������ʱ�����������
		if (m_hHostWnd)  // ֪ͨ���������Ƴ��Լ�
		{
			SyncWindowData data;
			data.m_hWnd = GetMyHWND();
			::SendMessage(m_hHostWnd, UI_WM_SYNC_WINDOW, REMOVE_SYNC_WINDOW, (LPARAM)&data);
		}

		// ֪ͨ���������Լ��Ĵ��ڣ���������������
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
	void   OnAddAnchorItem(HWND hHostWnd)     // �Լ���Ϊ������
	{
		UIASSERT(NULL == m_hHostWnd);
		m_hHostWnd = hHostWnd;
	}
	void   OnModifyAnchorItem(HWND hHostWnd)  // �Լ���Ϊ������
	{
		UIASSERT(NULL != m_hHostWnd);
	}
	void   OnRemoveAnchorItem(HWND hHostWnd)  // �Լ���Ϊ������
	{
		UIASSERT(hHostWnd == m_hHostWnd);
		m_hHostWnd = NULL;
	}

	// Host Window�ƶ�ʱ��ͬ��site Windowλ��ʱ����site window����Ϣ
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

	int    GetAvailableHWNDCount()  // �����ж��Ƿ�ɼ�����ʱ���ɼ�Ҳ��Ҫ������Ҫ��Ȼ����ʾʱ��λ�þͲ���ȷ��
	{
		int nCount = 0;
		int nSize = (int)m_vecAnchorItems.size();
		for (int i = 0; i < nSize; i++)
		{
			HWND hWnd = m_vecAnchorItems[i].m_hWnd;
			if (hWnd /*&& ::IsWindowVisible(hWnd)*/ && m_vecAnchorItems[i].m_bAnchorOn)
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
	HWND   m_hHostWnd;                 // ��ʾ�Լ���ǰҪ�����ĸ����ڽ����ƶ������ΪNULL���ʾ�Լ���������������
	bool   m_bSendByDeferWindowPos;    // WM_WINDOWPOSCHANGING��Ϣ�ķ�����
	byte   m_nAnchorMagnetCapability;  // ͣ���Ĵ�����������С�ڶ���pxʱ�Զ�ճ��
	bool   m_bAnchorToScreenEdge;      // �Ƿ����ͣ������Ļ��Ե
	bool   m_bAnchorToOtherSize;       // �Ƿ��������������������
	bool   m_bSizeMove;                // ������ǰ�Ƿ�������ק����
};
#else
//
// �����������İ�����ק��Ϣ���Լ���ʵ�ִ�����ק��Ч��
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
				if (hWnd && IsWindowVisible(hWnd))
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
			if (hWnd && ::IsWindowVisible(hWnd))
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

