#include "StdAfx.h"
#include "DirectSoundEngine.h"

CWavFile::CWavFile()
{

}
CWavFile::~CWavFile()
{
	this->Release();
}
void CWavFile::Release()
{

}

HRESULT CWavFile::RenderFile(const TCHAR* szFile)
{
	return E_NOTIMPL;
}
HRESULT CWavFile::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////

CMp3File::CMp3File()
{
	m_hMpg123 = NULL;
}
CMp3File::~CMp3File()
{
	this->Release();
}

void CMp3File::Release()
{
	if (NULL != m_hMpg123)
	{
		mpg123_tclose(m_hMpg123);
		mpg123_delete(m_hMpg123);
		m_hMpg123 = NULL;

		mpg123_exit();
	}
}
HRESULT CMp3File::RenderFile(const TCHAR* szFileName)
{	
	int ret = -1;
	bool bRet = false;
	do 
	{
		if (NULL == m_hMpg123)
		{
			if (MPG123_OK != mpg123_init())
				return E_FAIL;

			m_hMpg123 = mpg123_new(NULL, &ret);
			if (NULL == m_hMpg123 || MPG123_OK != ret)
			{
				mpg123_exit();
				return E_FAIL;
			}
		}
		else
		{
			mpg123_tclose(m_hMpg123);  // 关闭之前的文件
		}


		if (MPG123_OK != mpg123_topen(m_hMpg123, szFileName))
			break;

		long lRate = 0;
		int  nChannel = 0;
		int  nEncoding = 0;

		if (MPG123_OK != mpg123_getformat(m_hMpg123, &lRate, &nChannel, &nEncoding))
			break;

		if (MPG123_ENC_16 == (nEncoding & MPG123_ENC_16))
			nEncoding = 16;
		else if (MPG123_ENC_32 == (nEncoding & MPG123_ENC_32))
			nEncoding = 32;
		else
			nEncoding = 8;

		memset(&m_wfx, 0, sizeof(WAVEFORMATEX));
		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = nChannel;
		m_wfx.nSamplesPerSec = lRate;
		m_wfx.wBitsPerSample = nEncoding;
		m_wfx.nBlockAlign = nEncoding / 8 * nChannel;
		m_wfx.nAvgBytesPerSec = lRate * (nEncoding / 8) * nChannel;

		bRet = true;
	} while (0);
	
	if (false == bRet)
	{
		this->Release();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMp3File::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	int ret = mpg123_read(m_hMpg123, pBuffer, dwSizeToRead, (size_t*)pdwSizeRead);
	if (MPG123_OK != ret)
		return E_FAIL;

	return S_OK;
}

HRESULT CMp3File::SetCurPos(double percent)
{
	if (NULL == m_hMpg123)
		return E_FAIL;

	off_t length = mpg123_length(m_hMpg123);

	if (mpg123_seek(m_hMpg123, (off_t)(length*percent), SEEK_SET) < 0)
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CMp3File::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if (NULL == m_hMpg123)
		return E_FAIL;
	if (NULL == pdSeconds || NULL == pdPercent)
		return E_INVALIDARG;

	off_t length = mpg123_length(m_hMpg123);
	off_t cur = mpg123_tell(m_hMpg123);

	if (0 == length)
		return E_FAIL;

	*pdPercent = cur * 1.0 / length;

	off_t curFrame = mpg123_tellframe(m_hMpg123);
	double dFrameTime = mpg123_tpf(m_hMpg123);

	*pdSeconds = dFrameTime * curFrame;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

CDirectSoundEngine::CDirectSoundEngine(void)
{
	m_pMgr = NULL;
	m_pDirectSound8 = NULL;
	m_pDirectSoundBuffer8 = NULL;
	m_pDirectSoundNotify8 = NULL;
	m_pCurFile = NULL;
	m_pMp3File = NULL;
	m_pWavFile = NULL;

	for (int i = 0; i < EVENT_NOTIFY_COUNT+1; i++)
	{
		m_hEvents[i] = 0;
	}
	m_hEventThread = NULL;

	m_nDirectSoundBufferSize = 32*1024;
	m_nPerEventBufferSize = m_nDirectSoundBufferSize/EVENT_NOTIFY_COUNT;
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

	hr = m_pDirectSound8->SetCooperativeLevel(pWndEvent->m_hWnd, DSSCL_PRIORITY);  // 这个窗口句柄不能设置为NULL，否则会返回无效参数错误
	if (FAILED(hr))
		return hr;


	for (int i = 0; i < EVENT_NOTIFY_COUNT+1; i++)
	{
		m_hEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	m_hEventThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gNotifyThreadProc, (LPVOID)this, 0, NULL);
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
		SetEvent(m_hEvents[EVENT_NOTIFY_COUNT]);
		Sleep(1);
		TerminateThread(m_hEventThread, 0);
		CloseHandle(m_hEventThread);
		m_hEventThread = NULL;
	}
	for(int i = 0; i < EVENT_NOTIFY_COUNT+1; ++ i)
	{
		if(m_hEvents[i] != NULL)
		{
			CloseHandle(m_hEvents[i]);
			m_hEvents[i] = NULL;
		}
	}

	SAFE_RELEASE(m_pDirectSoundBuffer8);
	SAFE_RELEASE(m_pDirectSoundNotify8);
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
	SAFE_RELEASE(m_pDirectSoundNotify8);
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

		// 创建缓冲区

		DSBUFFERDESC desc;
		memset (&desc, 0, sizeof(DSBUFFERDESC));

		desc.dwSize = sizeof(DSBUFFERDESC);
		// 注：DSBCAPS_GLOBALFOCUS，如果不加上这个标志，当窗口失去焦点的时候，directsound 会停止播放
		//     DSBCAPS_CTRLPOSITIONNOTIFY，如果不加上这个标志，QueryInterface IID_IDirectSoundNotify8 会返回NOINTERFACE
		desc.dwFlags = DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPOSITIONNOTIFY;  
		desc.lpwfxFormat =  m_pCurFile->GetFormat();
		desc.dwBufferBytes = m_nDirectSoundBufferSize;    //3*desc.lpwfxFormat->nAvgBytesPerSec;  // 持续3秒的流缓冲区

		IDirectSoundBuffer* pBuffer = NULL;
		hr = m_pDirectSound8->CreateSoundBuffer(&desc, &pBuffer, NULL);
		if (FAILED(hr))
			return hr;

		hr = pBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDirectSoundBuffer8);
		hr = pBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pDirectSoundNotify8);  // 需要开启 DSBCAPS_CTRLPOSITIONNOTIFY
		SAFE_RELEASE(pBuffer);

		if (NULL == m_pDirectSoundNotify8)
			return hr;

		// 设置流缓冲的通知事件
		DSBPOSITIONNOTIFY  notifys[EVENT_NOTIFY_COUNT];
		for (int i = 0; i < EVENT_NOTIFY_COUNT; i++)
		{
			notifys[i].dwOffset = m_nDirectSoundBufferSize/EVENT_NOTIFY_COUNT*(i+1) - 1;
			notifys[i].hEventNotify = m_hEvents[i];
		}
		hr = m_pDirectSoundNotify8->SetNotificationPositions(EVENT_NOTIFY_COUNT, notifys);
		if (FAILED(hr))
			return hr;

		// 第一次填充完整的buffer
		hr = this->PushBuffer(0, m_nDirectSoundBufferSize);
		return hr;
	}

	return E_FAIL;;
}
HRESULT CDirectSoundEngine::Play()
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	// 流缓冲必须用DSBPLAY_LOOPING，这样当缓冲区读取时能回到起点继续读取
	HRESULT hr = m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);
	m_pWnd->StartTimer(true);

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
	SAFE_RELEASE(m_pDirectSoundNotify8);
	SAFE_RELEASE(m_pDirectSoundBuffer8);
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::SetCurPos(double percent)
{
	if (NULL == m_pCurFile || NULL == m_pDirectSoundBuffer8)
		return E_FAIL;

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  1 \n"));
	HRESULT hr = m_pDirectSoundBuffer8->Stop();
	if (FAILED(hr))
		return hr;

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  2 \n"));
	hr = m_pCurFile->SetCurPos(percent);
	if (FAILED(hr))
		return hr;

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  3 \n"));
	// 重新加载缓存
	hr = PushBuffer(0, m_nDirectSoundBufferSize);
	if (FAILED(hr))
		return hr;

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  4 \n"));
	hr = m_pDirectSoundBuffer8->SetCurrentPosition(0);
	if (FAILED(hr))
		return hr;

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  5 \n"));
	return m_pDirectSoundBuffer8->Play(0,0, DSBPLAY_LOOPING);

	::OutputDebugString(_T("CDirectSoundEngine::SetCurPos  6 \n"));

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


void CDirectSoundEngine::EventThreadProc()
{
	if (0 == EVENT_NOTIFY_COUNT)
		return;

	while(true)
	{
		DWORD nRet = ::WaitForMultipleObjects(EVENT_NOTIFY_COUNT, m_hEvents, FALSE, INFINITE);
		if (nRet == WAIT_FAILED)
			return;

		int nIndex = nRet - WAIT_OBJECT_0;
		if (nIndex >= 0 && nIndex < EVENT_NOTIFY_COUNT)
		{
			::OutputDebugString(_T("CDirectSoundEngine::EventThreadProc  1 \n"));
			if (FAILED (this->PushBuffer(m_nPerEventBufferSize*nIndex, m_nPerEventBufferSize)))
			{
				this->Stop();
				m_pMgr->Fire_on_mp3_stop();
			}
			else
			{
				
			}
			::OutputDebugString(_T("CDirectSoundEngine::EventThreadProc  2 \n"));
		}
		else if (nIndex == EVENT_NOTIFY_COUNT)
		{
			this->Stop();
		}
		else
		{
			continue;
		}
	}

	this->Stop();

	return;
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
	BYTE* pbSoundData = new BYTE[ m_nPerEventBufferSize ];
	hr = m_pCurFile->Read(pbSoundData, m_nPerEventBufferSize, &outsize);
	if (FAILED(hr))
	{
		delete[] pbSoundData;
		return hr;
	}

	memcpy(pBitPart1, pbSoundData, outsize);
	delete[] pbSoundData;

	if(pBitPart2 != 0)
	{
		pbSoundData = new BYTE[ m_nPerEventBufferSize ];
		hr = m_pCurFile->Read(pbSoundData, m_nPerEventBufferSize, &outsize);
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