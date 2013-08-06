#include "StdAfx.h"
#include "DirectSoundEngine.h"
#include "Mp3File.h"
#include "WavFile.h"
#include "WmaFile.h"
#include "Equalizer/eq.h"

//////////////////////////////////////////////////////////////////////////

double f[] = {16000, 8000, 4000, 2000, 1000, 500, 250, 125, 63, 32.1, 15.75};

double  GetFreqByType(E_EQ_FREQ e)
{
	return f[e];
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
	m_pWmaFile = NULL;

	for (int i = 0; i < NOTIFY_EVENT_COUNT; i++)
	{
		m_hEvents[i] = 0;
	}
	m_hEventThread = NULL;
	m_dwEventThreadID = 0;

//	this->SetBufferSize(32*1024);
	InitializeCriticalSection(&m_cs); 
	m_bEqEnable = false;

	m_nWriteCursor = m_nPlayCursor = 0;
}

CDirectSoundEngine::~CDirectSoundEngine(void)
{
	this->Release();
	::DeleteCriticalSection(&m_cs);
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
	m_pMessageOnlyWnd = pWndEvent;
	m_hMessageOnlyWnd = pWndEvent->m_hWnd;
	
	HRESULT hr = ::DirectSoundCreate8(NULL, &m_pDirectSound8, NULL);
	if (NULL == m_pDirectSound8)
		return hr;

	hr = m_pDirectSound8->SetCooperativeLevel(m_hMessageOnlyWnd, DSSCL_PRIORITY);  // ������ھ����������ΪNULL������᷵����Ч��������
	if (FAILED(hr))
		return hr;

	for (int i = 0; i < NOTIFY_EVENT_COUNT; i++)
	{
		m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	m_hEventThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gNotifyThreadProc, (LPVOID)this, 0, &m_dwEventThreadID);
	if (NULL == m_hEventThread)
		return E_FAIL;

	// ��������ʼ��
	init_equliazer(0);

	return S_OK;
}
HRESULT CDirectSoundEngine::Release()
{
	if(m_hEventThread != NULL)
	{
//		this->PostThreadMessage(DSMSG_QUIT,NULL);
		SetEvent(m_hEvents[NOTIFY_EVENT_COUNT-1]);
		WaitForSingleObject(m_hEventThread, 2000);
		TerminateThread(m_hEventThread, 0);
		CloseHandle(m_hEventThread);
		m_hEventThread = NULL;
		m_dwEventThreadID = 0;
	}

	m_pCurFile = NULL;   // ������ÿ�Ҫ�����߳̽���֮�󣬷�����ܵ����߳��п�ָ�����
	SAFE_DELETE(m_pMp3File);
	SAFE_DELETE(m_pWavFile);
	SAFE_DELETE(m_pWmaFile);

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
	
	// �ͷž�����
	uninit_equliazer();
	return S_OK;
}

HRESULT CDirectSoundEngine::RenderFile( const TCHAR* szFile, eMp3Type eType)
{
	if (NULL == szFile)
		return E_INVALIDARG;

	if (NULL == m_pDirectSound8)
		return E_FAIL;

	DECLARE_CS_PROTECT
		
	if (eType == MP3)
	{
		if (NULL == m_pMp3File)
		{
			m_pMp3File = new CMp3File();
		}
		m_pCurFile = m_pMp3File;
	}
	else if (eType == WMA)
	{
		if (NULL == m_pWmaFile)
		{
			m_pWmaFile = new CWmaFile();
		}
		m_pCurFile = m_pWmaFile;
	}
	else if (eType == WAV)
	{
		if (NULL == m_pWavFile)
		{
			m_pWavFile = new CWavFile();
		}
		m_pCurFile = m_pWavFile;
	}
	

	if (m_pCurFile)
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
		desc.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFX|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_GETCURRENTPOSITION2 ;  
		desc.lpwfxFormat =  m_pCurFile->GetFormat();
		SetBufferSize(desc.lpwfxFormat->nAvgBytesPerSec*2); // ��Ϊʹ��IIR�ľ�����������1channel8bits����Ƶ�����������2��������ɳɳ������������������ΪIIR����Ĭ�����еĶ���16bits�ɣ�
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
		m_nPlayCursor = m_nWriteCursor = 0;
		hr = this->PushBuffer(0, m_nDirectSoundBufferSize);
		if (FAILED(hr))
			return hr;

		m_pMgr->GetSA()->RenderFile(m_pCurFile->GetFormat()->nChannels, m_pCurFile->GetFormat()->wBitsPerSample/8);

#if 0
		//EQ
		DSEFFECTDESC effectdesc[10];
		for (int i = 0; i < 10; i++)
		{
			memset(&effectdesc[i], 0, sizeof(DSEFFECTDESC));
			effectdesc[i].dwSize = sizeof(DSEFFECTDESC);
			effectdesc[i].guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
		}
		hr = m_pDirectSoundBuffer8->SetFX(10, effectdesc, NULL);

		for (int i = 0; i < EQ_FREQ_COUNT; i++)
		{
			IDirectSoundFXParamEq8* pEq = NULL;
			m_pDirectSoundBuffer8->GetObjectInPath(GUID_DSFX_STANDARD_PARAMEQ,i,
				IID_IDirectSoundFXParamEq8, (void**)&pEq);

			DSFXParamEq param;
			param.fCenter = (float)f[i];
			param.fGain = 0;
			param.fBandwidth = 12;
			pEq->SetAllParameters(&param);
			pEq->Release();
		}
#endif
		return hr;
	}
	
	return E_FAIL;;
}

