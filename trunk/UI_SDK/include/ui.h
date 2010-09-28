#pragma  once

// ���Ҫ�ӱ�׼ C++ ���е���������������Ҫ������԰汾 (/MTd)�����ұ�����������Ϣ���� _Container_base������ Microsoft Visual C++ 2005 �п��Ժ��� C4251��---vector<Object*>�ᱨ��
#pragma warning(disable:4251)

#include <windows.h>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <tchar.h>
#include <assert.h>

using namespace std;


// ǰ������
namespace UI
{
class Object;
class Panel;
class Control;
class Window;

class UIGlobal;
}


#ifndef _UNICODE 
	#error "UI sdk requirs a unicode build."
#endif

#ifdef _UNICODE
	typedef wstring String;
#else
	typedef string  String;
#endif

#ifdef _DEBUG
	#include "other\vld.h"	// �ڴ�й¶��飬������ ���ӿ�Ŀ¼ ..\UI_SDK\lib
#endif

#include "define.h"
#include "util.h"
#include "atl\image.h"
#include "message.h"
#include "manager\mousemanager.h"
#include "manager\keyboardmanager.h"
#include "manager\imagemanager.h"
#include "manager\logmanager.h"
#include "manager\fontmanager.h"
#include "manager\colormanager.h"
#include "manager\topwindowmanager.h"
#include "object.h"
#include "panel\panel.h"
#include "panel\gridpanel.h"
#include "control\control.h"
#include "window\windowbackgroundrender.h"
#include "window\window.h"
//#include "window\modalwindow.h" (�ѷϳ�������װ��window���У���Ϊ������ʽ���е���
#include "window\systemwindow.h"
#include "window\customwindow.h"
#include "window\customexwindow.h"


/*
#include ".\SRC\window\SystemWindow.h"
*/

// control
#include "control\button.h"
#include "control\brushbutton.h"

#include "xmlparse\XMLParse.h"
#include "xmlparse\UIXmlParse.h"
#include "xmlparse\LogXmlParse.h"
#include "xmlparse\ImgXmlParse.h"
#include "xmlparse\fontxmlparse.h"
#include "xmlparse\colorxmlparse.h"
#include "xmlparse\layoutxmlparse.h"
#include "xmlparse\stylexmlparse.h"
#include "xmlparse\control\buttonparse.h"

#include "uiglobal.h"
using namespace UI;


//
// Global Function
namespace UI
{
	UIAPI    bool  UI_Initialize( const String& );

	void     _cdecl UI_LOG_DEBUG( TCHAR*, ... );
	void     _cdecl UI_LOG_INFO( TCHAR*, ... );
	void     _cdecl UI_LOG_WARN( TCHAR*, ... );
	void     _cdecl UI_LOG_ERROR( TCHAR*, ... );
	void     _cdecl UI_LOG_FATAL( TCHAR*, ... );

	void     _cdecl MOUSE_LOG_DEBUG( TCHAR*, ... );

	Image*   UI_GetImage( const String& strImageName );
	HFONT    UI_GetFont( const String& strFontName );
	COLORREF UI_GetColor( const String& strColorName );

	bool     UI_LoadLayout( Object* pRootObj );
	bool     UI_LoadStyle( const String& strStyles, map<String,String>& mapStyle );
}
