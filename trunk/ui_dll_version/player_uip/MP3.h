#pragma once
#include "MessageOnlyWindow.h"
#include "player_uip.h"
#include "sound\SpectrumAnalyser.h"
class IMp3EventCallback;
class CMP3;

class ISoundEngine
{
public:
	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent) = 0;
	virtual HRESULT  Release() = 0;

	virtual HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt ) = 0;
	virtual HRESULT  ClearRender() = 0;
	virtual HRESULT  Play() = 0;
	virtual HRESULT  Pause() = 0;
	virtual HRESULT  Stop() = 0;
	virtual HRESULT  SetCurPos(double) = 0;
	virtual HRESULT  GetCurPos(double* pdSeconds, double* pdPercent) = 0;
	virtual int      GetPlayBuffer( void *pBufferToFill,int FillBufferSize ) = 0;
	virtual void     OnNoitfy(UINT uMsg, WPARAM w, LPARAM l) = 0;

	virtual HRESULT  SetVolume(long) = 0;
	virtual HRESULT  SetPan(long lPan) = 0;
	virtual HRESULT  SetEq(E_EQ_FREQ eFreq, int nValue) = 0;
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
	bool    SetVolume(long lPercent);
	bool    SetPan(long lPanPercent);
	bool    Mute(bool);
	bool    SetVisualization(VisualizationInfo* pInfo);
	bool    SetEq(E_EQ_FREQ eFreq, int nValue);
	HWND    GetMainWnd() { return m_hMainWnd; }
	CSpectrumAnalyser* GetSA() { return &m_SA; }
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
	HWND     m_hMainWnd;

	long     m_nVolumn;;   // 记录当前音量，用于在取消静音时使用
	bool     m_bMute;      // 当前是否静音
	long     m_lPan;
	int      m_arrEqValue[EQ_FREQ_COUNT];
// 
	CMessageOnlyWindow   m_WndEvent;
	friend class CMessageOnlyWindow;

	ISoundEngine*   m_pCurrentEngine;
	ISoundEngine*   m_pDirectShowEngine;
	ISoundEngine*   m_pDirectSoundEngine;

	CSpectrumAnalyser         m_SA;
	list<IMp3EventCallback*>  m_listEventCallback;
};

