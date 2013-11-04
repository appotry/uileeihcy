#include "stdafx.h"
#include "dwmhelper.h"

DwmHelper::DwmHelper()
{
    pDwmExtendFrameIntoClientArea = NULL;

    m_hModule = LoadLibrary(_T("dwmapi.dll"));
    if (NULL == m_hModule)
        return;

    pDwmExtendFrameIntoClientArea = (funcDwmExtendFrameIntoClientArea)GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea");
    pDwmEnableBlurBehindWindow = (funcDwmEnableBlurBehindWindow)GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow");
    pDwmIsCompositionEnabled = (funcDwmIsCompositionEnabled)GetProcAddress(m_hModule, "DwmIsCompositionEnabled");

}
DwmHelper::~DwmHelper()
{
    if (m_hModule)
    {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

DwmHelper*  DwmHelper::GetInstance()
{
    static DwmHelper  dwm;
    return &dwm;
}

bool  DwmHelper::IsEnable()
{ 
    if (NULL == m_hModule)
        return false;

    if (NULL == pDwmIsCompositionEnabled)
        return false;

    BOOL b = FALSE;
    pDwmIsCompositionEnabled(&b);

    return b ? true:false;
};