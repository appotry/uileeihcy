#pragma once
#include "PlayerListData.h"
#include "playlistdlg.h"


class CPlayerListMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool   Load();
	bool   AddFile(const String& strFile);
	bool   AddDirectory(const String& strDir);

private:
	CPlayerListData   m_data;
	CPlayListDlg      m_dlg;

};
