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


CFrameWork::CFrameWork()
{
	m_pMgrPlayerList = NULL;
	m_pMgrMain = NULL;
}
CFrameWork::~CFrameWork()
{
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
		m_pMgrMain->Initialize();
	}
	return m_pMgrMain;
}
CPlayerListMgr* CFrameWork::GetPlayerListMgr()
{
	if (NULL == m_pMgrPlayerList)
	{
		m_pMgrPlayerList = new CPlayerListMgr;
		m_pMgrPlayerList->Initialize();
	}
	return m_pMgrPlayerList;
}