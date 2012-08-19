#include "stdafx.h"
#include "player.h"
#include "MainMgr.h"
#include "PlayerListMgr.h"


CFrameWork*  GetFrameWork()
{
	return CFrameWork::GetInstance();
}
CMainMgr* GetMainMgr()
{
	return ::GetFrameWork()->GetMainMgr();
}
CPlayerListMgr* GetPlayerListMgr()
{
	return ::GetFrameWork()->GetPlayerListMgr();
}


void IMgr::FireEvent(int nCmdType, int nCmdId, WPARAM wParam, LPARAM lParam)
{
	GetFrameWork()->FireEvent(this, nCmdType, nCmdId, wParam, lParam);
}


CFrameWork::CFrameWork()
{
	m_pMgrPlayerList = NULL;
	m_pMgrMain = NULL;
}
CFrameWork::~CFrameWork()
{
	m_vecMgr.clear();
}
void CFrameWork::Release()
{
	if (NULL != m_pMgrPlayerList)
	{
		m_pMgrPlayerList->Release();
		SAFE_DELETE(m_pMgrPlayerList);
	}

	if (NULL != m_pMgrMain)
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

void CFrameWork::FireEvent(IMgr* pSource,int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
	int nSize = (int)m_vecMgr.size();
	for (int i = 0; i < nSize; i++)
	{
		IMgr* pMgr = m_vecMgr[i];
		if (NULL == pMgr)
			continue;

		pMgr->DoEvent(pSource,nEventType,nEventId, wParam, lParam);
	}
}