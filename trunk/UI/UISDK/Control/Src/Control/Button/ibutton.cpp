#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "button.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IButtonBase, ButtonBase, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(IButton, Button, ButtonBase)

void  IButtonBase::SetCheck(int nCheckState)
{
    m_pButtonBaseImpl->SetCheck(nCheckState);
}
void  IButtonBase::SetChecked()
{
    m_pButtonBaseImpl->SetChecked();
}
void  IButtonBase::SetUnChecked()
{
    m_pButtonBaseImpl->SetUnChecked();
}
int   IButtonBase::GetCheck()
{
    return m_pButtonBaseImpl->GetCheck();
}
bool  IButtonBase::IsChecked()
{
    return m_pButtonBaseImpl->IsChecked();
}
void  IButtonBase::SetButtonStyle(int n)
{
    m_pButtonBaseImpl->SetButtonStyle(n);
}
//////////////////////////////////////////////////////////////////////////


void  IButton::SetIconFromFile(const TCHAR* szIconPath) { m_pButtonImpl->SetIconFromFile(szIconPath); }
void  IButton::SetIconFromHBITMAP(HBITMAP hBitmap) { m_pButtonImpl->SetIconFromHBITMAP(hBitmap); }
void  IButton::SetText(const TCHAR*  szText, bool bUpdate) { m_pButtonImpl->SetText(szText, bUpdate); }
const TCHAR*  IButton::GetText() { return m_pButtonImpl->GetText(); }
void  IButton::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType) { m_pButtonImpl->SetDrawFocusType(eType); }
void  IButton::SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType) { m_pButtonImpl->SetAutoSizeType(eType); }
}