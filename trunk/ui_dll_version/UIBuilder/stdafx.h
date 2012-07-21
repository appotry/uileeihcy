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

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)

#include <atltypes.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <Shellapi.h>
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

// 常量定义
#define  WINDOW_PADDING             10   // 窗口内控件距离窗口边缘的大小
#define  LEFT_TREE_WINDOW_WIDTH    200   // 左侧的窗口宽度

#define  WIDTH_IMAGEBUILD_LISTCTRL 180
#define  HEIGHT_TOOLBAR_BTN         22
#define  WIDTH_TOOLBAR_BTN          50
#define  HEIGHT_TAB                 20  // TAB控件的高度

// 自定义消息
#define WM_SET_DIRTY     (WM_USER+1)    // 传给CChildFrame的wParam: 0 非dirty, 1 dirty;
                                        // 传给CMainFrame的wParam low: 0 非dirty, 1 dirty; wparam high: res type; lparam: hskin
#define WM_GET_DIRTY     (WM_USER+2)    // return: 0 非dirty, 1 dirty
                                        // 传给CMainFrame的wParam:restype,lparam:hskin;
#define WM_GET_APP_DIRTY (WM_USER+3)    // 在关闭程序时，查询是否需要保存
#define WM_CLEAR_DIRTY   (WM_USER+4)    // 清除所在保存标志

// 在codedlg或viewdlg中，默认自己的父窗口是tabctrl -> builddlg -> childframe
#define SET_DIRTY3(x)  \
	GetParent().GetParent().GetParent().SendMessage(WM_SET_DIRTY, x, 0)
#define IS_DIRTY3()    \
	GetParent().GetParent().GetParent().SendMessage(WM_GET_DIRTY, 0, 0)

#define MAKE_RES_DATA(isdirty,restype)  MAKEWPARAM(restype,isdirty)
#define GET_RES_TYPE(data) (data & 0xffff)
#define GET_RES_DIRTY(data) ((data>>16)&0xffff)

#ifdef _DEBUG
#include "..\ui_sdk\include\other\vld.h"	// 内存泄露检查
#endif

#include "..\UI_SDK\include\ui.h"
#pragma  comment(lib, "..\\UI_SDK\\lib\\uidll.lib")

extern CAppModule      _Module;
extern ILogPtr         g_pLog;

void UIBUILDER_LOG_DEBUG( TCHAR* szContent, ... );
void UIBUILDER_LOG_INFO( TCHAR* szContent, ... );
void UIBUILDER_LOG_WARN( TCHAR* szContent, ... );
void UIBUILDER_LOG_ERROR( TCHAR* szContent, ... );
void UIBUILDER_LOG_FATAL( TCHAR* szContent, ...);

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
