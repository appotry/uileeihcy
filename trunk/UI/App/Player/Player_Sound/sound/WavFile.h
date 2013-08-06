#pragma once
#include "DirectSoundEngine.h"

// �����еĴ���ܶ�copy DXSDK�е�SDKwavefile.cpp

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

	MMCKINFO m_ck;          // Multimedia RIFF chunk��  wave���ݿ�
	MMCKINFO m_ckRiff;      // Use in opening a WAVE file�� waveͷ
	DWORD    m_dwSize;      // The size of the wave file, wave���ݿ��С�����������ļ��Ĵ�С
};
