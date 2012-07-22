#pragma once

//
//  计时器辅助类，在时间到达之后，将给该对象发送WM_TIMER消息
//
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

		UINT_PTR nTimerID = ::SetTimer(NULL, 0, nElapse, TimerHelper::TimerProc);
		m_mapTimerNotify[nTimerID] = pNotify;
		return nTimerID;
	}
	void KillTimer(UINT_PTR& nTimerIdRef)
	{
		if (m_mapTimerNotify.count(nTimerIdRef))
		{
			::KillTimer(0, nTimerIdRef);

			m_mapTimerNotify.erase(nTimerIdRef);
			nTimerIdRef = 0;
		}
	}

	static VOID CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime )
	{
// 		TimerIpml* pThis = (TimerIpml*)hwnd;
// 		pThis->__OnTimer__(idEvent);
		TimerHelper::GetInstance()->__OnTimer__(idEvent);
	}
	void  __OnTimer__(UINT_PTR idEvent)
	{
		Message* pNotify = NULL;
		if (0 == m_mapTimerNotify.count(idEvent))
		{
			UIASSERT(0);
			TimerHelper::GetInstance()->KillTimer(idEvent);
			return;
		}
		pNotify = m_mapTimerNotify[idEvent];
		if (NULL == pNotify)
		{
			UIASSERT(0);
			TimerHelper::GetInstance()->KillTimer(idEvent);
			return;
		}

		::UISendMessage(pNotify, WM_TIMER, idEvent, NULL);
	}

private:
//	CStdCallThunk            m_thunk;
	map<UINT_PTR, Message*>  m_mapTimerNotify;
};


