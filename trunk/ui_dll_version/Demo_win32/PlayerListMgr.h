#pragma once
#include "PlayerListData.h"
#include "playlistdlg.h"
#include "player.h"

class PlayerListItemInfo
{
public:
	String    m_strFilePath;   // ¸èÇúÂ·¾¶
	String    m_strFileTime;   // ¸èÇúÊ±³¤
};


class CPlayerListMgr : public IMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool   Initialize();
	bool   Release();

	bool   AddFile(const String& strFile);
	bool   AddDirectory(const String& strDir);

	HWND   ShowPlayerListDlg(HWND hParent);

	void   OnLoadItem(const String& strFile);
	
	int   GetItemCount() { return (int)m_vecPlayerList.size(); }
	PlayerListItemInfo*  GetItem(int i);

private:
	CPlayerListData   m_data;
	CPlayListDlg*     m_pPlaylistDlg;

public:
	vector<PlayerListItemInfo*>  m_vecPlayerList;
};
