// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
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

// ��������
#define  WINDOW_PADDING             10   // �����ڿؼ����봰�ڱ�Ե�Ĵ�С
#define  LEFT_TREE_WINDOW_WIDTH    200   // ���Ĵ��ڿ��

#define  WIDTH_IMAGEBUILD_LISTCTRL 180
#define  HEIGHT_TOOLBAR_BTN         22
#define  WIDTH_TOOLBAR_BTN          50
#define  HEIGHT_TAB                 20  // TAB�ؼ��ĸ߶�

// �Զ�����Ϣ
#define WM_SET_DIRTY     (WM_USER+1)    // ����CChildFrame��wParam: 0 ��dirty, 1 dirty;
                                        // ����CMainFrame��wParam low: 0 ��dirty, 1 dirty; wparam high: res type; lparam: hskin
#define WM_GET_DIRTY     (WM_USER+2)    // return: 0 ��dirty, 1 dirty
                                        // ����CMainFrame��wParam:restype,lparam:hskin;
#define WM_GET_APP_DIRTY (WM_USER+3)    // �ڹرճ���ʱ����ѯ�Ƿ���Ҫ����
#define WM_CLEAR_DIRTY   (WM_USER+4)    // ������ڱ����־

// ��codedlg��viewdlg�У�Ĭ���Լ��ĸ�������tabctrl -> builddlg -> childframe
#define SET_DIRTY3(x)  \
	GetParent().GetParent().GetParent().SendMessage(WM_SET_DIRTY, x, 0)
#define IS_DIRTY3()    \
	GetParent().GetParent().GetParent().SendMessage(WM_GET_DIRTY, 0, 0)

#define MAKE_RES_DATA(isdirty,restype)  MAKEWPARAM(restype,isdirty)
#define GET_RES_TYPE(data) (data & 0xffff)
#define GET_RES_DIRTY(data) ((data>>16)&0xffff)

#ifdef _DEBUG
#include "..\ui_sdk\include\other\vld.h"	// �ڴ�й¶���
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
