#include "stdafx.h"
#include "pluginmgr.h"
#include "3rd\markup\markup.h"
#include "App/IM/FrameWork/framework/src/framework.h"

CPluginManager::CPluginManager()
{
    m_nLoadTimerId = 0;
    m_nStartTick = 0;
}

CPluginManager::~CPluginManager()
{
    vector<PluginItem*>::iterator iter = m_list.begin();
    for ( ; iter != m_list.end(); iter++)
    {
        SAFE_DELETE((*iter));
    }
    m_list.clear();
}

bool  CPluginManager::LoadPluginXml()
{
    TCHAR  szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);

    String  strPluginXmlPath = szPath;
    strPluginXmlPath.append(_T("plugin\\plugin.xml"));

    CMarkup  markup;
    if (false == markup.Load(strPluginXmlPath.c_str()))
        return false;

    if (false == markup.FindElem())
        return false;
    if (false == markup.IntoElem())
        return false;

    while (markup.FindElem(_T("item")))
    {
        String  strName = markup.GetAttrib(_T("name"));
        String  strPath = markup.GetAttrib(_T("dllpath"));

        if (strName.empty() || strPath.empty())
            continue;

        int  nDelayLoad = _ttoi(markup.GetAttrib(_T("delayload")).c_str());
      
        PluginItem*  pItem = new PluginItem;
        pItem->strXmlName = strName;

        if (UI::Util::IsFullPath(strPath.c_str()))
        {
            pItem->strXmlDllPath = strPath;
        }
        else
        {
            TCHAR  szFullPath[MAX_PATH] = _T("");
            UI::Util::CalcFullPathByRelative(strPluginXmlPath.c_str(), strPath.c_str(), szFullPath);
            pItem->strXmlDllPath = szFullPath;
        }

        // 将路径中的 / 替换为  \ 
        int nLength = pItem->strXmlDllPath.length();
        for (int i = 0; i < nLength; i++)
        {
            if (pItem->strXmlDllPath[i] == _T('/'))
                pItem->strXmlDllPath[i] = _T('\\');
        }
        m_list.push_back(pItem);
    }

    return true;
}

VOID CALLBACK CPluginManager::TimerProc(HWND hwnd,
                               UINT uMsg,
                               UINT_PTR idEvent,
                               DWORD dwTime
                               )
{
    g_framework.m_mgrPlugin.OnTimer();
}

void  CPluginManager::OnTimer()
{
    vector<PluginItem*>::iterator iter = m_list.begin();

    int nRemainCount = 0;
    int nElapse = GetTickCount() - m_nStartTick;
    for (; iter != m_list.end(); iter++)
    {
        PluginItem* pItem = *iter;
        if (pItem->nDelayLoad > nElapse)
        {
            nRemainCount++;
            continue;
        }
        else if (NULL == pItem->hModule)
        {
            _load_dll(pItem);
        }
    }
    if (0 == nRemainCount)
    {
        KillTimer(NULL, m_nLoadTimerId);
        m_nLoadTimerId = 0;
        m_nStartTick = 0;
    }
}
void CPluginManager::Load()
{
    if (0 == m_list.size())
        return;

    if (0 == m_nLoadTimerId)
    {
        m_nLoadTimerId = ::SetTimer(NULL, 1, 100, &CPluginManager::TimerProc);
        m_nStartTick = GetTickCount();
    }
    OnTimer();
}


void  CPluginManager::_load_dll(PluginItem* pItem)
{
    if (pItem->hModule)
        return;

    HMODULE  hModule = ::LoadLibrary(pItem->strXmlDllPath.c_str());  // 增加引用计数
    if (NULL == hModule)
        return;

    IM::LPFUNCLOADUI pfuncLoadUI = (IM::LPFUNCLOADUI)GetProcAddress(hModule, "LoadUI");
    if (NULL == pfuncLoadUI)
        return;
    IM::LPFUNCLOADBIZ pfuncLoadBiz = (IM::LPFUNCLOADBIZ)GetProcAddress(hModule, "LoadBiz");
    if (NULL == pfuncLoadBiz)
        return;

    pItem->hModule = hModule;

    pItem->m_pUI = pfuncLoadUI(pItem->strXmlName.c_str(), IM::GetFramework());
    pItem->m_pBiz = pfuncLoadBiz(pItem->strXmlName.c_str(), IM::GetFramework());

    if (pItem->m_pBiz)
    {
        IM::GetFramework()->PostFunction(IM::Biz, IM::create_mem_fun2_pkg(&CFramework::OnPluginBizLoad, &g_framework, pItem->m_pBiz, pItem), true);
    }
    if (pItem->m_pUI)
    {
        IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun2_pkg(&CFramework::OnPluginUiLoad, &g_framework, pItem->m_pUI, pItem), true);
    }
    return;
}

long  CFramework::OnPluginBizLoad(IM::IBiz* pBiz, PluginItem* pItem)
{
    pBiz->ExecuteCommand(IM::BASE_CMD_ID_INIT, 0, 0);
    return 0;
}
long  CFramework::OnPluginUiLoad(IM::IUI* pUI, PluginItem* pItem)
{
    pUI->ExecuteCommand(IM::BASE_CMD_ID_INIT, 0, 0);
    return 0;
}