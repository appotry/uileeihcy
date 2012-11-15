#pragma once

//
//  计时器辅助类，在时间到达之后，将给该对象发送WM_TIMER消息
//
struct TimerItem
{
	int  nRepeatCount;   // 响应次数,-1表示无限
	Message* pNotify;    // 响应WM_TIMER的对象
	WPARAM   wParam;
	LPARAM   lParam; 

	TimerItem()
	{
		nRepeatCount = -1;
		pNotify = NULL;
		wParam = 0;
		lParam = 0;
	}
};
class TimerHelper
{
public:
	static TimerHelper* GetInstance()
	{
		static TimerHelper s;
		return &s;
	}

	TimerHelper()
	{
//		m_thunk.Init((DWORD_PTR)TimerHelper::TimerProc, this);
	}

	UINT_PTR SetNewTimer(int nElapse, Message* pNotify)
	{
		if (NULL == pNotify)
			return 0;
		
//		UINT_PTR nTimerID = ::SetTimer(NULL, 0, nElapse, (TIMERPROC)m_thunk.GetCodeAddress());

		TimerItem  ti;
		ti.pNotify = pNotify;
		return this->SetNewTimer(nElapse, &ti);
	}
	UINT_PTR SetNewTimer(int nElapse, TimerItem* pItem)
	{
		if (NULL == pItem)
			return 0;

		UINT_PTR nTimerID = ::SetTimer(NULL, 0, nElapse, TimerHelper::TimerProc);
		m_mapTimerItem[nTimerID] = *pItem;

//		UI_LOG_DEBUG(_T("%s timerid:%d, pNotify:%x"), _T(__FUNCTION__), nTimerID, pItem->pNotify);
		return nTimerID;
	}
	void KillTimer(UINT_PTR& nTimerIdRef)
	{
		if (m_mapTimerItem.count(nTimerIdRef))
		{
			::KillTimer(0, nTimerIdRef);
//			UI_LOG_DEBUG(_T("%s timerid:%d, pNotify:%x"), _T(__FUNCTION__), nTimerIdRef);

			m_mapTimerItem.erase(nTimerIdRef);
			nTimerIdRef = 0;
		}
	}

	static VOID CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime )
	{
// 		TimerIpml* pThis = (TimerIpml*)hwnd;
// 		pThis->__OnTimer__(idEvent);
		TimerHelper::GetInstance()->OnTimer(idEvent);
	}
	void OnTimer(UINT_PTR idEvent)
	{
		if (0 == m_mapTimerItem.count(idEvent))
		{
			UIASSERT(0);
			TimerHelper::GetInstance()->KillTimer(idEvent);
			return;
		}
		TimerItem& ti = m_mapTimerItem[idEvent];

		if (-1 != ti.nRepeatCount)
			ti.nRepeatCount --;

		if (0 == ti.nRepeatCount)
		{
			::KillTimer(0, idEvent);
		}
		::UISendMessage(ti.pNotify, ui_ WM_TIMER, idEvent, (LPARAM)&ti);
		if (0 == ti.nRepeatCount)
		{
			KillTimer(idEvent);
		}
	}

private:
//	CStdCallThunk            m_thunk;
//	map<UINT_PTR, Message*>  m_mapTimerNotify;
	map<UINT_PTR, TimerItem> m_mapTimerItem;
};


