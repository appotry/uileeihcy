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
	// ��ʼ��

	_Module.Init(NULL, hInstance);

	DWORD m_dwToken = 0;
	Gdiplus::GdiplusStartupInput input;
	Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, NULL );

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	//////////////////////////////////////////////////////////////////////////
	// ��ʾ

	CMainDlg dlg;
	dlg.Create(NULL);
	dlg.ShowWindow(SW_SHOW);
//	dlg.DoModal();  <-- DoModal �ᵼ��Dialog�ղ���WM_CHAR��Ϣ����IsDialogMessage������

	//////////////////////////////////////////////////////////////////////////
	// ѭ��

	theLoop.Run();
	_Module.RemoveMessageLoop();

	//////////////////////////////////////////////////////////////////////////
	// �˳� 

	Gdiplus::GdiplusShutdown(m_dwToken);
	_Module.Term();
	return 0;
}