#pragma once
#include "..\\MP3.h"
#include <DShow.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "strmiids.lib")

class CDirectShowEngine;

#define EVENTEX_NOTIFY_MSG  (WM_USER+1)
#define TIMER_ID_PROGRESS   1
class CMessageOnlyWindow : public CWindowImpl<CMessageOnlyWindow>
{
public:
	CMessageOnlyWindow(CDirectShowEngine* p);

	BEGIN_MSG_MAP_EX(CMessageOnlyWindow)
		MESSAGE_HANDLER_EX(EVENTEX_NOTIFY_MSG, OnNotifyMsg)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

	HRESULT   OnNotifyMsg(UINT msg, WPARAM w, LPARAM l);
	void      OnTimer(UINT_PTR nIDEvent);

	void      StartTimer( bool bStartNow = false );
	void      EndTimer();

private:
	CDirectShowEngine*  m_pThis;
};

class CDirectShowEngine : public ISoundEngine
{
public:
	CDirectShowEngine(void);
	virtual ~CDirectShowEngine(void);

public:
	HRESULT  Init(CMP3* pMgr, HWND hMainWnd);
	HRESULT  Release();

	HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt );
	HRESULT  Play();
	HRESULT  Pause();
	HRESULT  Stop();
	HRESULT  SetCurPos(double);
	HRESULT  SetVolume(double);
	HRESULT  Mute(bool);

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

	CMP3*  m_pMgr;

};
