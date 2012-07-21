// UIBuilder.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "UIBuilder.h"
#include "MainFrame.h"

// ȫ�ֱ���:
CAppModule      _Module;
//CUIBuilderMgr   g_uiBuilderMgr;
ILogPtr         g_pLog = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	::CoInitialize(0);
	_Module.Init( NULL, hInstance );

	CMessageLoop  theLoop;
	_Module.AddMessageLoop(&theLoop);
	
	// ��־��ʼ��
	HRESULT hr = S_OK;
	if ( NULL == g_pLog )
	{
 		hr = g_pLog.CreateInstance( __uuidof(Log) );
 		if( FAILED(hr) )
 		{
 			MessageBox(NULL, _T("��ʼ����־ģ��ʧ��"), _T(""), 0 );
				return false;
 		}
	}

	// ����
	CMainFrame  frameWnd;
	frameWnd.Create( NULL, NULL, _T("UI Builder"), WS_OVERLAPPEDWINDOW );
	frameWnd.ShowWindow(SW_SHOWMAXIMIZED);

	theLoop.Run();

	_Module.RemoveMessageLoop();
	_Module.Term();
	
	if( NULL != g_pLog )
		g_pLog.Release();

	::CoUninitialize();
	return (int) 0;
}

