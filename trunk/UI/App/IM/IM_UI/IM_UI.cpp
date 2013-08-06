// IM_UI.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "IM_UI.h"
#include "ui\LoginDlg.h"
#include "ui\ChatDlg.h"
#include "ui\MainDlg.h"
#include "logic\pluginmgr.h"

UI::IUIApplication* g_pUIApp = NULL;
UI::ISkinManager*   g_pSkinMgr = NULL;
ILog*  g_pLog = NULL;
long   g_lLogCookie = 0;
UI::IImageRender*   g_pShareSkinRender = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	CreateUIApplicationInstance(&g_pUIApp);
    LoadLog();                           // ������־

    if (false == LoadSkin())             // ����Ƥ��
    {
        ::MessageBox(NULL, _T("����Ƥ��ʧ��"), _T("Error"), MB_OK|MB_ICONWARNING);
        SAFE_RELEASE(g_pLog);
        g_pUIApp->Release();
        return 0;
    }
 
    UI_Ctrl_RegisterUIObject(g_pUIApp);   // ע��ؼ�
    IM_Ctrl_RegisterUIObject(g_pUIApp);   // ע��ؼ�

    long  lRet = ShowLogonDialog();
    if (IDOK == lRet)
    {
        CreateShareSkinRender();         // ��������Ƥ����Դ
        CPluginManager::Get()->LoadPlugin();
        ShowMainDialog();                // ��ʾ������
    }

    SAFE_DELETE_Ixxx(g_pShareSkinRender);
    SAFE_RELEASE(g_pLog);
	g_pUIApp->Release();
	return 0;
}

void  LoadLog()
{
    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String strLogPath = szPath;

    // ������־
    strLogPath.append(_T("IM\\im.xml"));
    g_pUIApp->SetLog(NULL, (BSTR)strLogPath.c_str());
    g_pUIApp->GetLog(&g_pLog);
    if (g_pLog)
    {
        ILogItem*  pLogItem = NULL;
        g_pLog->GetLogItemByName(_T("IM"), &pLogItem);
        if (pLogItem)
        {
            pLogItem->GetCookie(&g_lLogCookie);
        }
    }
}

bool  LoadSkin()
{
    g_pSkinMgr = g_pUIApp->GetSkinMgr();

    TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
    String str = szPath;
    str.append(_T("IM\\skin"));
    g_pUIApp->SetSkinDirection(str.c_str());

    if (FAILED(g_pUIApp->LoadSkin(_T("2012"))))
        return false;

    return true;
}

UINT  ShowLogonDialog()
{
    UINT  lRet = IDOK;
#if 1
    CLoginDlg*  pLogindlg = NULL;
    CLoginDlg::CreateInstance(g_pUIApp, &pLogindlg);
    lRet = pLogindlg->DoModal(g_pUIApp, _T("logindlg"), NULL);
    
    SAFE_DELETE_Ixxx(pLogindlg);
#elif 1
  	CChatDlg*  pChatDlg = NULL;
    CChatDlg::CreateInstance(g_pUIApp, &pChatDlg);
  	pChatDlg->DoModal(g_pUIApp, _T("chatdlg"), NULL);

    SAFE_DELETE_Ixxx(pChatDlg);
    lRet = IDCANCEL;
#endif

    return lRet;
}

void  ShowMainDialog()
{
    CMainDlg*  pMainDlg = NULL;
    CMainDlg::CreateInstance(g_pUIApp, &pMainDlg);
    pMainDlg->DoModal(g_pUIApp, _T("maindlg"), NULL);
    SAFE_DELETE_Ixxx(pMainDlg);
}

// material.render.type="image"
// material.render.image="skin"
// material.render.region="6"
// material.render.image.drawtype="bitbltrighttop"
void  CreateShareSkinRender()
{
    if (NULL == g_pUIApp)
        return;

    UI::IRenderBase*  pRenderBase = NULL;
    g_pUIApp->CreateRenderBase(UI::RENDER_TYPE_IMAGE, NULL, &pRenderBase);
    if (NULL == pRenderBase)
        return;

    g_pShareSkinRender = (UI::IImageRender*)pRenderBase->QueryInterface(UI::uiiidof(IImageRender));

    UI::IMapAttribute*  pMapAttrib = NULL;
    UI::UICreateIMapAttribute(&pMapAttrib);
    pMapAttrib->AddAttr(_T("render.image"), _T("skin"));
    pMapAttrib->AddAttr(_T("render.image.drawtype"), _T("stretch"));  // Ĭ��Ƥ��������ģʽ _T("bitbltrighttop")
    pMapAttrib->AddAttr(_T("render.type"), _T("image"));
    g_pShareSkinRender->SetAttribute(pMapAttrib, NULL, true);
    
    SAFE_RELEASE(pMapAttrib);
}