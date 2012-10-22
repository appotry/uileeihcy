#include "stdafx.h"
#include "SpectrumAnalyser.h"
#include "DirectSoundEngine.h"
#include <math.h>

CSpectrumAnalyser::CSpectrumAnalyser()
{
	m_pFFT = NULL;
	m_pSoundEngine = NULL;
	
	m_nChannels = 0;
	m_nBytePerSample = 0;
	m_nAnalyserSampleCount = 0;
	m_nSampleBufferSize = 0;
	m_nBandCount = 0;
	m_nSamplesPerBand = 0;

	m_pBandValue = NULL;
	m_pOldBandValue = NULL;
	m_pSampleBuffer = NULL;
	m_pLeftRightSampleData = NULL;
	m_hThread = NULL;
	m_bSuspend = FALSE;
	m_dwThreadID = 0;
	m_hEventSuspend = NULL;

	m_hRenderWnd = NULL;
	SetRectEmpty(&m_rcRender);
	m_hBkgndBmp = NULL;
	m_eType = VISUALIZATION_SPECTRUM;
	m_nFps = 1000/25;
	m_nBandWidth = 7;
	m_nBandGapWidth = 1;

	m_hRenderWndDC = NULL;
	m_hRenderWndMemDC = NULL;
	m_hMemBitmap = NULL;
	m_hOldBitmap = NULL;
}
CSpectrumAnalyser::~CSpectrumAnalyser()
{
	this->Release();
}

DWORD WINAPI gSpectrumAnalyserThreadProc( LPVOID lpParameter)
{
	CSpectrumAnalyser* pThis = (CSpectrumAnalyser*)lpParameter;
	if (NULL == pThis)
		return -1;

	pThis->ThreadProc();
	return 0;
}

HRESULT CSpectrumAnalyser::InitDefault(HWND hRenderWnd)
{
	this->SetAnalyserSampleCount(DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
	this->SetBandCount(30);
	this->SetRenderWnd(hRenderWnd);

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gSpectrumAnalyserThreadProc,
				(LPVOID)this, 0, &m_dwThreadID);
	m_bSuspend = THREAD_SUSPEND_BY_PLAY_STATE;
	m_hEventSuspend = CreateEvent(NULL, TRUE, FALSE, NULL);
	return S_OK;
}
HRESULT CSpectrumAnalyser::Release()
{
	if (NULL != m_hThread)
	{
		this->PostThreadMessage(DSMSG_QUIT,NULL);
		WaitForSingleObject(m_hThread, 2000);
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		CloseHandle(m_hEventSuspend);
		m_hThread = NULL;
		m_dwThreadID = 0;
	}
	SAFE_DELETE(m_pFFT);
	SAFE_ARRAY_DELETE(m_pBandValue);
	SAFE_ARRAY_DELETE(m_pOldBandValue);
	SAFE_ARRAY_DELETE(m_pSampleBuffer);
	SAFE_ARRAY_DELETE(m_pLeftRightSampleData);
	m_nAnalyserSampleCount = 0;
	m_nSampleBufferSize = 0;
	m_nChannels = 0;
	m_nBytePerSample = 0;

	m_FFTSrcSampleSize = 0;
	m_FFTDestSampleSize = 0;
	m_pSoundEngine = NULL;

	::SelectObject(m_hRenderWndMemDC, m_hOldBitmap);
	::DeleteDC(m_hRenderWndMemDC);
	::ReleaseDC(m_hRenderWnd, m_hRenderWndDC);
	::DeleteObject(m_hMemBitmap);
	::DeleteObject(m_hBkgndBmp);
	return S_OK;
}

void CSpectrumAnalyser::SetRenderWnd(HWND hRenderWnd)
{
	if( m_hRenderWnd != hRenderWnd)
	{
		if (NULL != m_hRenderWndMemDC)
		{
			::SelectObject(m_hRenderWndMemDC, m_hOldBitmap);
			::DeleteDC(m_hRenderWndMemDC);
		}
		if (NULL != m_hRenderWndDC)
			::ReleaseDC(m_hRenderWnd, m_hRenderWndDC);
	
		m_hRenderWndDC = GetDC(hRenderWnd);
		m_hRenderWndMemDC = CreateCompatibleDC(NULL);
		if (NULL != m_hMemBitmap)
		{
			m_hOldBitmap = (HBITMAP)::SelectObject(m_hRenderWndMemDC, m_hMemBitmap);
		}
		
		m_hRenderWnd = hRenderWnd;
	}
}

