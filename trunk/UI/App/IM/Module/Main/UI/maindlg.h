#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

namespace UI
{
    interface IStoryboard;
    interface IButton;
    interface ITabCtrl;
    interface IPanel;
}
namespace IM
{
    struct IMainDlgToolbarPlugin;
}
class  CMainUI;
class  ToolbarBtnPlugin;
class  MainDlgListPluginInfo
{
public:
    MainDlgListPluginInfo()
    {
        m_pButton = NULL;
        m_pContent = NULL;
        m_nId = 0;
        m_nIndex = 0;
    }

    int  m_nId;
    int  m_nIndex;
    
    UI::IButton*  m_pButton;
    UI::IObject*  m_pContent;
};

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
        UIMSG_WM_NOTIFY(UI_CONTACTLIST_NOTIFY_CONTACTITEM_DROPEVENT, OnContactDropEvent)
        UIMESSAGE_HANDLER_EX(UI_WM_DROPTARGETEVENT, OnDropEvent)
        UIMSG_BN_CLICKED2(m_pBtnChangeSkin, OnBtnChangeSkin)
        UIMSG_BN_CLICKED(_T("firend_mgr"), OnBtnFriendMgr)
        UIMSG_BN_CLICKED3(OnBtnClick)
		UIMSG_BN_RCLICK3(OnBtnRClick)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UIALT_MSG_MAP(1)
        UIMSG_BN_CLICKED3(OnToolbarPluginBtnClick)
    UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

    void  SetMainUI(CMainUI* p)
    {
        m_pMainUI = p;
    }
    void  Show();
    void  Hide();
public:
    void  ChangeTextureAlpha(int nNewAlpha);
    UI::IButton*  InsertPluginBtn2Servicebar(IM::IMainDlgToolbarPlugin* pPlugin, int nSort);
    void  ChangeToolbarPluginSort(UI::IButton* pBtn,int nSort);
    void  ChangeToolbarPluginVisible(UI::IButton* pBtn, bool bShow);

protected:
    void     OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void     OnSysCommand(UINT nID, CPoint point);
    LRESULT  OnShowModal(UINT nMsg, WPARAM w, LPARAM l);
    void     OnInitialize();
    void     OnBtnChangeSkin();
    void     OnBtnFriendMgr();
    LRESULT  OnContactDBClick(WPARAM w, LPARAM l);
    LRESULT  OnContactDropEvent(WPARAM w, LPARAM l);
    LRESULT  OnDropEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void     OnBtnClick(UI::IMessage* pMsgFrom);
	void     OnBtnRClick(UI::IMessage* pMsgFrom);

    void     OnToolbarPluginBtnClick(UI::IMessage* pMsgFrom);

protected:
    void  InitListPlugin();
    void  InitContactList();
    void  InitGroupList();
    void  InitRencentList();

    void  ShowList(MainDlgListPluginInfo* pListInfo);

private:
    UI::IButton*     m_pBtnChangeSkin;
    UI::IPanel*      m_pPanelServicebar;
    UI::IPanel*      m_pPanelList;

    vector<MainDlgListPluginInfo*>  m_vecListPlugin;
    MainDlgListPluginInfo*  m_pCurList;

    IContactList*    m_pContactList;
    IContactList*    m_pGroupList;
    IContactList*    m_pRecentList;

    UI::IRenderBase*  m_pServicebarBtnBkgndrender;
    UI::ITextRenderBase*  m_pServicebarBtnTextrender;

    CMainUI*  m_pMainUI;
};