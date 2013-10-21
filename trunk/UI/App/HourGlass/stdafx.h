// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma warning(disable:4996)


#include "atlbase.h"
#include "3rd\wtl80\include\atlapp.h"
#include "3rd\wtl80\include\atldlgs.h"

// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h" // 这个如果放在#include "ui.h"前面会导致gdiplus编译错误 

extern UI::IUIApplication*  g_pUIApp;

#include "config.h"
class CMainWnd;
class CConfig;
CConfig*  GetConfig();
CMainWnd*  GetMainWnd();

// TODO: reference additional headers your program requires here
