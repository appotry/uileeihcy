#pragma once


// 因为现在有两种实现FFT的方法，因此做一个基类，便于修改派生类的类型
class FFT
{
public:
	virtual ~FFT() = 0 {};
	virtual bool Init(int nSampleCount) = 0;
	virtual bool Release() = 0;
	virtual bool DoFFT(float* pData) = 0;
	virtual bool DoIFFT() = 0;
	virtual int  GetAmplitude(float** ppResult) = 0;
	virtual int  GetFFTOutputSize() = 0;
};





#define PI   3.1415925F
#define PI_2 6.283185F
#define PI_3 9.4247775F
#define PI_4 12.56637F
#define PI_5 15.7079625F
#define PI_6 18.849555F
#define PI_7 21.9911475F
#define PI_8 25.13274F
#define PI_9 28.2743325F

class CFastFourierTransform
{
private:
	float* xre;
	float* xim;
	float* mag;
	float* fftSin;
	float* fftCos;
	int* fftBr;
	int ss;
	int ss2;
	int nu;
	int nu1;

	int   BitRev(int j, int nu);
	void  PrepareFFTTables();

public:
	CFastFourierTransform(int pSampleSize);
	~CFastFourierTransform(void);

	float* Calculate(float* pSample, size_t pSampleSize);
	const float* GetResult() { return mag; };
};

//////////////////////////////////////////////////////////////////////////
//
// 目前的情况来看，CFastFourierTransform类得到的效果更加符合音乐的节奏
// 而FFTW实现的效果则是和TTPLAYER更加接近。
//
//////////////////////////////////////////////////////////////////////////

// 基于CFastFourierTransform类于FFT算法，对CFastFourierTransform进行包装
class CFastFourierTransformWrap : public FFT
{
public:
	CFastFourierTransformWrap();
	~CFastFourierTransformWrap();

	virtual bool Init(int nSampleCount);
	virtual bool Release();
	virtual bool DoFFT(float* pData);
	virtual bool DoIFFT();
	virtual int  GetAmplitude(float** ppResult);
	virtual int  GetFFTOutputSize();

public:
	CFastFourierTransform*  m_pFFT;
	int     m_nAnalyserSampleCount;
	int     m_nGetSamplePercent;  // 该值用于指示将传入的数据中，每隔多少个取一个数据来分析
};

#ifdef FFTW

#include "fftw3.h"
#pragma comment(lib, "libfftw3-3.lib")

// 基于FFTW开源库的FFT算法，对FFTW进行了一次包装
class FFTWWrap : public FFT
{
public:
	FFTWWrap();
	~FFTWWrap();

	virtual bool Init(int nSampleCount);
	virtual bool Release();
	virtual bool DoFFT(float* pData);
	virtual bool DoIFFT();
	virtual int  GetAmplitude(float** ppResult);
	virtual int  GetFFTOutputSize();

protected:
	double*        m_pfftw3_in;
	fftw_complex*  m_pfftw3_out;
	fftw_plan      m_fftw_plan;

	float*         m_pAmplitudes;   // 计算出来的振幅值，用于显示频谱图

	int    m_nAnalyserSampleCount;
};

#endif