#include "stdafx.h"
#include "WmaFile.h"

CWmaFile::CWmaFile()
{
	m_pWMSyncReader = NULL;
}
CWmaFile::~CWmaFile()
{

}
void CWmaFile::Release()
{

}

HRESULT CWmaFile::RenderFile(const TCHAR* szFile)
{
	if (NULL == m_pWMSyncReader)
	{
		HRESULT hr = WMCreateSyncReader(NULL, 0, &m_pWMSyncReader);
		if (FAILED(hr))
			return hr;
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
// 				BYTE* pValue = new BYTE[5];
// 				strcpy((char*)pValue,"TRUE");
// 				hr = m_pWMSyncReader->SetOutputSetting(m_iVideoOutputNumber,g_wszVideoSampleDurations,WMT_TYPE_BOOL,pValue,sizeof(pValue));
// 				delete []pValue;

				PWAVEFORMATEX pWaveFmtEx = (PWAVEFORMATEX)pMediaType->pbFormat;
				memcpy(&m_wfx, pWaveFmtEx, sizeof(m_wfx));
			}

			bRet = true;
		} 
		while (0);
		
		if (NULL != pMediaType)
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

// 	memset(&m_wfx, 0, sizeof(WAVEFORMATEX));
// 	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
// 	m_wfx.nChannels = nChannel;
// 	m_wfx.nSamplesPerSec = lRate;
// 	m_wfx.wBitsPerSample = nEncoding;
// 	m_wfx.nBlockAlign = nEncoding / 8 * nChannel;
// 	m_wfx.nAvgBytesPerSec = lRate * (nEncoding / 8) * nChannel;

	return S_OK;
}
HRESULT CWmaFile::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	INSSBuffer* pNSSBuffer = NULL;
	
	DWORD dwOffset = 0;
	for (int i = 0; i < 3; i++)
	{
		QWORD sampletime = 0;
		QWORD duration = 0;
		DWORD dwFlag = 0;
		DWORD dwOutputNum = 0;
		WORD  wStreamNum = 0;

		HRESULT hr = m_pWMSyncReader->GetNextSample((WORD)0/*dwSizeToRead*/, &pNSSBuffer, &sampletime, &duration, &dwFlag, &dwOutputNum, &wStreamNum);
		if (FAILED(hr))
			return hr;

 		pNSSBuffer->GetLength(pdwSizeRead);
		BYTE* p = pBuffer + dwOffset;
 		pNSSBuffer->GetBuffer(&p);

		dwOffset += *pdwSizeRead;

		if(dwOffset >= dwSizeToRead)
			return S_OK;

	}
	return S_OK;
}

HRESULT CWmaFile::SetCurPos(double percent)
{
	return E_NOTIMPL;
}
HRESULT CWmaFile::GetCurPos(double* pdSeconds, double* pdPercent)
{
	return E_NOTIMPL;
}