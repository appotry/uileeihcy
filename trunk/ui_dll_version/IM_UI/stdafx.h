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

#include <atlbase.h>
#pragma warning(push)
#pragma warning(disable:4996)  // atlapp.h (848) '_vswprintf': This function or variable may be unsafe.
#include <atlapp.h>
#pragma warning(pop)
#include <atlwin.h>
#include <atldlgs.h>

#include "ui.h"
#pragma comment(lib, "uidll.lib")
#include "..\UI_SDK\include\common\LeakDetect.h"  // ����������#include "ui.h"ǰ��ᵼ��gdiplus������� 