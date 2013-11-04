#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(ICustomWindow, CustomWindow, Window)

void  ICustomWindow::SetWindowResizeType(UINT nType)           { m_pCustomWindowImpl->SetWindowResizeType(nType); }
bool  ICustomWindow::IsWindowLayered()                         { return m_pCustomWindowImpl->IsWindowLayered(); }
void  ICustomWindow::SetWindowLayered(bool b)                  { m_pCustomWindowImpl->SetWindowLayered(b); }
int   ICustomWindow::GetWindowTransparentMaskType()            { return m_pCustomWindowImpl->GetWindowTransparentMaskType(); }
void  ICustomWindow::SetWindowTransparentMaskType(int type)    { m_pCustomWindowImpl->SetWindowTransparentMaskType(type); }
void  ICustomWindow::SetWindowTransparentColMask(COLORREF col) { m_pCustomWindowImpl->SetWindowTransparentColMask(col); }
void  ICustomWindow::SetWindowTransparentAlphaMask(int nAlpha) { m_pCustomWindowImpl->SetWindowTransparentAlphaMask(nAlpha); }
}