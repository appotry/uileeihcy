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
	virtual  HRESULT   GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent);
protected:
	mpg123_handle*     m_hMpg123;   // mp3解码器
	bool     m_bFileOpened;    // 为了解决文件不存在时，mpg123_open返回-1，此时再
	                           // 调用mpg123_close会报assert错误的问题
};
