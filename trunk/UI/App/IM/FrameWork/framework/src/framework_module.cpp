#include "stdafx.h"
#include "framework.h"
#include "3rd/markup/markup.h"
#include "moduleitem.h"
#include "App\IM\include\login_inc.h"
#include "App\IM\FrameWork\framework\src\event.h"



// long  CFramework::LoadModuleByTime_(const TCHAR* szTime)
// {
//     LoadModuleByTime(szTime);
//     SAFE_ARRAY_DELETE(szTime);
//     return 0;
// }
void  CFramework::LoadModuleByTime(const TCHAR* szTime)
{
    UIASSERT(!IsUIThread());
    if (NULL == szTime)
        return;

//     if (GetCurrentThreadId() != m_dwMainThreadId)
//     {
//         TCHAR* pszText = new TCHAR[_tcslen(szTime)+1];
//         _tcscpy(pszText, szTime);
//         PostFunction(IM::Main, IM::create_mem_fun1_pkg(&CFramework::LoadModules_, this, (const TCHAR*)pszText), false);
//         return;
//     }

    vector<ModuleItem*>::iterator iter = m_vecModules.begin();
    for (; iter != m_vecModules.end(); iter++)
    {
        ModuleItem* pItem = *iter;
        if (0 == _tcscmp(szTime, pItem->m_strLoadTime.c_str()))
        {
            if (pItem->IsLoaded())
                continue;

            pItem->Load(m_pIFramework);
            if (pItem->m_pBiz)
            {
                PostFunction(IM::Biz, IM::create_mem_fun2_pkg(&CFramework::OnModuleBizLoad, this, pItem->m_pBiz, pItem), true);
            }
            if (pItem->m_pUI)
            {
                PostFunction(IM::Ui, IM::create_mem_fun2_pkg(&CFramework::OnModuleUiLoad, this, pItem->m_pUI, pItem), true);
            }
        }
    }

    return;
}

// 从Biz线程去加载模块
void  CFramework::LoadModuleByName(const TCHAR* szName, IM::IUI** ppUI, IM::IBiz** ppBiz)
{
    UIASSERT(!IsUIThread());
    if (NULL == szName)
        return;

    ModuleItem* pItem = this->FindModule(szName);
    if (NULL == pItem)
        return;

    if (pItem->IsLoaded())
        return;

    pItem->Load(m_pIFramework);
    if (pItem->m_pBiz)
    {
        PostFunction(IM::Biz, IM::create_mem_fun2_pkg(&CFramework::OnModuleBizLoad, this, pItem->m_pBiz, pItem), true);

        if (ppBiz)
            *ppBiz = pItem->m_pBiz;
    }
    if (pItem->m_pUI)
    {
        PostFunction(IM::Ui, IM::create_mem_fun2_pkg(&CFramework::OnModuleUiLoad, this, pItem->m_pUI, pItem), true);

        if (ppUI)
            *ppUI = pItem->m_pUI;
    }

}

long  CFramework::OnModuleBizLoad(IM::IBiz* pBiz, ModuleItem* pItem)
{
    pBiz->ExecuteCommand(IM::BASE_CMD_ID_INIT, 0, 0);
    return 0;
}
long  CFramework::OnModuleUiLoad(IM::IUI* pUI, ModuleItem* pItem)
{   
    pUI->ExecuteCommand(IM::BASE_CMD_ID_INIT, 0, 0);
    return 0;
}

bool  CFramework::LoadFrameworkXml()
{
    if (m_vecModules.size() > 0)
        return false;

    TCHAR  szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);

    String  strFrameworkXmlPath = szPath;
    strFrameworkXmlPath.append(_T("framework.xml"));

    CMarkup  markup;
    if (false == markup.Load(strFrameworkXmlPath.c_str()))
        return false;

    if (false == markup.FindElem())
        return false;
    if (false == markup.IntoElem())
        return false;
    if (false == markup.FindElem(_T("Module")))
        return false;
    if (false == markup.IntoElem())
        return false;

    while (markup.FindElem())
    {
        String strLoadTime = markup.GetTagName();
        if (strLoadTime.empty())
            continue;

        if (false == markup.IntoElem())
            continue;

    
        while (markup.FindElem(_T("item")))
        {
            String  strName = markup.GetAttrib(_T("name"));
            String  strPath = markup.GetAttrib(_T("path"));

            if (strName.empty() || strPath.empty())
                continue;

            ModuleItem*  pItem = new ModuleItem;
            pItem->m_strName = strName;
            pItem->m_strLoadTime = strLoadTime;

            if (UI::Util::IsFullPath(strPath.c_str()))
            {
                pItem->m_strFilePath = strPath;
            }
            else
            {
                pItem->m_strFilePath = szPath;
                pItem->m_strFilePath.append(strPath);
            }

            // 将路径中的 / 替换为  \ 
            int nLength = pItem->m_strFilePath.length();
            for (int i = 0; i < nLength; i++)
            {
                if (pItem->m_strFilePath[i] == _T('/'))
                    pItem->m_strFilePath[i] = _T('\\');
            }
            m_vecModules.push_back(pItem);
        }
        
        markup.OutOfElem();
    }

    return true;
}

ModuleItem*  CFramework::FindModule(const TCHAR* szName)
{
    if (NULL == szName)
        return NULL;

    vector<ModuleItem*>::iterator iter = m_vecModules.begin();
    for (; iter != m_vecModules.end(); iter++)
    {
        if (0 == _tcscmp(szName, (*iter)->m_strName.c_str()))
        {
            return *iter;
        }
    }

    return NULL;
}

IM::IBiz*  CFramework::GetBiz(const TCHAR* szName)
{
    if (NULL == szName)
        return NULL;

    ModuleItem* pModule = FindModule(szName);
    if (NULL == pModule)
        return NULL;

    return pModule->GetBiz();
}
IM::IUI*  CFramework::GetUI(const TCHAR* szName)
{
    if (NULL == szName)
        return NULL;

    ModuleItem* pModule = FindModule(szName);
    if (NULL == pModule)
        return NULL;

    return pModule->GetUI();
}
