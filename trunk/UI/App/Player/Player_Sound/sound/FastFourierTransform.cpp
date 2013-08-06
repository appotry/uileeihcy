#include "StdAfx.h"
#include "FastFourierTransform.h"
#include <math.h>

CFastFourierTransform::CFastFourierTransform(int pSampleSize)
{
	xre = NULL;
	xim = NULL;
	mag = NULL;
	fftSin = NULL;
	fftCos = NULL;
	fftBr = NULL;

	ss = pSampleSize;
	ss2 = ss >> 1;
	nu = (int) (log((float)ss) / log((float)2));
	nu1 = nu - 1;

	xre = new float[ss]; // real part
	xim = new float[ss]; // image part
	mag = new float[ss2];

	PrepareFFTTables();
}

CFastFourierTransform::~CFastFourierTransform(void)
{
	if(xre != NULL)
		delete [] xre;

	if(xim != NULL)
		delete [] xim;

	if(mag != NULL)
		delete [] mag;

	if(fftSin != NULL)
		delete [] fftSin;

	if(fftCos != NULL)
		delete [] fftCos;

	if(fftBr != NULL)
		delete [] fftBr;

	xre = NULL;
	xim = NULL;
	mag = NULL;
	fftSin = NULL;
	fftCos = NULL;
	fftBr = NULL;
}

void CFastFourierTransform::PrepareFFTTables()
{
	int n2 = ss2;
	int nu1 = nu - 1;

	fftSin = new float[nu * n2];
	fftCos = new float[nu * n2];

	int k = 0;
	int x = 0;
	for (int l = 1; l <= nu; l++) {
		while (k < ss) {
			for (int i = 1; i <= n2; i++) {
				float p = (float)BitRev(k >> nu1, nu);
				float arg = (PI_2 * p) / (float) ss;
				fftSin[x] = (float) sin(arg);
				fftCos[x] = (float) cos(arg);
				k++;
				x++;
			}

			k += n2;
		}

		k = 0;
		nu1--;
		n2 >>= 1;
	}

	fftBr = new int[ss];
	for (k = 0; k < ss; k++)
		fftBr[k] = BitRev(k, nu);
}

int CFastFourierTransform::BitRev(int j, int nu) {
	int j1 = j;
	int k = 0;
	for (int i = 1; i <= nu; i++) {
		int j2 = j1 >> 1;
		k = ((k << 1) + j1) - (j2 << 1);
		j1 = j2;
	}

	return k;
}

float* CFastFourierTransform::Calculate(float* pSample, size_t pSampleSize) {
	int n2 = ss2;
	int nu1 = nu - 1;
	int wAps = pSampleSize / ss;
	size_t a = 0;

	for (size_t b = 0; a < pSampleSize; b++)
	{
		xre[b] = pSample[a];
		xim[b] = 0.0F;
		a += wAps;                  //?? 它这里为什么要跳着取数据？这里就导致了它比FFTW的输出少了1/wAps。而且见网上都是取该值为4...
	}

	// 以下在代码就完全不懂了...

	int x = 0;
	for (int l = 1; l <= nu; l++) 
	{
		for (int k = 0; k < ss; k += n2) 
		{
			for (int i = 1; i <= n2; i++) 
			{
				float c = fftCos[x];
				float s = fftSin[x];
				int kn2 = k + n2;
				float tr = xre[kn2] * c + xim[kn2] * s;
				float ti = xim[kn2] * c - xre[kn2] * s;
				xre[kn2] = xre[k] - tr;
				xim[kn2] = xim[k] - ti;
				xre[k] += tr;
				xim[k] += ti;
				k++;
				x++;
			}
		}

		nu1--;
		n2 >>= 1;
	}

	for (int k = 0; k < ss; k++) 
	{
		int r = fftBr[k];
		if (r > k) 
		{
			float tr = xre[k];
			float ti = xim[k];
			xre[k] = xre[r];
			xim[k] = xim[r];
			xre[r] = tr;
			xim[r] = ti;
		}
	}

	// 求出每个频率对应的振幅。先求模，然后第一个是 1/N，后面的都是 2/N
	mag[0] = (float) sqrt(xre[0] * xre[0] + xim[0] * xim[0]) / (float) ss;
	for (int i = 1; i < ss2; i++)
		mag[i] = (2.0F * (float) sqrt(xre[i] * xre[i] + xim[i] * xim[i])) / (float) ss;

	return mag;
}

//////////////////////////////////////////////////////////////////////////

