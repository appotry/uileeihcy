// HourGlass.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "HourGlass.h"
#include "mainwnd.h"
#include "config.h"

UI::IUIApplication* g_pUIApp = NULL;
bool  LoadSkin();
CMainWnd*  g_pMainWnd = NULL;

CMainWnd*  GetMainWnd()
{
	return g_pMainWnd;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

    CreateUIApplicationInstance(&g_pUIApp);

    if (false == LoadSkin())             // ¼ÓÔØÆ¤·ô
    {
        ::MessageBox(NULL, _T("¼ÓÔØÆ¤·ôÊ§°Ü"), _T("Error"), MB_OK|MB_ICONWARNING);
        g_pUIApp->Release();
        return 0;
    }

	// ¼ÓÔØÅäÖÃ
	GetConfig()->Load();

	CMainWnd::CreateInstance(g_pUIApp, &g_pMainWnd);
	g_pMainWnd->Create(g_pUIApp, _T("MainWnd"));
	g_pMainWnd->ShowWindow();

	g_pUIApp->MsgHandleLoop();
	SAFE_DELETE_Ixxx(g_pMainWnd);

    GetConfig()->Save();
    g_pUIApp->Release();
    return 0;
}

bool  LoadSkin()
{
    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String str = szPath;
    str.append(_T("skin"));

    String strLog = str;
    strLog.append(_T("\\Default\\skin.xml"));
    g_pUIApp->SetLog(NULL, strLog.c_str());

    g_pUIApp->SetSkinDirection(str.c_str());

    if (FAILED(g_pUIApp->LoadSkin(_T("Default"))))
        return false;

    UI_Ctrl_RegisterUIObject(g_pUIApp);
    return true;
}
