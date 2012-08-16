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
	bool  Add(const String& strPath);
	void  Remove(const String& strPath);
	
	int   GetItemCount() { return (int)m_vecPlayerList.size(); }
	PlayerListItemInfo*  GetItem(int i);

private:
	void  GetConfigXmlPath(String& str);

public:
	vector<PlayerListItemInfo*>  m_vecPlayerList;
};