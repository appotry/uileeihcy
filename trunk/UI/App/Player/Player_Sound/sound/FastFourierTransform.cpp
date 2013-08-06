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
		a += wAps;                  //?? ������ΪʲôҪ����ȡ���ݣ�����͵���������FFTW���������1/wAps�����Ҽ����϶���ȡ��ֵΪ4...
	}

	// �����ڴ������ȫ������...

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

	// ���ÿ��Ƶ�ʶ�Ӧ�����������ģ��Ȼ���һ���� 1/N������Ķ��� 2/N
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
	m_nGetSamplePercent = 1;    // ���ֵ���ϵĴ���ȡ����4...��������ȡ1����FFTW�������������һ��
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
	// û���ṩ��
	// ��Ҳд��������㷨��
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

	// һ������ΪN���źſ��Էֽ��N/2 + 1����������Ϣ��16���ź�=9������+9�����ң�
	// ����ɢ�źŴ����У����ø�����ʽ����ʾ�����Ҳ��Ǹ����õļ���

	m_pfftw3_in = (double*)fftw_malloc(sizeof(double)*nSampleCount);
	m_pfftw3_out = fftw_alloc_complex(sizeof(fftw_complex)*nOutputCount);
	
	m_pAmplitudes = new float[nOutputCount];  // ���ظ��ⲿʹ�õ����ֵ

	// ����һ�� fft �ƻ�
	m_fftw_plan = fftw_plan_dft_r2c_1d(nSampleCount, m_pfftw3_in, m_pfftw3_out, FFTW_ESTIMATE);

	// ����һ�� ifft �ƻ�
	// fftw_plan_dft_c2r_1d
	return true;
}

bool FFTWWrap::DoFFT(float* pData)
{
	if (NULL == pData)
		return false;

	// ��float -> double
	for (int i = 0; i < m_nAnalyserSampleCount; i++)
 		m_pfftw3_in[i] = (double)pData[i];
 	

 	fftw_execute(m_fftw_plan);

	return false;
}

// �����ÿһ��������ֵ
// TODO: FFT һ������N/2 + 1��������ֻ���ⲿN/2���������? 
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
	// Ŀǰ�ò��ϣ�δʵ��
	return false;
}

int FFTWWrap::GetFFTOutputSize()
{
	return m_nAnalyserSampleCount/2 + 1;
}

#endif