// richedit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "richedit.h"
#include "MainDlg.h"
#include <gdiplus.h>

// Global Variables:
CAppModule  _Module;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//////////////////////////////////////////////////////////////////////////
	// 初始化

	_Module.Init(NULL, hInstance);

	DWORD m_dwToken = 0;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, NULL );

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	//////////////////////////////////////////////////////////////////////////
	// 显示

	CMainDlg dlg;
	dlg.Create(NULL);
	dlg.ShowWindow(SW_SHOW);
//	dlg.DoModal();  <-- DoModal 会导致Dialog收不到WM_CHAR消息，被IsDialogMessage过滤了

	//////////////////////////////////////////////////////////////////////////
	// 循环

	theLoop.Run();
	_Module.RemoveMessageLoop();

	//////////////////////////////////////////////////////////////////////////
	// 退出 

	Gdiplus::GdiplusShutdown(m_dwToken);
	_Module.Term();
	return 0;
}