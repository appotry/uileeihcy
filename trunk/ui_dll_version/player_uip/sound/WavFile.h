#pragma once
#include "DirectSoundEngine.h"

class CWavFile : public ISoundFile
{
public:
	CWavFile();
	~CWavFile();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent) {return E_NOTIMPL;}
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent) {return E_NOTIMPL;}
};
