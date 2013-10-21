#pragma once
#include "App/IM/include/framework_inc.h"

class  PluginItem
{
public:
    PluginItem() 
    {
        hModule = NULL;
        nDelayLoad = 0;
        m_pBiz = NULL;
        m_pUI = NULL;
    }

public:
    // xml配置
    String   strXmlName;
    String   strXmlDllPath;
    int      nDelayLoad;

    // 数据
    HMODULE  hModule;

    IM::IBiz*  m_pBiz;
    IM::IUI*   m_pUI;
};


class CPluginManager
{
public:
    CPluginManager();
    ~CPluginManager();

public:
    bool  LoadPluginXml();
    void  Load();

    static VOID CALLBACK TimerProc(HWND hwnd,
            UINT uMsg,
            UINT_PTR idEvent,
            DWORD dwTime
        );
    void  OnTimer();

private:
    void  _load_dll(PluginItem* pItem);

protected:
    vector<PluginItem*>  m_list;

    UINT_PTR  m_nLoadTimerId;  // 用于延时加载
    int  m_nStartTick;         // 开始计时的时刻
};