#include "stdafx.h"
#include "LoginUI.h"
#include "App\IM\include\login_inc.h"
#include "LoginDlg.h"
#include "App\IM\Module\Login\Biz\LoginBiz.h"

CLoginUI::CLoginUI()
{
    m_pLoginDlg = NULL;

    m_pFramework = IM::GetFramework();
}

CLoginUI::~CLoginUI()
{
}

void  CLoginUI::Quit()
{
    if (m_pLoginDlg)
    {
        m_pLoginDlg->HideWindow();
    }
    IM::fun_pkg* pFuncOnClose = IM::create_mem_fun0_pkg(&CLoginBiz::OnClose, g_pLoginBiz);
    m_pFramework->PostFunction(IM::Biz, pFuncOnClose, false);
}

long  CLoginUI::Create()
{
    if (NULL == m_pLoginDlg)
    {
        CLoginDlg::CreateInstance(IM::GetUIApplication(), &m_pLoginDlg);
        m_pLoginDlg->SetLoginUI(this);

        m_pLoginDlg->Create(IM::GetUIApplication(), _T("logindlg"), NULL);
    }
    return 0;
}

long  CLoginUI::Show()
{
    if (m_pLoginDlg)
    {
        m_pLoginDlg->Show();
    }
    return 0;
}

long  CLoginUI::ShowLogining()
{
    if (m_pLoginDlg)
    {
        m_pLoginDlg->ShowLogining(NULL);
    }
    return 0;
}

long CLoginUI::DestroyWindow()
{
    if (m_pLoginDlg)
    {
        ::DestroyWindow(m_pLoginDlg->GetHWND());
        SAFE_DELETE_Ixxx(m_pLoginDlg);
    }
    return 0;
}

void CLoginUI::Destroy()
{
    IM::fun_pkg* pFuncDestroy = IM::create_mem_fun0_pkg(&CLoginUI::DestroyWindow, this);
    m_pFramework->PostFunction(IM::Ui, pFuncDestroy, false);
}

void  CLoginUI::Login(const TCHAR*  szAccount, const TCHAR* szPassword)
{
    LoginReqParam* pParam = new LoginReqParam;
    _tcsncpy(pParam->szAccount, szAccount, (sizeof(pParam->szAccount)/sizeof(TCHAR))-1);
    _tcsncpy(pParam->szPassword, szPassword, (sizeof(pParam->szPassword)/sizeof(TCHAR))-1);

    m_pFramework->PostFunction(IM::Biz, IM::create_mem_fun1_pkg(&CLoginBiz::OnLogin, g_pLoginBiz, pParam), false);
}


void CLoginUI::CancelLogin()
{
    m_pFramework->PostFunction(IM::Biz, IM::create_mem_fun0_pkg(&CLoginBiz::CancelLogin, g_pLoginBiz), false);
}
long  CLoginUI::OnCancelLoginOk()
{
    m_pLoginDlg->OnCancelLoginOk();
    return 0;
}

long  CLoginUI::ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam)
{
    if (nCommand == IM::BASE_CMD_ID_TASK)
    {
        IM::ITask* pTask = (IM::ITask*)wParam;
        switch (pTask->GetCommandId())
        {
        case LOGIN_UI_CMD_HIDE_AND_DESTROY:
            {
                m_pLoginDlg->Hide(false);
            }
            break;
        }
    }

    return 0;
}


long  CLoginUI::OnLoginRst(LoginRstParam* pParam)
{
    if (m_pLoginDlg)
    {
        m_pLoginDlg->OnLoginResult(pParam->m_lResultCode, pParam->m_szDesc);
    }
    SAFE_DELETE(pParam);
    return 0;
}
