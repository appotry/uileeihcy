#ifndef ICOLORRENDER_H_3886C7D7_40C7_44fd_B3E5_2E4E1C9417E3
#define ICOLORRENDER_H_3886C7D7_40C7_44fd_B3E5_2E4E1C9417E3
#include "UISDK\Kernel\Inc\Interface\irenderbase.h"

namespace UI
{

class ColorRender;
interface UISDKAPI IColorRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IColorRender, ColorRender)

    void  SetBkColor(COLORREF col);
    void  SetBorderColor(COLORREF col);
    void  SetBorderRegion(const CRegion4* prc);

};

//////////////////////////////////////////////////////////////////////////

class SysColorRender;
interface UISDKAPI ISysColorRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(ISysColorRender, SysColorRender);

    void  SetBkColor(int nColorIndex);
    void  SetBorderColor(int nColorIndex);

};

//////////////////////////////////////////////////////////////////////////

class GradientRender;
interface UISDKAPI IGradientRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IGradientRender, GradientRender)
};

//////////////////////////////////////////////////////////////////////////

class ColorListRender;
interface UISDKAPI IColorListRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IColorListRender, ColorListRender)

    void  SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);
    void  SetCount(int n);
};
}

#endif // ICOLORRENDER_H_3886C7D7_40C7_44fd_B3E5_2E4E1C9417E3