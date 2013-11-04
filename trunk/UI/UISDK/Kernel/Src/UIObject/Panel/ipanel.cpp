#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(IPanel, Panel, Object)

ILayout* IPanel::GetLayout()                    { return m_pPanelImpl->GetLayout(); }
void  IPanel::SetLayoutType(LAYOUTTYPE eLayoutType)  { m_pPanelImpl->SetLayoutType(eLayoutType); }
void  IPanel::SetMaterialRender(IRenderBase* p) { m_pPanelImpl->SetMaterialRender(p); }
IRenderBase*  IPanel::GetMaterialRender()       { return m_pPanelImpl->GetMaterialRender(); }
}