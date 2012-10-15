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

//  Sample��ʲô��
//     ��̫����������������PCM�����е�һ���ֽڡ�����һ��ȡ�����ݡ���16λ������һ��ȡ����һ���ˣ�
//   

class CSpectrumAnalyser
{
public:
	CSpectrumAnalyser(CDirectSoundEngine* pDirectSound);
	~CSpectrumAnalyser();

	HRESULT  InitDefault(HWND hRenderWnd);
	HRESULT  Release();
	HRESULT  RenderFile(int nChannel, int nBytePerSample);
	void     SetRenderWnd(HWND hRenderWnd);
	int      SetBandCount(int nCount);
	int      SetAnalyserSampleCount(int nCount = DEFAULT_FFT_SAMPLE_BUFFER_SIZE);

	BOOL     GetSampleBufferFromDSound();
	void     FFTSamples();
	void     TransformSamples();
	void     DrawBands();

	void     ThreadProc();
	void     Process();

public:
	int      m_nChannels;           // �������� 1(������)  2(������)
	int      m_nBytePerSample;      // ÿ��ȡ������ֽ�����1(8λ)  2(16λ)  
	
	int      m_nAnalyserSampleCount;// ÿ�η����Ļ�������С
	int      m_nSampleBufferSize;   // m_pSampleBuffer�Ĵ�С = m_nAnalyserSampleCount*m_nBytePerSample*m_nChannels
	signed char* m_pSampleBuffer;   // �������ŵ����ݣ�Ҳ����ҪFFTת�������ݡ�ע�⣺�������з��ŵ�
	
	int      m_nBandCound;          // Ҫ��ʾ��������������
	int      m_nSamplesPerBand;     // ÿһ���������а�����ȡ��������
	float*   m_pBandValue;          // ÿһ���������ĸ߶�ֵ
	float*   m_pOldBandValue;       // ����ÿһ��Band��һ�ε�FFTת����Ľ���������������Ļ���˥��

//	float    m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- ��ʵû�б�Ҫ��������
//	float    m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- ��Ϊ���ֻ��ȡ��left right��ƽ��ֵ����fft���������ֵ�Ǹ��ٷ���
    float*	 m_pLeftRightChannelData;// m_Left m_Right��ƽ��ֵ����СΪm_nAnalyserSampleCount

	HWND     m_hRenderWnd;           // ���ƴ���
	HANDLE   m_hThread;              // Ƶ�׷����߳�

	CDirectSoundEngine*     m_pDirectSound;

	int      m_FFTSrcSampleSize;    //  ?? FFT�Ĺ������
	int      m_FFTDestSampleSize;   //  ?? FFT�ķ���ֵ��С
	CFastFourierTransform*  m_pFFT;  // FFT����Ҷ�㷨
	
	
};