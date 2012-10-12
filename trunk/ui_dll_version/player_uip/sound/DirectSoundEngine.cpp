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

	m_pCurFile = NULL;
	SAFE_RELEASE(m_pDirectSoundBuffer8);
		
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

		return hr;
	}

	return E_FAIL;;
}




HRESULT CDirectSoundEngine::Play()
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
	m_pWnd->EndTimer();
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Stop()
{
	m_pWnd->EndTimer();

	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	m_pCurFile = NULL;
	m_pDirectSoundBuffer8->Stop();
	SAFE_RELEASE(m_pDirectSoundBuffer8);
	return E_NOTIMPL;
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

	hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);
	if (FAILED(hr))
		return hr;

	// ֪ͨ�����������µ�ǰ����
	if (NULL != m_pWnd)
		::PostMessage(m_pWnd->m_hWnd, EVENTEX_NOTIFY_MSG, TIMER_ID_PROGRESS, 0);

	return S_OK;
}

HRESULT CDirectSoundEngine::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	return m_pCurFile->GetCurPos(pdSeconds, pdPercent);
}
HRESULT CDirectSoundEngine::SetVolume(double percent)
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Mute(bool)
{
	return E_NOTIMPL;
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
			//this->Stop();
			while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				switch(msg.message)
				{
				case DSMSG_SET_CUR_POS:
					{
						DSMSG_PARAM_SET_CUR_POS* pParam = (DSMSG_PARAM_SET_CUR_POS*)msg.wParam;
						this->OnSetCurPos(pParam->dPercent);
						delete pParam;
					}
					break;
				}
				
			}
		}
		else
		{
			continue;
		}
	}

	this->Stop();

	return;
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