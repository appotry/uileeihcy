// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "App\IM\include\framework_inc.h"
#pragma comment(lib, "framework.lib")

// ������
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "App\IM\IM_Ctrls\inc\inc.h"
#pragma comment (lib, "IM_Ctrls.lib")

class CLoginBiz;
class CLoginUI;
extern CLoginBiz*  g_pLoginBiz;
extern CLoginUI*   g_pLoginUI;