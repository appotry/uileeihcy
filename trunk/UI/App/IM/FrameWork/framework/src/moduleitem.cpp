#include "stdafx.h"
#include "moduleitem.h"


ModuleItem::ModuleItem()
{
    m_hModule = NULL;
    m_pFuncLoadUI = NULL;
    m_pFuncLoadBiz = NULL;
    m_pBiz = NULL;
    m_pUI = NULL;
}

ModuleItem::~ModuleItem()
{
    if (m_hModule)
    {
        Unload();
    }
}

bool  ModuleItem::IsLoaded() 
{ 
    return m_hModule != NULL ? true:false; 
}

bool ModuleItem::Load(IM::IFramework* pFramework)
{
    HMODULE  hModule = ::LoadLibrary(m_strFilePath.c_str());  // 增加引用计数
    if (NULL == hModule)
        return false;

    IM::LPFUNCLOADUI pfuncLoadUI = (IM::LPFUNCLOADUI)GetProcAddress(hModule, "LoadUI");
    if (NULL == pfuncLoadUI)
        return false;
    IM::LPFUNCLOADBIZ pfuncLoadBiz = (IM::LPFUNCLOADBIZ)GetProcAddress(hModule, "LoadBiz");
    if (NULL == pfuncLoadBiz)
        return false;

    m_hModule = hModule;
    m_pFuncLoadUI = pfuncLoadUI;
    m_pFuncLoadBiz = pfuncLoadBiz;

    m_pBiz = m_pFuncLoadBiz(m_strName.c_str(), IM::GetFramework());
    m_pUI = m_pFuncLoadUI(m_strName.c_str(), IM::GetFramework());
    
    return true;
}

void  ModuleItem::Unload()
{
    if (m_hModule)
    {
//         m_pFuncUnload(m_strName.c_str());
// 
//     //    ::FreeLibrary(m_hModule);   // 减少引用计数  // TODO: 在这里释放会导致显示内存泄露数据时崩溃
        m_hModule = NULL;
        m_pFuncLoadUI = NULL;
        m_pFuncLoadBiz = NULL;
    }
}

IM::IBiz*  ModuleItem::GetBiz()
{
    return m_pBiz;        
}

IM::IUI*  ModuleItem::GetUI()
{
    return m_pUI;
}
