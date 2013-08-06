#include "StdAfx.h"
#include "Mp3File.h"


CMp3File::CMp3File()
{
	m_hMpg123 = NULL;
	m_bFileOpened = false;
}
CMp3File::~CMp3File()
{
	this->Release();
}

void CMp3File::Release()
{
	if (m_hMpg123)
	{
		if (m_bFileOpened)
		{
			mpg123_tclose(m_hMpg123);
			m_bFileOpened = false;
		}
		mpg123_delete(m_hMpg123);
		m_hMpg123 = NULL;

		mpg123_exit();
	}
}
HRESULT CMp3File::RenderFile(const TCHAR* szFileName)
{	
	int ret = -1;
	bool bRet = false;
	do 
	{
		if (NULL == m_hMpg123)
		{
			if (MPG123_OK != mpg123_init())
				return E_FAIL;

			m_hMpg123 = mpg123_new(NULL, &ret);
			if (NULL == m_hMpg123 || MPG123_OK != ret)
			{
				mpg123_exit();
				return E_FAIL;
			}
		}
		else
		{
			if (m_bFileOpened)
			{
				mpg123_tclose(m_hMpg123);  // 关闭之前的文件
				m_bFileOpened = false;
			}
		}


		int nRet = mpg123_topen(m_hMpg123, szFileName);
		if (MPG123_OK != nRet)
		{
			return E_FAIL;
		}
		m_bFileOpened = true;

		long lRate = 0;
		int  nChannel = 0;
		int  nEncoding = 0;

		if (MPG123_OK != mpg123_getformat(m_hMpg123, &lRate, &nChannel, &nEncoding))
			break;

		if (MPG123_ENC_16 == (nEncoding & MPG123_ENC_16))
			nEncoding = 16;
		else if (MPG123_ENC_32 == (nEncoding & MPG123_ENC_32))
			nEncoding = 32;
		else
			nEncoding = 8;

		memset(&m_wfx, 0, sizeof(WAVEFORMATEX));
		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = nChannel;
		m_wfx.nSamplesPerSec = lRate;
		m_wfx.wBitsPerSample = nEncoding;
		m_wfx.nBlockAlign = nEncoding / 8 * nChannel;
		m_wfx.nAvgBytesPerSec = lRate * (nEncoding / 8) * nChannel;

		bRet = true;
	} while (0);

	if (false == bRet)
	{
		this->Release();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMp3File::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	int ret = mpg123_read(m_hMpg123, pBuffer, dwSizeToRead, (size_t*)pdwSizeRead);
	if (MPG123_OK != ret)
		return E_FAIL;

	return S_OK;
}

HRESULT CMp3File::SetCurPos(double percent)
{
	if (NULL == m_hMpg123)
		return E_FAIL;

	off_t length = mpg123_length(m_hMpg123);

	if (mpg123_seek(m_hMpg123, (off_t)(length*percent), SEEK_SET) < 0)
		return E_FAIL;	

	return S_OK;
}

HRESULT CMp3File::GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent)
{
	// 因为这里的mpg123_tell读取出来的计数是sample计数的，不是bytes计数，因此将nPlayBufferSize转化一下
	nWriteBufferSize = nWriteBufferSize/(m_wfx.nChannels*m_wfx.wBitsPerSample>>3);  

	if (NULL == m_hMpg123)
		return E_FAIL;
	if (NULL == pdSeconds || NULL == pdPercent)
		return E_INVALIDARG;

	off_t length = mpg123_length(m_hMpg123);
	off_t cur = mpg123_tell(m_hMpg123) - nWriteBufferSize;  // 减去已经加入缓存区的数据

	if (0 == length)
		return E_FAIL;

	assert(cur >= 0);

	*pdPercent = cur * 1.0 / length;

	off_t curFrame = mpg123_tellframe(m_hMpg123);
	if (curFrame < 0)
		return E_FAIL;

	int   spf = mpg123_spf(m_hMpg123);
	int   nPlayingFrame = curFrame;
	if (nWriteBufferSize > 0)
	{
		nPlayingFrame = (int)((double)curFrame - spf*1.0/nWriteBufferSize);
	}
	
	assert(nPlayingFrame >= 0);

	double dFrameTime = mpg123_tpf(m_hMpg123);
	*pdSeconds = dFrameTime * nPlayingFrame;
	return S_OK;
}

HRESULT CMp3File::SetEq(int nBand, double nValue)
{
	mpg123_eq(m_hMpg123, MPG123_LR, nBand, nValue);
	return S_OK;
}