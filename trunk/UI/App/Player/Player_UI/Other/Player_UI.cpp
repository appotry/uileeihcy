// Demo_win32.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Player_UI.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "App\Player\Player_UI\Mgr\EqualizerMgr.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
#include "App\Player\Player_UI\Mgr\Lyric\LyricMgr.h"
#include "App\Player\Player_UI\UI\Playlistdlg.h"
#include "App\Player\Player_UI\UI\LyricDlg.h"

// 全局变量:
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
    // 启用内存泄露
	g_hInstance = hInstance;
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // 加载配置文件
    GetFrameWork();

    // 创建皮肤模块
	CreateUIApplicationInstance(&g_pUIApp);
	g_pSkinMgr = g_pUIApp->GetSkinMgr();

	TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String str = szPath;
    String strLogPath = szPath;

    // 加载日志
    strLogPath.append(_T("TTPlayer\\player.xml"));
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

    // 加载皮肤
    str.append(_T("TTPlayer\\skin"));
    g_pUIApp->SetSkinDirection(str.c_str());

    if (FAILED(g_pUIApp->LoadSkin(GetConfigData()->skin.m_strActiveSkinName.c_str())))
    {
        if (FAILED(g_pUIApp->LoadSkin(_T("mediaplay"))))
        {
            ::MessageBox(NULL, _T("加载皮肤失败"), _T("Error"), MB_OK|MB_ICONWARNING);

            // 退出
            SAFE_RELEASE(g_pLog);
            ::GetFrameWork()->Release();
            g_pUIApp->Release();

            return 0;
        }
    }    

    // 注册控件
    UI_Ctrl_RegisterUIObject(g_pUIApp);
    TTPlayer_Ctrl_RegisterUIObject(g_pUIApp);

    // 初始化应用程序
    GetMainMgr();
    GetPlayerListMgr();
    GetEqualizerMgr();
    GetLyricMgr();

    // 显示窗口
    ::ShowWindow(GetMainMgr()->GetMainWnd(), SW_SHOW);
    GetPlayerListMgr()->ToggleShowPlayerListDlg();
    GetEqualizerMgr()->ToggleShowEqualizerDlg();
    GetLyricMgr()->ToggleShowLyricDlg();

    ::UpdateWindow(GetMainMgr()->GetMainWnd());
    ::UpdateWindow(GetPlayerListMgr()->GetPalyListDlg()->GetHWND());
    ::UpdateWindow(GetEqualizerMgr()->GetEqualizerDlg()->GetHWND());
    ::UpdateWindow(GetLyricMgr()->GetLyricDlg()->GetHWND());

    // 等界面全部显示完成后，再加载数据
    UI::TimerItem  item;
    item.nRepeatCount = 1;
    item.pProc = InitialTimerProc;
    g_pUIApp->SetTimer(50, &item);

    // 消息泵
	g_pUIApp->MsgHandleLoop();

	// 退出
    SAFE_RELEASE(g_pLog);
	::GetFrameWork()->Release();
	g_pUIApp->Release();  // 注意，需要在所有的窗口全析构了之后，才可以调用UI_Exit释放资源，否则会导致野指针崩溃

	return 0;
}

void  InitialTimerProc(UINT_PTR, UI::TimerItem*  pItem)
{
    GetMainMgr()->Initialize2();
}


// 	MSG msg;
// 	HACCEL hAccelTable;
// 	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO_WIN32));

// 主消息循环:
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