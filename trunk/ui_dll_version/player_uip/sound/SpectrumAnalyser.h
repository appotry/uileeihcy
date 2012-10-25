#pragma  once
#include "FastFourierTransform.h"
#include "fftw3.h"
#pragma comment(lib, "libfftw3-3.lib")
#include "..\player_uip.h"
#include "..\\threadmessage.h"
class   ISoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE 16/*2048*/

enum THREAD_SUSPEND_REASON
{
	THREAD_SUSPEND_BY_PLAY_STATE = 0x01,   // 未播放
	THREAD_SUSPEND_BY_VISUAL_NONE = 0x02,  // 不显示频谱图
};

//////////////////////////////////////////////////////////////////////////
//
//  Direct Sound频谱分析类。解析并显示频谱图
//
//////////////////////////////////////////////////////////////////////////

//  Sample是什么？
//     是PCM数据中的一个音频数据。代表一次取样数据。
//   

// TODO:
//
//     TMD, 这里面的参数设置也太乱了。相互之间的依赖已经管不过来了！

class CSpectrumAnalyser
{
public:
	CSpectrumAnalyser();
	~CSpectrumAnalyser();

	HRESULT  InitDefault(HWND hRenderWnd);
	HRESULT  Release();
	HRESULT  RenderFile(int nChannel, int nBytePerSample);
	void     SetSoundEngine(ISoundEngine* pSoundEngine);
	int      GetAnslyserSampleCount();
	bool     SetVisualization(VisualizationInfo* pInfo);

	BOOL     GetSampleBufferFromDSound();
	void     FFTSamples();
	void     TransformSamples();
	void     DrawBands();
	void     DrawWave();

	void     Play();
	void     Pause();
	void     Stop();
	void     ThreadProc();

protected:
	void     Process();
	bool     EventMsgProc();
	bool     HandleEventMsg(MSG* pMsg);
	bool     PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam);

private:
	// 这些设置函数，是在分析线程中被调用的。因此不对外部开放，避免线程同步问题
	void     SetRenderWnd(HWND hRenderWnd);
	int      SetBandCount(int nCount);
	int      SetAnalyserSampleCount(int nCount = DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
	void     SetVisualizationType(E_VISUALIZATION_TYPE eType);
	
	bool     OnSetVisualization(VisualizationInfo* pInfo);
	void     OnPlay();
	void     OnPause();
	void     OnStop();
public:

	// 数据分析参数
	int      m_nChannels;            // 声道数量 1(单声道)  2(立体声)
	int      m_nBytePerSample;       // 每个取样点的字节数：1(8位)  2(16位)  
	
	int      m_nAnalyserSampleCount; // 每次分析的数据大小
	float*	 m_pLeftRightSampleData; // m_Left m_Right的平均值，大小为m_nAnalyserSampleCount

	int      m_nSampleBufferSize;    // m_pSampleBuffer的大小 = m_nAnalyserSampleCount*m_nBytePerSample*m_nChannels
	signed char* m_pSampleBuffer;    // 即将播放的数据，也是需要FFT转换的数据。注意：这里是有符号的
	
	int      m_FFTSrcSampleSize;     //  ?? FFT的构造参数
	int      m_FFTDestSampleSize;    //  ?? FFT的返回值大小
	CFastFourierTransform*  m_pFFT;  // FFT傅里叶算法
// 	double*        m_pfftw3_in;
// 	fftw_complex*  m_pfftw3_out;
// 	fftw_plan      m_fftw_plan;

	// 绘制参数
	HWND     m_hRenderWnd;           // 绘制窗口
	RECT     m_rcRender;             // 绘制区域
	E_VISUALIZATION_TYPE m_eType;    // 可视化效果类型
	int      m_nFps;                 // 分析的速度，被转换成 1000/fps

	int      m_nBandCount;           // 要显示的柱形条的数量
	int      m_nBandWidth;           // 每根柱形条的宽度
	int      m_nBandGapWidth;        // 两根柱形条之间的间隙
	int      m_nSamplesPerBand;      // 每一个柱形条中包含的取样点数量
	float*   m_pBandValue;           // 每一个柱形条的高度值
	float*   m_pOldBandValue;        // 保存每一个Band上一次的FFT转换后的结果，用于柱形条的缓慢衰减

	HDC      m_hRenderWndDC;
	HDC      m_hRenderWndMemDC;
	HBITMAP  m_hMemBitmap;
	HBITMAP  m_hOldBitmap;
	HBITMAP  m_hBkgndBmp;            // 窗口上的背景图。每一次的频谱图、波形图都绘制在该背景上

	// 线程数据
	HANDLE   m_hThread;              // 频谱分析线程
	DWORD    m_dwThreadID;           // 频谱分析线程ID
	LONG     m_bSuspend;             // 标志当前线程是否被挂起，被挂起时，不处理频谱分析。该变量仅在频谱分析线程中调用
	HANDLE   m_hEventSuspend;        // 标志当前线程是否被挂起。例如停止、暂停时，不进行频谱分析
	CRITICAL_SECTION  m_cs;

	// 其它模块数据
	ISoundEngine* m_pSoundEngine;
};