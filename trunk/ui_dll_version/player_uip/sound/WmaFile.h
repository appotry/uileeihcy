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
};
