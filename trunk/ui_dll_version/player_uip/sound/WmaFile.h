#pragma once
#include "DirectSoundEngine.h"
#include "wmsdk.h"
#pragma  comment(lib, "wmvcore.lib")


class CWmaFile : public ISoundFile
{
public:
	CWmaFile();
	~CWmaFile();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent);
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent);

protected:

	IWMSyncReader*     m_pWMSyncReader;
	INSSBuffer*        m_pNSSBuffer;       // 每次GetNextSample读取的数据，有可能外部还没有取完，等待下次再取
	int                m_nNSSBufferOffset; // 标志m_pNSSBuffer中读取了多少数据，还有多少数据未读完
	LARGE_INTEGER      m_nDuration;        // 总的时间
	QWORD              m_nSampleTime;      // 当前已读取的数据时间 100ns单位
	WORD               m_wStreamNum;       // ???
};
