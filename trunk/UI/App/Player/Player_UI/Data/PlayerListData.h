#pragma once

// 注：一般来说，由于播放列表项会很多，因此将播放列表配置文件与其它配置文件分开
class CPlayerListData
{
public:
	~CPlayerListData();

	void  Load();
	bool  Add(const String& strPath);
	bool  Remove(const String& strPath);
	bool  RemoveAll();

private:
	void  GetConfigXmlPath(String& str);

};