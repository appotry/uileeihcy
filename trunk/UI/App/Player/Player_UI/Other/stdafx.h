// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

// ������뽫λ������ָ��ƽ̨֮ǰ��ƽ̨��ΪĿ�꣬���޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

//#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define  __ATLRES_H__
extern HINSTANCE g_hInstance;

// C ����ʱͷ�ļ�
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


// ������
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
