#include "stdafx.h"
#include "MainDlg.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\itabctrl.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "App\IM\Module\Main\ui\mainui.h"
#include "App/IM/include/privatechat_inc.h"

#define SHOW_DURATION          300
#define HIDE_DURATION          300
#define STORYBOARD_ID_SHOW            1
#define STORYBOARD_ID_HIDE_CLOSE      2
#define STORYBOARD_ID_SWITCH_LIST_LEFT2RIGHT     3
#define STORYBOARD_ID_SWITCH_LIST_RIGHT2LEFT     4

CMainDlg::CMainDlg(void)
{
    m_pMainUI = NULL;

    m_pBtnChangeSkin = NULL;
    m_pPanelServicebar = NULL;
    m_pServicebarBtnBkgndrender = NULL;
    m_pServicebarBtnTextrender = NULL;

    m_pPanelList = NULL;
    m_pContactList = NULL;
    m_pGroupList = NULL;
    m_pRecentList = NULL;
    m_pCurList = NULL;
}
CMainDlg::~CMainDlg(void)
{
    SAFE_RELEASE(m_pServicebarBtnBkgndrender);
    SAFE_RELEASE(m_pServicebarBtnTextrender);

    int nSize = (int)m_vecListPlugin.size();
    for (int i = 0; i < nSize; i++)
    {
        MainDlgListPluginInfo* p = m_vecListPlugin[i];
        SAFE_DELETE(p);
    }
    m_vecListPlugin.clear();
}

void  CMainDlg::Show()
{
    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Show(STORYBOARD_ID_SHOW, SHOW_DURATION);
}
void  CMainDlg::Hide()
{
    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Hide(STORYBOARD_ID_HIDE_CLOSE, HIDE_DURATION);
}
LRESULT  CMainDlg::OnShowModal(UINT nMsg, WPARAM w, LPARAM l)
{
    SetMsgHandled(TRUE);
    
    Show();
    return 1;
}

void  CMainDlg::OnSysCommand(UINT nID, CPoint point)
{
    if (SC_CLOSE != nID)
    {
        SetMsgHandled(FALSE);
        return;
    }

    Hide();
    return;
}

void  CMainDlg::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        UI::IStoryboard* pStoryboard = ppArray[i];

        switch (pStoryboard->GetId())
        {
        case STORYBOARD_ID_HIDE_CLOSE:
            if (pStoryboard->IsFinish())
            {
                m_pMainUI->Quit();
            }
            break;

        case STORYBOARD_ID_SHOW:
            if (pStoryboard->IsFinish())
            {
                this->UpdateObject(); // 刷新一把，因为动画过程中SetCanCommit(false)，可能导致中间有些数据没有更新成功
            }
            break;

        case STORYBOARD_ID_SWITCH_LIST_LEFT2RIGHT:
            {
                UI::IObject* pObj1 = (UI::IObject*)pStoryboard->GetWParam();
                UI::IObject* pObj2 = (UI::IObject*)pStoryboard->GetLParam();

                int nCurValue = 0;
                pStoryboard->FindTimeline(0)->GetCurrentValue(&nCurValue);

                CRect  rcClient;
                m_pPanelList->GetClientRectAsWin32(&rcClient);
                pObj1->SetObjectPos(-nCurValue, 0, 0, 0, SWP_NOSIZE|SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);
                pObj2->SetObjectPos(rcClient.Width()-nCurValue, 0, 0, 0, SWP_NOSIZE|SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);

                if (pStoryboard->IsFinish())
                {
                    pObj1->SetVisible(false, false, false);
                }
                m_pPanelList->UpdateObject();
            }
            break;

        case STORYBOARD_ID_SWITCH_LIST_RIGHT2LEFT:
            {
                UI::IObject* pObj1 = (UI::IObject*)pStoryboard->GetWParam();
                UI::IObject* pObj2 = (UI::IObject*)pStoryboard->GetLParam();

                int nCurValue = 0;
                pStoryboard->FindTimeline(0)->GetCurrentValue(&nCurValue);

                CRect  rcClient;
                m_pPanelList->GetClientRectAsWin32(&rcClient);
                pObj1->SetObjectPos(nCurValue, 0, 0, 0, SWP_NOSIZE|SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);
                pObj2->SetObjectPos(nCurValue-rcClient.Width(), 0, 0, 0, SWP_NOSIZE|SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);

                if (pStoryboard->IsFinish())
                {
                    pObj1->SetVisible(false, false, false);
                }
                m_pPanelList->UpdateObject();
            }
            break;
        }
    }
}

