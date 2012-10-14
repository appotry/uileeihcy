#include "stdafx.h"
#include "SpectrumAnalyser.h"
#include "DirectSoundEngine.h"

CSpectrumAnalyser::CSpectrumAnalyser()
{
	m_pFFT = NULL;
	m_hRenderWnd = NULL;
	
	m_nChannels = 0;
	m_nBytePerSample = 0;
	m_nAnalyserBufferSize = 0;
	m_nBandCound = 0;
	m_nSamplesPerBand = 0;

	m_pBandValue = NULL;
	m_pOldBandValue = NULL;
}
CSpectrumAnalyser::~CSpectrumAnalyser()
{
	this->Release();
}

HRESULT CSpectrumAnalyser::Init()
{
	m_nAnalyserBufferSize = DEFAULT_FFT_SAMPLE_BUFFER_SIZE;
	m_FFTSrcSampleSize = m_nAnalyserBufferSize/4;
	m_pFFT = new CFastFourierTransform(m_nAnalyserBufferSize/4);  // ?? 为什么要除以4？
	
	return S_OK;
}
HRESULT CSpectrumAnalyser::Release()
{
	SAFE_DELETE(m_pFFT);
	return S_OK;
}

void CSpectrumAnalyser::SetRenderWnd(HWND hRenderWnd)
{
	m_hRenderWnd = hRenderWnd;
}
int CSpectrumAnalyser::SetBindCound(int nCount)
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

	return nOldCound;
}
HRESULT CSpectrumAnalyser::RenderFile(int nChannel, int nBytePerSample)
{
	m_nChannels = nChannel;
	m_nBytePerSample = nBytePerSample;

	m_FFTDestSampleSize=m_FFTSrcSampleSize/2;//why div 2?please read "How to do FFT"
	m_nSamplesPerBand=m_FFTDestSampleSize/m_nBandCound;
	
	return S_OK;
}

BOOL CSpectrumAnalyser::GetSampleBufferFromDSound()
{
	return m_pDirectSound->GetPlayBuffer(m_SampleBuffer,DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
}

void CSpectrumAnalyser::TransformSamples()
{
	int SampleSize=DEFAULT_FFT_SAMPLE_BUFFER_SIZE;
	if (m_nChannels == 1 && m_nBytePerSample == 1) 
	{
		for (int a = 0; a < SampleSize;a++)
		{
			m_Left[a] = (float) m_SampleBuffer[a] / 128.0F;
			m_Right[a] = m_Left[a];
		}

	} else if (m_nChannels == 2 && m_nBytePerSample == 1) {
		for (int a = 0; a < SampleSize;a++) {
			m_Left[a] = (float) m_SampleBuffer[a<<1] / 128.0F;
			m_Right[a] = (float) m_SampleBuffer[(a<<1)+1] / 128.0F;
			a++;
		}

	} else if ( m_nChannels == 1 &&  m_nBytePerSample == 2) {
		for (int a = 0; a <  SampleSize;a++) 
		{
			m_Left[a] = (float) (( m_SampleBuffer[(a<<1)+1] << 8) +
				m_SampleBuffer[a<<1]) / 32767.0F;
			m_Right[a] =  m_Left[a];
		}

	} else if ( m_nChannels == 2 &&  m_nBytePerSample == 2)
	{
		for (int a = 0; a <  SampleSize;a++) 
		{
			m_Left[a] = (float) (( m_SampleBuffer[(a<<2)+1] << 8) +
				m_SampleBuffer[(a<<2)]) / 32767.0F;
			m_Right[a] = (float) (( m_SampleBuffer[(a<<2)+3] << 8) +
				m_SampleBuffer[(a<<2)+2]) / 32767.0F;
		}

	}
}

void CSpectrumAnalyser::FFTSamples()
{	
	for (int a = 0; a < DEFAULT_FFT_SAMPLE_BUFFER_SIZE; a++) 
	{
		m_Left[a] = (m_Left[a] + m_Right[a]) / 2.0f;
	}

	float* FFTResult = m_pFFT->Calculate(m_Left, m_nAnalyserBufferSize); 
	if(m_pBandValue!=NULL)
	{
		for (int a = 0,  bd = 0; bd < m_nBandCound; a += (INT)m_nSamplesPerBand, bd++) 
		{
			float wFs = 0;
			for (int b = 0; b < (INT)m_nSamplesPerBand; b++) 
			{
				wFs += FFTResult[a + b];
			}
			if (wFs>m_MaxFqr)
			{
				m_MaxFqr=wFs;
			}
			m_pBandValue[bd]=wFs;

		}
		float Disten;
		for (int i=0;i<m_nBandCound;i++)
		{
			if(m_pBandValue[i]>m_pOldBandValue[i])
			{
				Disten=m_pBandValue[i]-m_pOldBandValue[i];
			}
			else
			{
				Disten=0;
			}
			m_pOldBandValue[i]=m_pBandValue[i];
			if (Disten<0.01f)
			{
				Disten*=20;
			}
			else if (Disten<0.05)
			{
				Disten*=10;
			}
			else if (Disten<0.1)
			{
				Disten*=5;
			}


			m_pBandValue[i]=Disten*1.5f;

		}
	}
}
// 直接在另一个线程中提交到窗口上面
void CSpectrumAnalyser::DrawBands()
{
	HDC  hDC = GetDC(m_hRenderWnd);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hMemDC, 200,100);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	for (int i = 0; i < m_nBandCound; i++)
	{
		int ny = 100 - (int)(100 * m_pBandValue[i]);
		int nx = i*8;

		RECT rc = {nx, ny,nx+8, 100};
		::FillRect(hMemDC,&rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}
	::BitBlt(hDC, 0,0,200,100,hMemDC,0,0,SRCCOPY);
	::SelectObject(hMemDC, hOldBmp);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(m_hRenderWnd,hDC);
}