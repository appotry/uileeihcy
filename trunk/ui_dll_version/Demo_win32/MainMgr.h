#pragma once
#include "player.h"
#include "PlayerListMgr.h"
class MainWindow;

class CMainMgr : public IMgr
{
public:
	CMainMgr();
	~CMainMgr();

	bool     Initialize();
	bool     Release();

protected:
	MainWindow*     m_pMainWindow;

protected:
	CPlayerListMgr  m_MgrPlayerList;

};