// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "../include/mail_inc.h"
#include "../UI/mailui.h"

CMailUI  g_mailUi;

HMODULE  g_hModule = NULL;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
        g_hModule = hModule;
        break;

	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


EXTERN_C
__declspec(dllexport) IM::IUI*  LoadUI(const TCHAR* szName, IM::IFramework* pFramework)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, PLUGIN_ID_MAIL))
    {
        return static_cast<IM::IUI*>(&g_mailUi);
    }

    return NULL;
}

EXTERN_C
__declspec(dllexport) IM::IBiz*  LoadBiz(const TCHAR* szName, IM::IFramework* pFramework)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, PLUGIN_ID_MAIL))
    {
    }
    return NULL;
}