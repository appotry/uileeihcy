#pragma once
class IMp3EventCallback;
class CMP3;

#define EVENTEX_NOTIFY_MSG  (WM_USER+1)
#define TIMER_ID_PROGRESS   1
class CMessageOnlyWindow : public CWindowImpl<CMessageOnlyWindow>
{
public:
	CMessageOnlyWindow(CMP3* p);

	BEGIN_MSG_MAP_EX(CMessageOnlyWindow)
		MESSAGE_HANDLER_EX(EVENTEX_NOTIFY_MSG, OnNotifyMsg)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

	HRESULT   OnNotifyMsg(UINT msg, WPARAM w, LPARAM l);
	void      OnTimer(UINT_PTR nIDEvent);

	void      StartTimer( bool bStartNow = false );
	void      EndTimer();

private:
	CMP3*   m_pThis;
};


class CMP3
{
public:
	CMP3(void);
	~CMP3(void);

	bool    Init();
	bool    Release();
	bool    AddEventCallback(IMp3EventCallback* p);
	
	bool    RenderFile( const String& strFile );
	bool    Play();
	bool    Pause();
	bool    Stop();
	bool    SetCurPos(double);
	bool    SetVolume(double);
	bool    Mute(bool);

protected:
	CComPtr<IGraphBuilder>   m_pGraphBuilder;
	CComPtr<IMediaControl>   m_pMediaControl;
	CComPtr<IMediaEventEx>   m_pMediaEventEx;
	CComPtr<IMediaSeeking>   m_pMediaSeeking;
	CComPtr<IMediaPosition>  m_pMediaPosition;
	CComPtr<IBasicAudio>     m_pBasicAudio;
	
	long     m_nVolumn;   // 记录当前音量，用于在取消静音时使用
	bool     m_bMute;

	CMessageOnlyWindow   m_hWndEvent;
	friend class CMessageOnlyWindow;


	list<IMp3EventCallback*>  m_listEventCallback;
};
