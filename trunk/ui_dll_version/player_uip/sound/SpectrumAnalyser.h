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
	CSpectrumAnalyser(CDirectSoundEngine* pDirectSound);
	~CSpectrumAnalyser();

	HRESULT  InitDefault(HWND hRenderWnd);
	HRESULT  Release();
	HRESULT  RenderFile(int nChannel, int nBytePerSample);
	void     SetRenderWnd(HWND hRenderWnd);
	int      SetBandCound(int nCount);
	int      SetAnalyserBufferSize(int nSize);

	void  FFTSamples();
	void  TransformSamples();
	void  DrawBands();


	BOOL GetSampleBufferFromDSound();
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
	int   m_nChannels;         // �������� 1(������)  2(������)
	int   m_nBytePerSample;    // ÿ��ȡ������ֽ�����1(8λ)  2(16λ)  
	int   m_nAnalyserBufferSize;  // ÿ�η����Ļ�������С
	int   m_FFTSrcSampleSize;
	int   m_FFTDestSampleSize;
	int   m_nBandCound;        // Ҫ��ʾ��������������
	int   m_nSamplesPerBand;   // ÿһ���������а�����ȡ��������
	float *m_pBandValue;       // ÿһ���������ĸ߶�ֵ
	float *m_pOldBandValue;    // ����ÿһ��Band��һ�ε�FFTת����Ľ��

	float m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- ��ʵû�б�Ҫ��������
	float m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- ��Ϊ���ֻ��ȡ��left right��ƽ��ֵ����fft���������ֵ�Ǹ��ٷ���

	float *m_FFTResult;
	float m_saDecay;
	
	HANDLE m_hThread;
	void   ThreadProc();
	float m_MaxFqr;

	signed char m_SampleBuffer[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // ע�⣺�������з��ŵ�

	CFastFourierTransform*  m_pFFT;
	CDirectSoundEngine*     m_pDirectSound;
	HWND    m_hRenderWnd;
};