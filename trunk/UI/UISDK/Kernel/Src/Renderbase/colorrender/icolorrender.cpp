#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Kernel\Src\Renderbase\colorrender\colorrender.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(IColorRender, ColorRender, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(ISysColorRender, SysColorRender, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IGradientRender, GradientRender, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IColorListRender, ColorListRender, RenderBase)

void  IColorRender::SetBkColor(COLORREF col)
{
    return m_pColorRenderImpl->SetBkColor(col);
}
void  IColorRender::SetBorderColor(COLORREF col)
{
    return m_pColorRenderImpl->SetBorderColor(col);
}
void  IColorRender::SetBorderRegion(const CRegion4* prc)
{
    m_pColorRenderImpl->SetBorderRegion(prc);
}
//////////////////////////////////////////////////////////////////////////

void  IColorListRender::SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder)
{  
    return m_pColorListRenderImpl->SetStateColor(nState, colorBk, bSetBk, colBorder, bSetBorder);
}
void  IColorListRender::SetCount(int n)
{
    m_pColorListRenderImpl->SetCount(n);
}

void  ISysColorRender::SetBkColor(int nColorIndex) { m_pSysColorRenderImpl->SetBkColor(nColorIndex); }
void  ISysColorRender::SetBorderColor(int nColorIndex) { m_pSysColorRenderImpl->SetBorderColor(nColorIndex); }

}