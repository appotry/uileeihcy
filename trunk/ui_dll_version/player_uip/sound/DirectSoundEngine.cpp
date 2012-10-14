#include "StdAfx.h"
#include "DirectSoundEngine.h"
#include "Mp3File.h"
#include "WavFile.h"


//////////////////////////////////////////////////////////////////////////
//
//  �߳�֪ͨ��Ϣ����

// ���õ�ǰ�Ľ���
#define  DSMSG_SET_CUR_POS    (WM_USER+1)    
class DSMSG_PARAM_SET_CUR_POS : public DSMSG_PARAM
{
public:
	double   dPercent;
};

#define  DSMSG_PLAY    (WM_USER+2)    
#define  DSMSG_PAUSE   (WM_USER+3)    
#define  DSMSG_STOP    (WM_USER+4)    

DSMSG_PARAM* BuildSetCurPosParam(double dPercent)
{
	DSMSG_PARAM_SET_CUR_POS* p = new DSMSG_PARAM_SET_CUR_POS;
	p->dPercent = dPercent;
	return (DSMSG_PARAM*)p;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CDirectSoundEngine::CDirectSoundEngine(void)
{
	m_pMgr = NULL;
	m_pDirectSound8 = NULL;
	m_pDirectSoundBuffer8 = NULL;
	m_pCurFile = NULL;
	m_pMp3File = NULL;
	m_pWavFile = NULL;

	for (int i = 0; i < NOTIFY_EVENT_COUNT; i++)
	{
		m_hEvents[i] = 0;
	}
	m_hEventThread = NULL;
	m_dwEventThreadID = 0;

	this->SetBufferSize(32*1024);
}

CDirectSoundEngine::~CDirectSoundEngine(void)
{
	this->Release();
}
DWORD WINAPI gNotifyThreadProc( LPVOID lpParameter)
{
	CDirectSoundEngine* pThis = (CDirectSoundEngine*)lpParameter;
	if (NULL == pThis)
		return -1;

	pThis->EventThreadProc();
	return 0;
}

HRESULT CDirectSoundEngine::Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent)
{
	if (NULL == pWndEvent || NULL == pMgr)
		return E_INVALIDARG;

	m_pMgr = pMgr;
	m_pWnd = pWndEvent;
	
	HRESULT hr = ::DirectSoundCreate8(NULL, &m_pDirectSound8, NULL);
	if (NULL == m_pDirectSound8)
		return hr;

	hr = m_pDirectSound8->SetCooperativeLevel(pWndEvent->m_hWnd, DSSCL_PRIORITY);  // ������ھ����������ΪNULL������᷵����Ч��������
	if (FAILED(hr))
		return hr;

	for (int i = 0; i < NOTIFY_EVENT_COUNT; i++)
	{
		m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	m_hEventThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gNotifyThreadProc, (LPVOID)this, 0, &m_dwEventThreadID);
	if (NULL == m_hEventThread)
		return E_FAIL;

	return S_OK;
}
HRESULT CDirectSoundEngine::Release()
{
	m_pCurFile = NULL;
	SAFE_DELETE(m_pMp3File);
	SAFE_DELETE(m_pWavFile);

	if(m_hEventThread != NULL)
	{
		SetEvent(m_hEvents[POSITION_EVENT_COUNT]);
		Sleep(1);
		TerminateThread(m_hEventThread, 0);
		CloseHandle(m_hEventThread);
		m_hEventThread = NULL;
		m_dwEventThreadID = 0;
	}
	for(int i = 0; i < NOTIFY_EVENT_COUNT; ++ i)
	{
		if(m_hEvents[i] != NULL)
		{
			CloseHandle(m_hEvents[i]);
			m_hEvents[i] = NULL;
		}
	}

	SAFE_RELEASE(m_pDirectSoundBuffer8);
	SAFE_RELEASE(m_pDirectSound8);
	
	return S_OK;
}

