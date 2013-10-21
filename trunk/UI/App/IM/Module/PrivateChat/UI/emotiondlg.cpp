#include "stdafx.h"
#include "emotiondlg.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"

EmotionDlg::EmotionDlg()
{
    m_pPrivateChatUI = NULL;
    m_hWndOwner = NULL;
    m_pEmotion1 = NULL;
    m_pEmotion2 = NULL;
}

EmotionDlg::~EmotionDlg()
{
    
}

void  EmotionDlg::OnInitialize()
{
    SetMsgHandled(FALSE);
    this->SetWindowResizeType(WRSB_NONE);
    m_pEmotion1 = (UI::IGifCtrl*)this->FindChildObject(_T("emotion1"));
    m_pEmotion2 = (UI::IGifCtrl*)this->FindChildObject(_T("emotion2"));
}

void  EmotionDlg::Show(POINT pt)
{
    if (m_pEmotion1)
        m_pEmotion1->Start();
    if (m_pEmotion2)
        m_pEmotion2->Start();

    SetWindowPos(GetHWND(), HWND_TOPMOST, pt.x, pt.y-GetHeight(), 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
    ::ShowWindow(GetHWND(), SW_SHOW);
}

void  EmotionDlg::OnKillFocus(IObject* pNewFocusObj)
{
    if (m_pEmotion1)
        m_pEmotion1->Stop();
    if (m_pEmotion2)
        m_pEmotion2->Stop();

    SetMsgHandled(FALSE);
    HideWindow();

    if (m_hWndOwner)
    {
        PostMessage(m_hWndOwner, WM_EMOTION_DLG_HIDE, 0, 0);
    }
}

BOOL  EmotionDlg::PreCreateWindow(CREATESTRUCT* pcs)
{
    pcs->style = WS_POPUP;
    pcs->dwExStyle = WS_EX_TOOLWINDOW;
    return TRUE;
}

void  EmotionDlg::OnLButtonDown(UINT nFlags, POINT point)
{
    // Test. 不想再开发一个表情控件了

    if (!m_hWndOwner)
        return;

    if (m_pEmotion1)
    {
        CRect rc;
        m_pEmotion1->GetWindowRect(&rc);
        if (rc.PtInRect(point))
        {
            PostMessage(m_hWndOwner, WM_EMOTION_INSERT, 1, 0);
            OnKillFocus(NULL);
            return;
        }
    }
    
    if (m_pEmotion2)
    {
        CRect rc;
        m_pEmotion2->GetWindowRect(&rc);
        if (rc.PtInRect(point))
        {
            PostMessage(m_hWndOwner, WM_EMOTION_INSERT, 2, 0);
            OnKillFocus(NULL);
            return;
        }
    }
}