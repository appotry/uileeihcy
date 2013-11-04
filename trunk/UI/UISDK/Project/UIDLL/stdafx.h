// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

#include <atlbase.h>
#include <atlcomcli.h>

using namespace std;
#pragma warning(disable:4996)

#pragma comment(lib, "comctl32.lib")

// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#ifdef _UNICODE
#	pragma comment(lib,"uiutil.lib")
#else
#	pragma comment(lib,"uiutil.lib")
#endif

// 注：将#import "msxml6.dll"放在leakdetect.h后面会导致new函数定义被修改，内存泄露无法得到文件line
#import "msxml6.dll" raw_interfaces_only named_guids

// 注：禁用GdiplusBase.h中的new/delete 重载，使用leakdetect的new/delete
//#include <GdiPlus.h>
#include "3rd\gdiplus\gdiplusfix.h"

// 自己的公开头文件
#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"
#include "UISDK\Kernel\Src\Base\globalfunc.h"

using namespace UI;  // 在本模块内部放开 using 使用限制

extern HINSTANCE g_hInstance;

/*

#if 0 // -- 架构改造
*/