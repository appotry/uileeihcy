// richedit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "richedit.h"

// Global Variables:
CAppModule  _Module;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_Module.Init(NULL, hInstance);

	CSimpleDialog<IDD_ABOUTBOX> dlg;
	dlg.DoModal();

	_Module.Term();
	return 0;
}