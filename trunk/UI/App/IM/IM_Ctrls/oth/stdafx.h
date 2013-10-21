// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "UISDK\Project\UIUtil\h\Util.h"
#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#include "App\Player\Player_Ctrls\Inc\inc.h"

#pragma comment(lib, "uiutil.lib")
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "App\IM\IM_Ctrls\inc\inc.h"