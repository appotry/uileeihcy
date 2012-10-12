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
    
#define POSITION_EVENT_COUNT 2    // ֪ͨ�¼�����
#define NOTIFY_EVENT_COUNT  (POSITION_EVENT_COUNT+1)    // EVENT���������һ����msg֪ͨ��

class DSMSG_PARAM
{
};

// �����ļ�����Ļ���
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

	HANDLE         m_hEvents[NOTIFY_EVENT_COUNT];  // ����position��֪ͨ�¼�
	HANDLE         m_hEventThread;
	DWORD          m_dwEventThreadID;              // �¼�֪ͨ�߳�ID

	int            m_nDirectSoundBufferSize;       // �������Ĵ�С������һ����Ա�����������Ժ�̬�޸�
	int            m_nPerEventBufferSize;          // ÿ���¼���Ҫ����buffer��С������������ֻ����һ��

	CMP3*          m_pMgr;
	CMessageOnlyWindow*  m_pWnd;
};
