#include "stdafx.h"
#include "App\Player\Player_Ctrls\Inc\imultilabel.h"
#include "App\Player\Player_Ctrls\Src\MultiLabel\multilabel.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(IMultiLabel, MultiLabel, IControl)

void  IMultiLabel::SetContent(const TCHAR* szContent, const TCHAR szSeparator) { return m_pMultiLabelImpl->SetContent(szContent, szSeparator); }
void  IMultiLabel::StartAnimateSwitch(int nTime) { return m_pMultiLabelImpl->StartAnimateSwitch(nTime); }
void  IMultiLabel::StopAnimateSwitch() { return m_pMultiLabelImpl->StopAnimateSwitch(); }