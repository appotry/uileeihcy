#ifndef ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D
#define ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D

#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
#define WRSB_TOP              0x8000     // ¡ü
#define WRSB_LEFT             0x4000     // ¡û
#define WRSB_BOTTOM           0x2000     // ¡ý
#define WRSB_RIGHT            0x1000     // ¡ú
#define WRSB_TOPLEFT          0x0800     // ¨I
#define WRSB_TOPRIGHT         0x0400     // ¨J
#define WRSB_BOTTOMLEFT       0x0200     // ¨L
#define WRSB_BOTTOMRIGHT      0x0100     // ¨K
#define WRSB_CAPTION          0x0080     // 
#define WRSB_ALL              WRSB_CAPTION | WRSB_TOP | WRSB_LEFT | WRSB_BOTTOM | WRSB_RIGHT | WRSB_TOPLEFT | WRSB_TOPRIGHT | WRSB_BOTTOMLEFT | WRSB_BOTTOMRIGHT
#define WRSB_NONE             0x0

class CustomWindow;
interface UISDKAPI ICustomWindow : public IWindow
{
    UI_DECLARE_Ixxx_INTERFACE(ICustomWindow, CustomWindow)

    void  SetWindowResizeType(UINT nType);
    bool  IsWindowLayered();
    void  SetWindowLayered(bool b);
    int   GetWindowTransparentMaskType();
    void  SetWindowTransparentMaskType(int type);
    void  SetWindowTransparentColMask(COLORREF col);
    void  SetWindowTransparentAlphaMask(int nAlpha);
};

}

#endif // ICUSTOMWINDOW_H_E0B1A6A7_425D_4218_A842_5BE5418F4E7D