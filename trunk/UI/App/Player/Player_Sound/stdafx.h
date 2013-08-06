// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <assert.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
#pragma warning(disable:4996)  // 'wcsncpy': This function or variable may be unsafe. Consider using wcsncpy_s instead. To disable deprecation

#include <fstream>
using namespace std;

#include <atlbase.h>
#include <atlstr.h>
#include "atlwin.h"
#include <list>
using namespace std;
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")

#include "UISDK\Kernel\Inc\Util\atlcrack.h"
#include "UISDK\Kernel\Inc\Base\commondefine.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"