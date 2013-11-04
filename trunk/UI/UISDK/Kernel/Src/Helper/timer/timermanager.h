#pragma once

namespace UI
{

//
//  ��ʱ�������࣬��ʱ�䵽��֮�󣬽����ö�����WM_TIMER��Ϣ
//
class TimerHelper
{
public:
    typedef map<UINT_PTR, TimerItem> _MyMap;
    typedef map<UINT_PTR, TimerItem>::iterator _MyIter;

	static TimerHelper* GetInstance();
	TimerHelper();

	UINT_PTR  SetTimer(int nElapse, IMessage* pNotify);
    UINT_PTR  SetTimerById(int nElapse, int nId, IMessage* pNotify);
	UINT_PTR  SetTimer(int nElapse, TimerItem* pItem);
	void  KillTimer(UINT_PTR& nTimerIdRef);
    void  KillTimerById(int nId, IMessage* pNotify);

	static VOID CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime );
	void  OnTimer(UINT_PTR idEvent);

private:
//	CStdCallThunk            m_thunk;
//	map<UINT_PTR, Message*>  m_mapTimerNotify;
	map<UINT_PTR, TimerItem> m_mapTimerItem;
};


}