#include "stdafx.h"
#include "mainbiz.h"
#include "App\IM\include\task.h"
#include "App\IM\Module\Main\ui\mainui.h"
#include "App\IM\include\privatechat_inc.h"
#include "App\IM\include\login_inc.h"

CMainBiz::CMainBiz()
{
    m_pPrivateChatBiz = NULL;
    m_pFrameWork = IM::GetFramework();
}
CMainBiz::~CMainBiz()
{
}

// 退出
long  CMainBiz::OnQuit()
{
    m_pFrameWork->PostFunction(IM::Ui, IM::create_mem_fun0_pkg(&CMainUI::DestroyWindow, g_pMainUI), true);
    m_pFrameWork->Exit();
    return 0;
}

long  CMainBiz::ExecuteCommand( long nCommand, WPARAM wParam, LPARAM lParam )
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
//         switch (pTask->GetCommandId())
//         {
//         case MAIN_BIZ_CMD_QUIT:
//             {
//                 
//             }
//             break;
//         }
    }
    else if (IM::BASE_CMD_ID_INIT == nCommand)
    {
        // 加载主界面
        ShowDialog();
    }

    return 0;
}

long  CMainBiz::ShowDialog()
{
    IM::fun_pkg* pfunCreate = IM::create_mem_fun0_pkg(&CMainUI::Create, g_pMainUI);
    m_pFrameWork->PostFunction(IM::Ui, pfunCreate, true);

    IM::fun_pkg* pfunShow = IM::create_mem_fun0_pkg(&CMainUI::Show, g_pMainUI);
    m_pFrameWork->PostFunction(IM::Ui, pfunShow, false);

    IM::IUI* pLoginUI = m_pFrameWork->GetUI(MODULE_ID_LOGIN);
    m_pFrameWork->PostTask(IM::CreateTask(pLoginUI, LOGIN_UI_CMD_HIDE_AND_DESTROY, 0, 0));

    return 0;
}
long  CMainBiz::HideDialog()
{
    return 0;
}

long  CMainBiz::OpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* p)
{
    if (!m_pPrivateChatBiz)
    {
        m_pPrivateChatBiz = (IM::IPrivateChatBiz*)IM::GetFramework()->GetBiz(MODULE_ID_PRIVATECHAT);
    
        if (!m_pPrivateChatBiz)
        {
            IM::GetFramework()->LoadModuleByName(MODULE_ID_PRIVATECHAT, NULL, (IM::IBiz**)&m_pPrivateChatBiz);
        }
    }

    if (m_pPrivateChatBiz)
    {
        p->AddRef();
        m_pPrivateChatBiz->OpenPrivateChatDlg(p);
    }
    SAFE_RELEASE(p);
    return 0;
}