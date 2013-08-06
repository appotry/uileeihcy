#pragma once


class  PluginItemInfo
{
public:
    PluginItemInfo() { nSort = 0; }

    // 加载数据
    String   strPlugin;  // com.tencent.mail
    String   strName;    // 我的邮箱
    String   strDesc;    
    String   strDllPath; 
    // ...

    // 界面数据
    String   strText;   
    String   strIconPath;
    String   strIcon2Path;
    String   strTooltip;

    int  nSort;
};


class CPluginManager
{
public:
    ~CPluginManager();
    static  CPluginManager*  Get();

    void  GetPluginDir(TCHAR*  szPath);
    void  LoadPlugin();

    int  GetPluginCount() { return (int)m_list.size(); }
    PluginItemInfo*  GetPluginItem(DWORD n);

private:
    void  OnFindPlugin(const String& strName, const String& strDirPath);

protected:
    vector<PluginItemInfo*>  m_list;
};