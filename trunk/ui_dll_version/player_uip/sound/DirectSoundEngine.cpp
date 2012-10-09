#include "StdAfx.h"
#include "DirectSoundEngine.h"


HRESULT CMp3File::RenderFile(const TCHAR* szFileName)
{	
	if(mpg123_init() != MPG123_OK)
		return -1;
	int ret = -1;
	m_hMpg123 = mpg123_new(NULL, &ret);
	if(m_hMpg123 == NULL || ret != MPG123_OK)
		return -1;

	if(mpg123_topen(m_hMpg123, szFileName) != MPG123_OK)
		return -1;


	long rate = 0;
	int channel = 0;
	int encoding = 0;

	if(mpg123_getformat(m_hMpg123, &rate, &channel, &encoding) != MPG123_OK)
		return -1;
	if((encoding & MPG123_ENC_16) == MPG123_ENC_16)
		encoding = 16;
	else if((encoding & MPG123_ENC_32) == MPG123_ENC_32)
		encoding = 32;
	else
		encoding = 8;

	memset(&m_wfx, 0, sizeof(WAVEFORMATEX));
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nChannels = channel;
	m_wfx.nSamplesPerSec = rate;
	m_wfx.wBitsPerSample = encoding;
	m_wfx.nBlockAlign = encoding / 8 * channel;
	m_wfx.nAvgBytesPerSec = rate * (encoding / 8) * channel;

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

CDirectSoundEngine::CDirectSoundEngine(void)
{
	m_pMgr = NULL;
	m_pDirectSound8 = NULL;
	m_pDirectSoundBuffer = NULL;
}

CDirectSoundEngine::~CDirectSoundEngine(void)
{
	this->Release();
}


HRESULT CDirectSoundEngine::Init(CMP3* pMgr, HWND hMainWnd)
{
	m_pMgr = pMgr;
	
	HRESULT hr = ::DirectSoundCreate8(NULL, &m_pDirectSound8, NULL);
	if (NULL == m_pDirectSound8)
		return hr;

	hr = m_pDirectSound8->SetCooperativeLevel(hMainWnd, DSSCL_PRIORITY);  // 这个窗口句柄不能设置为NULL，否则会返回无效参数错误
	if (FAILED(hr))
		return hr;

	return S_OK;
}
HRESULT CDirectSoundEngine::Release()
{
	SAFE_RELEASE(m_pDirectSoundBuffer);
	SAFE_RELEASE(m_pDirectSound8);
	return S_OK;
}

HRESULT CDirectSoundEngine::RenderFile( const TCHAR* szFile, const TCHAR* szExt )
{
	if (NULL == szFile)
		return E_INVALIDARG;

	if (NULL == m_pDirectSound8)
		return E_FAIL;

	if (0 == _tcsicmp(szExt, _T("wav")))
	{

	}
	else if (0 == _tcsicmp(szExt, _T("mp3")))
	{

	}
	

	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Play()
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Pause()
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Stop()
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::SetCurPos(double)
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::SetVolume(double)
{
	return E_NOTIMPL;
}
HRESULT CDirectSoundEngine::Mute(bool)
{
	return E_NOTIMPL;
}