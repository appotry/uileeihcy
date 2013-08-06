#ifndef IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421
#define IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

// 在编辑框中按下了回车键
// message: UI_WM_NOTIFY
// code:    UI_EN_RETURN
#define UI_EN_RETURN   136181700

// 在编辑框中按下了ESC键
// message: UI_WM_NOTIFY
// code:    UI_EN_RETURN
#define UI_EN_ESC      136181701

// 在编辑框中按下了tab键
// message: UI_WM_NOTIFY
// code:    UI_EN_TAB
#define UI_EN_TAB      136211621

// 在编辑框中的文本发生了变化
// message: UI_WM_NOTIFY
// code:    UI_EN_CHANGE
#define UI_EN_CHANGE   136221429

class Edit;
interface UICTRLAPI IEdit : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IEdit, Edit);

    void  SetText(const TCHAR* szText);
    const TCHAR*  GetText();

    void  SetSel(int nStartChar, int nEndChar);
    void  GetSel(int& nStartChar,int& nEndChar) const;
};

}

#endif // IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421