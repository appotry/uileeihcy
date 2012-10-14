#pragma  once
#include "FastFourierTransform.h"

class   CDirectSoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE 2048
#define DEFAULT_FPS 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05f

//////////////////////////////////////////////////////////////////////////
//
//  Direct SoundƵ�׷����ࡣ��������ʾƵ��ͼ
//
//////////////////////////////////////////////////////////////////////////

class CSpectrumAnalyser
{
public:
	CSpectrumAnalyser()
	{
		m_pFFT = NULL;
		m_hRenderWnd = NULL;
	}
	~CSpectrumAnalyser()
	{
		SAFE_DELETE(m_pFFT);
	}

	void  FFTSamples();
	void  TransformSamples();
	void  DrawBands();


	BOOL GetSampleBufferFromDSound();
	void Create(HWND hWnd, int nChannel, int nBytePerSample);
	void Process()
	{
		//////////////////////////////////////////////////////////////////////////

		if (GetSampleBufferFromDSound())
		{
			TransformSamples();
			FFTSamples();
			DrawBands();
		}

		//////////////////////////////////////////////////////////////////////////
	}
public:
	int   m_channel;
	int   m_sampleType;
	int   m_FFTSrcSampleSize;
	int   m_FFTDestSampleSize;
	int   m_Bands;
	int   m_saMultiplier;

	float m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- ��ʵû�б�Ҫ��������
	float m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- ��Ϊ���ֻ��ȡ��left right��ƽ��ֵ����fft���������ֵ�Ǹ��ٷ���

	float *m_FFTResult,*m_pBands;
	float m_saDecay;
	float *m_OldFFT;
	float m_MaxFqr;

	byte  m_SampleBuffer[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];

	CFastFourierTransform*  m_pFFT;
	CDirectSoundEngine*     m_pDirectSound;
	HWND    m_hRenderWnd;
};