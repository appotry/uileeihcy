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
	dlg.DoModal();

	_Module.Term();
	return 0;
}