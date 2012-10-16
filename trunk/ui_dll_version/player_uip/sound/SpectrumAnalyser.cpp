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
	m_nBandCound = 0;
	m_nSamplesPerBand = 0;

	m_pBandValue = NULL;
	m_pOldBandValue = NULL;
	m_pSampleBuffer = NULL;
	m_pLeftRightChannelData = NULL;
	m_hThread = NULL;
	m_bSuspend = false;

	m_hRenderWnd = NULL;
	SetRectEmpty(&m_rcRender);
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
				(LPVOID)this, CREATE_SUSPENDED , NULL);
	m_bSuspend = true;
	return S_OK;
}
HRESULT CSpectrumAnalyser::Release()
{
	SAFE_DELETE(m_pFFT);
	SAFE_ARRAY_DELETE(m_pBandValue);
	SAFE_ARRAY_DELETE(m_pOldBandValue);
	SAFE_ARRAY_DELETE(m_pSampleBuffer);
	SAFE_ARRAY_DELETE(m_pLeftRightChannelData);
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
		m_hRenderWndMemDC = CreateCompatibleDC(m_hRenderWndDC);
		if (NULL != m_hMemBitmap)
		{
			m_hOldBitmap = (HBITMAP)::SelectObject(m_hRenderWndMemDC, m_hMemBitmap);
		}
		
		m_hRenderWnd = hRenderWnd;
	}
}
int CSpectrumAnalyser::SetBandCount(int nCount)
{
	if (m_nBandCound == nCount)
		return m_nBandCound;

	int nOldCound = m_nBandCound;
	m_nBandCound = nCount;

	SAFE_ARRAY_DELETE(m_pBandValue);
	SAFE_ARRAY_DELETE(m_pOldBandValue);

	m_pBandValue    = new float[m_nBandCound];
	m_pOldBandValue = new float[m_nBandCound];
	memset(m_pBandValue,    0, sizeof(float)*m_nBandCound);
	memset(m_pOldBandValue, 0, sizeof(float)*m_nBandCound);

	m_nSamplesPerBand = m_FFTDestSampleSize/m_nBandCound;
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

	SAFE_DELETE(m_pFFT);
	m_pFFT = new CFastFourierTransform(m_FFTSrcSampleSize);  
	
	SAFE_ARRAY_DELETE(m_pLeftRightChannelData);
	m_pLeftRightChannelData = new float[m_nAnalyserSampleCount];

	return nOldSize;
}

bool CSpectrumAnalyser::SetVisualization(VisualizationInfo* pInfo)
{
	if (NULL == pInfo)
		return false;
	
	bool bHandled = false;
	if (false == m_bSuspend)
	{
		bHandled = true;
		Pause();
	}

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

				m_hMemBitmap = CreateCompatibleBitmap(m_hRenderWndMemDC, nNewW,nNewW);
				m_hOldBitmap = (HBITMAP)::SelectObject(m_hRenderWndMemDC, m_hMemBitmap);
			}
		}
	}
	if (pInfo->nMask & VI_MASK_TYPE)
	{
		m_eType = pInfo->eType;

		if (m_eType != VISUALIZATION_SPECTRUM)  // 释放不是该类型的数据
		{
			SAFE_ARRAY_DELETE(m_pBandValue);
			SAFE_ARRAY_DELETE(m_pOldBandValue);
		}
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

	if (bHandled)
	{
		Play();
	}
	return true;
}
HRESULT CSpectrumAnalyser::RenderFile(int nChannel, int nBytePerSample)
{
	m_nChannels = nChannel;
	m_nBytePerSample = nBytePerSample;
	
	SAFE_ARRAY_DELETE(m_pSampleBuffer);
	m_nSampleBufferSize = m_nChannels*m_nBytePerSample*m_nAnalyserSampleCount;
	m_pSampleBuffer = new signed char[m_nSampleBufferSize];

	return S_OK;
}

// 从directsound中获取当前正在播放的数据，用于频谱分析
BOOL CSpectrumAnalyser::GetSampleBufferFromDSound()
{
	if (NULL == m_pSoundEngine || NULL == m_pSampleBuffer)
		return FALSE;

	return m_pSoundEngine->GetPlayBuffer(m_pSampleBuffer, m_nSampleBufferSize);
}

