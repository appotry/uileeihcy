// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <algorithm>

using namespace std;

#include "UILog_i.h"
#include "..\UIUtil\h\Util.h"
#pragma comment(lib,"uiutil_u.lib")
using namespace UI;

using namespace ATL;

#include "LogManager.h"
#include "LogXmlParse.h"
#include "LogItem.h"

extern HINSTANCE  g_hInstance;