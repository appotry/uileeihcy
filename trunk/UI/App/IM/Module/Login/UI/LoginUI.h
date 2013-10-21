#pragma once
#include "App\IM\include\framework_inc.h"

class CLoginDlg;
class LoginRstParam;

class CLoginUI : public IM::IUI
{
public:
    CLoginUI();
    ~CLoginUI();

    virtual long  ExecuteCommand(long lId, WPARAM wParam, LPARAM lParam);

public:
    // 点击关闭按钮
    void  Quit();

    // 创建窗口
    long  Create();

    // 显示
    long  Show();

    // 显示正在登录界面
    long  ShowLogining();

    // 关闭，销毁
    long  DestroyWindow();

    // 登录成功，销毁
    void  Destroy();

    // 点击登录
    void  Login(const TCHAR*  szAccount, const TCHAR* szPassword);

    // 登录过程点击取消
    void  CancelLogin();
    long  OnCancelLoginOk();

    long  OnLoginRst(LoginRstParam* pParam);
        
public:
    IM::IFramework*  m_pFramework;
    CLoginDlg*  m_pLoginDlg;
};

class LoginReqParam
{
public:
    LoginReqParam()
    {
        memset(szAccount, 0, sizeof(szAccount));
        memset(szPassword, 0, sizeof(szPassword));
    }
    TCHAR  szAccount[256];
    TCHAR  szPassword[256];
};
