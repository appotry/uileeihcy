#include "stdafx.h"
#include "mailbtn.h"
#include "../Resource.h"
#include "UISDK/Kernel/Inc/Util/iimage.h"
#include "UISDK/Control/Inc/Interface/ibutton.h"

MailBtn::MailBtn()
{
    m_pBtn = NULL;
}
MailBtn::~MailBtn()
{
}

const TCHAR*  MailBtn::GetName()
{
    return L"mail";
}
const TCHAR*  MailBtn::GetText()
{
    return L"我的邮箱";
}
const TCHAR*  MailBtn::GetDesc()
{
    return L"我的邮箱";
}
HBITMAP  MailBtn::GetIcon()
{
    UI::IImage image;
    image.LoadFromResource(g_hModule, IDB_PNG1, _T("PNG"));
    return image.Detach();
}
HBITMAP  MailBtn::GetIcon2()
{
    UI::IImage image;
    image.LoadFromResource(g_hModule, IDB_PNG2, _T("PNG"));
    return image.Detach();
}
void  MailBtn::OnCreate(UI::IButton* p)
{
    m_pBtn = p;
    p->SetText(L"10", false);
    p->AddAttribute(XML_TOOLTIP, L"我的邮箱\n  收件箱(10)");
    p->UpdateLayout(false);
}
void  MailBtn::OnClick()
{
    ShellExecute(NULL, _T("open"), _T("http://mail.163.com"), NULL, NULL, SW_SHOW);
}