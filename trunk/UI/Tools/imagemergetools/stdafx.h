// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

#pragma warning(disable:4996)
#pragma warning(disable:4005)

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <atlbase.h>
#include "atlapp.h"
#include "atlcrack.h"
#include "atlctrls.h"
#include "atldlgs.h"
#include "UISDK\Kernel\Inc\Base\inc.h"
#pragma comment(lib, "UISDK.lib")
#include "UISDK\Kernel\Inc\Util\iimage.h"


#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
