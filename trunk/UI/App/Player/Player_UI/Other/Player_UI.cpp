// Demo_win32.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Player_UI.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "App\Player\Player_UI\Mgr\EqualizerMgr.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
#include "App\Player\Player_UI\Mgr\Lyric\LyricMgr.h"
#include "App\Player\Player_UI\UI\Playlistdlg.h"
#include "App\Player\Player_UI\UI\LyricDlg.h"

// ȫ�ֱ���:
HINSTANCE g_hInstance;
UI::IUIApplication* g_pUIApp = NULL;
UI::ISkinManager*   g_pSkinMgr = NULL;
ILog*  g_pLog = NULL;
long   g_lLogCookie = 0;

void  InitialTimerProc(UINT_PTR, UI::TimerItem*  pItem);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    // �����ڴ�й¶
	g_hInstance = hInstance;
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // ���������ļ�
    GetFrameWork();

    // ����Ƥ��ģ��
	CreateUIApplicationInstance(&g_pUIApp);
	g_pSkinMgr = g_pUIApp->GetSkinMgr();

	TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String str = szPath;
    String strLogPath = szPath;

    // ������־
    strLogPath.append(_T("player.xml"));
    g_pUIApp->SetLog(NULL, (BSTR)strLogPath.c_str());
    g_pUIApp->GetLog(&g_pLog);
    if (g_pLog)
    {
        ILogItem*  pLogItem = NULL;
        g_pLog->GetLogItemByName(_T("TTPlayer"), &pLogItem);
        if (pLogItem)
        {
            pLogItem->GetCookie(&g_lLogCookie);
        }
    }

    // ����Ƥ��
    str.append(_T("skin"));
    g_pUIApp->SetSkinDirection(str.c_str());

    if (FAILED(g_pUIApp->LoadSkin(GetConfigData()->skin.m_strActiveSkinName.c_str())))
    {
        if (FAILED(g_pUIApp->LoadSkin(_T("mediaplay"))))
        {
            ::MessageBox(NULL, _T("����Ƥ��ʧ��"), _T("Error"), MB_OK|MB_ICONWARNING);

            // �˳�
            SAFE_RELEASE(g_pLog);
            ::GetFrameWork()->Release();
            g_pUIApp->Release();

            return 0;
        }
    }    

    // ע��ؼ�
    UI_Ctrl_RegisterUIObject(g_pUIApp);
    TTPlayer_Ctrl_RegisterUIObject(g_pUIApp);

    // ��ʼ��Ӧ�ó���
    GetMainMgr();
    GetPlayerListMgr();
    GetEqualizerMgr();
    GetLyricMgr();

    // ��ʾ����
    ::ShowWindow(GetMainMgr()->GetMainWnd(), SW_SHOW);
    GetPlayerListMgr()->ToggleShowPlayerListDlg();
    GetEqualizerMgr()->ToggleShowEqualizerDlg();
    GetLyricMgr()->ToggleShowLyricDlg();

    ::UpdateWindow(GetMainMgr()->GetMainWnd());
    ::UpdateWindow(GetPlayerListMgr()->GetPalyListDlg()->GetHWND());
    ::UpdateWindow(GetEqualizerMgr()->GetEqualizerDlg()->GetHWND());
    ::UpdateWindow(GetLyricMgr()->GetLyricDlg()->GetHWND());

    // �Ƚ���ȫ����ʾ��ɺ��ټ�������
    UI::TimerItem  item;
    item.nRepeatCount = 1;
    item.pProc = InitialTimerProc;
    g_pUIApp->SetTimer(50, &item);

    // ��Ϣ��
	g_pUIApp->MsgHandleLoop();

	// �˳�
    SAFE_RELEASE(g_pLog);
	::GetFrameWork()->Release();
	g_pUIApp->Release();  // ע�⣬��Ҫ�����еĴ���ȫ������֮�󣬲ſ��Ե���UI_Exit�ͷ���Դ������ᵼ��Ұָ�����

	return 0;
}

void  InitialTimerProc(UINT_PTR, UI::TimerItem*  pItem)
{
    GetMainMgr()->Initialize2();
}


// 	MSG msg;
// 	HACCEL hAccelTable;
// 	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO_WIN32));

// ����Ϣѭ��:
// 	while (GetMessage(&msg, NULL, 0, 0))
// 	{
// 		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
// 		{
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 	}

//	g_pUIApp->UseInnerTooltipsUI(_T("control_tooltips"));




#define TTPLAYER_LOG_LEVEL(l,c)     LOG_LEVEL(g_pLog, g_lLogCookie,l,c)
//#define MOUSE_LOG_LEVEL(l,c)  if (NULL == g_pUIApplication) return ; LOG_LEVEL( g_pUIApplication->m_pLog, _T("mouse"),l,c )

void TTPLAYER_LOG_DEBUG( TCHAR* szContent, ... ) { TTPLAYER_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent ); }
void TTPLAYER_LOG_INFO( TCHAR* szContent, ... )  { TTPLAYER_LOG_LEVEL(LOG_LEVEL_INFO, szContent ); }
void TTPLAYER_LOG_WARN( TCHAR* szContent, ... )  { TTPLAYER_LOG_LEVEL(LOG_LEVEL_WARN, szContent ); }
void TTPLAYER_LOG_ERROR( TCHAR* szContent, ... ) { TTPLAYER_LOG_LEVEL(LOG_LEVEL_ERROR, szContent ); }
void TTPLAYER_LOG_FATAL( TCHAR* szContent, ...)  { TTPLAYER_LOG_LEVEL(LOG_LEVEL_FATAL, szContent ); }