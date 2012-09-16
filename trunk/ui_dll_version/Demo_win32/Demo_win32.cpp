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
 
	map<int, int> m;
	for (int i = 0; i < 100; i++)
	{
		m[i] = i;
	}

	int nTick1 = GetTickCount();
	for (int i = 0; i < 100000; i++)
	{
		if (m.count(99))
		{
			int n = m[99];
		}
	}
	int nTick2= GetTickCount();
	int nDiff1 = nTick2 - nTick1;  // 1310

	nTick1 = GetTickCount();

	for (int i = 0; i < 100000; i++)
	{
		map<int,int>::iterator iter = m.find(99);
		if (m.end() != iter)
		{
			int n = iter->second;
		}
	}
	
	nTick2= GetTickCount();
	int nDiff2 = nTick2 - nTick1;  // 1310

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


