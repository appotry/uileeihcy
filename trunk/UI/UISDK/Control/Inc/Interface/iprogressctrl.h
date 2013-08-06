#ifndef IPROGRESSCTRL_H_F31EC133_08A9_445f_8945_A669A163A5F0
#define IPROGRESSCTRL_H_F31EC133_08A9_445f_8945_A669A163A5F0
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

class ProgressCtrl;
interface UICTRLAPI IProgressCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IProgressCtrl, ProgressCtrl)

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
};

}

#endif // IPROGRESSCTRL_H_F31EC133_08A9_445f_8945_A669A163A5F0