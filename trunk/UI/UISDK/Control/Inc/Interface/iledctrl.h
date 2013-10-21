#ifndef ILEDCTRL_H_83C9A3B2_9690_4262_8435_7161EF5B7754
#define ILEDCTRL_H_83C9A3B2_9690_4262_8435_7161EF5B7754
#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

namespace UI
{

class LEDCtrl;
interface UICTRLAPI ILEDCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ILEDCtrl, LEDCtrl)

    void  SetIndexMap(const TCHAR* szText);
    void  SetText(const TCHAR* szText, bool bUpdate=true);
};

struct LEDEX_ITEM_INFO
{
    TCHAR  character;
    int nPos;
    int nWidth;
};
class LEDExCtrl;
interface UICTRLAPI ILEDExCtrl : public IButtonBase
{
    UI_DECLARE_Ixxx_INTERFACE(ILEDExCtrl, LEDExCtrl)

    void  SetItemInfo(LEDEX_ITEM_INFO* pInfoArray, int nCount);
    void  SetText(const TCHAR* szText, bool bUpdate=true);
};

}

#endif // ILEDCTRL_H_83C9A3B2_9690_4262_8435_7161EF5B7754