HRESULT  CDirectSoundEngine::ClearRender()  // ��������������delete m_pDirectSoundBuffer8����Ϊbuffer����߳̿�������ʹ��m_pDirectSoundBuffer8����û���ü���ӦOnStop
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DECLARE_CS_PROTECT;
//	this->PostThreadMessage(DSMSG_CLEAR, NULL);
	this->OnClearRender();
	return S_OK;
}

HRESULT CDirectSoundEngine::OnClearRender()
{
	m_pCurFile = NULL;
	SAFE_RELEASE(m_pDirectSoundBuffer8);
	return S_OK;
}

HRESULT CDirectSoundEngine::Play()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DECLARE_CS_PROTECT;

//	this->PostThreadMessage(DSMSG_PLAY, NULL);
	OnPlay();
	return S_OK;
}

HRESULT CDirectSoundEngine::OnPlay()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	// �����������DSBPLAY_LOOPING����������������ȡʱ�ܻص���������ȡ
	HRESULT hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);

	if (SUCCEEDED(hr))
	{
		m_pMgr->GetSA()->Play();

// 		m_pMessageOnlyWnd->StartTimer(false);  // �����������UI��㱨���Ⱥ�ʱ�䣬ע��Ҫ�� true�������������߳�
// 		if (m_pMessageOnlyWnd)         // ���ڲ������߳��У�postmessage֪ͨ�����������µ�ǰ����
// 			::PostMessage(m_pMessageOnlyWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);
	}
	return hr;
}
HRESULT CDirectSoundEngine::Pause()
{
	if (NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DECLARE_CS_PROTECT;
//	this->PostThreadMessage(DSMSG_PAUSE, NULL);
	OnPause();
	return S_OK;
}
HRESULT CDirectSoundEngine::OnPause()
{
	if (NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

//	m_pMessageOnlyWnd->EndTimer();
	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if (SUCCEEDED(hr))
	{
		m_pMgr->GetSA()->Pause();
	}
	return hr;
}
HRESULT CDirectSoundEngine::Stop()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DECLARE_CS_PROTECT;
//	this->PostThreadMessage(DSMSG_STOP, NULL);
	OnStop();
	return S_OK;
}

HRESULT CDirectSoundEngine::OnStop()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	m_nPlayCursor = 0;
//	m_pMessageOnlyWnd->EndTimer();

	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if(SUCCEEDED(hr))
	{
		// ��ջ�����
// 		LPVOID pBitPart1 = NULL;
// 		DWORD  dwSizePart1 = 0;
// 		HRESULT hr = m_pDirectSoundBuffer8->Lock(0, 0, &pBitPart1, &dwSizePart1,NULL, NULL, DSBLOCK_ENTIREBUFFER);
// 		if (FAILED(hr))
// 			return hr;
// 
// 		memset(pBitPart1, 0, dwSizePart1);
// 		hr = m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, NULL, 0);
// 		if (FAILED(hr))
// 			return hr;
// 
// 		m_nWriteCursor = dwSizePart1;

		m_pCurFile->SetCurPos(0);
		hr = m_pDirectSoundBuffer8->SetCurrentPosition(0);
		if (FAILED(hr))
			return hr;

		// �ٴ����������buffer�������´������Ȼ���Ÿ��ļ�
		m_nPlayCursor = m_nWriteCursor = 0;
		hr = this->PushBuffer(0, m_nDirectSoundBufferSize);
		if (FAILED(hr))
			return hr;

		m_pMgr->GetSA()->Stop();
	}
	return hr;
}
HRESULT CDirectSoundEngine::SetCurPos(double percent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	DECLARE_CS_PROTECT;
//	this->PostThreadMessage(DSMSG_SET_CUR_POS, BuildSetCurPosParam(percent));
	OnSetCurPos(percent);
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
	if (FAILED(hr))   // �п��ܽ�λ���϶������ļ���������
	{
		this->Stop();
		m_pMgr->Fire_on_mp3_finish();
		return hr;
	}

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
// 	if (m_pMessageOnlyWnd)
// 		::PostMessage(m_pMessageOnlyWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);

	return S_OK;
}

