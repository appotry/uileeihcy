#include "stdafx.h"
#include "toolbaraddbtn.h"
#include "../Resource.h"
#include "UISDK/Kernel/Inc/Util/iimage.h"
#include "UISDK/Control/Inc/Interface/ibutton.h"

ToolbarAddBtn::ToolbarAddBtn()
{
    m_pButton = NULL;
}
ToolbarAddBtn::~ToolbarAddBtn()
{

}

const TCHAR*  ToolbarAddBtn::GetName()
{
    return L"add";
}
const TCHAR*  ToolbarAddBtn::GetText()
{
    return L"";
}
const TCHAR*  ToolbarAddBtn::GetDesc()
{
    return L"界面管理器";
}
HBITMAP  ToolbarAddBtn::GetIcon()
{
//     UI::IImage image;
//     image.LoadFromResource(g_hModule, IDB_PNG1, _T("PNG"));
//     return image.Detach();

    UI::IUIApplication*  pUIApp = IM::GetUIApplication();
    UI::IImageRes* pImageRes = pUIApp->GetActiveSkinImageRes();
    if (!pImageRes)
        return NULL;

    return pImageRes->LoadBitmap(L"changeskin_pluginadd");
}

HBITMAP  ToolbarAddBtn::GetIcon2()
{
    UI::IUIApplication*  pUIApp = IM::GetUIApplication();
    UI::IImageRes* pImageRes = pUIApp->GetActiveSkinImageRes();
    if (!pImageRes)
        return NULL;

    return pImageRes->LoadBitmap(L"changeskin_pluginadd");
}

void  ToolbarAddBtn::OnCreate(UI::IButton* p)
{
    m_pButton = p;
    p->AddAttribute(XML_TOOLTIP, _T("界面管理器"));
}

void  ToolbarAddBtn::OnClick()
{
    
}