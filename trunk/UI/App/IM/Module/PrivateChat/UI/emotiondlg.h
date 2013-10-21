#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
class CPrivateChatUI;

// 延迟通知表情窗口关闭
#define WM_EMOTION_DLG_HIDE (WM_USER+121)

// 插入表情到输入框中
// wParam
// lParam
#define WM_EMOTION_INSERT   (WM_USER+122)

namespace UI
{
    interface IGifCtrl;
}
class EmotionDlg : public UI::ICustomWindow
{
public:
    EmotionDlg();
    ~EmotionDlg();

    UI_BEGIN_MSG_MAP_Ixxx(EmotionDlg)
        UIMSG_WM_KILLFOCUS(OnKillFocus)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

public:
    void  OnKillFocus(IObject* pNewFocusObj);
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnInitialize();
    void  OnLButtonDown(UINT nFlags, POINT point);

    void  SetPrivateChatUI(CPrivateChatUI* p)
    {
        m_pPrivateChatUI = p;
    }
    void  SetParam(HWND hWnd)
    {
        m_hWndOwner = hWnd;
    }
    void  Show(POINT pt);


public:
    CPrivateChatUI*  m_pPrivateChatUI;
    HWND  m_hWndOwner;

    UI::IGifCtrl*  m_pEmotion1;
    UI::IGifCtrl*  m_pEmotion2;
};