#ifndef ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315
#define ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

class SliderCtrl;
interface UICTRLAPI ISliderCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ISliderCtrl, SliderCtrl);

    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine);
    int   GetPos();
    int   GetPage();
    int   GetLine();
    void  GetRange(int& nLower, int& nUpper);
    int   GetRange();

    void  SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType);
    PROGRESS_SCROLL_DIRECTION_TYPE  GetDirectionType();
    int   SetTickFreq(int nFreq);
};

}

#endif // ISLIDERCTRL_H_955F5E3E_3091_4ca4_96CD_52929B078315