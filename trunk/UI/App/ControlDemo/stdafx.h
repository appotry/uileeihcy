// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#ifdef _UNICODE
#	pragma comment(lib,"uiutil.lib")
#else
#	pragma comment(lib,"uiutil.lib")
#endif

#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib, "UICtrls.lib")

// #include "App\IM\IM_Ctrls\inc\inc.h"
// #pragma comment(lib, "IM_Ctrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h"

extern UI::IUIApplication* g_pUIApp;
extern HINSTANCE g_hInstance;