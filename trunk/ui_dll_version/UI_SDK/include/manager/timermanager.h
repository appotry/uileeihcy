#pragma once

//
// ������Ҫʹ�ü�ʱ���Ķ��󣬿��ԴӸü̳У����ö�������Message������
// ��Ϊ��ʹ�÷��� WM_TIMER �ķ�ʽ����֪ͨ
//
template<class T>
class TimerIpml
{
public:
	TimerIpml()
	{
		m_thunk.Init((DWORD_PTR)TimerIpml<T>::TimerProc, this);
	}
	UINT_PTR SetNewTimer(int nElapse)
	{
		return ::SetTimer(NULL, 0, nElapse, (TIMERPROC)m_thunk.GetCodeAddress());
	}
	void KillTimer(UINT_PTR& nTimerIdRef)
	{
		::KillTimer(0, nTimerIdRef);
		nTimerIdRef = 0;
	}

	static VOID CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime )
	{
		TimerIpml* pThis = (TimerIpml*)hwnd;
		pThis->__OnTimer__(idEvent);
	}
	void  __OnTimer__(UINT_PTR idEvent)
	{
		Message* pThis = (Message*)this;
		if (NULL == pThis)
		{
			UIASSERT(0);
			return;
		}

		::UISendMessage(pThis, WM_TIMER, idEvent, NULL);
	}
private:
	CStdCallThunk    m_thunk;
};


