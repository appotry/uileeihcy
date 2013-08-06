#include "stdafx.h"
#include "MainDlg.h"
#include "ChangeSkinDlg.h"
#include "ChatDlg.h"
#include "contactmgrdlg.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\itabctrl.h"
#include "App\IM\IM_UI\Resource.h"
#include "App\IM\IM_UI\logic\pluginmgr.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"

#define SHOW_HIDE_DURATION          300
#define TIMELINE_ID_SHOW            1
#define TIMELINE_ID_HIDE_CLOSE      2

CMainDlg::CMainDlg(void)
{
    m_pBtnChangeSkin = NULL;
    m_pChangeSkinDlg = NULL;
    m_pChatDlg = NULL;
    m_pTabCtrl = NULL;
    m_pPanelServicebar = NULL;
    m_pServicebarBtnBkgndrender = NULL;
    m_pServicebarBtnTextrender = NULL;

    m_pContactList = NULL;
    m_pGroupList = NULL;
    m_pRecentList = NULL;
    m_pFriendMgrDlg = NULL;
}
CMainDlg::~CMainDlg(void)
{
    SAFE_DELETE_Ixxx(m_pChangeSkinDlg);
    SAFE_DELETE_Ixxx(m_pChatDlg);
    SAFE_DELETE_Ixxx(m_pFriendMgrDlg);

    SAFE_RELEASE(m_pServicebarBtnBkgndrender);
    SAFE_RELEASE(m_pServicebarBtnTextrender);
}

LRESULT  CMainDlg::OnShowModal(UINT nMsg, WPARAM w, LPARAM l)
{
    SetMsgHandled(TRUE);
    
    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return 0;

    p->Show(TIMELINE_ID_SHOW, SHOW_HIDE_DURATION);
    return 1;
}

void  CMainDlg::OnSysCommand(UINT nID, CPoint point)
{
    if (SC_CLOSE != nID)
    {
        SetMsgHandled(FALSE);
        return;
    }

    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Hide(TIMELINE_ID_HIDE_CLOSE, SHOW_HIDE_DURATION);
    return;
}

void  CMainDlg::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        if (ppArray[i]->IsFinish() && ppArray[i]->GetID() == TIMELINE_ID_HIDE_CLOSE)
        {
            this->EndDialog(IDOK);
        }
    }
}

void  CMainDlg::OnInitialize()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);
    HRESULT hr = SetCanDrop(true);
    UIASSERT(SUCCEEDED(hr));
    this->ModifyStyle(OBJECT_RECEIVE_DRAGDROPEVENT, 0, false);

    m_pBtnChangeSkin = (UI::IButton*)this->FindChildObject(_T("btn_chnageskin"));
    m_pTabCtrl = (UI::ITabCtrl*)this->FindChildObject(_T("tabctrl"));
    m_pPanelServicebar = (UI::IPanel*)this->FindChildObject(_T("panel_servicebar"));

    m_pContactList = (IContactList*)m_pTabCtrl->FindChildObject(_T("contactlist"));
//     m_pGroupList = NULL;
//     m_pRecentList = NULL;

    InitPluginServicebar();
    InitContactList();

    if (NULL == GetMaterialRender())
    {
        SetMaterialRender(g_pShareSkinRender);
    }
}


void  CMainDlg::InitPluginServicebar()
{
    int nCount = CPluginManager::Get()->GetPluginCount();
    for (int i = 0; i < nCount; i++)
    {
        InsertPluginBtn2Servicebar(CPluginManager::Get()->GetPluginItem(i), true);
    }
}

void  CMainDlg::InitContactList()
{
    if (!m_pContactList)
        return;

    CONTACTLIST_GROUPITEM_INFO ginfo = {0};
    ginfo.lId = 0;
    ginfo.pszText = _T("我的好友");
    ginfo.nMask = CLGI_MASK_TEXT;
    m_pContactList->InsertGroup(&ginfo);

    ginfo.lId = 1;
    ginfo.pszText = _T("我的朋友");
    m_pContactList->InsertGroup(&ginfo);


    CONTACTLIST_CONTACTITEM_INFO cInfo;
    for (int i = 0; i < 10; i++)
    {
        cInfo.lId = i;
        cInfo.lGroupId = 0;
        cInfo.pszMoodphrase = NULL;
        cInfo.pszNickName = NULL;
        cInfo.pszPortraitPath = NULL;

        cInfo.nMask = CLCI_MASK_NICKNAME|CLCI_MASK_MOODPHRASE;
        cInfo.pszNickName = _T("leeihcy");
        cInfo.pszMoodphrase = _T("leeihcy");

        m_pContactList->InsertContact(&cInfo);
    }

    m_pContactList->UpdateItemRect(NULL, true);
}
void  CMainDlg::InitGroupList()
{

}
void  CMainDlg::InitRencentList()
{

}

void  CMainDlg::OnBtnChangeSkin()
{
    if (NULL == m_pChangeSkinDlg)
    {
        // m_pChangeSkinDlg 
        CChangeSkinDlg::CreateInstance(GetUIApplication(), &m_pChangeSkinDlg);
        m_pChangeSkinDlg->Create(GetUIApplication(), _T("changeskindlg"), GetHWND());
        m_pChangeSkinDlg->SetMainDlg(this);
    }

    m_pChangeSkinDlg->ShowWindow();
}

