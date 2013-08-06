#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(ISystemScrollBar, SystemScrollBar, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(ISystemHScrollBar, SystemHScrollBar, SystemScrollBar)
UI_IMPLEMENT_Ixxx_INTERFACE(ISystemVScrollBar, SystemVScrollBar, SystemScrollBar)

void  ISystemScrollBar::SetIScrollBarMgr(IScrollBarManager* p) { m_pSystemScrollBarImpl->SetIScrollBarMgr(p); }

}
