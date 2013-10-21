// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "../Biz/LoginBiz.h"
#include "../UI/LoginUI.h"

CLoginBiz  g_LoginBiz;
CLoginUI   g_LoginUI;

CLoginBiz*  g_pLoginBiz = &g_LoginBiz;
CLoginUI*   g_pLoginUI = &g_LoginUI;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        break;
	case DLL_THREAD_ATTACH:
        break;
	case DLL_THREAD_DETACH:
        break;
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

    if (0 == _tcscmp(szName, MODULE_ID_LOGIN))
    {
        return static_cast<IM::IUI*>(g_pLoginUI);
    }

    return NULL;
}

EXTERN_C
__declspec(dllexport) IM::IBiz*  LoadBiz(const TCHAR* szName, IM::IFramework* pFramework)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, MODULE_ID_LOGIN))
    {
        return static_cast<IM::IBiz*>(g_pLoginBiz);
    }
    return NULL;
}