#ifndef DWMHELPER_H_93320746_EDA2_4d66_B1CA_35327C5C4B84
#define DWMHELPER_H_93320746_EDA2_4d66_B1CA_35327C5C4B84

namespace UI
{

// Blur behind data structures
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified

typedef struct _DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct _MARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} MARGINS, *PMARGINS;

typedef HRESULT (__stdcall *funcDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (__stdcall *funcDwmEnableBlurBehindWindow)(HWND hWnd, __in const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (__stdcall *funcDwmIsCompositionEnabled)(BOOL *pfEnabled);
class DwmHelper
{
private:
    DwmHelper();
    ~DwmHelper();

public:
    static DwmHelper*  GetInstance();
    bool  IsEnable();

    funcDwmExtendFrameIntoClientArea  pDwmExtendFrameIntoClientArea;
    funcDwmEnableBlurBehindWindow     pDwmEnableBlurBehindWindow;
    funcDwmIsCompositionEnabled       pDwmIsCompositionEnabled;

    HMODULE  m_hModule;
};

}

#endif // DWMHELPER_H_93320746_EDA2_4d66_B1CA_35327C5C4B84