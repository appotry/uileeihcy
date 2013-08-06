#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledctrl.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(ILEDCtrl, LEDCtrl, IControl)

void  ILEDCtrl::SetIndexMap(const TCHAR* szText) { return m_pLEDCtrlImpl->SetIndexMap(szText); }
void  ILEDCtrl::SetText(const TCHAR* szText, bool bUpdate) { return m_pLEDCtrlImpl->SetText(szText, bUpdate); }

}