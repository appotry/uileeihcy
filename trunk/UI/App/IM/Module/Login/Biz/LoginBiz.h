#pragma once
#include "App\IM\include\login_inc.h"
class LoginReqParam;

class CLoginBiz : public IM::ILoginBiz
{
public:
    CLoginBiz();
    ~CLoginBiz();

public:
    virtual long  ExecuteCommand(long lId, WPARAM wParam, LPARAM lParam);

    virtual long  ShowDialog(long lType);
    virtual long  HideDialog(long lType);

public:
    long  OnClose();
    long  OnLogin(LoginReqParam* pParam);
    long  CancelLogin();
    long  OnLoginServerResult();

private:
    bool  m_bCancelLogin;
    UINT_PTR  m_nTimerTest;
    
};



class LoginRstParam
{
public:
    enum LoginRstCode
    {
        SUCCESS,
        INVALID_ACCOUNT,
        INVALID_PASSWORD,
        ACCOUNT_NOT_EXIST,
        PASSWORD_ERROR,
    };

    LoginRstParam()
    {
        m_lResultCode = SUCCESS;
        memset(m_szDesc, 0, sizeof(m_szDesc));
    }

    LoginRstCode  m_lResultCode;  
    TCHAR  m_szDesc[256];
};