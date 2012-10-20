#pragma once
#include "player.h"
#include "PlayerListMgr.h"
#include "PlayerConfigData.h"
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

	bool     SetVisualizationType(int nType);
	void     HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);
	const CConfigData*  GetConfigData() { return &m_config; }

protected:	
	virtual void   on_mp3_stop();
	virtual void   on_mp3_progress_ind(double dSeconds, double dPercent);
	virtual void   on_mp3_volume_ind(long lVolumn);

protected:
	MainWindow*           m_pMainWindow;
	PlayerListItemInfo*   m_pCurPlayingItem;  // 当前正在插放的文件, TODO: 是否应该考虑只保留一份数据，其它Mgr直接引用该数据，而不是自己也保留一份。避免野指针或者数据不同步

	CPlayerConfigData     m_configFile;       // 配置数据保存/读取
	CConfigData           m_config;           // 配置数据
};