HRESULT CDirectSoundEngine::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	int process= 0;
	DWORD PlayCursor = 0,WriteCursor = 0;
	if (SUCCEEDED(m_pDirectSoundBuffer8->GetCurrentPosition(&PlayCursor,NULL)))  // �ڶ��������з��ص�WriteCursor��������д�������write cursor��������ʲô����Ҳ��֪����
	{
		if (0 == m_nWriteCursor)
			process = 0;
		else
		{
			WriteCursor = m_nWriteCursor%m_nDirectSoundBufferSize;
			process = GetDistance(PlayCursor,WriteCursor);
		}
	}
	else
	{
		return E_FAIL;
	}
	
	return m_pCurFile->GetCurPos(process, pdSeconds, pdPercent);
}
HRESULT CDirectSoundEngine::SetVolume(long lVolumn)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	HRESULT hr = m_pDirectSoundBuffer8->SetVolume(lVolumn);
	return hr;
}

// ��ʵ�������������������ĳ����������������ͦ��ֵ�
// DSBPAN_LEFT to DSBPAN_RIGHT.
HRESULT CDirectSoundEngine::SetPan(long lPan)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	HRESULT hr = m_pDirectSoundBuffer8->SetPan(lPan);
	return hr;
}

// �ù�����Ҫdirectsoundbuffer����DSBCAPS_CTRLFX
HRESULT CDirectSoundEngine::EnableEq(bool bEnable)
{
	m_bEqEnable = bEnable;
	return S_OK;
}

HRESULT CDirectSoundEngine::SetEq(E_EQ_FREQ eFreq, int nValue)
{
	if (eFreq > EQ_FREQ_COUNT || eFreq < 0)
		return false;

	if (NULL == m_pDirectSoundBuffer8 || NULL == m_pCurFile)
		return E_FAIL;

#if 0
	IDirectSoundFXParamEq8* pEq = NULL;
	HRESULT hr = m_pDirectSoundBuffer8->GetObjectInPath(GUID_DSFX_STANDARD_PARAMEQ, eFreq,
		IID_IDirectSoundFXParamEq8, (void**)&pEq);
	if (FAILED(hr) || NULL == pEq)
		return hr;

	DSFXParamEq param;
	param.fCenter = (float)GetFreqByType(eFreq);
	param.fGain = (float)nValue;
	param.fBandwidth = 36;
	hr = pEq->SetAllParameters(&param);
	pEq->Release();

	return hr;
#endif


#if 1
	int nChannels = m_pCurFile->GetFormat()->nChannels;
	for (int i = 0; i < 2/*nChannels*/; i++)
	{
		if (EQ_FREQ_PREAMP == eFreq)
		{
			::set_eq_value((float)nValue, -1, i);
		}
		else
		{
			::set_eq_value((float)nValue, eFreq, i);
		}
	}
	
#endif
//	m_pCurFile->SetEq(eFreq, nValue);
	return S_OK;
}

HRESULT CDirectSoundEngine::SetEqPreamp(int nValue)
{
	if (NULL == m_pDirectSoundBuffer8 || NULL == m_pCurFile)
		return E_FAIL;

	if (nValue > 12 || nValue < -12)
		return E_INVALIDARG;

	int nChannels = m_pCurFile->GetFormat()->nChannels;
	for (int i = 0; i < 2/*nChannels*/; i++)
	{
		::set_eq_value((float)nValue, -1, i);
	}
	
	return S_OK;
}