HRESULT CDirectSoundEngine::RenderFile( const TCHAR* szFile, const TCHAR* szExt )
{
	if (NULL == szFile)
		return E_INVALIDARG;

	if (NULL == m_pDirectSound8)
		return E_FAIL;

	this->ClearRender();
		
	if (0 == _tcsicmp(szExt, _T("wav")))
	{
		if (NULL == m_pWavFile)
		{
			m_pWavFile = new CWavFile();
		}
		m_pCurFile = m_pWavFile;
	}
	else if (0 == _tcsicmp(szExt, _T("mp3")))
	{
		if (NULL == m_pMp3File)
		{
			m_pMp3File = new CMp3File();
		}
		m_pCurFile = m_pMp3File;
	}

	if (NULL != m_pCurFile)
	{
		HRESULT hr = m_pCurFile->RenderFile(szFile);
		if (FAILED(hr))
			return hr;

		// ����������

		DSBUFFERDESC desc;
		memset (&desc, 0, sizeof(DSBUFFERDESC));

		desc.dwSize = sizeof(DSBUFFERDESC);
		// ע��DSBCAPS_GLOBALFOCUS����������������־��������ʧȥ�����ʱ��directsound ��ֹͣ����
		//     DSBCAPS_CTRLPOSITIONNOTIFY����������������־��QueryInterface IID_IDirectSoundNotify8 �᷵��NOINTERFACE
		desc.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY;  
		desc.lpwfxFormat =  m_pCurFile->GetFormat();
		SetBufferSize(desc.lpwfxFormat->nAvgBytesPerSec);
		desc.dwBufferBytes = m_nDirectSoundBufferSize;    //3*desc.lpwfxFormat->nAvgBytesPerSec;  // ����3�����������

		IDirectSoundBuffer*  pBuffer = NULL;
		IDirectSoundNotify8* pNotify = NULL;
		hr = m_pDirectSound8->CreateSoundBuffer(&desc, &pBuffer, NULL);
		if (FAILED(hr))
			return hr;

		hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDirectSoundBuffer8);
		hr = pBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&pNotify);  // ��Ҫ���� DSBCAPS_CTRLPOSITIONNOTIFY
		SAFE_RELEASE(pBuffer);
		if (NULL == pNotify)
			return hr;

		// �����������֪ͨ�¼�
		DSBPOSITIONNOTIFY  notifys[POSITION_EVENT_COUNT];
		for (int i = 0; i < POSITION_EVENT_COUNT; i++)
		{
			notifys[i].dwOffset = m_nDirectSoundBufferSize/POSITION_EVENT_COUNT*(i+1) - 1;
			notifys[i].hEventNotify = m_hEvents[i];
		}
		hr = pNotify->SetNotificationPositions(POSITION_EVENT_COUNT, notifys);
		SAFE_RELEASE(pNotify);
		if (FAILED(hr))
			return hr;

		// ��һ�����������buffer
		hr = this->PushBuffer(0, m_nDirectSoundBufferSize);

		m_SA.Create(m_pMgr->GetMainWnd(), m_pCurFile->GetFormat()->nChannels, m_pCurFile->GetFormat()->wBitsPerSample/8);
		return hr;
	}

	return E_FAIL;;
}

HRESULT  CDirectSoundEngine::ClearRender()
{
	m_pCurFile = NULL;
	SAFE_RELEASE(m_pDirectSoundBuffer8);
	return S_OK;
}

HRESULT CDirectSoundEngine::Play()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	this->PostThreadMessage(DSMSG_PLAY, NULL);
	return S_OK;
}

