#pragma once
#include "PlayerListData.h"
#include "playlistdlg.h"
#include "player.h"

class PlayerListItemInfo
{
public:
	void    SetFilePath(const String& str);

	const String& GetFilePath(){ return m_strFilePath; }
	const String& GetFileName(){ return m_strFileName; }
	const String& GetFileTime(){ return m_strFileTime; }

protected:
	String    m_strFilePath;   // 歌曲路径
	String    m_strFileName;   // 文件名
	String    m_strFileTime;   // 歌曲时长
};

enum PLAY_MODE
{
	SINGLE_ONCE,   // 单曲播放
	SINGLE_LOOP,   // 单曲循环
	ALL_ONCE,      // 顺序播放
	ALL_LOOP,      // 循环播放
	ALL_RAND,      // 随机播放
};

class CPlayerListMgr : public IMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool   Initialize();
	bool   Release();

	void   AddFile(const String& strFile);
	bool   AddDirectory(const String& strDir);
	bool   RemoveAllItem();
	bool   RemovePlayListItem(PlayerListItemInfo* pInfo);

	HWND   ToggleShowPlayerListDlg(HWND hParent);
	void   OnPlayListDlgHide();

	void   SetPlayMode(PLAY_MODE e) { m_ePlayMode = e; }
	PLAY_MODE GetPlayMode() { return m_ePlayMode; }

	void   OnLoadItem(const String& strFile);
	
	int   GetItemCount() { return (int)m_vecPlayerList.size(); }
	PlayerListItemInfo*  GetItem(int i);
	int   GetItemIndex(PlayerListItemInfo*);


	PlayerListItemInfo*  GetNextPlayItem(bool& bPlay);
	PlayerListItemInfo*  GetPrevItem();
	PlayerListItemInfo*  GetNextItem();

	virtual  void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

private:
	CPlayerListData   m_data;           // 数据保存/读取
	CPlayListDlg*     m_pPlaylistDlg;   // 播放列表窗口

	vector<PlayerListItemInfo*>  m_vecPlayerList;  // 播放列表
	list<PlayerListItemInfo*>    m_lstPlayHistory; // 随机播放历史，用于播放上一首时返回
	PlayerListItemInfo*   m_pCurPlayingItem;  // 当前正在插放的文件
	PLAY_MODE             m_ePlayMode;        // 当前播放模式
};
