#include "StdAfx.h"
#include "MP3.h"
#include "player_uip.h"
#include "sound/DirectShowEngine.h"
#include "sound/DirectSoundEngine.h"

CMP3::CMP3(void)
{
	m_pCurrentEngine = NULL;
	m_pDirectShowEngine = NULL;
	m_pDirectSoundEngine = NULL;
}

CMP3::~CMP3(void)
{
	this->Release();
}

bool CMP3::Init(HWND hMainWnd)
{
	m_pDirectShowEngine = new CDirectShowEngine();
	m_pDirectSoundEngine= new CDirectSoundEngine();

	m_pDirectShowEngine->Init(this, hMainWnd);
	m_pDirectSoundEngine->Init(this, hMainWnd);

	return true;
}


bool CMP3::Release()
{
	if (NULL != m_pDirectShowEngine)
	{
		m_pDirectShowEngine->Release();
		SAFE_DELETE(m_pDirectShowEngine);
	}
	if (NULL != m_pDirectSoundEngine)
	{
		m_pDirectSoundEngine->Release();
		SAFE_DELETE(m_pDirectSoundEngine);
	}
	return true;
}

bool CMP3::RenderFile( const String& strFile )
{
	m_pCurrentEngine = NULL;

	// 简单判断后缀名来决定采用哪个engine来播放
	size_t nPos = strFile.find_last_of(_T('.'));
	if (String::npos == nPos)
		return false;

	nPos++;
	String strExt = strFile.substr(nPos,strFile.length()-nPos);
	if (0 == _tcsicmp(strExt.c_str(), _T("mp3")))
	{
		if (SUCCEEDED(m_pDirectSoundEngine->RenderFile(strFile.c_str(), strExt.c_str())))
		{
			m_pCurrentEngine = m_pDirectShowEngine;
		}
	}
	else if (0 == _tcsicmp(strExt.c_str(), _T("wma")))
	{

	}
	else if (0 == _tcsicmp(strExt.c_str(), _T("wav")))
	{

	}

	if (NULL == m_pCurrentEngine)
	{
		m_pDirectShowEngine->RenderFile(strFile.c_str(), strExt.c_str());
		m_pCurrentEngine = m_pDirectShowEngine;
	}
	
	return true;
}

bool CMP3::Play()
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->Play()))
		return true;
	else
		return false;
}
bool CMP3::Pause()
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->Pause()))
		return true;
	else
		return false;
}
bool CMP3::Stop()
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->Stop()))
		return true;
	else
		return false;
}

bool CMP3::SetCurPos(double percent)
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->SetCurPos(percent)))
		return true;
	else
		return false;
}

bool CMP3::SetVolume(double percent)
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->SetVolume(percent)))
		return true;
	else
		return false;
}

bool CMP3::Mute(bool bMute)
{
	if (NULL == m_pCurrentEngine)
		return false;

	if (SUCCEEDED(m_pCurrentEngine->Mute(bMute)))
		return true;
	else
		return false;
}

bool CMP3::AddEventCallback(IMp3EventCallback* p)
{
	if( NULL == p )
		return false;

	// TODO: 检测是否存在+删除
	m_listEventCallback.push_back(p);

	return true;
}

void CMP3::Fire_on_mp3_volume_ind(long lVolumn)
{
	list<IMp3EventCallback*>::iterator iter = this->m_listEventCallback.begin();
	list<IMp3EventCallback*>::iterator iterEnd = this->m_listEventCallback.end();

	for(; iter != iterEnd; iter++)
	{
		(*iter)->on_mp3_volume_ind(lVolumn);
	}
}

void CMP3::Fire_on_mp3_stop()
{
	list<IMp3EventCallback*>::iterator iter = m_listEventCallback.begin();
	list<IMp3EventCallback*>::iterator iterEnd = m_listEventCallback.end();

	for(; iter != iterEnd; iter++)
	{
		(*iter)->on_mp3_stop();
	}
}

void CMP3::Fire_on_mp3_progress_ind(LONGLONG llCur, LONGLONG llDuration)
{
	list<IMp3EventCallback*>::iterator iter = m_listEventCallback.begin();
	list<IMp3EventCallback*>::iterator iterEnd =m_listEventCallback.end();

	for(; iter != iterEnd; iter++)
	{
		(*iter)->on_mp3_progress_ind(llCur,llDuration);
	}
}