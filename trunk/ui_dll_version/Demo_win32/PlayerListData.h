#pragma once

// ע��һ����˵�����ڲ����б����ܶ࣬��˽������б������ļ������������ļ��ֿ�
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