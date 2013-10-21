#include "stdafx.h"
#include "LoginBiz.h"
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\main_inc.h"
#include "App\IM\Module\Login\ui\LoginUI.h"

CLoginBiz::CLoginBiz()
{
    m_bCancelLogin = false;
    m_nTimerTest = 0;
}

CLoginBiz::~CLoginBiz()
{
}


long CLoginBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//         }
    }
    else if (nCommand == IM::BASE_CMD_ID_INIT)
    {
        ShowDialog(0);
    }

    return 0;
}


long CLoginBiz::ShowDialog(long lType)
{
    IM::fun_pkg* pfunCreate = IM::create_mem_fun0_pkg(&CLoginUI::Create, g_pLoginUI);
    IM::GetFramework()->PostFunction(IM::Ui, pfunCreate, true);

//    m_pFrameWork->PostTask(IM::CreateTask(g_pLoginUI, LOGIN_UI_CMD_INIT_CONFIG, 0, 0));

    Sleep(500); // 预留时间给ui线程的flash加载各种Dll和线程，否则会导致登录窗口的显示动画第一帧到达的时间过期

    IM::fun_pkg* pfunShow = IM::create_mem_fun0_pkg(&CLoginUI::Show, g_pLoginUI);
    IM::GetFramework()->PostFunction(IM::Ui, pfunShow, false);

    return 0;
}
long CLoginBiz::HideDialog(long lType)
{
    return 0;
}


long  CLoginBiz::OnClose()
{
    IM::fun_pkg* pfunDestroy = IM::create_mem_fun0_pkg(&CLoginUI::DestroyWindow, g_pLoginUI);
    IM::GetFramework()->PostFunction(IM::Ui, pfunDestroy, true);
    IM::GetFramework()->Exit();

    return 0;
}
VOID CALLBACK LoginServerTimerProc(HWND hwnd,
                        UINT uMsg,
                        UINT_PTR idEvent,
                        DWORD dwTime
                        )
{
    ::KillTimer(NULL, idEvent);
    g_pLoginBiz->OnLoginServerResult();
}

long  CLoginBiz::OnLogin(LoginReqParam* pParam)
{
    // 账号校验
    // ...

    // 显示登录界面
    IM::fun_pkg* pfunShowLogining = IM::create_mem_fun0_pkg(&CLoginUI::ShowLogining, g_pLoginUI);
    IM::GetFramework()->PostFunction(IM::Ui, pfunShowLogining, false);

    // 服务器校验
    // ...
    m_nTimerTest = SetTimer(NULL, 0, 3000, LoginServerTimerProc);
   // ::Sleep(3000);   // -- 模拟
    
    SAFE_DELETE(pParam);
    return 0;
}

long  CLoginBiz::CancelLogin()
{
    if (!m_nTimerTest)
        return 0;

    m_bCancelLogin = true;

    // Test
    KillTimer(NULL, m_nTimerTest);
    m_bCancelLogin = false;
    IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun0_pkg(&CLoginUI::OnCancelLoginOk, g_pLoginUI), false);

    return 0;
}

long  CLoginBiz::OnLoginServerResult()
{
    // 检测是否取消了登录 -- 模拟
    if (m_bCancelLogin)
    {
        m_bCancelLogin = false;
        IM::GetFramework()->PostFunction(IM::Ui, IM::create_mem_fun0_pkg(&CLoginUI::OnCancelLoginOk, g_pLoginUI), false);
        return 0;
    }

    // 隐藏登录窗口
    LoginRstParam* pLoginRstParam = new LoginRstParam;
    IM::GetFramework()->PostFunction(IM::Ui, create_mem_fun1_pkg(&CLoginUI::OnLoginRst, g_pLoginUI, pLoginRstParam), false);

    // 加载登录成员后的组件列表
    IM::GetFramework()->PostTask(IM::CreateTask(NULL, IM::MAIN_CMD_ID_LOGON, 0, 0));

    return 0;
}