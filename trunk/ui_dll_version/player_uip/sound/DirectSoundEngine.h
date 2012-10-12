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
    
#define POSITION_EVENT_COUNT 2    // 通知事件个数
#define NOTIFY_EVENT_COUNT  (POSITION_EVENT_COUNT+1)    // EVENT数量（最后一个是msg通知）

class DSMSG_PARAM
{
};

// 各种文件解码的基类
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


class CDirectSoundEngine : public ISoundEngine
{
public:
	CDirectSoundEngine(void);
	virtual ~CDirectSoundEngine(void);

	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent);
	virtual HRESULT  Release();

	virtual HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt );
	virtual HRESULT  ClearRender();
	virtual HRESULT  Play();
	virtual HRESULT  Pause();
	virtual HRESULT  Stop();
	virtual HRESULT  SetCurPos(double);
	virtual HRESULT  GetCurPos(double* pdSeconds, double* pdPercent);
	virtual HRESULT  SetVolume(long);

protected:
	HRESULT PushBuffer(int nStart, int nCount);
	void    SetBufferSize(int nSize);
	int     GetBufferSize() { return m_nDirectSoundBufferSize; }

public:
	void    EventThreadProc();
	bool    PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam);
	HRESULT OnSetCurPos(double dPercent);
	HRESULT OnPlay();
	HRESULT OnPause();
	HRESULT OnStop();

protected:
	IDirectSound8*        m_pDirectSound8;
	IDirectSoundBuffer8*  m_pDirectSoundBuffer8;

	ISoundFile*    m_pCurFile;
	ISoundFile*    m_pMp3File;
	ISoundFile*    m_pWavFile;

	HANDLE         m_hEvents[NOTIFY_EVENT_COUNT];  // 各个position的通知事件
	HANDLE         m_hEventThread;
	DWORD          m_dwEventThreadID;              // 事件通知线程ID

	int            m_nDirectSoundBufferSize;       // 缓冲区的大小。作成一个成员变量，便于以后动态修改
	int            m_nPerEventBufferSize;          // 每次事件需要填充的buffer大小，保存起来，只计算一次

	CMP3*          m_pMgr;
	CMessageOnlyWindow*  m_pWnd;
};
