// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <cctype>
#include <algorithm>

using namespace std;

#include "UILog_i.h"
#include "UISDK\Project\UIUtil\h\Util.h"
#pragma comment(lib,"uiutil.lib")
using namespace UI;

using namespace ATL;

#include "LogManager.h"
#include "LogXmlParse.h"
#include "LogItem.h"

extern HINSTANCE  g_hInstance;