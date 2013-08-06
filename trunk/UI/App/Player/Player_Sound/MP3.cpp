#include "StdAfx.h"
#include "MP3.h"
#include "player_sound.h"
#include "sound/DirectShowEngine.h"
#include "sound/DirectSoundEngine.h"
#include "tagparse/mp3tag.h"
#include "tagparse/wmatag.h"

//////////////////////////////////////////////////////////////////////////
// 通过文件名的后缀判断该文件类型
eMp3Type  GetFileTypeByFileExt(const TCHAR* szFilePath)
{
	if (NULL == szFilePath)
		return UNK;

	String  strFile = szFilePath;

	// 简单判断后缀名来决定采用哪个engine来播放
	size_t nPos = strFile.find_last_of(_T('.'));
	if (String::npos == nPos)
		return UNK;

	nPos++;
	String strExt = strFile.substr(nPos,strFile.length()-nPos);

	if (0 == _tcsicmp(strExt.c_str(), _T("mp3")))
		return MP3;
	if (0 == _tcsicmp(strExt.c_str(), _T("wma")))
		return WMA;
	if (0 == _tcsicmp(strExt.c_str(), _T("wav")))
		return WAV;

	return UNK;
}

// 通过读取文件内容的前若干字节判断文件类型
// 注：mp3类型文件没有固定的标识，目前只简单的判断后缀获取
eMp3Type  GetFileTypeByFilePrefix(const TCHAR* szFilePath)
{
	if (NULL == szFilePath)
		return UNK;

	fstream  f;
	f.open(szFilePath, ios_base::in|ios_base::binary);
	if (f.fail())
		return UNK;

	const byte wma_file_head[16] = {0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C};
	const byte wav_file_head[4] = {'R', 'I', 'F', 'F'};

	byte b[16] = {0};
	f.read((char*)b, 16);
	f.close();

	if (0 == memcmp(b, wma_file_head, 16))
		return WMA;
	if (0 == memcmp(b, wav_file_head, 4))
		return WAV;

	return GetFileTypeByFileExt(szFilePath);
}

//////////////////////////////////////////////////////////////////////////

CMP3::CMP3(void)
{
	m_pCurrentEngine = NULL;
	m_pDirectShowEngine = NULL;
	m_pDirectSoundEngine = NULL;
	m_nVolumn = DSBVOLUME_MAX;
	m_bMute = false;
	m_hMainWnd = NULL;
	m_lPan = DSBPAN_CENTER;
	m_bEqEnable = false;
	memset(m_arrEqValue, 0, sizeof(int)*EQ_FREQ_COUNT);
	
}

CMP3::~CMP3(void)
{
	this->Release();
}

bool CMP3::Init(HWND hMainWnd)
{
	if (NULL == m_WndEvent.Create(HWND_MESSAGE))
		return false;

	m_hMainWnd = hMainWnd;
	m_pDirectShowEngine = new CDirectShowEngine();
	m_pDirectSoundEngine= new CDirectSoundEngine();

	HRESULT hr = m_pDirectShowEngine->Init(this, &m_WndEvent);
	assert(SUCCEEDED(hr));
	hr = m_pDirectSoundEngine->Init(this, &m_WndEvent);
	assert(SUCCEEDED(hr));

	m_WndEvent.SetEnginePtr(this, (CDirectShowEngine*)m_pDirectShowEngine, (CDirectSoundEngine*)m_pDirectSoundEngine);

	m_SA.InitDefault(m_hMainWnd);
	return true;
}


bool CMP3::Release()
{
	m_SA.Release();

	if (m_pDirectShowEngine)
	{
		m_pDirectShowEngine->Release();
		SAFE_DELETE(m_pDirectShowEngine);
	}
	if (m_pDirectSoundEngine)
	{
		m_pDirectSoundEngine->Release();
		SAFE_DELETE(m_pDirectSoundEngine);
	}
	if( m_WndEvent.IsWindow() )
	{
		m_WndEvent.DestroyWindow();
	}
	return true;
}

