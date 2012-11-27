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

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atldlgs.h>

#include "ui.h"
#pragma comment(lib, "uidll.lib")
#include "..\UI_SDK\include\common\LeakDetect.h"  // 这个如果放在#include "ui.h"前面会导致gdiplus编译错误 