#pragma once
#include "App/IM/include/main_inc.h"

class MailBtn : public IM::IMainDlgToolbarPlugin
{
public:
    MailBtn();
    ~MailBtn();

protected:
    virtual const TCHAR*  GetName();      // mail
    virtual const TCHAR*  GetText();      // 99
    virtual const TCHAR*  GetDesc();      // xxx����
    virtual HBITMAP  GetIcon();           // �������ϵ�ͼ��    
    virtual HBITMAP  GetIcon2();          // ���ô����ϵ�ͼ��
    virtual void  OnCreate(UI::IButton* p);  // ����֪ͨ
    virtual void  OnClick();

private:
    UI::IButton*   m_pBtn;   // �����ϵİ�ť
};