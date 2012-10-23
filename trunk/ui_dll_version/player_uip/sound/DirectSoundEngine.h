#pragma once
#include "..\\MP3.h"

#include <MMSystem.h>
#include <MMReg.h>
#include <dsound.h>
#include <DxErr.h>
#include "mpg123.h"
#include "..\\threadmessage.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dxguid.lib")
#ifdef _DEBUG
#pragma comment(lib, "libmpg123_d.lib")
#else
#pragma comment(lib, "libmpg123.lib")
#endif



#define POSITION_EVENT_COUNT 2    // 通知事件个数
#define NOTIFY_EVENT_COUNT  (POSITION_EVENT_COUNT+1)    // EVENT数量（最后一个是msg通知[msg机制已被废弃]）


// 各种文件解码的基类
class ISoundFile
{
public:
	virtual  ~ISoundFile() = 0 {};

	// 设置要播放的文件。负责初始化和解析文件的WAVEFORMATEX
	virtual  HRESULT   RenderFile(const TCHAR* szFile) = 0;

	// 读取dwSizeToRead大小的数据到directsound buffer中进行播放，pdwSizeRead返回所读取到的数据大小 
	// 当读取结束时，返回FAILED hr，directsound将 stop
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead) = 0;

	// 返回创建directsound buffer使用的参数
	virtual  WAVEFORMATEX*  GetFormat() { return &m_wfx; }

	// 设置当前播放的位置
	virtual  HRESULT   SetCurPos(double percent) = 0;

	// 返回当前的播放进度（该函数获取的数据有问题，不是获取当前播放位置，而不获取到了当前读位置，有误差，需要改进）
	virtual  HRESULT   GetCurPos(int nPlayBufferSize, double* pdSeconds, double* pdPercent) = 0;

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
	virtual int      GetPlayBuffer( void *pBufferToFill,int FillBufferSize );
	virtual void     OnNoitfy(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual HRESULT  SetVolume(long);
	virtual HRESULT  SetPan(long lPan);

protected:
	HRESULT PushBuffer(int nStart, int nCount);
	void    SetBufferSize(int nSize);
	int     GetBufferSize() { return m_nDirectSoundBufferSize; }

public:
	void    EventThreadProc();

	DWORD   GetDistance( int Cursor1,int Cursor2 );
	int     GetAvailable( DWORD* PlayCursor, DWORD* WriteCursor,int* bufferSize, BOOL fromPlayCursor );

protected:
//	bool    EventMsgProc();
//	bool    PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam);
	HRESULT OnSetCurPos(double dPercent);
	HRESULT OnPlay();
	HRESULT OnPause();
	HRESULT OnStop();
	HRESULT OnClearRender();


protected:
	IDirectSound8*        m_pDirectSound8;
	IDirectSoundBuffer8*  m_pDirectSoundBuffer8;

	ISoundFile*    m_pCurFile;
	ISoundFile*    m_pMp3File;
	ISoundFile*    m_pWavFile;
	ISoundFile*    m_pWmaFile;

	HANDLE         m_hEvents[NOTIFY_EVENT_COUNT];  // 各个position的通知事件
	HANDLE         m_hEventThread;
	DWORD          m_dwEventThreadID;              // 事件通知线程ID // TODO. 该变量可删除了

	int            m_nDirectSoundBufferSize;       // 缓冲区的大小。作成一个成员变量，便于以后动态修改
	int            m_nPerEventBufferSize;          // 每次事件需要填充的buffer大小，保存起来，只计算一次

	CMP3*          m_pMgr;
	CMessageOnlyWindow*  m_pMessageOnlyWnd;
	HWND           m_hMessageOnlyWnd;
	
	CRITICAL_SECTION  m_cs;
};