// bool CDirectSoundEngine::PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam)
// {
// 	if (FALSE == ::PostThreadMessage(m_dwEventThreadID, uMsg, (WPARAM)pParam, 0))
// 		return false;
// 
// 	::SetEvent(m_hEvents[POSITION_EVENT_COUNT]);
// 	return true;
// }
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
			DECLARE_CS_PROTECT;

			if (NULL == m_pDirectSoundBuffer8)
				continue;

			if (FAILED (this->PushBuffer(m_nPerEventBufferSize*nIndex, m_nPerEventBufferSize)))
			{
				::PostMessage(m_hMessageOnlyWnd, EVENTEX_NOTIFY_MSG, 0, (LPARAM)this);
				// this->Stop();
				// m_pMgr->Fire_on_mp3_finish(); �ú�����֧�ֶ��̣߳����Ը������߳�postmessage��ʵ��ֹͣ��ǰ�ļ�
			}
			else
			{
				
			}
		}

 		else if (nIndex == POSITION_EVENT_COUNT)
		{
			// �˳���֪ͨ
			return;
		}
// 		{
// 			assert(0);
// 			if(false == this->EventMsgProc())
// 				return ;
// 		}
		else
		{
			continue;
		}
	}

	this->Stop();

	return;
}

// ����false��ʾ���˳��߳���
// bool CDirectSoundEngine::EventMsgProc()
// {
// 	MSG  msg;
// 	while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
// 	{
// 		DSMSG_PARAM* pDSMSG_PARAM = (DSMSG_PARAM*)msg.wParam;
// 		switch(msg.message)
// 		{
// 		case DSMSG_SET_CUR_POS:
// 			{
// 				DSMSG_PARAM_SET_CUR_POS* pParam = static_cast<DSMSG_PARAM_SET_CUR_POS*>(pDSMSG_PARAM);
// 				this->OnSetCurPos(pParam->dPercent);
// 			}
// 			break;
// 		case DSMSG_PLAY:
// 			this->OnPlay();
// 			break;
// 
// 		case DSMSG_PAUSE:
// 			this->OnPause();
// 			break;
// 
// 		case DSMSG_STOP:
// 			this->OnStop();
// 			break;
// 
// 		case DSMSG_CLEAR:
// 			this->OnClearRender();
// 			break;
// 
// 		case DSMSG_QUIT:
// 			return false;
// 			break;
// 
// 		}
// 
// 		SAFE_DELETE(pDSMSG_PARAM);
// 	}
// 	return true;
// }

// ��message only windowת����Ϣ���л���ǰ�߳�
void  CDirectSoundEngine::OnNoitfy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->Stop();
	m_pMgr->Fire_on_mp3_finish();
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

	// If the buffer was lost, restore and retry lock. 
	if (DSERR_BUFFERLOST == hr) 
	{ 
#ifdef _DEBUG
		assert(0 && _T("DSERR_BUFFERLOST == hr ��������������"));
#endif
		m_pDirectSoundBuffer8->Restore(); 
		hr = m_pDirectSoundBuffer8->Lock(nStart, nCount, &pBitPart1, &dwSizePart1, &pBitPart2, &dwSizePart2, 0);
	} 

	if (FAILED(hr))
		return hr;

	DWORD outsize = 0;
	BYTE* pbSoundData = new BYTE[ dwSizePart1 ];
	hr = m_pCurFile->Read(pbSoundData, dwSizePart1, &outsize);
	if (FAILED(hr))       // ˵�����������
	{
		delete[] pbSoundData;
		m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
		return hr;   
	}

	// ����������1
	if (m_bEqEnable)
	{
		do_equliazer((short*)pbSoundData,outsize, m_pCurFile->GetFormat()->nSamplesPerSec, m_pCurFile->GetFormat()->nChannels);
//		Equliazation()
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
			m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
			return hr;
		}

		// ����������2
		if (m_bEqEnable)
		{
			do_equliazer((short*)pbSoundData,outsize, m_pCurFile->GetFormat()->nSamplesPerSec, m_pCurFile->GetFormat()->nChannels);
		}

		memcpy(pBitPart2, pbSoundData, outsize);
		delete[] pbSoundData;
	}

	m_nWriteCursor += dwSizePart2 + dwSizePart1;
	hr = m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
	return hr;
}


//////////////////////////////////////////////////////////////////////////
// FFT part

DWORD CDirectSoundEngine::GetDistance( int Cursor1,int Cursor2 )
{
	int distance = Cursor2 - Cursor1;
	while (distance <= 0)
		distance += m_nDirectSoundBufferSize;
	return distance;
}

// TODO: ����������ȡbuffer
// This method returns write pointers only. The application should not try to read sound data from this pointer, 
// because the data might not be valid. For example, if the buffer is located in on-card memory, the pointer 
// might be an address to a temporary buffer in system memory. When IDirectSoundBuffer8::Unlock is called, the
// contents of this temporary buffer are transferred to the on-card memory.
//
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
// 	if (FillBufferSize>process)
// 	{
// 		return 0;
// 	}
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