HRESULT CDirectSoundEngine::OnPlay()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	// �����������DSBPLAY_LOOPING����������������ȡʱ�ܻص���������ȡ
	HRESULT hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);

	m_pWnd->StartTimer(true);  // �����������UI��㱨���Ⱥ�ʱ��
	return hr;
}
HRESULT CDirectSoundEngine::Pause()
{
	if (NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	this->PostThreadMessage(DSMSG_PAUSE, NULL);
	return S_OK;
}
HRESULT CDirectSoundEngine::OnPause()
{
	if (NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	m_pWnd->EndTimer();
	return m_pDirectSoundBuffer8->Stop();
}
HRESULT CDirectSoundEngine::Stop()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	this->PostThreadMessage(DSMSG_STOP, NULL);
	return S_OK;
}

HRESULT CDirectSoundEngine::OnStop()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	m_pWnd->EndTimer();

	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if(SUCCEEDED(hr))
	{
		// ��ջ�����
		LPVOID pBitPart1 = NULL;
		DWORD  dwSizePart1 = 0;
		HRESULT hr = m_pDirectSoundBuffer8->Lock(0, 0, &pBitPart1, &dwSizePart1,NULL, NULL, DSBLOCK_ENTIREBUFFER);
		if (FAILED(hr))
			return hr;

		memset(pBitPart1, 0, dwSizePart1);
		hr = m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, NULL, 0);
		if (FAILED(hr))
			return hr;

		hr = m_pDirectSoundBuffer8->SetCurrentPosition(0);
		if (FAILED(hr))
			return hr;

		m_pCurFile->SetCurPos(0);
	}
	return hr;
}
HRESULT CDirectSoundEngine::SetCurPos(double percent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	this->PostThreadMessage(DSMSG_SET_CUR_POS, BuildSetCurPosParam(percent));
	return S_OK;
}
HRESULT CDirectSoundEngine::OnSetCurPos(double dPercent)
{
	if (NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DWORD dwState = 0;
	m_pDirectSoundBuffer8->GetStatus(&dwState);  // ��ȡ��ǰ��״̬������������λ�ú�ָ�

	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if (FAILED(hr))
		return hr;

	hr = m_pCurFile->SetCurPos(dPercent);
	if (FAILED(hr))
		return hr;

	// ���¼��ػ���
	hr = PushBuffer(0, GetBufferSize());  // ??? ע��������ﲻ����������ݵĻ����������������ϴβ��������ݣ�����Ϊ�
	if (FAILED(hr))
		return hr;

	hr = m_pDirectSoundBuffer8->SetCurrentPosition(0);
	if (FAILED(hr))
		return hr;

	if (dwState & DSBSTATUS_PLAYING)
	{
		hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);
		if (FAILED(hr))
			return hr;
	}

	// ֪ͨ�����������µ�ǰ����
	if (NULL != m_pWnd)
		::PostMessage(m_pWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);

	return S_OK;
}

HRESULT CDirectSoundEngine::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	return m_pCurFile->GetCurPos(pdSeconds, pdPercent);
}
HRESULT CDirectSoundEngine::SetVolume(long lVolumn)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	HRESULT hr = m_pDirectSoundBuffer8->SetVolume(lVolumn);
	return hr;
}

bool CDirectSoundEngine::PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam)
{
	if (FALSE == ::PostThreadMessage(m_dwEventThreadID, uMsg, (WPARAM)pParam, 0))
		return false;

	::SetEvent(m_hEvents[POSITION_EVENT_COUNT]);
	return true;
}
void CDirectSoundEngine::EventThreadProc()
{
	if (0 == POSITION_EVENT_COUNT)
		return;

	// ֪ͨ���̴߳�����Ϣ����
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	while(true)
	{
		DWORD nRet = ::WaitForMultipleObjects(NOTIFY_EVENT_COUNT, m_hEvents, FALSE, INFINITE);
		if (nRet == WAIT_FAILED)
			return;

		int nIndex = nRet - WAIT_OBJECT_0;
		if (nIndex >= 0 && nIndex < POSITION_EVENT_COUNT)
		{
			if (FAILED (this->PushBuffer(m_nPerEventBufferSize*nIndex, m_nPerEventBufferSize)))
			{
				this->Stop();
				m_pMgr->Fire_on_mp3_stop();
			}
			else
			{
				
			}
		}
		else if (nIndex == POSITION_EVENT_COUNT)
		{
			this->EventMsgProc();
		}
		else
		{
			continue;
		}
	}

	this->Stop();

	return;
}

void CDirectSoundEngine::EventMsgProc()
{
	MSG  msg;
	while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		DSMSG_PARAM* pDSMSG_PARAM = (DSMSG_PARAM*)msg.wParam;
		switch(msg.message)
		{
		case DSMSG_SET_CUR_POS:
			{
				DSMSG_PARAM_SET_CUR_POS* pParam = static_cast<DSMSG_PARAM_SET_CUR_POS*>(pDSMSG_PARAM);
				this->OnSetCurPos(pParam->dPercent);
			}
			break;
		case DSMSG_PLAY:
			this->OnPlay();
			break;

		case DSMSG_PAUSE:
			this->OnPause();
			break;

		case DSMSG_STOP:
			this->OnStop();
			break;
		}

		SAFE_DELETE(pDSMSG_PARAM);
	}
}

