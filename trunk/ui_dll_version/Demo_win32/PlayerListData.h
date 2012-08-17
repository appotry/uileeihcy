#pragma once


class CPlayerListData
{
public:
	~CPlayerListData();

	void  Load();
	bool  Add(const String& strPath);
	void  Remove(const String& strPath);
	

private:
	void  GetConfigXmlPath(String& str);

};