// Demo_win32.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Demo_win32.h"
#include "MainMgr.h"
#include "Playlistdlg.h"


// ȫ�ֱ���:
HINSTANCE g_hInstance;
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	g_hInstance = hInstance;
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	COLORREF col = RGB(16,25,24);
	WORD h = 0, s = 0, l = 0;
	::ColorRGBToHLS(col, &h, &l, &s);

	COLORREF col2 = RGB(0,255,255);
	::ColorRGBToHLS(col2, &h,&l,&s);
	
	col2 = RGB(0,0,255);
	ColorRGBToHLS(col2, &h,&l,&s);
	int a = 0;

	// ע�����пؼ�
	UI_RegisterUIObjectCreateData(TTPlayerPlaylistCtrl::XmlName(), 
		(s_UICreateInstancePtr)TTPlayerPlaylistCtrl::_CreatorClass::UICreateInstance);

	// ����Ƥ����Դ
	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	String str = szPath;
	str += _T("ttplayer.uiproj");
	if( false == UI::UI_Initialize( str  ) )
	{
		::MessageBox( NULL, _T("Error Initialize UI"), _T("Error!"), MB_OK );
		return 0;
	}

	// ����Ӧ�ó���
	::GetMainMgr();

	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO_WIN32));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// �˳�
	::GetFrameWork()->Release();
	::UI_Release();  // ע�⣬��Ҫ�����еĴ���ȫ������֮�󣬲ſ��Ե���UI_Exit�ͷ���Դ������ᵼ��Ұָ�����

	return 0;
}


