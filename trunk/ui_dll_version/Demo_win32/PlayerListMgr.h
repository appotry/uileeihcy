#pragma once
#include "PlayerListData.h"
#include "playlistdlg.h"
#include "player.h"


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

private:
	CPlayerListData   m_data;
	CPlayListDlg*     m_pPlaylistDlg;
};
