// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "../UI/privatechatui.h"
#include "../Biz/privatechatbiz.h"

CPrivateChatUI    g_PrivateChatUI;
CPrivateChatBiz   g_PrivateChatBiz;

CPrivateChatUI*   g_pPrivateChatUI = &g_PrivateChatUI;
CPrivateChatBiz*  g_pPrivateChatBiz = &g_PrivateChatBiz;

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

    if (0 == _tcscmp(szName, MODULE_ID_PRIVATECHAT))
    {
        return static_cast<IM::IBiz*>(g_pPrivateChatBiz);
    }
    return NULL;
}


EXTERN_C
__declspec(dllexport) IM::IUI*  LoadUI(const TCHAR* szName, IM::IFramework* pFrameWork)
{
    if (NULL == szName)
        return NULL;

    if (0 == _tcscmp(szName, MODULE_ID_PRIVATECHAT))
    {
        return static_cast<IM::IUI*>(g_pPrivateChatUI);
    }
    return NULL;
}
