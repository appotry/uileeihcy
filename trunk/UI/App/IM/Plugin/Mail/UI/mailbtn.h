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
    virtual const TCHAR*  GetDesc();      // xxx邮箱
    virtual HBITMAP  GetIcon();           // 主窗口上的图标    
    virtual HBITMAP  GetIcon2();          // 设置窗口上的图标
    virtual void  OnCreate(UI::IButton* p);  // 创建通知
    virtual void  OnClick();

private:
    UI::IButton*   m_pBtn;   // 界面上的按钮
};