#pragma once
#include "DirectSoundEngine.h"

// 该类中的代码很多copy DXSDK中的SDKwavefile.cpp

class CWavFile : public ISoundFile
{
public:
	CWavFile();
	~CWavFile();

	void     Release();

	virtual  HRESULT   RenderFile(const TCHAR* szFile);
	virtual  HRESULT   Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);

	virtual  HRESULT   SetCurPos(double percent);
	virtual  HRESULT   GetCurPos(int nWriteBufferSize, double* pdSeconds, double* pdPercent) ;

protected:
	HRESULT  Close();
	HRESULT  ReadMMIO();
	HRESULT  ResetFile();
	DWORD    GetSize();

protected:
	HMMIO    m_hMmio;
	CHAR*    m_pResourceBuffer;

	MMCKINFO m_ck;          // Multimedia RIFF chunk，  wave数据块
	MMCKINFO m_ckRiff;      // Use in opening a WAVE file， wave头
	DWORD    m_dwSize;      // The size of the wave file, wave数据块大小，不是整个文件的大小
};
