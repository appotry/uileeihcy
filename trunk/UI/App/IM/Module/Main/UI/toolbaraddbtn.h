#pragma once
#include "App/IM/include/main_inc.h"

class ToolbarAddBtn : public IM::IMainDlgToolbarPlugin
{
public:
    ToolbarAddBtn();
    ~ToolbarAddBtn();

protected:
    virtual const TCHAR*  GetName();      // mail
    virtual const TCHAR*  GetText();      // 99
    virtual const TCHAR*  GetDesc();      // xxx����
    virtual HBITMAP  GetIcon();           // �������ϵ�ͼ��    
    virtual HBITMAP  GetIcon2();          // ���ô����ϵ�ͼ��
    virtual void  OnCreate(UI::IButton* p);  // ����֪ͨ
    virtual void  OnClick();

    UI::IButton*  m_pButton;
};