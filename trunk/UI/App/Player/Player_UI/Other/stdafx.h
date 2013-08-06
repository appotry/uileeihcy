// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

//#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define  __ATLRES_H__
extern HINSTANCE g_hInstance;

// C 运行时头文件
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <list>
using namespace std;

#include "shlwapi.h"

#include <atlbase.h>
#include <atlwin.h>
#include "3rd\wtl80\include\atlapp.h"
#include "3rd\wtl80\include\atldlgs.h"
#include "3rd\wtl80\include\atlctrls.h"


// 依赖库
#include "UISDK\Project\UIUtil\h\util.h"
#ifdef _UNICODE
#	pragma comment(lib,"uiutil.lib")
#else
#	pragma comment(lib,"uiutil.lib")
#endif

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "App\Player\Player_Sound\player_sound.h"
#pragma comment(lib,"player_sound.lib")

#include "App\Player\Player_UI\Inc\player_inc.h"

#include "App\Player\Player_Ctrls\Inc\inc.h"
#include "App\Player\Player_Ctrls\inc\ittplayerplaylistctrl.h"
#pragma comment (lib, "Player_Ctrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h"

extern UI::IUIApplication* g_pUIApp;
extern UI::ISkinManager*   g_pSkinMgr;

void TTPLAYER_LOG_DEBUG( TCHAR* szContent, ... );
void TTPLAYER_LOG_INFO( TCHAR* szContent, ... ); 
void TTPLAYER_LOG_WARN( TCHAR* szContent, ... ); 
void TTPLAYER_LOG_ERROR( TCHAR* szContent, ... );
void TTPLAYER_LOG_FATAL( TCHAR* szContent, ...);