void CSpectrumAnalyser::Play()
{
	if (NULL != m_hThread && m_bSuspend)
	{
		DWORD dwRet = ::ResumeThread(m_hThread);
		if (1 == dwRet)
			m_bSuspend = false;
	}
}
void CSpectrumAnalyser::Pause()
{
	if (NULL != m_hThread && false == m_bSuspend)
	{
		::SuspendThread(m_hThread);
		m_bSuspend = true;
	}
}
void CSpectrumAnalyser::Stop()
{
	if (NULL != m_hThread && false == m_bSuspend)
	{
		::SuspendThread(m_hThread);
		m_bSuspend = true;
	}
}
void CSpectrumAnalyser::ThreadProc()
{
	while(1)
	{
		this->Process();
		Sleep(m_nFps); 
	}
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
	if (NULL == m_pLeftRightChannelData)
		return;

	int SampleSize = m_nAnalyserSampleCount;
	if (m_nChannels == 1 && m_nBytePerSample == 1) 
	{
		for (int i = 0; i < SampleSize; i++)
		{
// 			m_Left[a] = (float) m_pSampleBuffer[a] / 128.0F;
// 			m_Right[a] = m_Left[a];

			m_pLeftRightChannelData[i] = (float)m_pSampleBuffer[i] / 128.0f;
		}

	} 
	else if (m_nChannels == 2 && m_nBytePerSample == 1) 
	{
		for (int i = 0; i < SampleSize; i+=2) 
		{
// 			m_Left[i] = (float) m_pSampleBuffer[i<<1] / 128.0F;
// 			m_Right[i] = (float) m_pSampleBuffer[(i<<1)+1] / 128.0F;

			int n = i<<1;
			m_pLeftRightChannelData[i] = (m_pSampleBuffer[n] + m_pSampleBuffer[n+1]) / 256.0F;
		}

	} 
	else if ( m_nChannels == 1 &&  m_nBytePerSample == 2) 
	{
		for (int i = 0; i <  SampleSize; i++) 
		{
// 			m_Left[i] = (float) (( m_pSampleBuffer[(i<<1)+1] << 8) +
// 				m_pSampleBuffer[i<<1]) / 32767.0F;
// 			m_Right[i] =  m_Left[i];

			m_pLeftRightChannelData[i] = (float) (( m_pSampleBuffer[(i<<1)+1] << 8) + m_pSampleBuffer[i<<1]) / 32767.0F;
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

			m_pLeftRightChannelData[i] = (fLeft+fRight)/65534.0F;
		}

	}
}

void CSpectrumAnalyser::FFTSamples()
{	
// 	for (int a = 0; a < m_nAnalyserSampleCount; a++) 
// 	{
// 		m_Left[a] = (m_Left[a] + m_Right[a]) / 2.0f;
// 	}

	float* pFFTResult = m_pFFT->Calculate(m_pLeftRightChannelData, m_nAnalyserSampleCount); 
	for (int i = 0, nBandIndex = 0; nBandIndex < m_nBandCound; i += m_nSamplesPerBand, nBandIndex++) 
	{
		float fBandValue = 0;
		// 计算一个柱形条的能量值。（每个柱形条是包含了m_nSamplesPerBand个取样点）
		for (int j = 0; j < (INT)m_nSamplesPerBand; j++) 
		{
			fBandValue += pFFTResult[i + j];
		}

		// 下面的这些代码完成是抄的，不明白取这些值的原由
		fBandValue = (fBandValue * (float) log(nBandIndex + 2.0F));   // -- Log filter.

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
	::FillRect(m_hRenderWndMemDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));

	HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	for (int i = 0; i < m_nBandCound; i++)
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
	HDC  hDC = GetDC(m_hRenderWnd);
	HDC  hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hMemDC, 240,100);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HPEN hOldPen = (HPEN)::SelectObject(hMemDC, hPen);
	int nWidth = 240;
	int nHeight = 100;
	
	
	BOOL bRet = MoveToEx(hMemDC, 0, nHeight/2, NULL);
	int nSampleCountPerPX = m_nAnalyserSampleCount/240;
#if 0
	for (int i = 0,j=0; i < m_nAnalyserSampleCount; j++)
	{
		float fData = 0;
		for (int k = 0; k <nCount; k++)
		{
			fData += m_pLeftRightChannelData[i+k];
		}
		fData = fData/nCount;

				
		int n = fData* nHeight/2;
		int ny = nHeight/2 - n;

		i+= nCount;
		int nx = j;
		
		bRet = LineTo(hMemDC,nx,ny);
	}	
#else
	float fPrevData = 0;
	float f = 0.3f;
	int   nPrevY = 50;
	int   fPrevDirection = 0;  // 
	float fDatas[240] = {0};

	for (int i = 0,j=0; j < 240; j++)
	{
		fDatas[j] = m_pLeftRightChannelData[j];
	}

	// 求相邻结点的平均数据
// 	float fDatas2[240] = {0};
// #define AAA 0
// 	for (int i = 0+AAA; i < 240-AAA; i++)
// 	{
// 
// 		for (int j = i-AAA; j <= i+AAA; j++)
// 		{
// 			fDatas2[i] += fDatas[j];
// 		}
// 		fDatas2[i] = fDatas2[i]/(AAA*2.0+1.0);
// 	}


#define XXX 0
	for (int i = 0; i < 240; i++)
	{
		int nx = i;
		int n = (int)(fDatas[i]* nHeight/2);
		int ny = nHeight/2 - n;

#define SETPIXEL(x,y) ::SetPixel(hMemDC,x,y,RGB(255,255,255));
		if (ny > nPrevY + XXX)
		{
			// 往上爬一格
			SETPIXEL(nx,nPrevY);
			SETPIXEL(nx,++nPrevY);
		}
		else if (ny < nPrevY-XXX)
		{
			// 下降一格
			SETPIXEL(nx,nPrevY);
			SETPIXEL(nx,--nPrevY);
		}
		else if (ny == nPrevY)
		{
			// 不爬,往右延伸一格
			SETPIXEL(nx-1,ny);
			SETPIXEL(nx,ny);
		}
	}
#endif

	::BitBlt(hDC, 0,0,240,100,hMemDC,0,0,SRCCOPY);
	::SelectObject(hMemDC, hOldPen);
	::DeleteObject(hPen);
	::SelectObject(hMemDC, hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(m_hRenderWnd,hDC);
}