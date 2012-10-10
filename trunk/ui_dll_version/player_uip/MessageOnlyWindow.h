#pragma once
class CDirectShowEngine;
class CDirectSoundEngine;
class CMP3;

#define EVENTEX_NOTIFY_MSG  (WM_USER+1)
#define TIMER_ID_PROGRESS   1
class CMessageOnlyWindow : public CWindowImpl<CMessageOnlyWindow>
{
public:
	CMessageOnlyWindow();

	BEGIN_MSG_MAP_EX(CMessageOnlyWindow)
		MESSAGE_HANDLER_EX(EVENTEX_NOTIFY_MSG, OnNotifyMsg)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

	HRESULT   OnNotifyMsg(UINT msg, WPARAM w, LPARAM l);
	void      OnTimer(UINT_PTR nIDEvent);

	void      StartTimer( bool bStartNow = false );
	void      EndTimer();
	
	void      SetEnginePtr(CMP3* pMgr, CDirectShowEngine* p1, CDirectSoundEngine* p2);

private:
// 	CDirectShowEngine*  m_pDirectShow;
// 	CDirectSoundEngine* m_pDirectSound;
	CMP3*  m_pMgr;
};