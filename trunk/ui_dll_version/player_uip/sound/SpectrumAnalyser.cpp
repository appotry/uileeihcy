#include "stdafx.h"
#include "SpectrumAnalyser.h"
#include "DirectSoundEngine.h"

void CSpectrumAnalyser::Create(HWND hWnd, int nChannel, int nBytePerSample)
{
	m_hRenderWnd = hWnd;
	m_pFFT = new CFastFourierTransform(DEFAULT_FFT_SAMPLE_BUFFER_SIZE/4);

	//////////////////////////////////////////////////////////////////////////
	// FFT Part
	m_channel = nChannel;
	m_sampleType = nBytePerSample;
	m_FFTSrcSampleSize=DEFAULT_FFT_SAMPLE_BUFFER_SIZE/4;
	m_FFTDestSampleSize=m_FFTSrcSampleSize/2;//why div 2?please read "How to do FFT"
	m_Bands = 30;
	m_saMultiplier=m_FFTDestSampleSize/m_Bands;

	m_pBands = new float[m_Bands];
	m_OldFFT = new float[m_Bands];
	memset(m_pBands, 0, sizeof(float)*m_Bands);
	memset(m_OldFFT, 0, sizeof(float)*m_Bands);
	//////////////////////////////////////////////////////////////////////////
}

BOOL CSpectrumAnalyser::GetSampleBufferFromDSound()
{
	return m_pDirectSound->GetPlayBuffer(m_SampleBuffer,DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
}

void CSpectrumAnalyser::TransformSamples()
{
	int SampleSize=DEFAULT_FFT_SAMPLE_BUFFER_SIZE;
	if (m_channel == 1 && m_sampleType == 1) 
	{
		for (int a = 0; a < SampleSize;a++)
		{
			m_Left[a] = (float) m_SampleBuffer[a] / 128.0F;
			m_Right[a] = m_Left[a];
		}

	} else if (m_channel == 2 && m_sampleType == 1) {
		for (int a = 0; a < SampleSize;a++) {
			m_Left[a] = (float) m_SampleBuffer[a<<1] / 128.0F;
			m_Right[a] = (float) m_SampleBuffer[(a<<1)+1] / 128.0F;
			a++;
		}

	} else if ( m_channel == 1 &&  m_sampleType == 2) {
		for (int a = 0; a <  SampleSize;a++) 
		{
			m_Left[a] = (float) (( m_SampleBuffer[(a<<1)+1] << 8) +
				m_SampleBuffer[a<<1]) / 32767.0F;
			m_Right[a] =  m_Left[a];
		}

	} else if ( m_channel == 2 &&  m_sampleType == 2)
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

	float* FFTResult = m_pFFT->Calculate(m_Left, DEFAULT_FFT_SAMPLE_BUFFER_SIZE);//FFT was complete
	if(m_pBands!=NULL)
	{
		for (int a = 0,  bd = 0; bd < m_Bands; a += (INT)m_saMultiplier, bd++) 
		{
			float wFs = 0;
			for (int b = 0; b < (INT)m_saMultiplier; b++) 
			{
				wFs += FFTResult[a + b];
			}
			if (wFs>m_MaxFqr)
			{
				m_MaxFqr=wFs;
			}
			m_pBands[bd]=wFs;

		}
		float Disten;
		for (int i=0;i<m_Bands;i++)
		{
			if(m_pBands[i]>m_OldFFT[i])
			{
				Disten=m_pBands[i]-m_OldFFT[i];
			}
			else
			{
				Disten=0;
			}
			m_OldFFT[i]=m_pBands[i];
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


			m_pBands[i]=Disten*1.5f;

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
	for (int i = 0; i < m_Bands; i++)
	{
		int ny = 100 - (int)(100 * m_pBands[i]);
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