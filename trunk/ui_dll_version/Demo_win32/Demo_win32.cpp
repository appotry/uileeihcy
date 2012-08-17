// Demo_win32.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Demo_win32.h"
#include "MainMgr.h"
#include "Playlistdlg.h"

//内存泄露检查改进版本
#ifdef _DEBUG
#	define DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#	define DEBUG_CLIENTBLOCK
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#ifdef _DEBUG
#	define new DEBUG_CLIENTBLOCK
#endif


// 全局变量:
HINSTANCE g_hInstance;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_hInstance = hInstance;

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	// 注册自有控件
	UI_RegisterUIObjectCreateData(TTPlayerPlaylistCtrl::XmlName(), 
		(s_UICreateInstancePtr)TTPlayerPlaylistCtrl::_CreatorClass::UICreateInstance);

	// 加载皮肤资源
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	String str = szPath;
	str += _T("ttplayer.uiproj");
	if( false == UI::UI_Initialize( str  ) )
	{
		::MessageBox( NULL, _T("Error Initialize UI"), _T("Error!"), MB_OK );
		return 0;
	}

	// 启动应用程序
	::GetMainMgr();

	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO_WIN32));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// 退出
	::GetFrameWork()->Release();
	::UI_Release();  // 注意，需要在所有的窗口全析构了之后，才可以调用UI_Exit释放资源，否则会导致野指针崩溃

	return 0;
}


