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

	void     Play(PlayerListItemInfo* pItem);	
	void     Play();
	void     Stop();
	void     Pause();

	void     HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

protected:
	MainWindow*           m_pMainWindow;
	PlayerListItemInfo*   m_pCurPlayingItem;  // 当前正在插放的文件
};