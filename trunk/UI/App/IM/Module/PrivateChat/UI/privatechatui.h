#pragma once
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\privatechat_inc.h"
class CChatDlg;
namespace IM
{
    struct ISkinUI;
}
namespace UI
{
    interface IButton;
}
class EmotionDlg;

class PrivateChatDlgInfo
{
public:
    PrivateChatDlgInfo();
    ~PrivateChatDlgInfo();

public:
    String  m_strUserId;
    CChatDlg*  m_pChatDlg;
};

class CPrivateChatUI : public IM::IUI
{
public:
    CPrivateChatUI();
    ~CPrivateChatUI();

    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

    void  DelayDestroyDlg(CChatDlg* p);
    void  ShowEmotionDlg(HWND hWnd, POINT pt);

    long  OnOpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* pTask);

protected:
    
    void  DestroyDlg(CChatDlg* p);

    PrivateChatDlgInfo*  FindDlg(const TCHAR* szUserId);
    PrivateChatDlgInfo*  FindDlg2(CChatDlg* pDlg);

public:
    vector<PrivateChatDlgInfo*>  m_vecPrivateChatDlg;
    IM::ISkinUI*  m_pSkinUI;
    EmotionDlg*   m_pEmotionDlg;
};