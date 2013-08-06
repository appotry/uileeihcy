#include "stdafx.h"
#include "player.h"
#include "MainMgr.h"
#include "PlayerListMgr.h"
#include "EqualizerMgr.h"
#include "App\Player\Player_UI\Mgr\Lyric\LyricMgr.h"

CFrameWork*     GetFrameWork()     { return CFrameWork::GetInstance(); }
CMainMgr*       GetMainMgr()       { return ::GetFrameWork()->GetMainMgr(); }
CPlayerListMgr* GetPlayerListMgr() { return ::GetFrameWork()->GetPlayerListMgr(); }
CEqualizerMgr*  GetEqualizerMgr()  { return ::GetFrameWork()->GetEqualizerMgr(); }
CLyricMgr*      GetLyricMgr()      { return ::GetFrameWork()->GetLyricMgr(); }
CConfigData*    GetConfigData()    { return ::GetFrameWork()->GetConfigData(); }

void IMgr::FireEvent(int nCmdType, int nCmdId, WPARAM wParam, LPARAM lParam)
{
	GetFrameWork()->FireEvent(this, nCmdType, nCmdId, wParam, lParam);
}


CFrameWork::CFrameWork()
{
	m_pMgrPlayerList = NULL;
	m_pMgrMain = NULL;
	m_pMgrEqualizer = NULL;

    // ¼ÓÔØÅäÖÃÎÄ¼þ
    m_configFile.Load(&m_config);
}

CFrameWork::~CFrameWork()
{
    this->m_configFile.Save(&m_config);
	m_vecMgr.clear();
}
void CFrameWork::Release()
{
	if (m_pMgrEqualizer)
	{
		m_pMgrEqualizer->Release();
		SAFE_DELETE(m_pMgrEqualizer);
	}

    if (m_pMgrLyric)
    {
        m_pMgrLyric->Release();
        SAFE_DELETE(m_pMgrLyric);
    }

	if (m_pMgrPlayerList)
	{
		m_pMgrPlayerList->Release();
		SAFE_DELETE(m_pMgrPlayerList);
	}

	if (m_pMgrMain)
	{
		m_pMgrMain->Release();
		SAFE_DELETE(m_pMgrMain);
	}
}

CMainMgr* CFrameWork::GetMainMgr()
{
	if (NULL == m_pMgrMain)
	{
		m_pMgrMain = new CMainMgr;
		m_vecMgr.push_back(m_pMgrMain);

		m_pMgrMain->Initialize();
	}
	return m_pMgrMain;
}
CPlayerListMgr* CFrameWork::GetPlayerListMgr()
{
	if (NULL == m_pMgrPlayerList)
	{
		m_pMgrPlayerList = new CPlayerListMgr;
		m_vecMgr.push_back(m_pMgrPlayerList);

		m_pMgrPlayerList->Initialize();
	}
	return m_pMgrPlayerList;
}

CEqualizerMgr*  CFrameWork::GetEqualizerMgr()
{
	if (NULL == m_pMgrEqualizer)
	{
		m_pMgrEqualizer = new CEqualizerMgr;
		m_vecMgr.push_back(m_pMgrEqualizer);

		m_pMgrEqualizer->Initialize();
	}
	return m_pMgrEqualizer;
}

CLyricMgr*  CFrameWork::GetLyricMgr()
{
    if (NULL == m_pMgrLyric)
    {
        m_pMgrLyric = new CLyricMgr;
        m_vecMgr.push_back(m_pMgrLyric);

        m_pMgrLyric->Initialize();
    }
    return m_pMgrLyric;
}

bool  CFrameWork::SaveConfig()
{
    return m_configFile.Save(&m_config);
}

void CFrameWork::FireEvent(IMgr* pSource,int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
	int nSize = (int)m_vecMgr.size();
	for (int i = 0; i < nSize; i++)
	{
		IMgr* pMgr = m_vecMgr[i];
		if (NULL == pMgr)
			continue;

		pMgr->HandleEvent(pSource,nEventType,nEventId, wParam, lParam);
	}
}