// band仅对频谱图有效，对波形图没有意义
int CSpectrumAnalyser::SetBandCount(int nCount)
{
	if (m_nBandCount == nCount || 0 == nCount)
		return m_nBandCount;

	int nOldCound = m_nBandCount;
	m_nBandCount = nCount;

	SAFE_ARRAY_DELETE(m_pBandValue);
	SAFE_ARRAY_DELETE(m_pOldBandValue);

	m_pBandValue    = new float[m_nBandCount];
	m_pOldBandValue = new float[m_nBandCount];
	memset(m_pBandValue,    0, sizeof(float)*m_nBandCount);
	memset(m_pOldBandValue, 0, sizeof(float)*m_nBandCount);

	m_nSamplesPerBand = m_FFTDestSampleSize/m_nBandCount;
	return nOldCound;
}

// 设置每次要分析的样本数
int CSpectrumAnalyser::SetAnalyserSampleCount(int nCount)
{
	if (m_nAnalyserSampleCount == nCount)
		return m_nAnalyserSampleCount;

	int nOldSize = m_nAnalyserSampleCount;
	m_nAnalyserSampleCount = nCount;
	m_FFTSrcSampleSize  = m_nAnalyserSampleCount/4;  // ?? 为什么要除以4？
	m_FFTDestSampleSize = m_FFTSrcSampleSize/2;      // FFT计算返回的值是输入值的一半
	if (0 != m_nBandCount)
		m_nSamplesPerBand = m_FFTDestSampleSize/m_nBandCount;

	SAFE_DELETE(m_pFFT);
	m_pFFT = new CFastFourierTransform(m_FFTSrcSampleSize);  
	
	SAFE_ARRAY_DELETE(m_pLeftRightSampleData);
	m_pLeftRightSampleData = new float[m_nAnalyserSampleCount];

	return nOldSize;
}
int CSpectrumAnalyser::GetAnslyserSampleCount()
{
	if (m_eType == VISUALIZATION_SPECTRUM)
	{
		return m_nAnalyserSampleCount;
	}
	else if (m_eType == VISUALIZATION_WAVE)
	{
		return m_rcRender.right - m_rcRender.left;
	}
	return 0;
}

