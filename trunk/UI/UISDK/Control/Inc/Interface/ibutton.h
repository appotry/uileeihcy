#ifndef IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A
#define IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{
class ButtonBase;
interface UICTRLAPI IButtonBase : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IButtonBase, ButtonBase)

    void  SetCheck(int nCheckState);
    void  SetChecked();
    void  SetUnChecked();
    int   GetCheck();
    bool  IsChecked();
    void  SetButtonStyle(int n);
};

class Button;
interface UICTRLAPI IButton : public IButtonBase
{
    UI_DECLARE_Ixxx_INTERFACE(IButton, Button);

    void  SetText(const TCHAR*  szText);
    const TCHAR* GetText();
    
    void  SetIconFromFile(const TCHAR* szIconPath);

    void  SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
    void  SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType);
};

}

#endif // IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A