// 由CChangeSkinDlg调用的。因为只有一个主窗口的纹理层是不同的，单独通知maindlg一下，不再做成一个share render了
void  CMainDlg::OnTextureAlphaChanged(int nNewAlpha)
{
    if (NULL == m_pTabCtrl)
        return;

    if (m_pChatDlg)
        m_pChatDlg->OnTextureAlphaChanged(nNewAlpha);

    UI::IPanel*  pContentPanel = m_pTabCtrl->GetContentPanel();
    if (NULL == pContentPanel)
        return;

    UI::IRenderBase* p = pContentPanel->GetBkRender();
    if (NULL == p)
        return;

    UI::IImageRender*  pBkgndRender = (UI::IImageRender*)p->QueryInterface(UI::uiiidof(IImageRender));
    if (NULL == pBkgndRender)
        return;

    pBkgndRender->SetAlpha(nNewAlpha);
}

LRESULT  CMainDlg::OnContactDBClick(WPARAM w, LPARAM l)
{
    if (NULL == m_pChatDlg)
    {
        CChatDlg::CreateInstance(GetUIApplication(), &m_pChatDlg);
        m_pChatDlg->Create(GetUIApplication(), _T("chatdlg"), GetHWND());
    }

    m_pChatDlg->ShowWindow();
    return 0;
}
void  CMainDlg::OnBtnFriendMgr()
{
    if (NULL == m_pFriendMgrDlg)
    {
        CContactMgrDlg::CreateInstance(GetUIApplication(), &m_pFriendMgrDlg);
        m_pFriendMgrDlg->Create(GetUIApplication(), _T("friendmgrdlg"), GetHWND());
    }

    m_pFriendMgrDlg->ShowWindow();
}

void  CMainDlg::InsertPluginBtn2Servicebar(PluginItemInfo*  pPlugin, bool bAddOrRemove)
{
    if (NULL == pPlugin || NULL == m_pPanelServicebar)
        return;

    // 删除/隐藏 按钮
    UI::IObject* pObject = m_pPanelServicebar->FindChildObject(pPlugin->strPlugin.c_str());
    if (false == bAddOrRemove)
    {
//         SAFE_DELETE_Ixxx(pObject);
//         m_pPanelServicebar->UpdateLayout(true);

        pObject->SetVisible(false, true, true);
        return;
    }
    
    // 显示
    if (pObject)
    {
        pObject->SetVisible(true, true, true);
        return;
    }

    // 插入按钮
    UI::IUIApplication*  pUIApplication = GetUIApplication();
    UI::IButton*  pBtn = NULL;
    UI::IButton::CreateInstance(pUIApplication, &pBtn);
    m_pPanelServicebar->AddChild(pBtn);

    pBtn->SetID(pPlugin->strPlugin.c_str());
    CRegion4 r(4,0,4,0);
    pBtn->SetPaddingRegion(&r);
    pBtn->SetTransparent(true);
    pBtn->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
    pBtn->AddAttribute(XML_TOOLTIP, pPlugin->strTooltip.c_str());
    pBtn->SetText(pPlugin->strText.c_str());
    pBtn->SetIconFromFile(pPlugin->strIcon2Path.c_str());
    
    // 背景
    {
        if (NULL == m_pServicebarBtnBkgndrender)
        {
            pUIApplication->CreateRenderBase(UI::RENDER_TYPE_IMAGELIST, NULL, &m_pServicebarBtnBkgndrender);
            UI::IImageListRender*  pImagelistRender = (UI::IImageListRender*)m_pServicebarBtnBkgndrender->QueryInterface(UI::uiiidof(IImageListRender));
            
            // 图片
            UI::IImageRes*  pImgRes = pUIApplication->GetActiveSkinImageRes();
            UI::IRenderBitmap*  pBkImg = NULL;
            pImgRes->GetBitmap(_T("button2"), UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, &pBkImg);
            pImagelistRender->SetIImageListRenderBitmap(static_cast<UI::IImageListRenderBitmap*>(pBkImg));
            SAFE_RELEASE(pBkImg);

            // 拉伸参数
            pImagelistRender->SetImageDrawType(UI::DRAW_BITMAP_STRETCH);
            UI::Image9Region r;
            r.Set(3);
            pImagelistRender->SetImageStretch9Region(&r);
        }
        pBtn->SetBkgndRender(m_pServicebarBtnBkgndrender);
    }
    // 字体
    {
        if (NULL == m_pServicebarBtnTextrender)
        {
            pUIApplication->CreateTextRenderBase(UI::TEXTRENDER_TYPE_CONTRASTCOLOR, this, &m_pServicebarBtnTextrender);

            if (m_pServicebarBtnTextrender)
            {
                UI::IMapAttribute*  pMapAttrib = NULL;
                UICreateIMapAttribute(&pMapAttrib);
                m_pServicebarBtnTextrender->SetAttribute(pMapAttrib,NULL,false);
                SAFE_RELEASE(pMapAttrib);
                m_pServicebarBtnTextrender->SetTextEffect(UI::TEXT_EFFECT_HALO);
                m_pServicebarBtnTextrender->SetDrawTextParam(3, 0);
            }
        }
        pBtn->SetTextRender(m_pServicebarBtnTextrender);
    }
    
    m_pPanelServicebar->UpdateLayout(true);
}

LRESULT  CMainDlg::OnDropEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::DROPTARGETEVENT_TYPE eEventType = (UI::DROPTARGETEVENT_TYPE)wParam;
    UI::DROPTARGETEVENT_DATA* pData = (UI::DROPTARGETEVENT_DATA*)lParam;

    if (pData)
    {
        *pData->pdwEffect = DROPEFFECT_MOVE;
    }
    switch (eEventType)
    {
    case UI::_DragEnter:
        break;
    case UI::_DragOver:
        break;
    case UI::_DragLeave:
        break;
    case UI::_Drop:
        break;
    }

    return 0;
}