// UIDLL.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
//	g_pUIApplication->SetModuleInstance(hModule);
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

