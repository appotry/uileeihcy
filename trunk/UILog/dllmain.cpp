// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "UILog_i.h"
#include "dllmain.h"

CUILogModule _AtlModule;
HINSTANCE  g_hInstance;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInstance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
