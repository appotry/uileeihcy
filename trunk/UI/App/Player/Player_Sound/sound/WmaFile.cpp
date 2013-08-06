#include "stdafx.h"
#include "WmaFile.h"

CWmaFile::CWmaFile()
{
	m_pWMSyncReader = NULL;
	m_pNSSBuffer = NULL;
	m_nNSSBufferOffset = NULL;
	m_nDuration.QuadPart = 0;
	m_wStreamNum = 0;
	m_nSampleTime = 0;
}
CWmaFile::~CWmaFile()
{
	this->Release();
}
void CWmaFile::Release()
{
	SAFE_RELEASE(m_pNSSBuffer);
	m_nNSSBufferOffset = 0;
	SAFE_RELEASE(m_pWMSyncReader);
}

// 可参考MSDN: ms-help://MS.MSDNQTR.v90.en/wmform11/htm/toidentifyoutputnumbers.htm
//             ms-help://MS.MSDNQTR.v90.en/wmform11/htm/assigningoutputformats.htm
HRESULT CWmaFile::RenderFile(const TCHAR* szFile)
{
	if (NULL == m_pWMSyncReader)
	{
		HRESULT hr = WMCreateSyncReader(NULL, 0, &m_pWMSyncReader);
		if (FAILED(hr))
			return hr;
	}
	else
	{
		m_pWMSyncReader->Close();
	}

	if (NULL == m_pWMSyncReader)
		return E_FAIL;

	HRESULT hr = m_pWMSyncReader->Open(szFile);
	if (FAILED(hr))
		return hr;

	// 读取文件格式
	DWORD  dwOutputCount = 0;
	hr = m_pWMSyncReader->GetOutputCount(&dwOutputCount);
	if (FAILED(hr))
		return hr;
	
	for (DWORD i = 0; i < dwOutputCount; i++)
	{
		IWMOutputMediaProps* pMediaProps;
		hr = m_pWMSyncReader->GetOutputProps(i, &pMediaProps);
		if (FAILED(hr))
			continue;

		WM_MEDIA_TYPE* pMediaType = NULL;
		bool  bRet = false;
		do 
		{
			// 获取所需要的大小
			DWORD cbType = 0;
			hr = pMediaProps->GetMediaType(NULL, &cbType);
			if (FAILED(hr))
				break;

			pMediaType = (WM_MEDIA_TYPE*)malloc(cbType);
			hr = pMediaProps->GetMediaType(pMediaType, &cbType);
			if (FAILED(hr))
				break;

			if(IsEqualGUID(pMediaType->formattype, WMFORMAT_WaveFormatEx))
			{
				PWAVEFORMATEX pWaveFmtEx = (PWAVEFORMATEX)pMediaType->pbFormat;
				memcpy(&m_wfx, pWaveFmtEx, sizeof(m_wfx));

				IWMHeaderInfo* pWMHeaderInfo = NULL;
				hr = m_pWMSyncReader->QueryInterface(IID_IWMHeaderInfo, (void**)&pWMHeaderInfo);
				if (SUCCEEDED(hr))
				{
					WORD wStream = m_wStreamNum;
					WMT_ATTR_DATATYPE dataType;
					
					WORD  wLen = sizeof(LONGLONG);
					hr = pWMHeaderInfo->GetAttributeByName(&wStream, _T("Duration"), &dataType, (BYTE*)&m_nDuration.QuadPart, &wLen);
					pWMHeaderInfo->Release();
					pWMHeaderInfo = NULL;
				}

				m_pWMSyncReader->GetStreamNumberForOutput(i, &m_wStreamNum);
			}

			bRet = true;
		} 
		while (0);

			
		if (pMediaType)
		{
			free(pMediaType);
			pMediaType = NULL;
		}
		SAFE_RELEASE(pMediaProps);

		if (false == bRet)
		{
			continue;
		}
	}
	return S_OK;
}