void  CMainDlg::OnInitialize()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);
    HRESULT hr = SetCanDrop(true);
    UIASSERT(SUCCEEDED(hr));
    this->ModifyStyle(OBJECT_STYLE_RECEIVE_DRAGDROPEVENT, 0, false);

    m_pBtnChangeSkin = (UI::IButton*)this->FindChildObject(_T("btn_chnageskin"));
    m_pPanelServicebar = (UI::IPanel*)this->FindChildObject(_T("panel_servicebar"));
    if (m_pPanelServicebar)
    {
        UI::IButton*  pToolbarAddBtn = (UI::IButton*)m_pPanelServicebar->FindChildObject(_T("btn_toolbar_add"));
        if (pToolbarAddBtn)
        {
            pToolbarAddBtn->SetUserData((LPVOID)0x7FFFFFFF);  // 排序
        }
    }

    m_pPanelList = (UI::IPanel*)this->FindChildObject(_T("panel_list"));
    if (m_pPanelList)
    {
        m_pContactList = (IContactList*)m_pPanelList->FindChildObject(_T("contactlist"));
    }

    
//     m_pGroupList = NULL;
//     m_pRecentList = NULL;

    InitListPlugin();

    InitContactList();
}


// 加载列表控件等插件
void  CMainDlg::InitListPlugin()
{
    // 1. 联系人列表
    {
        MainDlgListPluginInfo*  pInfo = new MainDlgListPluginInfo;
        pInfo->m_nId = 0;
        pInfo->m_nIndex = 0;
        pInfo->m_pButton = (UI::IButton*)FindChildObject(_T("tab_contacter"));
        pInfo->m_pContent = static_cast<UI::IObject*>(m_pContactList);
        if (pInfo->m_pButton)
        {
            pInfo->m_pButton->ModifyStyleEx(BUTTON_STYLE_CLICK_ONDOWN, 0, 0);
            pInfo->m_pButton->SetChecked();
        }
        if (pInfo->m_pContent)
        {
            pInfo->m_pContent->SetVisible(true, false, false);
        }
        m_vecListPlugin.push_back(pInfo);
        m_pCurList = pInfo;
    }

    // 2. 群组列表
    {
        MainDlgListPluginInfo*  pInfo = new MainDlgListPluginInfo;
        pInfo->m_nId = 1;
        pInfo->m_nIndex = 1;
        pInfo->m_pButton = (UI::IButton*)FindChildObject(_T("tab_group"));
        pInfo->m_pContent = FindChildObject(_T("grouplist"));
        if (pInfo->m_pButton)
        {
            pInfo->m_pButton->ModifyStyleEx(BUTTON_STYLE_CLICK_ONDOWN, 0, 0);
        }
        if (pInfo->m_pContent)
        {
            pInfo->m_pContent->SetVisible(false, false, false);
        }
        m_vecListPlugin.push_back(pInfo);
    }


    // 3. 最近联系人列表 
    {
        MainDlgListPluginInfo*  pInfo = new MainDlgListPluginInfo;
        pInfo->m_nId = 2;
        pInfo->m_nIndex = 2;
        pInfo->m_pButton = (UI::IButton*)FindChildObject(_T("tab_recent"));
        pInfo->m_pContent = FindChildObject(_T("recentlist"));
        if (pInfo->m_pButton)
        {
            pInfo->m_pButton->ModifyStyleEx(BUTTON_STYLE_CLICK_ONDOWN, 0, 0);
        }
        if (pInfo->m_pContent)
        {
            pInfo->m_pContent->SetVisible(false, false, false);
        }
        m_vecListPlugin.push_back(pInfo);
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
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 30; i++)
        {
            cInfo.lId = i;
            cInfo.lGroupId = j;
            cInfo.pszMoodphrase = NULL;
            cInfo.pszNickName = NULL;
            cInfo.pszPortraitPath = NULL;

            cInfo.nMask = CLCI_MASK_NICKNAME|CLCI_MASK_MOODPHRASE|CLCI_MASK_PORTRAIT|CLCI_MASK_GRAY_PORTRAIT; 
            cInfo.pszNickName = _T("leeihcy");
            cInfo.pszMoodphrase = _T("気もち");

            TCHAR szPath[MAX_PATH] = _T("");
            _stprintf(szPath, _T("head_%d"), (i%8) + 1);
            cInfo.pszPortraitPath = szPath;

            cInfo.bGrayPortrait = i > 10 ? true:false;
            m_pContactList->InsertContact(&cInfo);
        }
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
    m_pMainUI->ShowSkinDlg();
}

