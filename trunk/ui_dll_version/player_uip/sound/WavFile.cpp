#include "StdAfx.h"
#include "WavFile.h"


CWavFile::CWavFile()
{

}
CWavFile::~CWavFile()
{
	this->Release();
}
void CWavFile::Release()
{

}

HRESULT CWavFile::RenderFile(const TCHAR* szFile)
{
	return E_NOTIMPL;
}
HRESULT CWavFile::Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead)
{
	return E_NOTIMPL;
}