#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"

namespace UI
{
    interface IStoryboard;
    interface IButton;
    interface ITabCtrl;
    interface IPanel;
}
interface IContactList;
class  CChangeSkinDlg;
class  CChatDlg;
class  PluginItemInfo;
class  CContactMgrDlg;

class CMainDlg : public UI::ICustomWindow
{
public:
    CMainDlg(void);
    ~CMainDlg(void);

    UI_BEGIN_MSG_MAP_Ixxx(CMainDlg)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMESSAGE_HANDLER_EX(UI_WM_SHOWMODALWINDOW, OnShowModal)
        UIMSG_WM_NOTIFY(UI_CONTACTLIST_NOTIFY_CONTACTITEM_DBCLICK, OnContactDBClick)
        UIMESSAGE_HANDLER_EX(UI_WM_DROPTARGETEVENT, OnDropEvent)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_BN_CLICKED2(m_pBtnChangeSkin, OnBtnChangeSkin)
        UIMSG_BN_CLICKED(_T("firend_mgr"), OnBtnFriendMgr)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

public:
    void  OnTextureAlphaChanged(int nNewAlpha);
    void  InsertPluginBtn2Servicebar(PluginItemInfo*  pPlugin, bool bAddOrRemove);

protected:
    void     OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void     OnSysCommand(UINT nID, CPoint point);
    LRESULT  OnShowModal(UINT nMsg, WPARAM w, LPARAM l);
    void     OnInitialize();
    void     OnBtnChangeSkin();
    void     OnBtnFriendMgr();
    LRESULT  OnContactDBClick(WPARAM w, LPARAM l);
    LRESULT  OnDropEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  InitPluginServicebar();
    void  InitContactList();
    void  InitGroupList();
    void  InitRencentList();

private:
    UI::IButton*     m_pBtnChangeSkin;
    UI::ITabCtrl*    m_pTabCtrl;
    UI::IPanel*      m_pPanelServicebar;
    IContactList*    m_pContactList;
    IContactList*    m_pGroupList;
    IContactList*    m_pRecentList;

    UI::IRenderBase*  m_pServicebarBtnBkgndrender;
    UI::ITextRenderBase*  m_pServicebarBtnTextrender;

    CChangeSkinDlg*  m_pChangeSkinDlg;
    CChatDlg*        m_pChatDlg;
    CContactMgrDlg*   m_pFriendMgrDlg;

};