// 由CChangeSkinDlg调用的。因为只有一个主窗口的纹理层是不同的，单独通知maindlg一下，不再做成一个share render了
void  CMainDlg::ChangeTextureAlpha(int nNewAlpha)
{
     if (NULL == m_pPanelList)
         return;
 
     UI::IRenderBase* p = m_pPanelList->GetBkRender();
     if (NULL == p)
         return;
 
     UI::IImageRender*  pBkgndRender = (UI::IImageRender*)p->QueryInterface(UI::uiiidof(IImageRender));
     if (NULL == pBkgndRender)
         return;
 
     pBkgndRender->SetAlpha(nNewAlpha);
}

LRESULT  CMainDlg::OnContactDBClick(WPARAM w, LPARAM l)
{
    m_pMainUI->OpenPrivateChatDlg(NULL, 0, 0, 0);
    return 0;
}
void  CMainDlg::OnBtnFriendMgr()
{
}


// 
// <xButton id="btn_toolbar_add" tooltip="界面管理器" styleclass="main.toolbarbtn" iconalign="center" width="20"
// foregnd.render.image="changeskin_pluginadd" foregnd.render.type="image" />
// 
UI::IButton*  CMainDlg::InsertPluginBtn2Servicebar(IM::IMainDlgToolbarPlugin* pPlugin, int nSort)
{
    if (NULL == pPlugin || NULL == m_pPanelServicebar)
        return NULL;

    // 删除/隐藏 按钮
//     UI::IObject* pObject = m_pPanelServicebar->FindChildObject(pData->m_pPlugin->GetName());
//     if (false == bAddOrRemove)
//     {
// //         SAFE_DELETE_Ixxx(pObject);
// //         m_pPanelServicebar->UpdateLayout(true);
// 
//         pObject->SetVisible(false, true, true);
//         return;
//     }
//     
//     // 显示
//     if (pObject)
//     {
//         pObject->SetVisible(true, true, true);
//         return;
//     }

    // 插入按钮
    UI::IUIApplication*  pUIApplication = GetUIApplication();
    UI::IButton*  pBtn = NULL;
    UI::IButton::CreateInstance(pUIApplication, &pBtn);

    this->ChangeToolbarPluginSort(pBtn, nSort);

    UI::IMapAttribute* pAttrib = NULL;
    UI::UICreateIMapAttribute(&pAttrib);
    pAttrib->AddAttr(XML_STYLECLASS, _T("main.toolbarbtn"));
    pBtn->ParseStyleAndSetAttribute(pAttrib, false);
    SAFE_RELEASE(pAttrib);

    pBtn->SetID(pPlugin->GetName());
    pBtn->SetIconFromHBITMAP(pPlugin->GetIcon());

    // 使用styleclass替代了
//     CRegion4 r(4,0,4,0);
//     pBtn->SetPaddingRegion(&r);
//     pBtn->SetTransparent(true);
//     pBtn->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);

    // 背景
//     {
//         if (NULL == m_pServicebarBtnBkgndrender)
//         {
//             pUIApplication->CreateRenderBase(UI::RENDER_TYPE_IMAGELIST, NULL, &m_pServicebarBtnBkgndrender);
//             UI::IImageListRender*  pImagelistRender = (UI::IImageListRender*)m_pServicebarBtnBkgndrender->QueryInterface(UI::uiiidof(IImageListRender));
//             
//             // 图片
//             UI::IImageRes*  pImgRes = pUIApplication->GetActiveSkinImageRes();
//             UI::IRenderBitmap*  pBkImg = NULL;
//             pImgRes->GetBitmap(_T("button2"), UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, &pBkImg);
//             pImagelistRender->SetIImageListRenderBitmap(static_cast<UI::IImageListRenderBitmap*>(pBkImg));
//             SAFE_RELEASE(pBkImg);
// 
//             // 拉伸参数
//             pImagelistRender->SetImageDrawType(UI::DRAW_BITMAP_STRETCH);
//             UI::Image9Region r;
//             r.Set(3);
//             pImagelistRender->SetImageStretch9Region(&r);
//         }
//         pBtn->SetBkgndRender(m_pServicebarBtnBkgndrender);
//     }
    // 字体
//     {
//         if (NULL == m_pServicebarBtnTextrender)
//         {
//             pUIApplication->CreateTextRenderBase(UI::TEXTRENDER_TYPE_CONTRASTCOLOR, this, &m_pServicebarBtnTextrender);
// 
//             if (m_pServicebarBtnTextrender)
//             {
//                 UI::IMapAttribute*  pMapAttrib = NULL;
//                 UICreateIMapAttribute(&pMapAttrib);
//                 m_pServicebarBtnTextrender->SetAttribute(pMapAttrib,NULL,false);
//                 SAFE_RELEASE(pMapAttrib);
//                 m_pServicebarBtnTextrender->SetTextEffect(UI::TEXT_EFFECT_HALO);
//                 m_pServicebarBtnTextrender->SetDrawTextParam(3, 0);
//             }
//         }
//         pBtn->SetTextRender(m_pServicebarBtnTextrender);
//     }
    
    pBtn->SetNotify(this, 1);
    m_pPanelServicebar->UpdateLayout(true);
    return pBtn;
}

