// UIDLL.cpp : ���� DLL Ӧ�ó������ڵ㡣
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
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