void CDirectSoundEngine::SetBufferSize(int nSize)
{
	m_nDirectSoundBufferSize = nSize;
	m_nPerEventBufferSize = m_nDirectSoundBufferSize/POSITION_EVENT_COUNT;
}
// �����������������
HRESULT CDirectSoundEngine::PushBuffer(int nStart, int nCount)
{
	if (NULL == m_pDirectSoundBuffer8 || NULL == m_pCurFile)
		return E_FAIL;

	LPVOID pBitPart1 = NULL,   pBitPart2 = NULL;
	DWORD   dwSizePart1 = NULL, dwSizePart2 = NULL;

	HRESULT hr = m_pDirectSoundBuffer8->Lock(nStart, nCount, &pBitPart1, &dwSizePart1, &pBitPart2, &dwSizePart2, 0);
	if (FAILED(hr))
		return hr;

	DWORD outsize = 0;
	BYTE* pbSoundData = new BYTE[ dwSizePart1 ];
	hr = m_pCurFile->Read(pbSoundData, dwSizePart1, &outsize);
	if (FAILED(hr))
	{
		delete[] pbSoundData;
		return hr;
	}

	memcpy(pBitPart1, pbSoundData, outsize);
	delete[] pbSoundData;

	if(pBitPart2 != 0)
	{
		pbSoundData = new BYTE[ dwSizePart2 ];
		hr = m_pCurFile->Read(pbSoundData, dwSizePart2, &outsize);
		if (FAILED(hr))
		{
			delete[] pbSoundData;
			return hr;
		}

		memcpy(pBitPart2, pbSoundData, outsize);
		delete[] pbSoundData;
	}

	hr = m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
	return hr;
}


//////////////////////////////////////////////////////////////////////////
// FFT part

DWORD CDirectSoundEngine::GetDistance( int Cursor1,int Cursor2 )
{
	int distance = Cursor2 - Cursor1;
	while (distance < 0)
		distance += m_nDirectSoundBufferSize;
	return distance;
}

//BufferSize record whole DS buffer Size where can be written
int CDirectSoundEngine::GetAvailable( DWORD* PlayCursor, DWORD* WriteCursor,int* bufferSize, BOOL fromPlayCursor )
{
	return 0;
#if 0
	int available=0;
	if (NULL == m_pDirectSoundBuffer8 || NULL == m_pCurFile)
	{
		return 0;
	}
	if (FAILED(m_pDirectSoundBuffer8->GetCurrentPosition(PlayCursor,WriteCursor)))
	{
		return 0;
	}
	int Processing=GetDistance((int)*PlayCursor,(int)*WriteCursor);
	if ((UINT)Processing > m_nDirectSoundBufferSize/2)
	{
		*WriteCursor=*PlayCursor;
		Processing=0;
	}
	*bufferSize=m_nDirectSoundBufferSize;
	if (fromPlayCursor)
	{
		*bufferSize += Processing;
	}
	if(m_WritePos==-1)
	{
		available=*bufferSize;
	}
	else
	{
		int currWriteAhead = GetDistance( fromPlayCursor ? (int)*PlayCursor : (int)*WriteCursor, m_WritePos);
		if (currWriteAhead > *bufferSize) 
		{
			*bufferSize = currWriteAhead;
			available = 0;
		} 
		else 
		{
			available = *bufferSize - currWriteAhead;
		}
	}
	return available;
#endif
}
int CDirectSoundEngine::GetPlayBuffer( void *pBufferToFill,int FillBufferSize )
{
	if (NULL == m_pDirectSoundBuffer8 || NULL == m_pCurFile)
	{
		return 0;
	}
	DWORD PlayCursor,WriteCursor,buffer1Locklen,buffer2Locklen;
	void* buffer1, *buffer2;
	if (FAILED(m_pDirectSoundBuffer8->GetCurrentPosition(&PlayCursor,&WriteCursor)))
	{
		return 0;
	}
	int process=GetDistance(PlayCursor,WriteCursor);
	if (FillBufferSize>process)
	{
		return 0;
	}
	HRESULT res=m_pDirectSoundBuffer8->Lock(PlayCursor, FillBufferSize,
		(LPVOID *) &buffer1, &buffer1Locklen,
		(LPVOID *) &buffer2, &buffer2Locklen,
		0);
	if (SUCCEEDED(res))
	{
		if (buffer2)
		{
			memcpy_s(pBufferToFill,buffer1Locklen,buffer1,buffer1Locklen);
			memcpy_s((UCHAR *)pBufferToFill+buffer1Locklen,buffer2Locklen,buffer2,buffer2Locklen);
		}
		else
		{
			memcpy_s(pBufferToFill,FillBufferSize,buffer1,buffer1Locklen);
		}
		m_pDirectSoundBuffer8->Unlock(buffer1, buffer1Locklen, buffer2, buffer2Locklen);

		return buffer1Locklen+buffer2Locklen;
	}
	else
	{
		return 0;
	}
}
