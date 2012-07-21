// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include "atlwin.h"
#include "atlcrack.h"
#include <string>
#include <list>
using namespace std;

#include <DShow.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "strmiids.lib")



// 常用缩写定义
#define SAFE_DELETE(p) \
	if( NULL != p ) \
	{ \
		delete p; \
		p = NULL; \
	}

#define SAFE_ARRAY_DELETE(p) \
	if( NULL != p ) \
	{ \
		delete[] p; \
		p = NULL; \
	}

#define SAFE_RELEASE(p) \
	if( NULL != p ) \
	{ \
		p->Release(); \
		p = NULL; \
	}

#define SAFE_RELEASE2(p) \
	if( NULL != p ) \
	{ \
		p.Release(); \
		p = NULL; \
	}

#ifdef _UNICODE
typedef wstring String;
#else
typedef string  String;
#endif