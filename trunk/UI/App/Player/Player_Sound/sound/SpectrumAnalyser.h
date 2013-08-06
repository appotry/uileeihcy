#pragma  once
#include "FastFourierTransform.h"
#include "..\player_sound.h"
#include "..\\threadmessage.h"
class   ISoundEngine;

#define DEFAULT_FFT_SAMPLE_BUFFER_SIZE   2048/*2048*/    // FFT�Ĵ�С������Ҷ�������Ϊ2048.��������С�㲻���ܸ���ʡ��������

enum THREAD_SUSPEND_REASON
{
	THREAD_SUSPEND_BY_PLAY_STOP = 0x01,     // δ����
	THREAD_SUSPEND_BY_PLAY_PAUSE = 0x02,    // ��ͣ
	THREAD_SUSPEND_BY_VISUAL_NONE = 0x04,   // ����ʾƵ��ͼ
};


//////////////////////////////////////////////////////////////////////////
//
//  Direct SoundƵ�׷����ࡣ��������ʾƵ��ͼ
//
//////////////////////////////////////////////////////////////////////////

//  Sample��ʲô��
//     ��PCM�����е�һ����Ƶ���ݡ�����һ��ȡ�����ݡ�
//   

// TODO:
//
//     TMD, ������Ĳ�������Ҳ̫���ˡ��໥֮��������Ѿ��ܲ������ˣ�

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
    void     BlurWave();
    void     BlurWaveBitmap();

	void     Play();
	void     Pause();
	void     Stop();
	void     ThreadProc();

	HBITMAP  GetVisualSnapshot();
	void     ReleaseVisualSnapshot();

protected:
	void     Process();
	bool     EventMsgProc();
	bool     HandleEventMsg(MSG* pMsg);
	bool     PostThreadMessage(UINT uMsg, DSMSG_PARAM* pParam);

private:
	void     SetRenderWnd(HWND hRenderWnd);
	int      SetBandCount(int nCount);
	int      SetAnalyserSampleCount(int nCount = DEFAULT_FFT_SAMPLE_BUFFER_SIZE);
	void     SetVisualizationType(E_VISUALIZATION_TYPE eType);
	
	bool     OnSetVisualization(VisualizationInfo* pInfo);

	void     OnPlay();
	void     OnPause();
	void     OnStop();
public:

	// ���ݷ�������
	int      m_nChannels;            // �������� 1(������)  2(������)
	int      m_nBytePerSample;       // ÿ��ȡ������ֽ�����1(8λ)  2(16λ)  
	
	int      m_nAnalyserSampleCount; // ÿ��FFT���������ݴ�С
	float*	 m_pLeftRightSampleData; // m_Left m_Right������ƽ��ֵ����СΪm_nAnalyserSampleCount��

	int      m_nSampleBufferSize;    // m_pSampleBuffer�Ĵ�С = m_nAnalyserSampleCount*m_nBytePerSample*m_nChannels
	signed char* m_pSampleBuffer;    // �������ŵ����ݣ�Ҳ����ҪFFTת�������ݡ�ע�⣺�������з��ŵ�
	
	FFT*     m_pFFT;                 // FFT����Ҷ�㷨

	// ���Ʋ���
	HWND     m_hRenderWnd;           // ���ƴ���
	RECT     m_rcRender;             // ��������
	E_VISUALIZATION_TYPE m_eType;    // ���ӻ�Ч������
	int      m_nFps;                 // �������ٶȣ���ת���� 1000/fps

	int      m_nBandCount;           // Ҫ��ʾ��������������
	int      m_nBandWidth;           // ÿ���������Ŀ��
	int      m_nBandGapWidth;        // ����������֮��ļ�϶
	int      m_nSamplesPerBand;      // ÿһ���������а�����ȡ��������
	float*   m_pBandValue;           // ÿһ���������ĸ߶�ֵ
	float*   m_pOldBandValue;        // ����ÿһ��Band��һ�ε�FFTת����Ľ���������������Ļ���˥��
	int*     m_pPeaksValue;
	int*     m_pPeaksDelay; 

    HBITMAP*  m_pArrayWaveBlur;
    int      m_nArrayWaveBlurSize;
    int      m_nArrayWaveBlurIndex;

    Gdiplus::Point*   m_pPrevWaveData;

	HDC      m_hRenderWndDC;
	HDC      m_hRenderWndMemDC;
	HBITMAP  m_hMemBitmap;
	HBITMAP  m_hOldBitmap;
	HBITMAP  m_hBkgndBmp;            // �����ϵı���ͼ��ÿһ�ε�Ƶ��ͼ������ͼ�������ڸñ�����
	HBITMAP  m_hForegndBmp;          // ��������ͼƬ

    HBITMAP  m_hWaveMemBitmap;
    HDC      m_hWaveMemBitmapDC;

	// �߳�����
	HANDLE   m_hThread;              // Ƶ�׷����߳�
	DWORD    m_dwThreadID;           // Ƶ�׷����߳�ID
	LONG     m_bSuspend;             // ��־��ǰ�߳��Ƿ񱻹��𣬱�����ʱ��������Ƶ�׷������ñ�������Ƶ�׷����߳��е���
	HANDLE   m_hEventSuspend;        // ��־��ǰ�߳��Ƿ񱻹�������ֹͣ����ͣʱ��������Ƶ�׷���
	CRITICAL_SECTION  m_cs;

	// ����ģ������
	ISoundEngine* m_pSoundEngine;
};