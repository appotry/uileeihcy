// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <ShellAPI.h>     // ShellExecute
#include <atlbase.h>
#include <atlwin.h>

#include <Imm.h>
#pragma comment(lib, "Imm32.lib")

#include "UISDK\Project\UIUtil\h\Util.h"
#pragma comment(lib, "UIUtil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")

#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Base\leakdetect.h"
using namespace UI;
