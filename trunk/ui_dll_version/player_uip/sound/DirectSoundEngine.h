#pragma once
#include "..\\MP3.h"

#include <MMSystem.h>
#include <MMReg.h>
#include <dsound.h>
#include <DxErr.h>
#include "mpg123.h"

#ifdef _DEBUG
#pragma comment(lib, "libmpg123_d.lib")
#else
#pragma comment(lib, "libmpg123.lib")
#endif

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dxguid.lib")

//
// TODO:
//   在等待线程中调用COM接口的STOP函数，会有问题吗？
//

class ISoundFile
{
public:
	virtual  ~ISoundFile() = 0 {};
	virtual  HRESULT   RenderFile(const TCHAR* szFile) = 0;
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead) = 0;
	virtual  WAVEFORMATEX*  GetFormat() { return &m_wfx; }
	virtual  HRESULT   SetCurPos(double percent) = 0;
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent) = 0;

protected:
	WAVEFORMATEX   m_wfx;
};
class CWavFile : public ISoundFile
{
public:
	CWavFile();
	~CWavFile();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent) {return E_NOTIMPL;}
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent) {return E_NOTIMPL;}
};
class CMp3File : public ISoundFile
{
public:
	CMp3File();
	~CMp3File();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent);
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent);
protected:
	mpg123_handle*     m_hMpg123;   // mp3解码器
};

#define USE_THREAD
#define POSITION_EVENT_COUNT 2

#ifdef USE_THREAD
#define NOTIFY_EVENT_COUNT  (POSITION_EVENT_COUNT+1)
#else
#define NOTIFY_EVENT_COUNT  POSITION_EVENT_COUNT
#endif


class DSMSG_PARAM
{};

class CDirectSoundEngine : public ISoundEngine
{
public:
	CDirectSoundEngine(void);
	virtual ~CDirectSoundEngine(void);

	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent);
	virtual HRESULT  Release();

	virtual HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt );
	virtual HRESULT  Play();
	virtual HRESULT  Pause();
	virtual HRESULT  Stop();
	virtual HRESULT  SetCurPos(double);
	virtual HRESULT  GetCurPos(double* pdSeconds, double* pdPercent);
	virtual HRESULT  SetVolume(double);
	virtual HRESULT  Mute(bool);

protected:
	HRESULT PushBuffer(int nStart, int nCount);
	void    SetBufferSize(int nSize);
	int     GetBufferSize() { return m_nDirectSoundBufferSize; }

public:
#ifdef USE_THREAD	
	void    EventThreadProc();
	bool    PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam);
	HRESULT OnSetCurPos(double dPercent);

#else
	void    TimerCallback(UINT nTimerID, UINT nMsg);
#endif

protected:
	IDirectSound8*        m_pDirectSound8;
	IDirectSoundBuffer8*  m_pDirectSoundBuffer8;

	ISoundFile*    m_pCurFile;
	ISoundFile*    m_pMp3File;
	ISoundFile*    m_pWavFile;

	HANDLE         m_hEvents[NOTIFY_EVENT_COUNT];  // 各个position的通知事件
#ifdef USE_THREAD
	HANDLE         m_hEventThread;
	DWORD          m_dwThreadID;
#else
	UINT           m_nTimerID;
#endif
	int            m_nDirectSoundBufferSize;         // 缓冲区的大小。作成一个成员变量，便于以后动态修改
	int            m_nPerEventBufferSize;            // 每次事件需要填充的buffer大小，保存起来，只计算一次

	CMP3*          m_pMgr;
	CMessageOnlyWindow*  m_pWnd;
};
