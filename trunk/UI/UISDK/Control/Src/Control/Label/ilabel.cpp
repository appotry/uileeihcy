#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Control\Src\Control\Label\label.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(ILabel, Label, IControl);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPictureCtrl, PictureCtrl, IControl);
UI_IMPLEMENT_Ixxx_INTERFACE2(IGifCtrl, GifCtrl, IControl);

const TCHAR*  ILabel::GetText() { return m_pLabelImpl->GetText(); }
void  ILabel::SetText(const TCHAR* pszText, bool bRedraw) { m_pLabelImpl->SetText(pszText, bRedraw); }


bool  IGifCtrl::Start() { return m_pGifCtrlImpl->Start(); }
bool  IGifCtrl::Pause() { return m_pGifCtrlImpl->Pause(); }
bool  IGifCtrl::Stop()  { return m_pGifCtrlImpl->Stop(); }
}