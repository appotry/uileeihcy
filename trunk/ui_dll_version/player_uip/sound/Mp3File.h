#pragma once
#include "DirectSoundEngine.h"


class CMp3File : public ISoundFile
{
public:
	CMp3File();
	~CMp3File();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent);
	virtual  HRESULT   GetCurPos(double* pdSeconds, double* pdPercent);
protected:
	mpg123_handle*     m_hMpg123;   // mp3½âÂëÆ÷
};
