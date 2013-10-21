// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

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

#if 0

EXTERN_C
__declspec(dllexport) void  Load(const TCHAR* szName, IM::IFramework* pFrameWork, IM::IBiz** ppBiz, IM::IUI** ppUI)
{
    if (NULL == szName)
        return;

    if (0 == _tcscmp(szName, MODULE_ID_TRAYICON))
    {
//         if (NULL == g_pLoginBiz)
//         {
//             g_pLoginBiz = new CLoginBiz(pFrameWork);
//             pFrameWork->RegisterBiz(static_cast<IM::IBiz*>(g_pLoginBiz));
// 
//             if (ppBiz)
//                 *ppBiz = static_cast<IM::IBiz*>(g_pLoginBiz);
//         }
//         if (NULL == g_pLoginUI)
//         {
//             g_pLoginUI = new CLoginUI(pFrameWork);
//             pFrameWork->RegisterUI(static_cast<IM::IUI*>(g_pLoginUI));
// 
//             if (ppUI)
//                 *ppUI = static_cast<IM::IUI*>(g_pLoginUI);
//         }
    }
}


EXTERN_C
__declspec(dllexport) void  Unload(const TCHAR* szName)
{
    // 注：IBusiness的析构函数不是虚的，因此不能直接delete IBusiness

    if (NULL == szName)
        return;

    if (0 == _tcscmp(szName, MODULE_ID_TRAYICON))
    {
//         SAFE_DELETE(g_pLoginBiz);
//         SAFE_DELETE(g_pLoginUI);
        return;
    }
}

#endif