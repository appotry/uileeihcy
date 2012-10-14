#pragma  once
#include "FastFourierTransform.h"

class   CDirectSoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE 2048
#define DEFAULT_FPS 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05f

//////////////////////////////////////////////////////////////////////////
//
//  Direct Sound频谱分析类。解析并显示频谱图
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
	int   m_nChannels;         // 声道数量 1(单声道)  2(立体声)
	int   m_nBytePerSample;    // 每个取样点的字节数：1(8位)  2(16位)  
	int   m_nAnalyserBufferSize;  // 每次分析的缓冲区大小
	int   m_FFTSrcSampleSize;
	int   m_FFTDestSampleSize;
	int   m_nBandCound;        // 要显示的柱形条的数量
	int   m_nSamplesPerBand;   // 每一个柱形条中包含的取样点数量
	float *m_pBandValue;       // 每一个柱形条的高度值
	float *m_pOldBandValue;    // 保存每一个Band上一次的FFT转换后的结果

	float m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- 其实没有必要保存两份
	float m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- 因为最后只是取了left right的平均值传给fft。这里面的值是个百分数

	float *m_FFTResult;
	float m_saDecay;
	
	HANDLE m_hThread;
	void   ThreadProc();
	float m_MaxFqr;

	signed char m_SampleBuffer[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // 注意：这里是有符号的

	CFastFourierTransform*  m_pFFT;
	CDirectSoundEngine*     m_pDirectSound;
	HWND    m_hRenderWnd;
};