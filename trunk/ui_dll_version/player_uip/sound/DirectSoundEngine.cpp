#include "StdAfx.h"
#include "DirectSoundEngine.h"
#include "Mp3File.h"
#include "WavFile.h"
#include "WmaFile.h"

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

	this->SetBufferSize(4068/*32*1024*/);
	InitializeCriticalSection(&m_cs); 
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

	hr = m_pDirectSound8->SetCooperativeLevel(m_hMessageOnlyWnd, DSSCL_PRIORITY);  // 这个窗口句柄不能设置为NULL，否则会返回无效参数错误
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
	SAFE_DELETE(m_pWmaFile);

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

	DECLARE_CS_PROTECT
		
	if (0 == _tcsicmp(szExt, _T("mp3")))
	{
		if (NULL == m_pMp3File)
		{
			m_pMp3File = new CMp3File();
		}
		m_pCurFile = m_pMp3File;
	}
	else if (0 == _tcsicmp(szExt, _T("wma")))
	{
		if (NULL == m_pWmaFile)
		{
			m_pWmaFile = new CWmaFile();
		}
		m_pCurFile = m_pWmaFile;
	}
	else if (0 == _tcsicmp(szExt, _T("wav")))
	{
		if (NULL == m_pWavFile)
		{
			m_pWavFile = new CWavFile();
		}
		m_pCurFile = m_pWavFile;
	}
	

	if (NULL != m_pCurFile)
	{
		HRESULT hr = m_pCurFile->RenderFile(szFile);
		if (FAILED(hr))
			return hr;

		// 创建缓冲区

		DSBUFFERDESC desc;
		memset (&desc, 0, sizeof(DSBUFFERDESC));

		desc.dwSize = sizeof(DSBUFFERDESC);
		// 注：DSBCAPS_GLOBALFOCUS，如果不加上这个标志，当窗口失去焦点的时候，directsound 会停止播放
		//     DSBCAPS_CTRLPOSITIONNOTIFY，如果不加上这个标志，QueryInterface IID_IDirectSoundNotify8 会返回NOINTERFACE
		desc.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY;  
		desc.lpwfxFormat =  m_pCurFile->GetFormat();
		SetBufferSize(desc.lpwfxFormat->nAvgBytesPerSec);
		desc.dwBufferBytes = m_nDirectSoundBufferSize;    //3*desc.lpwfxFormat->nAvgBytesPerSec;  // 持续3秒的流缓冲区

		IDirectSoundBuffer*  pBuffer = NULL;
		IDirectSoundNotify8* pNotify = NULL;
		hr = m_pDirectSound8->CreateSoundBuffer(&desc, &pBuffer, NULL);
		if (FAILED(hr))
			return hr;

		hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDirectSoundBuffer8);
		hr = pBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&pNotify);  // 需要开启 DSBCAPS_CTRLPOSITIONNOTIFY
		SAFE_RELEASE(pBuffer);
		if (NULL == pNotify)
			return hr;

		// 设置流缓冲的通知事件
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

		// 第一次填充完整的buffer
		hr = this->PushBuffer(0, m_nDirectSoundBufferSize);
		if (FAILED(hr))
			return hr;

		m_pMgr->GetSA()->RenderFile(m_pCurFile->GetFormat()->nChannels, m_pCurFile->GetFormat()->wBitsPerSample/8);

		m_pDirectSoundBuffer8->SetPan(DSBPAN_RIGHT);
		return hr;
	}

	return E_FAIL;;
}

HRESULT  CDirectSoundEngine::ClearRender()  // 不能在这里立即delete m_pDirectSoundBuffer8，因为buffer填充线程可能正在使用m_pDirectSoundBuffer8，还没来得及响应OnStop
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

	// 流缓冲必须用DSBPLAY_LOOPING，这样当缓冲区读取时能回到起点继续读取
	HRESULT hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);

	if (SUCCEEDED(hr))
	{
		m_pMgr->GetSA()->Play();

		m_pMessageOnlyWnd->StartTimer(false);  // 这个是用于向UI层汇报进度和时间，注不要用 true参数，不是主线程
		if (NULL != m_pMessageOnlyWnd)         // 由于不是主线程中，postmessage通知界面立即更新当前进度
			::PostMessage(m_pMessageOnlyWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);
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

	m_pMessageOnlyWnd->EndTimer();
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

	m_pMessageOnlyWnd->EndTimer();

	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if(SUCCEEDED(hr))
	{
		// 清空缓冲区
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
	m_pDirectSoundBuffer8->GetStatus(&dwState);  // 获取当前的状态，用于设置完位置后恢复

	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if (FAILED(hr))
		return hr;

	hr = m_pCurFile->SetCurPos(dPercent);
	if (FAILED(hr))
		return hr;

	// 重新加载缓存
	hr = PushBuffer(0, GetBufferSize());  // ??? 注：如果这里不填充所有数据的话，后面会继续播放上次残留的数据，这是为嘛？
	if (FAILED(hr))   // 有可能将位置拖动到了文件结束处了
	{
		this->Stop();
		m_pMgr->Fire_on_mp3_stop();
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

	// 通知界面立即更新当前进度
	if (NULL != m_pMessageOnlyWnd)
		::PostMessage(m_pMessageOnlyWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);

	return S_OK;
}

HRESULT CDirectSoundEngine::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	int process= 0;
	DWORD PlayCursor = 0,WriteCursor = 0;
	if (SUCCEEDED(m_pDirectSoundBuffer8->GetCurrentPosition(&PlayCursor,&WriteCursor)))
	{
		process = GetDistance(PlayCursor,WriteCursor);
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

HRESULT CDirectSoundEngine::SetPan(long lPanPercent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	HRESULT hr = m_pDirectSoundBuffer8->SetPan(lPanPercent*100);
	return hr;
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

	// 通知该线程创建消息队列
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
				// m_pMgr->Fire_on_mp3_stop(); 该函数不支持多线程
			}
			else
			{
				
			}
		}

 		else if (nIndex == POSITION_EVENT_COUNT)
		{
			// 退出的通知
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

// 返回false表示该退出线程了
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

// 向message only window转发消息来切换当前线程
void  CDirectSoundEngine::OnNoitfy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->Stop();
	m_pMgr->Fire_on_mp3_stop();
}

void CDirectSoundEngine::SetBufferSize(int nSize)
{
	m_nDirectSoundBufferSize = nSize;
	m_nPerEventBufferSize = m_nDirectSoundBufferSize/POSITION_EVENT_COUNT;
}
// 往缓冲区中添加数据
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
	if (FAILED(hr))       // 说明播放完毕了
	{
		delete[] pbSoundData;
		m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
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
			m_pDirectSoundBuffer8->Unlock(pBitPart1, dwSizePart1, pBitPart2, dwSizePart2);
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
	while (distance <= 0)
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

// TODO: 不能这样获取buffer
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

