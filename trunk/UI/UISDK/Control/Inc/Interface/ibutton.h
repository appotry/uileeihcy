#ifndef IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A
#define IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{


// Virtual 扩展，用于派生类中绘制焦点状态
//
//		message:UI_WM_NOTIFY
//		code:   UI_BN_DRAWFOCUS
//		wparam: IRenderTarget*
//		lParam: NA
//
#define UI_BN_DRAWFOCUS  139222332

//
//	按钮右击事件
// 
#define UI_BN_RCLICK     139222334

	// void OnBtnRClick( );
#define UIMSG_BN_RCLICK2(ptr, func)                   \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_BN_RCLICK &&                       \
        pMsgFrom == (UI::IMessage*)ptr)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( );                                      \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

	// void  OnBnRClick(IMessage*  pMsgFrom);
#define UIMSG_BN_RCLICK3(func)                        \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_BN_RCLICK)                         \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func(pMsgFrom);                               \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }
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

    void  SetText(const TCHAR*  szText, bool bUpdate=true);
    const TCHAR* GetText();
    
    void  SetIconFromFile(const TCHAR* szIconPath);
    void  SetIconFromHBITMAP(HBITMAP hBitmap);   // 注：该HBITMAP最后由内部销毁

    void  SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
    void  SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType);
};

}

#endif // IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A