#include "stdafx.h"
#include "SpectrumAnalyser.h"
#include "DirectSoundEngine.h"
#include <math.h>

CSpectrumAnalyser::CSpectrumAnalyser(CDirectSoundEngine* pDirectSound)
{
	m_pFFT = NULL;
	m_hRenderWnd = NULL;
	m_pDirectSound = pDirectSound;
	
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

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gSpectrumAnalyserThreadProc, (LPVOID)this, 0, NULL);
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
	return S_OK;
}

void CSpectrumAnalyser::SetRenderWnd(HWND hRenderWnd)
{
	m_hRenderWnd = hRenderWnd;
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
	if (NULL == m_pDirectSound || NULL == m_pSampleBuffer)
		return FALSE;

	return m_pDirectSound->GetPlayBuffer(m_pSampleBuffer, m_nSampleBufferSize);
}
void CSpectrumAnalyser::ThreadProc()
{
	while(1)
	{
		this->Process();
		Sleep(50);  // 大概 20fps
	}
}
void CSpectrumAnalyser::Process()
{
	if (GetSampleBufferFromDSound())
	{
		TransformSamples();
		FFTSamples();
		DrawBands();
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
	HDC  hDC = GetDC(m_hRenderWnd);
	HDC  hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hMemDC, 200,100);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	for (int i = 0; i < m_nBandCound; i++)
	{
		int ny = 100 - (int)(100 * m_pBandValue[i]);
		int nx = i*8;

		RECT rc = {nx, ny,nx+7, 100};
		::FillRect(hMemDC,&rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}
	::BitBlt(hDC, 0,0,200,100,hMemDC,0,0,SRCCOPY);
	::SelectObject(hMemDC, hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(m_hRenderWnd,hDC);
}