// 在分析线程中被调用
void CSpectrumAnalyser::SetVisualizationType(E_VISUALIZATION_TYPE eType)
{
	if (m_eType == eType)
		return;
	m_eType = eType;

	SAFE_ARRAY_DELETE(m_pLeftRightSampleData);
	SAFE_ARRAY_DELETE(m_pSampleBuffer);
	SAFE_ARRAY_DELETE(m_pBandValue);
	SAFE_ARRAY_DELETE(m_pOldBandValue);

	if (VISUALIZATION_WAVE == m_eType)
	{
		int nCount = m_rcRender.right - m_rcRender.left;
		m_pLeftRightSampleData = new float[nCount];

		m_nSampleBufferSize = nCount*m_nChannels*m_nBytePerSample;
		m_pSampleBuffer = new signed char[m_nSampleBufferSize];

		m_bSuspend &= ~THREAD_SUSPEND_BY_VISUAL_NONE;
	}
	else if (VISUALIZATION_SPECTRUM == m_eType)
	{
		m_pBandValue    = new float[m_nBandCount];
		m_pOldBandValue = new float[m_nBandCount];
		memset(m_pBandValue,    0, sizeof(float)*m_nBandCount);
		memset(m_pOldBandValue, 0, sizeof(float)*m_nBandCount);

		m_pLeftRightSampleData = new float[m_nAnalyserSampleCount];

		m_nSampleBufferSize = m_nAnalyserSampleCount*m_nChannels*m_nBytePerSample;
		m_pSampleBuffer = new signed char[m_nSampleBufferSize];

		m_bSuspend &= ~THREAD_SUSPEND_BY_VISUAL_NONE;
	}
	else
	{
		m_bSuspend |= THREAD_SUSPEND_BY_VISUAL_NONE;
	}
}
void CSpectrumAnalyser::SetSoundEngine(ISoundEngine* pSoundEngine) 
{
	m_pSoundEngine = pSoundEngine; 
}
bool CSpectrumAnalyser::SetVisualization(VisualizationInfo* pInfo)
{
	if (NULL == pInfo)
		return false;
	
	// 注：m_BkgndBmp特殊，需要立即更新(HLS变换...)
	if (pInfo->nMask & VI_MASK_BKGND_BMP)
	{
	//	m_hBkgndBmp = pInfo->hBkgndBmp;
		HBITMAP hTemp = (HBITMAP)::InterlockedExchange((LONG*)&m_hBkgndBmp, (LONG)pInfo->hBkgndBmp);
		SAFE_DELETE_GDIOBJECT(hTemp);
	}

	BOOL bRet = this->PostThreadMessage(DSMSG_SET_VISUALIZATION, BuildSetVisualizationParam(pInfo));
	return bRet?true:false;
}
bool CSpectrumAnalyser::OnSetVisualization(VisualizationInfo* pInfo)
{
	if (pInfo->nMask & VI_MASK_HWND)
	{
		this->SetRenderWnd(pInfo->hWnd);
	}
	if (pInfo->nMask & VI_MASK_RECT)
	{
		int nOldW = m_rcRender.right - m_rcRender.left;
		int nOldH = m_rcRender.bottom - m_rcRender.top;
		int nNewW = pInfo->rcRender.right - pInfo->rcRender.left;
		int nNewH = pInfo->rcRender.bottom - pInfo->rcRender.top;

		::CopyRect(&m_rcRender, &(pInfo->rcRender));

		if (nOldH != nNewH || nOldW != nNewW)
		{
			if (NULL != m_hRenderWndMemDC)
			{
				::SelectObject(m_hRenderWndMemDC, m_hOldBitmap);
				::DeleteObject(m_hMemBitmap);

				BITMAPINFO bmi;
				memset( &bmi, 0, sizeof( BITMAPINFO ) );
				bmi.bmiHeader.biSize = sizeof( BITMAPINFO );
				bmi.bmiHeader.biWidth = nNewW;
				bmi.bmiHeader.biHeight = -nNewH;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = USHORT( 32 );
				bmi.bmiHeader.biCompression = BI_RGB;

				void* pBits = NULL;
				m_hMemBitmap = ::CreateDIBSection( NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0 );
				m_hOldBitmap = (HBITMAP)::SelectObject(m_hRenderWndMemDC, m_hMemBitmap);
			}
		}
	}
	if (pInfo->nMask & VI_MASK_TYPE)
	{
		this->SetVisualizationType(pInfo->eType);
	}

	if (pInfo->nMask & VI_MASK_FPS)
	{
		int nFps = pInfo->nFps;
		if (0 != nFps)
		{
			m_nFps = 1000/nFps; // 直接计算出每次要sleep的时间(ms)
		}
	}

	if (pInfo->nMask & VI_MASK_SPECTRUM_BAND_COUNT)
	{
		this->SetBandCount(pInfo->nSpectrumBandCount);
	}
	if (pInfo->nMask & VI_MASK_SPECTRUM_BAND_WIDTH)
	{
		m_nBandWidth = pInfo->nSpectrumBandWidth;
	}
	if (pInfo->nMask & VI_MASK_SPECTRUM_GAP_WIDTH)
	{
		m_nBandWidth = pInfo->nSpectrumGapWidth;
	}
	

	return true;
}
HRESULT CSpectrumAnalyser::RenderFile(int nChannel, int nBytePerSample)
{
	if (m_nChannels == nChannel && m_nBytePerSample == nBytePerSample)
		return S_FALSE;

	m_nChannels = nChannel;
	m_nBytePerSample = nBytePerSample;
	
	SAFE_ARRAY_DELETE(m_pSampleBuffer);
	switch(m_eType)
	{
	case VISUALIZATION_SPECTRUM:
	case VISUALIZATION_WAVE:
		m_nSampleBufferSize = GetAnslyserSampleCount()*m_nChannels*m_nBytePerSample;
		m_pSampleBuffer = new signed char[m_nSampleBufferSize];
		break;
	}

	return S_OK;
}

// 从directsound中获取当前正在播放的数据，用于频谱分析
BOOL CSpectrumAnalyser::GetSampleBufferFromDSound()
{
	if (NULL == m_pSoundEngine || NULL == m_pSampleBuffer)
		return FALSE;

	return m_pSoundEngine->GetPlayBuffer(m_pSampleBuffer, m_nSampleBufferSize);
}

