// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <assert.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��
#pragma warning(disable:4996)  // 'wcsncpy': This function or variable may be unsafe. Consider using wcsncpy_s instead. To disable deprecation

#include <atlbase.h>
#include <atlstr.h>
#include "atlwin.h"
#include "atlcrack.h"
#include <list>
using namespace std;

#include "..\UI_SDK\include\common\comdef.h"
#include "..\UI_SDK\include\common\LeakDetect.h"