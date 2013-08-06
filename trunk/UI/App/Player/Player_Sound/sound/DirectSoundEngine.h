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
//#pragma comment(lib, "sound\\mpg123-1.14.4\\ports\\MSVC++\\2008\\Debug\\libmpg123.lib")
#pragma comment(lib, "libmpg123_d.lib")
#else
#pragma comment(lib, "libmpg123.lib")
#endif



#define POSITION_EVENT_COUNT 2    // ֪ͨ�¼�����
#define NOTIFY_EVENT_COUNT  (POSITION_EVENT_COUNT+1)    // EVENT���������һ����msg֪ͨ[msg�����ѱ�����]��


// �����ļ�����Ļ���
class ISoundFile
{
public:
	virtual  ~ISoundFile() = 0 {};

	// ����Ҫ���ŵ��ļ��������ʼ���ͽ����ļ���WAVEFORMATEX
	virtual  HRESULT   RenderFile(const TCHAR* szFile) = 0;

	// ��ȡdwSizeToRead��С�����ݵ�directsound buffer�н��в��ţ�pdwSizeRead��������ȡ�������ݴ�С 
	// ����ȡ����ʱ������FAILED hr��directsound�� stop
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead) = 0;

	// ���ش���directsound bufferʹ�õĲ���
	virtual  WAVEFORMATEX*  GetFormat() { return &m_wfx; }

	// ���õ�ǰ���ŵ�λ��
	virtual  HRESULT   SetCurPos(double percent) = 0;

	// ���ص�ǰ�Ĳ��Ž��ȣ��ú�����ȡ�����������⣬���ǻ�ȡ��ǰ����λ�ã�������ȡ���˵�ǰ��λ�ã�������Ҫ�Ľ���
	virtual  HRESULT   GetCurPos(int nPlayBufferSize, double* pdSeconds, double* pdPercent) = 0;

	virtual  HRESULT   SetEq(int nBand, double nValue){ return E_NOTIMPL; } // �����ã�TO DELETE
protected:
	WAVEFORMATEX   m_wfx;
};

// ����direct sound�е� write cursor:
//  The write cursor moves with the play cursor, not with data written to the buffer. 
//  If you're streaming data, you are responsible for maintaining your own pointer into 
//  the buffer to indicate where the next block of data should be written.
//
//  An application can retrieve the play and write cursors by calling the 
//  IDirectSoundBuffer8::GetCurrentPosition method. The IDirectSoundBuffer8::SetCurrentPosition 
//  method lets you move the play cursor. Applications do not control the position of 
//  the write cursor.


class CDirectSoundEngine : public ISoundEngine
{
public:
	CDirectSoundEngine(void);
	virtual ~CDirectSoundEngine(void);

	virtual HRESULT  Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent);
	virtual HRESULT  Release();

	virtual HRESULT  RenderFile( const TCHAR* szFile, eMp3Type eType);
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
	virtual HRESULT  SetEq(E_EQ_FREQ eFreq, int nValue);
	virtual HRESULT  SetEqPreamp(int nValue);
	virtual HRESULT  EnableEq(bool bEnable);

protected:
	HRESULT PushBuffer(int nStart, int nCount);
	void    SetBufferSize(int nSize);
	int     GetBufferSize() { return m_nDirectSoundBufferSize; }

public:
	void    EventThreadProc();

	DWORD   GetDistance( int Cursor1,int Cursor2 );

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

	HANDLE         m_hEvents[NOTIFY_EVENT_COUNT];  // ����position��֪ͨ�¼�
	HANDLE         m_hEventThread;
	DWORD          m_dwEventThreadID;              // �¼�֪ͨ�߳�ID // TODO. �ñ�����ɾ����

	int            m_nDirectSoundBufferSize;       // �������Ĵ�С������һ����Ա�����������Ժ�̬�޸�
	int            m_nPerEventBufferSize;          // ÿ���¼���Ҫ����buffer��С������������ֻ����һ��

	LONGLONG       m_nPlayCursor;
	LONGLONG       m_nWriteCursor;                 // ����Directsound�����ṩĿǰ��buffer���ύ�˶������ݣ���Ҫ�����Լ�ά��һ��

	CMP3*          m_pMgr;
	CMessageOnlyWindow*  m_pMessageOnlyWnd;
	HWND           m_hMessageOnlyWnd;
	
	bool           m_bEqEnable;                    // �Ƿ����þ�����

	CRITICAL_SECTION  m_cs;
};