CFastFourierTransformWrap::CFastFourierTransformWrap()
{
	m_pFFT = NULL;
	m_nAnalyserSampleCount = 0;
	m_nGetSamplePercent = 1;    // 这个值网上的代码取的是4...，我这里取1，和FFTW的输入输出保持一致
}
CFastFourierTransformWrap::~CFastFourierTransformWrap()
{
	this->Release();
}
bool CFastFourierTransformWrap::Init(int nSampleCount)
{
	this->Release();
	m_nAnalyserSampleCount = nSampleCount;
	m_pFFT = new CFastFourierTransform(nSampleCount/m_nGetSamplePercent); 

	return true;
}
bool CFastFourierTransformWrap::Release()
{
	SAFE_DELETE(m_pFFT);
	return true;
}
bool CFastFourierTransformWrap::DoFFT(float* pData)
{
	float* pFFTResult = m_pFFT->Calculate(pData, m_nAnalyserSampleCount); 
	return true;
}
bool CFastFourierTransformWrap::DoIFFT()
{
	// 没有提供。
	// 我也写不出这个算法来
	return false;
}
int CFastFourierTransformWrap::GetAmplitude(float** ppResult)
{
	*ppResult = (float*)m_pFFT->GetResult();
	return GetFFTOutputSize();
}
int CFastFourierTransformWrap::GetFFTOutputSize()
{
	return m_nAnalyserSampleCount/(2*m_nGetSamplePercent);
}
//////////////////////////////////////////////////////////////////////////

#ifdef FFTW
FFTWWrap::FFTWWrap()
{
	m_pfftw3_in = NULL;
	m_pfftw3_out = NULL;
	m_fftw_plan = NULL;
	m_pAmplitudes = NULL;

	m_nAnalyserSampleCount = 0;
}

FFTWWrap::~FFTWWrap()
{
	this->Release();
}
bool FFTWWrap::Release()
{
	if (m_fftw_plan)
	{
		fftw_destroy_plan(m_fftw_plan);
		m_fftw_plan = NULL;
	}
	if (m_pfftw3_in)
	{
		fftw_free(m_pfftw3_in);
		m_pfftw3_in = NULL;
	}
	if (m_pfftw3_out)
	{
		fftw_free(m_pfftw3_out);
		m_pfftw3_out = NULL;
	}
	SAFE_ARRAY_DELETE(m_pAmplitudes);

	return true;
}
bool FFTWWrap::Init(int nSampleCount)
{
	if (nSampleCount%2 != 0)
	{
		assert(0 && _T("FFTWImpl::Init nSampleCount%2 != 0"));
		return false;
	}
	
	this->Release();
	m_nAnalyserSampleCount = nSampleCount;
	int nOutputCount = GetFFTOutputSize();

	// 一个长度为N的信号可以分解成N/2 + 1个正余弦信息（16个信号=9个余弦+9个正弦）
	// 在离散信号处理中，运用复数形式来表示正余弦波是个常用的技术

	m_pfftw3_in = (double*)fftw_malloc(sizeof(double)*nSampleCount);
	m_pfftw3_out = fftw_alloc_complex(sizeof(fftw_complex)*nOutputCount);
	
	m_pAmplitudes = new float[nOutputCount];  // 返回给外部使用的振幅值

	// 创建一个 fft 计划
	m_fftw_plan = fftw_plan_dft_r2c_1d(nSampleCount, m_pfftw3_in, m_pfftw3_out, FFTW_ESTIMATE);

	// 创建一个 ifft 计划
	// fftw_plan_dft_c2r_1d
	return true;
}

bool FFTWWrap::DoFFT(float* pData)
{
	if (NULL == pData)
		return false;

	// 将float -> double
	for (int i = 0; i < m_nAnalyserSampleCount; i++)
 		m_pfftw3_in[i] = (double)pData[i];
 	

 	fftw_execute(m_fftw_plan);

	return false;
}

// 计算出每一个点的振幅值
// TODO: FFT 一共返回N/2 + 1个复数，只给外部N/2个结果行吗? 
int FFTWWrap::GetAmplitude(float** ppResult)
{
	if (NULL == m_pAmplitudes)
		return 0;

	int nCount = GetFFTOutputSize();
	for (int i = 0; i < nCount; i++)
	{
		double a = m_pfftw3_out[i][0];
		double b = m_pfftw3_out[i][1];
		double c = sqrt(a*a + b*b);

		if (i == 0 || i == nCount-1)
		{
			m_pAmplitudes[i] = (float)(c/m_nAnalyserSampleCount);
		}
		else
		{
			m_pAmplitudes[i] = (float)(c*2/m_nAnalyserSampleCount);
		}
	}
	*ppResult = m_pAmplitudes;
	return nCount;
}

bool FFTWWrap::DoIFFT()
{
	// fftw_plan_dft_c2r_1d + fftw_execute
	// 目前用不上，未实现
	return false;
}

int FFTWWrap::GetFFTOutputSize()
{
	return m_nAnalyserSampleCount/2 + 1;
}

#endif