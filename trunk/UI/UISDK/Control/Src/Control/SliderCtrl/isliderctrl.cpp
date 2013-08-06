#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Control\Src\Control\SliderCtrl\sliderctrl.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE2(ISliderCtrl, SliderCtrl, IControl)

int   ISliderCtrl::SetPos(int nPos, bool bUpdate) { return m_pSliderCtrlImpl->SetPos(nPos, bUpdate); }
int   ISliderCtrl::SetPage(int nPage, bool bUpdate){ return m_pSliderCtrlImpl->SetPage(nPage, bUpdate); }
void  ISliderCtrl::SetRange(int nLower, int nUpper, bool bUpdate) { m_pSliderCtrlImpl->SetRange(nLower, nUpper, bUpdate); }
void  ISliderCtrl::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate){ m_pSliderCtrlImpl->SetScrollInfo(lpsi, bUpdate); }

void  ISliderCtrl::SetLine(int nLine) { m_pSliderCtrlImpl->SetLine(nLine); }
int   ISliderCtrl::GetPos() { return m_pSliderCtrlImpl->GetPos(); }
int   ISliderCtrl::GetPage(){ return m_pSliderCtrlImpl->GetPage(); }
int   ISliderCtrl::GetLine(){ return m_pSliderCtrlImpl->GetLine(); }
void  ISliderCtrl::GetRange(int& nLower, int& nUpper) { m_pSliderCtrlImpl->GetRange(nLower, nUpper); }
int   ISliderCtrl::GetRange() { return m_pSliderCtrlImpl->GetRange(); }
void  ISliderCtrl::SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_pSliderCtrlImpl->SetDirectionType(eType); }
PROGRESS_SCROLL_DIRECTION_TYPE  ISliderCtrl::GetDirectionType() { return m_pSliderCtrlImpl->GetDirectionType(); }
int   ISliderCtrl::SetTickFreq(int nFreq) { return m_pSliderCtrlImpl->SetTickFreq(nFreq); }
}