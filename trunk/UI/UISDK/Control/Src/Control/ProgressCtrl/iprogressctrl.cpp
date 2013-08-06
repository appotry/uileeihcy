#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"
#include "UISDK\Control\Src\Control\ProgressCtrl\progressctrl.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IProgressCtrl, ProgressCtrl, IControl)

int   IProgressCtrl::SetPos(int nPos, bool bUpdate) { return m_pProgressCtrlImpl->SetPos(nPos, bUpdate); }
int   IProgressCtrl::SetPage(int nPage, bool bUpdate){ return m_pProgressCtrlImpl->SetPage(nPage, bUpdate); }
void  IProgressCtrl::SetRange(int nLower, int nUpper, bool bUpdate) { m_pProgressCtrlImpl->SetRange(nLower, bUpdate); }
void  IProgressCtrl::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate){ m_pProgressCtrlImpl->SetScrollInfo(lpsi, bUpdate); }

void  IProgressCtrl::SetLine(int nLine) { m_pProgressCtrlImpl->SetLine(nLine); }
int   IProgressCtrl::GetPos() { return m_pProgressCtrlImpl->GetPos(); }
int   IProgressCtrl::GetPage(){ return m_pProgressCtrlImpl->GetPage(); }
int   IProgressCtrl::GetLine(){ return m_pProgressCtrlImpl->GetLine(); }
void  IProgressCtrl::GetRange(int& nLower, int& nUpper) { m_pProgressCtrlImpl->GetRange(nLower, nUpper); }
int   IProgressCtrl::GetRange() { return m_pProgressCtrlImpl->GetRange(); }
void  IProgressCtrl::SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_pProgressCtrlImpl->SetDirectionType(eType); }
PROGRESS_SCROLL_DIRECTION_TYPE  IProgressCtrl::GetDirectionType() { return m_pProgressCtrlImpl->GetDirectionType(); }

}