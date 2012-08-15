#pragma once

class PlayerListItemInfo
{
public:
	String    m_strFilePath;
};
class CPlayerListData
{
public:
	~CPlayerListData();

	void  Load();
	void  Add(const String& strPath);
	void  Remove(const String& strPath);
	

public:
	vector<PlayerListItemInfo*>  m_vecPlayerList;
};