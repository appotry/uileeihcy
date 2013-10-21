// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "../Business/skinbiz.h"
#include "../UI/skinui.h"

SkinUI    g_SkinUI;
SkinBiz   g_SkinBiz;
SkinUI*   g_pSkinUI = &g_SkinUI;
SkinBiz*  g_pSkinBiz = &g_SkinBiz;

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

    if (0 == _tcscmp(szName, MODULE_ID_SKIN))
    {
        return static_cast<IM::IBiz*>(g_pSkinBiz);
    }
    return NULL;
}


EXTERN_C
__declspec(dllexport) IM::IUI*  LoadUI(const TCHAR* szName, IM::IFramework* pFrameWork)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, MODULE_ID_SKIN))
    {
        return static_cast<IM::IUI*>(g_pSkinUI);
    }
    return NULL;
}