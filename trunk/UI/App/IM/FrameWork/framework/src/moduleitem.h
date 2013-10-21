#pragma once
#include "App\IM\include\framework_inc.h"

class  ModuleItem
{
public:
    ModuleItem();
    ~ModuleItem();

public:
    bool  Load(IM::IFramework* pFramework);
    void  Unload();
    bool  IsLoaded();

    IM::IBiz*  GetBiz();
    IM::IUI*  GetUI();

    HMODULE  m_hModule;
    String   m_strName;
    String   m_strFilePath;
    String   m_strLoadTime;   // 加载时机，如"startup" "logon" "onneed"

    IM::IBiz*  m_pBiz;
    IM::IUI*  m_pUI;

private:
    IM::LPFUNCLOADUI   m_pFuncLoadUI;
    IM::LPFUNCLOADBIZ  m_pFuncLoadBiz;
};