bool CMP3::RenderFile(const TCHAR* szPath)
{
	if (NULL == szPath)
		return false;

	String strFile = szPath;
	m_pCurrentEngine = NULL;
	if (m_pDirectShowEngine)
	{
		m_pDirectShowEngine->ClearRender();
	}
	if (m_pDirectSoundEngine)
	{
		m_pDirectSoundEngine->ClearRender();
	}

	// 简单判断后缀名来决定采用哪个engine来播放
	eMp3Type eType = GetFileTypeByFilePrefix(szPath);
	if (eType == MP3 ||
		eType == WMA ||
		eType == WAV )
	{
		if (SUCCEEDED(m_pDirectSoundEngine->RenderFile(strFile.c_str(), eType)))
		{
			m_pCurrentEngine = m_pDirectSoundEngine;
		}
	}

	if (NULL == m_pCurrentEngine)
	{
		if (FAILED(m_pDirectShowEngine->RenderFile(strFile.c_str(), eType)))
			return false;

		m_pCurrentEngine = m_pDirectShowEngine;
	}

	if (m_pCurrentEngine)
	{
		m_pCurrentEngine->SetVolume(m_bMute?DSBVOLUME_MIN:m_nVolumn);
		m_pCurrentEngine->SetPan(m_lPan);
		m_pCurrentEngine->EnableEq(m_bEqEnable);
		for (int i = 0; i < EQ_FREQ_COUNT; i++)
		{
			m_pCurrentEngine->SetEq((E_EQ_FREQ)i, m_arrEqValue[i]);
		}
	}
	m_SA.SetSoundEngine(m_pCurrentEngine);

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

bool  CMP3::GetCurPos(double* pdSecond, double* pdPercent)
{
    if (NULL == m_pCurrentEngine)
        return false;

    if (SUCCEEDED(m_pCurrentEngine->GetCurPos(pdSecond, pdPercent)))
        return true;
    else
        return false;
}

int g_volumes[] = 
{
	-10000,-6418, -6147, -6000, -5892,
	-4826, -4647, -4540, -4477, -4162,
	-3876, -3614, -3500, -3492, -3374,
	-3261, -3100, -3153, -3048, -2947,
	-2849, -2755, -2700, -2663, -2575, 
	-2520, -2489, -2406, -2325, -2280, 
	-2246, -2170, -2095, -2050, -2023, 
	-1952, -1900, -1884, -1834, -1820, 
	-1800, -1780, -1757, -1695, -1636, 
	-1579, -1521, -1500, -1464, -1436, 
	-1420, -1408, -1353, -1299, -1246, 
	-1195, -1144, -1096, -1060, -1049, 
	-1020, -1003, -957,  -912,  -868, 
	-800,  -774,  -784,  -760,  -744,  
	-705,  -667,  -630,  -610,  -594,  
	-570,  -558,  -525,  -493,  -462,  
	-432,  -403,  -375,  -348,  -322,  
	-297,  -285,  -273,  -250,  -228,  
	-207,  -187,  -176,  -168,  -150,  
	-102,  -75,  -19,    -10,   0, 0
};


bool CMP3::SetVolume(long lPercent)
{
	if( lPercent < 0 )
		lPercent = 0;
	if( lPercent >= sizeof(g_volumes)/sizeof(int) )
		lPercent = sizeof(g_volumes)/sizeof(int) -1;

	m_nVolumn = (long)( g_volumes[(int)lPercent] );

	if (m_bMute)
		return true;

	if (NULL == m_pCurrentEngine)
		return true;
	
	if (SUCCEEDED(m_pCurrentEngine->SetVolume(m_nVolumn)))
		return true;
	else
		return false;
}

bool CMP3::Mute(bool bMute)
{
	m_bMute = bMute;
	if (NULL == m_pCurrentEngine)
		return true;

	HRESULT hr = E_FAIL;
	if (bMute)
		 hr = m_pCurrentEngine->SetVolume(DSBVOLUME_MIN);
	else
		hr = m_pCurrentEngine->SetVolume(m_nVolumn);

	if (SUCCEEDED(hr))
		return true;
	else
		return false;
}

// 为了均匀切换声道，必须再从数组中取对应的值，而不是线性的
bool CMP3::SetPan(long lPanPercent)
{
	if (lPanPercent > 0)
	{
		int nIndex = 100 - lPanPercent;
		if (nIndex>100 || nIndex<0)
			return false;

 		m_lPan = (long)( g_volumes[nIndex] );
 		m_lPan *= -1;
	}
	else if (lPanPercent < 0)
	{
		int nIndex = 100 + lPanPercent;
		if (nIndex>100 || nIndex<0)
			return false;

		m_lPan = (long)( g_volumes[nIndex] );
	}
	else
		m_lPan = 0;

	if (NULL == m_pCurrentEngine)
		return true;

	HRESULT hr = m_pCurrentEngine->SetPan(m_lPan);

	if (SUCCEEDED(hr))
		return true;
	else
		return false;
}

bool CMP3::SetVisualization(VisualizationInfo* pInfo)
{
	return m_SA.SetVisualization(pInfo);
}

bool CMP3::SetEq(E_EQ_FREQ eFreq, int nValue)
{
	if (eFreq > EQ_FREQ_COUNT || eFreq < 0)
		return false;

	m_arrEqValue[eFreq]	= nValue;

	if (NULL == m_pCurrentEngine)
		return true;

	HRESULT hr = m_pCurrentEngine->SetEq(eFreq, nValue);

	if (SUCCEEDED(hr))
		return true;
	else
		return false;
}

bool CMP3::EnableEq(bool bEnable)
{
	m_bEqEnable = bEnable;

	if (NULL == m_pCurrentEngine)
		return true;

	m_pCurrentEngine->EnableEq(m_bEqEnable);
	return true;
}
bool CMP3::ReSetEq()
{
	for (int i = 0; i < EQ_FREQ_COUNT; i++)
	{
		m_arrEqValue[i] = 0;
	}
	if (NULL == m_pCurrentEngine)
		return true;

	for (int i = 0; i < EQ_FREQ_COUNT; i++)
	{
		m_pCurrentEngine->SetEq((E_EQ_FREQ)i, m_arrEqValue[i]);
	}

	return true;

}

bool CMP3::AddEventCallback(IMp3EventCallback* p)
{
	if( NULL == p )
		return false;

	// TODO: 检测是否存在+删除
	m_listEventCallback.push_back(p);

	return true;
}


void CMP3::Fire_on_mp3_finish()
{
	list<IMp3EventCallback*>::iterator iter = m_listEventCallback.begin();
	list<IMp3EventCallback*>::iterator iterEnd = m_listEventCallback.end();

	for(; iter != iterEnd; iter++)
	{
		(*iter)->on_mp3_finish();
	}
}

// void CMP3::Fire_on_mp3_progress_ind(double dSeconds, double dPercent)
// {
// 	list<IMp3EventCallback*>::iterator iter = m_listEventCallback.begin();
// 	list<IMp3EventCallback*>::iterator iterEnd =m_listEventCallback.end();
// 
// 	for(; iter != iterEnd; iter++)
// 	{
// 		(*iter)->on_mp3_progress_ind(dSeconds, dPercent);
// 	}
// }

bool CMP3::GetTag(const TCHAR* szPath, ITagParseCallback* pCallback)
{
	if (NULL == pCallback)
		return false;

	eMp3Type eType = GetFileTypeByFilePrefix(szPath);
	if (UNK == eType)
		return false;

	switch (eType)
	{
	case MP3:
		{
			CMP3TagID3 mp3id3v1;
			mp3id3v1.ParseFile(szPath, pCallback);
		}
		break;

	case WMA:
		{
			CWMATag  tag;
			tag.ParseFile(szPath, pCallback);
		}
		break;
	}
	return false;
}

#include "tagparse/genre.h"
const wchar_t* CMP3::ParseGenreID(int n)
{
	int nSizeOfTable = sizeof(genre_table)/sizeof(wchar_t*);

	if (n < 0 || n >= nSizeOfTable)
		return NULL;

	return genre_table[n];
}