void  CMainDlg::ChangeToolbarPluginSort(UI::IButton* pBtn, int nSort)
{
    if (NULL == pBtn)
        return;

    pBtn->RemoveMeInTheTree();

    UI::IObject* pChild = NULL;
    while (pChild = m_pPanelServicebar->EnumChildObject(pChild))
    {
        int nThisSort = (int)pChild->GetUserData();
        if (nThisSort > nSort)
        {
            pBtn->InsertBefore(pChild);
            break;
        }
    }
    if (NULL == pBtn->GetParentObject())
    {
        m_pPanelServicebar->AddChild(pBtn);
    }
    pBtn->SetUserData((LPVOID)nSort);
}

void  CMainDlg::ChangeToolbarPluginVisible(UI::IButton* pBtn, bool bShow)
{
    if (pBtn)
    {
        pBtn->SetVisible(bShow);
    }
}
void  CMainDlg::OnToolbarPluginBtnClick(UI::IMessage* pMsgFrom)
{
    m_pMainUI->OnToolbarPluginBtnClick(static_cast<UI::IButton*>(pMsgFrom));
}

LRESULT  CMainDlg::OnContactDropEvent(WPARAM wParam, LPARAM lParam)
{
    UI::DROPTARGETEVENT_TYPE eEventType = (UI::DROPTARGETEVENT_TYPE)wParam;
    UI::DROPTARGETEVENT_DATA* pData = (UI::DROPTARGETEVENT_DATA*)lParam;

    if (!pData)
        return 0;

    switch (eEventType)
    {
    case UI::_DragEnter:
    case UI::_DragOver:
        {
            FORMATETC format = {0};
            format.dwAspect = DVASPECT_CONTENT;
            format.cfFormat = CF_TEXT;
            format.tymed = TYMED_HGLOBAL;

            bool bOk = false;
            if (SUCCEEDED(pData->pDataObj->QueryGetData(&format)))
            {
                bOk = true;
            }
            if (!bOk)
            {
                format.cfFormat = CF_UNICODETEXT;
                if (SUCCEEDED(pData->pDataObj->QueryGetData(&format)))
                    bOk = true;
            }

            if (bOk)
            {
//                 bool bCtrl = (pData->grfKeyState & MK_CONTROL) ? true:false;
//                 bool bShift = (pData->grfKeyState & MK_SHIFT) ? true:false;
//                 if (bCtrl && bShift)
//                     *(pData->pdwEffect) |= DROPEFFECT_LINK;
//                 else if (bCtrl)
//                     *(pData->pdwEffect) |= DROPEFFECT_COPY;
//                 else
//                     *(pData->pdwEffect) |= DROPEFFECT_MOVE;

                *(pData->pdwEffect) = DROPEFFECT_COPY;
            }
        }
        break;

    case UI::_Drop:
        {
            *(pData->pdwEffect) = DROPEFFECT_COPY;   // <<-- 决定了最后的操作，是否要删除原数据

            FORMATETC format = {0};
            format.dwAspect = DVASPECT_CONTENT;
            format.cfFormat = CF_UNICODETEXT;
            format.tymed = TYMED_HGLOBAL;

            STGMEDIUM  medium = {0};

            BSTR  bstrText;
            bool bOk = false;
            if (SUCCEEDED(pData->pDataObj->GetData(&format, &medium)))
            {
                bOk = true;
                const TCHAR* szText = (const TCHAR*)::GlobalLock(medium.hGlobal);
                bstrText = SysAllocString(szText);
                ::GlobalUnlock(medium.hGlobal);
                ::ReleaseStgMedium(&medium);
            }
            if (!bOk)
            {
                format.cfFormat = CF_TEXT;
                if (SUCCEEDED(pData->pDataObj->GetData(&format, &medium)))
                {
                    bOk = true;
                    const char* szText = (const char*)::GlobalLock(medium.hGlobal);
                    bstrText = SysAllocString(CA2T(szText));
                    ::GlobalUnlock(medium.hGlobal);
                    ::ReleaseStgMedium(&medium);
                }
            }

            if (bOk)
            {
                m_pMainUI->OpenPrivateChatDlg(NULL, IM::SENDTEXT, (WPARAM)bstrText, 0);
            }
        }
        break;

    }
    return 0;
}
LRESULT  CMainDlg::OnDropEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::DROPTARGETEVENT_TYPE eEventType = (UI::DROPTARGETEVENT_TYPE)wParam;
    UI::DROPTARGETEVENT_DATA* pData = (UI::DROPTARGETEVENT_DATA*)lParam;

    if (pData)
    {
        *pData->pdwEffect = DROPEFFECT_NONE;
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

void  CMainDlg::OnBtnClick(UI::IMessage* pMsgFrom)
{
    SetMsgHandled(TRUE);
    int nSize = (int)m_vecListPlugin.size();
    for (int i = 0; i < nSize; i++)
    {
        if (static_cast<UI::IMessage*>(m_vecListPlugin[i]->m_pButton) == pMsgFrom)
        {
            ShowList(m_vecListPlugin[i]);
            return;
        }
    }

    SetMsgHandled(FALSE);
}
void  CMainDlg::OnBtnRClick(UI::IMessage* pMsgFrom)
{
	if (m_pContactList->GetLayoutType() == CONTACTLIST_LAYOUT_BIGFACE)
		m_pContactList->SetLayoutType(CONTACTLIST_LAYOUT_SMALLFACE);
	else
		m_pContactList->SetLayoutType(CONTACTLIST_LAYOUT_BIGFACE);

	m_pContactList->UpdateItemRect();
}

// 切换Tab
void  CMainDlg::ShowList(MainDlgListPluginInfo* pListInfo)
{
    if (NULL == pListInfo)
        return;

    if (m_pCurList == pListInfo)
        return;

    UI::IObject*  pAnimate1 = NULL;
    UI::IObject*  pAnimate2 = NULL;
    int nIndex1 = 0;
    int nIndex2 = 0;

    if (m_pCurList->m_pButton)
        m_pCurList->m_pButton->SetUnChecked();
    pAnimate1 = m_pCurList->m_pContent;
    nIndex1 = m_pCurList->m_nIndex;

    m_pCurList = pListInfo;
    if (m_pCurList->m_pButton)
        m_pCurList->m_pButton->SetChecked();
    pAnimate2 = m_pCurList->m_pContent;
    nIndex2 = m_pCurList->m_nIndex;

    if (NULL == pAnimate1 || NULL == pAnimate2)
    {
        if (pAnimate1)
            pAnimate1->SetVisible(false, false, false);
        if (pAnimate2)
            pAnimate2->SetVisible(true, false, false);

        m_pPanelList->UpdateLayout(true);
        return;
    }

    // 注： 这里没有采用图片移动法，因这要创建三副图片才能实现该效果，太消耗内存，换用移动控件的方法来实现动画

    // 将要显示的对象位置预置好
    CRect rcClient;
    m_pPanelList->GetClientRectAsWin32(&rcClient);
    if (nIndex2 > nIndex1)
        pAnimate2->SetObjectPos(rcClient.Width(), 0, rcClient.Width(), rcClient.Height(), SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);
    else
        pAnimate2->SetObjectPos(-rcClient.Width(), 0, rcClient.Width(), rcClient.Height(), SWP_NOUPDATELAYOUTPOS|SWP_NOREDRAW);
    pAnimate2->SetVisible(true, false, false);

    UI::IStoryboard* pStoryboard = GetUIApplication()->GetAnimateMgr()->CreateStoryboard();
    UI::IIntAccelerateMove* pMoveAlgo = NULL;
    UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(
        UI::TLV_INT, 0, UI::ITMA_Accelerate, (UI::IMoveAlgorithm**)&pMoveAlgo);

    pMoveAlgo->SetParam1(0, m_pPanelList->GetWidth(), 100, m_pPanelList->GetWidth()*20/1000.0f);
    
    if (nIndex2 > nIndex1)
        pStoryboard->SetId(STORYBOARD_ID_SWITCH_LIST_LEFT2RIGHT);
    else
        pStoryboard->SetId(STORYBOARD_ID_SWITCH_LIST_RIGHT2LEFT);

    pStoryboard->SetNotifyObj(this);
    pStoryboard->SetWParam((WPARAM)pAnimate1);
    pStoryboard->SetLParam((LPARAM)pAnimate2);
    pStoryboard->Begin();
}