#pragma once


class PlayListItemInfo
{
public:
	String  m_strFilePath;
	String  m_strFileTime;
	
};

class CPlayerListMgr
{
public:
	CPlayerListMgr(void);
	~CPlayerListMgr(void);

public:
	bool   Load();
	bool   AddFile(const String& strFile);
	bool   AddDirectory(const String& strDir);
};
