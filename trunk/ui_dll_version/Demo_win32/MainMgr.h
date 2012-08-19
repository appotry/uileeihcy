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

	HWND     GetMainWnd();

	void     DoEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

protected:
	MainWindow*     m_pMainWindow;
};