#pragma once
#include "..\\MP3.h"
#include <DShow.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "strmiids.lib")

class CDirectShowEngine;

class CDirectShowEngine : public ISoundEngine
{
public:
	CDirectShowEngine(void);
	virtual ~CDirectShowEngine(void);

public:
	HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent);
	HRESULT  Release();

	HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt );
	HRESULT  ClearRender();
	HRESULT  Play();
	HRESULT  Pause();
	HRESULT  Stop();
	HRESULT  SetCurPos(double);
	HRESULT  GetCurPos(double* pdSeconds, double* pdPercent);
	HRESULT  SetVolume(long lVolumn);

public:
	HRESULT  OnNoitfy(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CComPtr<IGraphBuilder>   m_pGraphBuilder;
	CComPtr<IMediaControl>   m_pMediaControl;
	CComPtr<IMediaEventEx>   m_pMediaEventEx;
	CComPtr<IMediaSeeking>   m_pMediaSeeking;
	CComPtr<IMediaPosition>  m_pMediaPosition;
	CComPtr<IBasicAudio>     m_pBasicAudio;

	long     m_nVolumn;   // 记录当前音量，用于在取消静音时使用
	bool     m_bMute;

	CMessageOnlyWindow*  m_pWnd;
	friend class CMessageOnlyWindow;

	CMP3*  m_pMgr;

};
