#pragma once
#include "player.h"
#include "PlayerListMgr.h"
class MainWindow;


class CMainMgr : public IMgr, public IMp3EventCallback
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
	void     PlayNext();
	void     PlayPrev();

	void     HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

protected:	
	virtual void   on_mp3_stop();
	virtual void   on_mp3_progress_ind(double dSeconds, double dPercent);
	virtual void   on_mp3_volume_ind(long lVolumn);

protected:
	MainWindow*           m_pMainWindow;
	PlayerListItemInfo*   m_pCurPlayingItem;  // 当前正在插放的文件
};