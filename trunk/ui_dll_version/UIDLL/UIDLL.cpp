// UIDLL.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

// �ڴ�й¶���Ľ��汾
#ifdef _DEBUG
#	define DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#	define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#	define new DEBUG_CLIENTBLOCK
#endif


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

