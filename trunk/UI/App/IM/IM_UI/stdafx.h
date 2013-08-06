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

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <atlbase.h>
#pragma warning(push)
#pragma warning(disable:4996)  // atlapp.h (848) '_vswprintf': This function or variable may be unsafe.
#include "3rd\wtl80\include\atlapp.h"
#pragma warning(pop)
#include <atlwin.h>
#include "3rd\wtl80\include\atldlgs.h"

// ������
#include "UISDK\Project\UIUtil\h\util.h"
#pragma comment(lib,"uiutil.lib")

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#pragma comment(lib,"uisdk.lib")
#pragma comment(lib,"uictrls.lib")

#include "UISDK\Kernel\Inc\Base\leakdetect.h" // ����������#include "ui.h"ǰ��ᵼ��gdiplus������� 

#include "App\IM\IM_Ctrls\inc\inc.h"
#pragma comment (lib, "IM_Ctrls.lib")

#include "UISDK\Kernel\Inc\Interface\iimagerender.h"

extern UI::IUIApplication*  g_pUIApp;
extern UI::IImageRender*    g_pShareSkinRender;
extern UI::ISkinManager*    g_pSkinMgr;