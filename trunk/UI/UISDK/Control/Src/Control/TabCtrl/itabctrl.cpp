#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\itabctrl.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrl.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrlbutton.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(ITabCtrlBase, TabCtrlBase, IPanel)
UI_IMPLEMENT_Ixxx_INTERFACE(ITabCtrl, TabCtrl, TabCtrlBase)
UI_IMPLEMENT_Ixxx_INTERFACE(ITabCtrlButton, TabCtrlButton, Button);


IPanel*  ITabCtrlBase::GetHeadPanel()
{
    return m_pTabCtrlBaseImpl->GetHeadPanel();
}
IPanel*  ITabCtrlBase::GetContentPanel()
{
    return m_pTabCtrlBaseImpl->GetContentPanel();
}

void  ITabCtrl::AddItem(const TCHAR*  szText, IObject* pContentObj) { return m_pTabCtrlImpl->AddItem(szText, pContentObj); }