//
//	注：为什么连play函数也要post到分析线程中处理？
//      因为有可能上一次的stop调用后，onstop还没有触发，又响应了接下来的play函数
//      导致线程被onstop给挂起
//
void CSpectrumAnalyser::Play()
{
	this->PostThreadMessage(DSMSG_PLAY, NULL);
}
void CSpectrumAnalyser::OnPlay()
{
	m_bSuspend &= ~THREAD_SUSPEND_BY_PLAY_STATE;
}

void CSpectrumAnalyser::Pause()
{
	this->PostThreadMessage(DSMSG_PAUSE, NULL);
}

void CSpectrumAnalyser::OnPause()
{
	m_bSuspend |= THREAD_SUSPEND_BY_PLAY_STATE;
}
void CSpectrumAnalyser::Stop()
{
	this->PostThreadMessage(DSMSG_STOP, NULL);
}
void CSpectrumAnalyser::OnStop()
{
	m_bSuspend |= THREAD_SUSPEND_BY_PLAY_STATE;
}
void CSpectrumAnalyser::ThreadProc()
{
	// 通知该线程创建消息队列
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	int nTimeStamp1 = 0, nTimeStamp2 = 0;
	int nSleepTime = 0;
	
	while(true)
	{
		DWORD nRet = ::WaitForSingleObject(m_hEventSuspend, INFINITE);  // 用于在暂停或未播放时，挂起线程
		if (nRet == WAIT_FAILED)
			return;

		ResetEvent(m_hEventSuspend);   // 重置

		if (nRet == WAIT_OBJECT_0)
		{
			if (false == this->EventMsgProc())      // 处理事件，比如检查是否将m_bSuspend置为false了
				return;
		}

		while(0 == m_bSuspend)         // 开始不断的分析频谱数据，直到有事件将m_bSuspend置为true
		{
			nTimeStamp1 = GetTickCount();  // 统计获取消息队列中的消息花费时间

			//////////////////////////////////////////////////////////////////////////
			this->Process();               // 处理数据分析
			//////////////////////////////////////////////////////////////////////////

			while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			{
				if (false == this->HandleEventMsg(&msg))
					return ;
			}
			nTimeStamp2 = GetTickCount();
			nSleepTime = m_nFps - (nTimeStamp2-nTimeStamp1);

			if (nSleepTime > 0)
				Sleep(nSleepTime); 
		}
	}
}

// return false表示退出线程
bool CSpectrumAnalyser::EventMsgProc()
{
	MSG msg;
	while(::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		if (false == this->HandleEventMsg(&msg))
			return false;
	}

	return true;
}

// return false表示退出线程
bool CSpectrumAnalyser::HandleEventMsg(MSG* pMsg)
{
	DSMSG_PARAM* pDSMSG_PARAM = (DSMSG_PARAM*)pMsg->wParam;
	switch(pMsg->message)
	{
	case DSMSG_SET_VISUALIZATION:
		{
			DSMSG_PARAM_SET_VISUALIZATION* p = (DSMSG_PARAM_SET_VISUALIZATION*)pDSMSG_PARAM;
			this->OnSetVisualization(&p->info);
		}
		break;

	case DSMSG_PLAY:
		{
			this->OnPlay();
		}
		break;

	case DSMSG_PAUSE:
		{
			this->OnPause();
		}
		break;

	case DSMSG_STOP:
		{
			this->OnStop();
		}
		break;

	case DSMSG_QUIT:
		{
			return false;
		}
		break;
	}
	SAFE_DELETE(pDSMSG_PARAM);
	return true;
}

bool CSpectrumAnalyser::PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam)
{
	if (NULL == m_hEventSuspend || 0 == m_dwThreadID)
		return false;

	if (FALSE == ::PostThreadMessage(m_dwThreadID, uMsg, (WPARAM)pParam, 0))
		return false;

	::SetEvent(m_hEventSuspend);
	return true;
}

void CSpectrumAnalyser::Process()
{
	if (GetSampleBufferFromDSound())
	{
		TransformSamples();
		
		if (m_eType == VISUALIZATION_SPECTRUM)
		{
 			FFTSamples();
	 		DrawBands();
		}
		else
		{
			DrawWave();
		}
	}
}

