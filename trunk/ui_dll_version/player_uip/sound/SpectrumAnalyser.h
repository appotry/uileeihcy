#pragma  once
#include "FastFourierTransform.h"
#include "..\player_uip.h"
class   ISoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE 2048
#define DEFAULT_FPS 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05f

//////////////////////////////////////////////////////////////////////////
//
//  Direct Sound频谱分析类。解析并显示频谱图
//
//////////////////////////////////////////////////////////////////////////

//  Sample是什么？
//     不太清楚。在这里假设是PCM数据中的一个音频数据。代表一次取样数据。
//   

class CSpectrumAnalyser
{
public:
	CSpectrumAnalyser();
	~CSpectrumAnalyser();

	HRESULT  InitDefault(HWND hRenderWnd);
	HRESULT  Release();
	HRESULT  RenderFile(int nChannel, int nBytePerSample);
	void     SetSoundEngine(ISoundEngine* pSoundEngine) { m_pSoundEngine = pSoundEngine; }
	void     SetRenderWnd(HWND hRenderWnd);
	int      SetBandCount(int nCount);
	int      SetAnalyserSampleCount(int nCount = DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
	int      GetAnslyserSampleCount();
	bool     SetVisualization(VisualizationInfo* pInfo);
	void     SetVisualizationType(E_VISUALIZATION_TYPE eType);

	BOOL     GetSampleBufferFromDSound();
	void     FFTSamples();
	void     TransformSamples();
	void     DrawBands();
	void     DrawWave();

	void     Play();
	void     Pause();
	void     Stop();
	void     ThreadProc();
	void     Process();

public:
	int      m_nChannels;           // 声道数量 1(单声道)  2(立体声)
	int      m_nBytePerSample;      // 每个取样点的字节数：1(8位)  2(16位)  
	
	int      m_nAnalyserSampleCount;// 每次分析的缓冲区大小
	int      m_nSampleBufferSize;   // m_pSampleBuffer的大小 = m_nAnalyserSampleCount*m_nBytePerSample*m_nChannels
	signed char* m_pSampleBuffer;   // 即将播放的数据，也是需要FFT转换的数据。注意：这里是有符号的
	
//	float    m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- 其实没有必要保存两份
//	float    m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- 因为最后只是取了left right的平均值传给fft。这里面的值是个百分数
    float*	 m_pLeftRightChannelData;// m_Left m_Right的平均值，大小为m_nAnalyserSampleCount

	// 绘制参数
	HWND     m_hRenderWnd;           // 绘制窗口
	RECT     m_rcRender;             // 绘制区域
	E_VISUALIZATION_TYPE m_eType;    // 可视化效果类型
	int      m_nFps;                 // 分析的速度

	HDC      m_hRenderWndDC;
	HDC      m_hRenderWndMemDC;
	HBITMAP  m_hMemBitmap;
	HBITMAP  m_hOldBitmap;

	int      m_nBandCound;          // 要显示的柱形条的数量
	int      m_nBandWidth;          // 每根柱形条的宽度
	int      m_nBandGapWidth;       // 两根柱形条之间的间隙
	int      m_nSamplesPerBand;     // 每一个柱形条中包含的取样点数量
	float*   m_pBandValue;          // 每一个柱形条的高度值
	float*   m_pOldBandValue;       // 保存每一个Band上一次的FFT转换后的结果，用于柱形条的缓慢衰减


	HANDLE   m_hThread;              // 频谱分析线程
	bool     m_bSuspend;             // 

	ISoundEngine* m_pSoundEngine;

	int      m_FFTSrcSampleSize;    //  ?? FFT的构造参数
	int      m_FFTDestSampleSize;   //  ?? FFT的返回值大小
	CFastFourierTransform*  m_pFFT;  // FFT傅里叶算法
	
	
};