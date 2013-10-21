// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "App\IM\Module\Main\Biz\mainbiz.h"
#include "App\IM\Module\Main\UI\mainui.h"

CMainBiz  g_MainBiz;
CMainUI  g_MainUI;

CMainBiz*  g_pMainBiz = &g_MainBiz;
CMainUI*   g_pMainUI = &g_MainUI;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



EXTERN_C
__declspec(dllexport) IM::IBiz*  LoadBiz(const TCHAR* szName, IM::IFramework* pFrameWork)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, MODULE_ID_MAIN))
    {
        return static_cast<IM::IBiz*>(g_pMainBiz);
    }
    return NULL;
}


EXTERN_C
__declspec(dllexport) IM::IUI*  LoadUI(const TCHAR* szName, IM::IFramework* pFrameWork)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, MODULE_ID_MAIN))
    {
        return static_cast<IM::IUI*>(g_pMainUI);
    }
    return NULL;
}