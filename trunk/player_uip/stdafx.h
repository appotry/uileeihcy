// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

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



// ������д����
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