//
// 注：WMSyncRead没有提供读取指定大小字节数据的方法
//     我们应该根据GetNextSample返回出来的NSSBuffer的大小来判断要读取多少次
//     当读取的数据比dwSizeToRead多的时候，我们要将这个数据保存下来m_pNSSBuffer
//     下次再读取的时候，先从保存的m_pNSSBuffer中读取
HRESULT CWmaFile::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	HRESULT hr = E_FAIL;
	*pdwSizeRead = 0;

	while(true)
	{
		//////////////////////////////////////////////////////////////////////////
		// 分析数据

		if (m_pNSSBuffer) // 上一次Read还有残留数据，或者是下面的代码:GetNextSample读取到的数据放在这里进行分析
		{
			BYTE* p = NULL;
			DWORD dwLength = 0;
			hr = m_pNSSBuffer->GetBufferAndLength(&p, &dwLength);
			if (FAILED(hr))
				return hr;

			int nRemain = dwLength - m_nNSSBufferOffset;  // 剩余数据大小
			int nNeed = dwSizeToRead - *pdwSizeRead;
			if (nRemain > nNeed)
			{
				memcpy (pBuffer+(*pdwSizeRead), p+m_nNSSBufferOffset, nNeed);
				m_nNSSBufferOffset += nNeed;
				*pdwSizeRead += nNeed;

				return S_OK;
			}
			else if (nRemain == nNeed)
			{
				memcpy(pBuffer+(*pdwSizeRead), p+m_nNSSBufferOffset, nNeed);
				*pdwSizeRead += nNeed;

				SAFE_RELEASE(m_pNSSBuffer);
				m_nNSSBufferOffset = 0;
				return S_OK;
			}
			else                  // 剩余的数据不够填充，需要继续读取下一个sample
			{
				memcpy(pBuffer+(*pdwSizeRead), p+m_nNSSBufferOffset, nRemain);
				*pdwSizeRead += nRemain;

				SAFE_RELEASE(m_pNSSBuffer);
				m_nNSSBufferOffset = 0;
			}

		}

		//////////////////////////////////////////////////////////////////////////
		// 读取数据 

		assert(NULL == m_pNSSBuffer);
		assert(0 == m_nNSSBufferOffset);

		
		QWORD duration = 0;
		DWORD dwFlag = 0;
		DWORD dwOutputNum = 0;
		WORD  wStreamNum = 0;

		HRESULT hr = m_pWMSyncReader->GetNextSample(m_wStreamNum, &m_pNSSBuffer, &m_nSampleTime, &duration, &dwFlag, &dwOutputNum, &wStreamNum);
		if (FAILED(hr))
		{
			if (*pdwSizeRead > 0)
				return S_OK;    // 读取完了最后一个Sample，但仍然需要将读取出来的其它数据提交到directsound中
			else
				return hr;      // 真的是读完了
		}

		//////////////////////////////////////////////////////////////////////////
		// 进入下一次循环进行分析
	}
	return hr;
}

// SetRangeByFrame不会用，总是报错。因此调用SetRange来设置。
HRESULT CWmaFile::SetCurPos(double percent)
{
	if (NULL == m_pWMSyncReader)
		return E_FAIL;

	HRESULT hr = E_FAIL;
// 	if (0 == percent)
// 	{
// 		hr = m_pWMSyncReader->SetRangeByFrame(m_wStreamNum, 1, 0);
// 		m_nSampleTime = 0;
// 	}
// 	else
	{
		QWORD qw = (QWORD)(m_nDuration.QuadPart*percent);
		hr = m_pWMSyncReader->SetRange(qw, 0);
		m_nSampleTime = qw;
	}
	return hr;
}
HRESULT CWmaFile::GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent)
{
	if (NULL == m_pWMSyncReader || 0 == m_nDuration.QuadPart)
		return E_FAIL;

	double dDiff = nWriteBufferSize*1.0/m_wfx.nAvgBytesPerSec;  // 已经加入缓存区的数据

	*pdSeconds = (double)(m_nSampleTime/10000000.0) - dDiff;
	*pdPercent = (double)(m_nSampleTime*1.0/m_nDuration.QuadPart);

	return S_OK;
}