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

	virtual  HRESULT   SetEq(int nBand, double nValue);
		
protected:
	mpg123_handle*     m_hMpg123;   // mp3������
	bool     m_bFileOpened;    // Ϊ�˽���ļ�������ʱ��mpg123_open����-1����ʱ��
	                           // ����mpg123_close�ᱨassert���������
};
