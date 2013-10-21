#ifndef IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421
#define IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

// �ڱ༭���а����˻س���
// message: UI_WM_NOTIFY
// code:    UI_EN_RETURN
#define UI_EN_RETURN   136181700

// �ڱ༭���а�����ESC��
// message: UI_WM_NOTIFY
// code:    UI_EN_RETURN
#define UI_EN_ESC      136181701

// �ڱ༭���а�����tab��
// message: UI_WM_NOTIFY
// code:    UI_EN_TAB
#define UI_EN_TAB      136211621

// �ڱ༭���е��ı������˱仯
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

    bool  IsReadOnly();
    void  SetReadOnly(bool b, bool bUpdate);
};

}

#endif // IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421