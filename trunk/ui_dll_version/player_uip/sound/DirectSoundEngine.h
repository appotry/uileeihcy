#pragma once
#include "..\\MP3.h"

#include <MMSystem.h>
#include <MMReg.h>
#include <dsound.h>
#include <DxErr.h>
#include "mpg123.h"

#ifdef _DEBUG
#pragma comment(lib, "libmpg123_d.lib")
#else
#pragma comment(lib, "libmpg123.lib")
#endif

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dxguid.lib")


class ISoundFile
{
public:
	virtual  HRESULT   RenderFile(const TCHAR* szFile) = 0;

protected:
	WAVEFORMATEX   m_wfx;
};
class CWaveFile : public ISoundFile
{
public:
};
class CMp3File : public ISoundFile
{
public:
	virtual  HRESULT   RenderFile(const TCHAR* szFile);

protected:
	mpg123_handle*     m_hMpg123;   // mp3½âÂëÆ÷
};

class CDirectSoundEngine : public ISoundEngine
{
public:
	CDirectSoundEngine(void);
	virtual ~CDirectSoundEngine(void);

	virtual HRESULT  Init(CMP3* pMgr, HWND hMainWnd);
	virtual HRESULT  Release();

	virtual HRESULT  RenderFile( const TCHAR* szFile, const TCHAR* szExt );
	virtual HRESULT  Play();
	virtual HRESULT  Pause();
	virtual HRESULT  Stop();
	virtual HRESULT  SetCurPos(double);
	virtual HRESULT  SetVolume(double);
	virtual HRESULT  Mute(bool);

protected:
	IDirectSound8*       m_pDirectSound8;
	IDirectSoundBuffer*  m_pDirectSoundBuffer;

	ISoundFile*    m_pSoundFile;

	CMP3*   m_pMgr;
};
