#pragma once


// ��Ϊ����������ʵ��FFT�ķ����������һ�����࣬�����޸������������
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
// Ŀǰ�����������CFastFourierTransform��õ���Ч�����ӷ������ֵĽ���
// ��FFTWʵ�ֵ�Ч�����Ǻ�TTPLAYER���ӽӽ���
//
//////////////////////////////////////////////////////////////////////////

// ����CFastFourierTransform����FFT�㷨����CFastFourierTransform���а�װ
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
	int     m_nGetSamplePercent;  // ��ֵ����ָʾ������������У�ÿ�����ٸ�ȡһ������������
};

#ifdef FFTW

#include "fftw3.h"
#pragma comment(lib, "libfftw3-3.lib")

// ����FFTW��Դ���FFT�㷨����FFTW������һ�ΰ�װ
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

	float*         m_pAmplitudes;   // ������������ֵ��������ʾƵ��ͼ

	int    m_nAnalyserSampleCount;
};

#endif