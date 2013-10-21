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
    // ����رհ�ť
    void  Quit();

    // ��������
    long  Create();

    // ��ʾ
    long  Show();

    // ��ʾ���ڵ�¼����
    long  ShowLogining();

    // �رգ�����
    long  DestroyWindow();

    // ��¼�ɹ�������
    void  Destroy();

    // �����¼
    void  Login(const TCHAR*  szAccount, const TCHAR* szPassword);

    // ��¼���̵��ȡ��
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
