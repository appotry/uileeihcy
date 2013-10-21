// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <tchar.h>
#include <ShellAPI.h>

#include "App\IM\include\framework_inc.h"
#pragma comment(lib, "framework.lib")

// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")


extern HMODULE  g_hModule;