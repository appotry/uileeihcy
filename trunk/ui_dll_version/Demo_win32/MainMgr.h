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

	bool     ShowPlayerListDlg();

protected:
	MainWindow*     m_pMainWindow;
};