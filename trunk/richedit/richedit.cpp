// richedit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "richedit.h"
#include "MainDlg.h"

// Global Variables:
CAppModule  _Module;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_Module.Init(NULL, hInstance);

	CMainDlg dlg;
	dlg.Create(NULL);
	dlg.ShowWindow(SW_SHOW);
//	dlg.DoModal();  <-- DoModal 会导致Dialog收不到WM_CHAR消息，被IsDialogMessage过滤了

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	theLoop.Run();
	_Module.RemoveMessageLoop();



	_Module.Term();
	return 0;
}