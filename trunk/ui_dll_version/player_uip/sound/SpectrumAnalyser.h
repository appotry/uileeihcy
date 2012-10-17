#pragma  once
#include "FastFourierTransform.h"
#include "..\player_uip.h"
class   ISoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE 2048
#define DEFAULT_FPS 30
#define DEFAULT_SPECTRUM_ANALYSER_DECAY 0.05f

//////////////////////////////////////////////////////////////////////////
//
//  Direct SoundƵ�׷����ࡣ��������ʾƵ��ͼ
//
//////////////////////////////////////////////////////////////////////////

//  Sample��ʲô��
//     ��̫����������������PCM�����е�һ����Ƶ���ݡ�����һ��ȡ�����ݡ�
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
	int      m_nChannels;           // �������� 1(������)  2(������)
	int      m_nBytePerSample;      // ÿ��ȡ������ֽ�����1(8λ)  2(16λ)  
	
	int      m_nAnalyserSampleCount;// ÿ�η����Ļ�������С
	int      m_nSampleBufferSize;   // m_pSampleBuffer�Ĵ�С = m_nAnalyserSampleCount*m_nBytePerSample*m_nChannels
	signed char* m_pSampleBuffer;   // �������ŵ����ݣ�Ҳ����ҪFFTת�������ݡ�ע�⣺�������з��ŵ�
	
//	float    m_Left[DEFAULT_FFT_SAMPLE_BUFFER_SIZE];  // <-- ��ʵû�б�Ҫ��������
//	float    m_Right[DEFAULT_FFT_SAMPLE_BUFFER_SIZE]; // <-- ��Ϊ���ֻ��ȡ��left right��ƽ��ֵ����fft���������ֵ�Ǹ��ٷ���
    float*	 m_pLeftRightChannelData;// m_Left m_Right��ƽ��ֵ����СΪm_nAnalyserSampleCount

	// ���Ʋ���
	HWND     m_hRenderWnd;           // ���ƴ���
	RECT     m_rcRender;             // ��������
	E_VISUALIZATION_TYPE m_eType;    // ���ӻ�Ч������
	int      m_nFps;                 // �������ٶ�

	HDC      m_hRenderWndDC;
	HDC      m_hRenderWndMemDC;
	HBITMAP  m_hMemBitmap;
	HBITMAP  m_hOldBitmap;

	int      m_nBandCound;          // Ҫ��ʾ��������������
	int      m_nBandWidth;          // ÿ���������Ŀ��
	int      m_nBandGapWidth;       // ����������֮��ļ�϶
	int      m_nSamplesPerBand;     // ÿһ���������а�����ȡ��������
	float*   m_pBandValue;          // ÿһ���������ĸ߶�ֵ
	float*   m_pOldBandValue;       // ����ÿһ��Band��һ�ε�FFTת����Ľ���������������Ļ���˥��


	HANDLE   m_hThread;              // Ƶ�׷����߳�
	bool     m_bSuspend;             // 

	ISoundEngine* m_pSoundEngine;

	int      m_FFTSrcSampleSize;    //  ?? FFT�Ĺ������
	int      m_FFTDestSampleSize;   //  ?? FFT�ķ���ֵ��С
	CFastFourierTransform*  m_pFFT;  // FFT����Ҷ�㷨
	
	
};