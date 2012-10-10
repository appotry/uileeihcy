#pragma once
#include "MessageOnlyWindow.h"
class IMp3EventCallback;
class CMP3;

class ISoundEngine
{
public:
	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent) = 0;
	virtual HRESULT  Release() = 0;

	virtual HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt ) = 0;
	virtual HRESULT  Play() = 0;
	virtual HRESULT  Pause() = 0;
	virtual HRESULT  Stop() = 0;
	virtual HRESULT  SetCurPos(double) = 0;
	virtual HRESULT  GetCurPos(double* pdSeconds, double* pdPercent) = 0;
	virtual HRESULT  SetVolume(double) = 0;
	virtual HRESULT  Mute(bool) = 0;
};



class CMP3
{
public:
	CMP3(void);
	~CMP3(void);

	bool    Init(HWND hMainWnd);
	bool    Release();
	bool    AddEventCallback(IMp3EventCallback* p);
	
	bool    RenderFile( const String& strFile );
	bool    Play();
	bool    Pause();
	bool    Stop();
	bool    SetCurPos(double);
	bool    SetVolume(double);
	bool    Mute(bool);

public:
	void    Fire_on_mp3_volume_ind(long lVolumn);
	void    Fire_on_mp3_stop();
	void    Fire_on_mp3_progress_ind(double dSeconds, double dPercent);
protected:

// 	CComPtr<IGraphBuilder>   m_pGraphBuilder;
// 	CComPtr<IMediaControl>   m_pMediaControl;
// 	CComPtr<IMediaEventEx>   m_pMediaEventEx;
// 	CComPtr<IMediaSeeking>   m_pMediaSeeking;
// 	CComPtr<IMediaPosition>  m_pMediaPosition;
// 	CComPtr<IBasicAudio>     m_pBasicAudio;
// 	
// 	long     m_nVolumn;   // 记录当前音量，用于在取消静音时使用
// 	bool     m_bMute;
// 
	CMessageOnlyWindow   m_WndEvent;
	friend class CMessageOnlyWindow;

	ISoundEngine*   m_pCurrentEngine;
	ISoundEngine*   m_pDirectShowEngine;
	ISoundEngine*   m_pDirectSoundEngine;

	list<IMp3EventCallback*>  m_listEventCallback;
};

