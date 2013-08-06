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
	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent);
	virtual HRESULT  Release();

	virtual HRESULT  RenderFile( const TCHAR* szFile, eMp3Type eType);
	virtual HRESULT  ClearRender();
	virtual HRESULT  Play();
	virtual HRESULT  Pause();
	virtual HRESULT  Stop();
	virtual HRESULT  SetCurPos(double);
	virtual HRESULT  GetCurPos(double* pdSeconds, double* pdPercent);
	virtual int      GetPlayBuffer( void *pBufferToFill,int FillBufferSize ){ return 0; }  // 不支持
	virtual void     OnNoitfy(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual HRESULT  SetVolume(long lVolumn);
	virtual HRESULT  SetPan(long lPan) { return E_NOTIMPL; }
	virtual HRESULT  SetEq(E_EQ_FREQ eFreq, int nValue) { return E_NOTIMPL; }
	virtual HRESULT  EnableEq(bool bEnable) { return E_NOTIMPL; }
public:
	

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
