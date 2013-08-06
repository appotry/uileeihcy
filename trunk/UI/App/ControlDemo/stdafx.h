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

// ������
#include "UISDK\Project\UIUtil\h\util.h"
#ifdef _UNICODE
#	pragma comment(lib,"uiutil.lib")
#else
#	pragma comment(lib,"uiutil.lib")
#endif

#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib, "UICtrls.lib")

// #include "App\IM\IM_Ctrls\inc\inc.h"
// #pragma comment(lib, "IM_Ctrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h"

extern UI::IUIApplication* g_pUIApp;
extern HINSTANCE g_hInstance;