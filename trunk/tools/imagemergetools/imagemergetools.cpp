// imagemergetools.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "imagemergetools.h"
#include "MainDlg.h"

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