void CSpectrumAnalyser::TransformSamples()
{
	if (NULL == m_pLeftRightSampleData)
		return;

	int SampleSize = GetAnslyserSampleCount();
	if (m_nChannels == 1 && m_nBytePerSample == 1) 
	{
		for (int i = 0; i < SampleSize; i++)
		{
// 			m_Left[a] = (float) m_pSampleBuffer[a] / 128.0F;
// 			m_Right[a] = m_Left[a];

			m_pLeftRightSampleData[i] = (float)m_pSampleBuffer[i] / 128.0f;
		}

	} 
	else if (m_nChannels == 2 && m_nBytePerSample == 1) 
	{
		for (int i = 0; i < SampleSize; i+=2) 
		{
// 			m_Left[i] = (float) m_pSampleBuffer[i<<1] / 128.0F;
// 			m_Right[i] = (float) m_pSampleBuffer[(i<<1)+1] / 128.0F;

			int n = i<<1;
			m_pLeftRightSampleData[i] = (m_pSampleBuffer[n] + m_pSampleBuffer[n+1]) / 256.0F;
		}

	} 
	else if ( m_nChannels == 1 &&  m_nBytePerSample == 2) 
	{
		for (int i = 0; i <  SampleSize; i++) 
		{
// 			m_Left[i] = (float) (( m_pSampleBuffer[(i<<1)+1] << 8) +
// 				m_pSampleBuffer[i<<1]) / 32767.0F;
// 			m_Right[i] =  m_Left[i];

			m_pLeftRightSampleData[i] = (float) (( m_pSampleBuffer[(i<<1)+1] << 8) + m_pSampleBuffer[i<<1]) / 32767.0F;
		}
	} 
	else if (m_nChannels == 2 &&  m_nBytePerSample == 2)
	{
		for (int i = 0; i <  SampleSize; i++)
		{
// 			m_Left[a] = (float) (( m_pSampleBuffer[(a<<2)+1] << 8) +
// 				m_pSampleBuffer[(a<<2)]) / 32767.0F;
// 			m_Right[a] = (float) (( m_pSampleBuffer[(a<<2)+3] << 8) +
// 				m_pSampleBuffer[(a<<2)+2]) / 32767.0F;

			int n = i << 2;
			float fLeft = (float) (( m_pSampleBuffer[n+1] << 8) + m_pSampleBuffer[n]);
			float fRight = (float) (( m_pSampleBuffer[n+3] << 8) + m_pSampleBuffer[n+2]);

			m_pLeftRightSampleData[i] = (fLeft+fRight)/65534.0F;
		}
	}
}

void CSpectrumAnalyser::FFTSamples()
{	
// 	for (int a = 0; a < m_nAnalyserSampleCount; a++) 
// 	{
// 		m_Left[a] = (m_Left[a] + m_Right[a]) / 2.0f;
// 	}

	float* pFFTResult = m_pFFT->Calculate(m_pLeftRightSampleData, m_nAnalyserSampleCount); 
	for (int i = 0, nBandIndex = 0; nBandIndex < m_nBandCount; i += m_nSamplesPerBand, nBandIndex++) 
	{
		float fBandValue = 0;
		// 计算一个柱形条的能量值。（每个柱形条是包含了m_nSamplesPerBand个取样点）
		for (int j = 0; j < (INT)m_nSamplesPerBand; j++) 
		{
			fBandValue += pFFTResult[i + j];
		}

		// 下面的这些代码完成是抄的，不明白取这些值的原由
//		fBandValue = (fBandValue * (float) log(nBandIndex + 2.0F));   // -- Log filter.

		if (fBandValue > 0.005F && fBandValue < 0.009F)
			fBandValue *= 9.0F * PI;
		else if (fBandValue > 0.01F && fBandValue < 0.1F)
			fBandValue *= 3.0F * PI;
		else if (fBandValue > 0.1F && fBandValue < 0.5F)
			fBandValue *= PI; //enlarge PI times, if do not, the bar display abnormally, why??

		if (fBandValue > 1.0F) 
		{
			fBandValue = 0.9F;
		}
// 		if (fBandValue<0.01f)
// 		{
// 			fBandValue *= 20;
// 		}
// 		else if (fBandValue<0.05)
// 		{
// 			fBandValue*=10;
// 		}
// 		else if (fBandValue<0.1)
// 		{
// 			fBandValue*=5;
// 		}

		// -- Compute SA decay...
		if (fBandValue >= (m_pOldBandValue[nBandIndex] - 0.05f)) // 取新值
		{
			m_pOldBandValue[nBandIndex] = fBandValue;
		} 
		else 
		{
			m_pOldBandValue[nBandIndex] -= 0.05f;                // 缓慢的衰减
			if (m_pOldBandValue[nBandIndex] < 0) 
			{
				m_pOldBandValue[nBandIndex] = 0;
			}
			fBandValue = m_pOldBandValue[nBandIndex];
		}

		m_pBandValue[nBandIndex] = fBandValue;
	}
}
// 直接在另一个线程中提交到窗口上面
void CSpectrumAnalyser::DrawBands()
{
	int nWidth = m_rcRender.right-m_rcRender.left;
	int nHeight = m_rcRender.bottom-m_rcRender.top;
	RECT rc = {0,0, nWidth, nHeight};

	HDC hDC = ::CreateCompatibleDC(m_hRenderWndMemDC);
	SelectObject(hDC, m_hBkgndBmp);
	::BitBlt(m_hRenderWndMemDC, 0,0,nWidth, nHeight, hDC, 0,0, SRCCOPY);
	::DeleteDC(hDC);

	HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	for (int i = 0; i < m_nBandCount; i++)
	{
		int ny = nHeight - (int)(nHeight * m_pBandValue[i]);
		int nx = i * (m_nBandGapWidth+m_nBandWidth);

		RECT rc = {nx, ny, nx+m_nBandWidth, nHeight};
		::FillRect(m_hRenderWndMemDC, &rc, hWhiteBrush);
	}
	::BitBlt(m_hRenderWndDC, m_rcRender.left, m_rcRender.top, nWidth, nHeight, m_hRenderWndMemDC,0,0,SRCCOPY);
}

