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
	virtual  HRESULT   GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent);

protected:

	IWMSyncReader*     m_pWMSyncReader;
	INSSBuffer*        m_pNSSBuffer;       // ÿ��GetNextSample��ȡ�����ݣ��п����ⲿ��û��ȡ�꣬�ȴ��´���ȡ
	int                m_nNSSBufferOffset; // ��־m_pNSSBuffer�ж�ȡ�˶������ݣ����ж�������δ����
	LARGE_INTEGER      m_nDuration;        // �ܵ�ʱ��
	QWORD              m_nSampleTime;      // ��ǰ�Ѷ�ȡ������ʱ�� 100ns��λ
	WORD               m_wStreamNum;       // ???
};
