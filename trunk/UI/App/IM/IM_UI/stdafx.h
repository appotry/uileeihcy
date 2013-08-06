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

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <atlbase.h>
#pragma warning(push)
#pragma warning(disable:4996)  // atlapp.h (848) '_vswprintf': This function or variable may be unsafe.
#include "3rd\wtl80\include\atlapp.h"
#pragma warning(pop)
#include <atlwin.h>
#include "3rd\wtl80\include\atldlgs.h"

// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h" // 这个如果放在#include "ui.h"前面会导致gdiplus编译错误 

#include "App\IM\IM_Ctrls\inc\inc.h"
#pragma comment (lib, "IM_Ctrls.lib")

#include "UISDK\Kernel\Inc\Interface\iimagerender.h"

extern UI::IUIApplication*  g_pUIApp;
extern UI::IImageRender*    g_pShareSkinRender;
extern UI::ISkinManager*    g_pSkinMgr;