void CSpectrumAnalyser::DrawWave()
{
	int   nWidth = m_rcRender.right - m_rcRender.left;
	int   nHeight = m_rcRender.bottom - m_rcRender.top;
	
	RECT rc = {0,0, nWidth, nHeight};
//	::FillRect(m_hRenderWndMemDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));

	HDC hDC = ::CreateCompatibleDC(m_hRenderWndMemDC);
	SelectObject(hDC, m_hBkgndBmp);
	::BitBlt(m_hRenderWndMemDC, 0,0,nWidth, nHeight, hDC, 0,0, SRCCOPY);
	::DeleteDC(hDC);

	HPEN  hPen = ::CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HPEN  hOldPen = (HPEN)::SelectObject(m_hRenderWndMemDC, hPen);

#define XXX 0  // 定义一个值，相差多大时表示相关一级

	int    nPrevY = (int)((nHeight - m_pLeftRightSampleData[0]* nHeight)/2);  // 计算第一个
	POINT* ppt = new POINT[2*nWidth];

	for (int i = 1; i < nWidth; i++)   // 从第二个开始循环
	{
		int nx = i;
		int ny = (int)((nHeight - m_pLeftRightSampleData[i]* nHeight)/2);

#define SETPIXEL(x,y) ::SetPixel(m_hRenderWndMemDC,x,y,RGB(25,77,92));
		if (ny > nPrevY + XXX)
		{
			// 往上爬一格
			ppt[i*2].x = nx;
			ppt[i*2].y = nPrevY;
			ppt[i*2+1].x = nx;
			ppt[i*2+1].y = ++nPrevY;
// 			SETPIXEL(nx,nPrevY);
// 			SETPIXEL(nx,++nPrevY);
		}
		else if (ny < nPrevY-XXX)
		{
			// 下降一格
			ppt[i*2].x = nx;
			ppt[i*2].y = nPrevY;
			ppt[i*2+1].x = nx;
			ppt[i*2+1].y = --nPrevY;
// 			SETPIXEL(nx,nPrevY);
// 			SETPIXEL(nx,--nPrevY);
		}
		else// if (ny == nPrevY)
		{
			// 不爬,往右延伸一格
			ppt[i*2].x = nx-1;
			ppt[i*2].y = nPrevY;
			ppt[i*2+1].x = nx;
			ppt[i*2+1].y = nPrevY;
// 			SETPIXEL(nx-1,ny);
// 			SETPIXEL(nx,ny);
		}
	}
	
	Polyline(m_hRenderWndMemDC, ppt, 2*nWidth);
	delete[] ppt;

	::BitBlt(m_hRenderWndDC, m_rcRender.left, m_rcRender.top,nWidth,nHeight,m_hRenderWndMemDC,0,0,SRCCOPY);
	::SelectObject(m_hRenderWndMemDC, hOldPen);
	::DeleteObject(hPen);
}