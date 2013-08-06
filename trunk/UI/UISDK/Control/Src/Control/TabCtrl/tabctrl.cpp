#include "stdafx.h"
#include "tabctrl.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

// 注
// 1. 如何解决系统类型按钮，当前选中按钮在置顶显示的问题
//    在刷新附近按钮时不能覆盖当前按钮

HRESULT  TabCtrl::UIParseLayoutElement(IUIElement* pElement, IUIApplication*  pUIApp, IObject* pParent, IObject** ppOut)
{
    ITabCtrl*  pTabCtrl = NULL;
    ITabCtrl::CreateInstance(pUIApp, &pTabCtrl);
    *ppOut = static_cast<IObject*>(pTabCtrl);

    TabCtrl* pImpl = pTabCtrl->GetImpl();

    // 解析自己的属性
    IMapAttribute*  pMapAttrib = NULL;
    IStyleManager*  pStyleMgr = pUIApp->GetActiveSkinStyleMgr();

    pElement->GetAttribList(&pMapAttrib);
    pStyleMgr->ParseStyle(TabCtrl::GetXmlName(), pMapAttrib);

    UISendMessage(pTabCtrl, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);
    SAFE_RELEASE(pMapAttrib);
    pParent->AddChild(pTabCtrl);

    // 解析子结点
    IUIChildNodeEnum*  pEnum = pElement->EnumChild();
    if (NULL == pElement)
        return S_OK;

    IUIElement*  pChildElement = NULL;
    while (pChildElement = pEnum->NextElement())
    {
        pImpl->UIParseNewItem(pChildElement);
        SAFE_RELEASE(pChildElement);
    }

    SAFE_RELEASE(pEnum);
    return S_OK;
}

HRESULT  TabCtrl::UIParseNewItem(IUIElement* pChildElem)
{
    CComBSTR  bstrTagName;
    pChildElem->GetTagName(&bstrTagName);

    if (bstrTagName != XML_ITEM)
        return E_FAIL;

    IObject*  pIObjHead = NULL;
    IObject*  pIObjContent = NULL;

    IUIApplication*  pUIApp = m_pITabCtrl->GetUIApplication();
    ILayoutManager*  pLayoutMgr = pUIApp->GetActiveSkinLayoutMgr();

    // 获取head/content
    IUIElement*  pHead = pChildElem->FindChild(L"head");
    if (pHead)
    {
        IUIElement* pChildElem = pHead->FirstChild();
        if (pChildElem)
        {
            pLayoutMgr->LoadLayout(pChildElem, m_pPanelHead, &pIObjHead);
            SAFE_RELEASE(pChildElem);
        }
        SAFE_RELEASE(pHead);
    }
    if (NULL == pIObjHead)
        return E_FAIL;

    IUIElement*  pContent = pChildElem->FindChild(L"content");
    if (pContent)
    {
        IUIElement* pChildElem = pContent->FirstChild();
        if (pChildElem)
        {
            pLayoutMgr->LoadLayout(pChildElem, m_pPanelContent, &pIObjContent);
            SAFE_RELEASE(pChildElem);
        }
        SAFE_RELEASE(pContent);
    }

    __super::AddItem(0, pIObjHead, pIObjContent);
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////
TabCtrlItemBase::TabCtrlItemBase()
{
    m_nId = 0;
    m_pBtn = NULL;
    m_pContent = NULL;
    m_nIndex = 0;
    m_pData = NULL;

    m_pPrev = NULL;
    m_pNext = NULL;
}

//////////////////////////////////////////////////////////////////////////
TabCtrlBase::TabCtrlBase()
{
    m_pITabCtrlBase = NULL;
    m_pFirstItem = NULL;
    m_pLastItem = NULL;
    m_nCount = 0;
    m_nHeadHeight = 0;
    m_pCurItem = NULL;

    m_pPanelHead = NULL;
    m_pPanelContent = NULL;
    m_eHeadLayoutType = TABCTRL_HEAD_LAYOUT_TYPE_FIX;
}
TabCtrlBase::~TabCtrlBase()
{
    TabCtrlItemBase*  p = m_pFirstItem;
    TabCtrlItemBase*  pNext = NULL;

    while (p)
    {
        pNext = p->m_pNext;
        SAFE_DELETE(p);
        p = pNext;
    }
    m_pFirstItem = m_pLastItem = NULL;
}

HRESULT  TabCtrlBase::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(ITabCtrlBase, IPanel);
    long lRet = GetCurMsg()->lRet;
    if (FAILED(lRet))
        return lRet;

    IMapAttribute*  pMapAttrib = NULL;

    UICreateIMapAttribute(&pMapAttrib);

    IPanel::CreateInstance(p, &m_pPanelHead);
    IPanel::CreateInstance(p, &m_pPanelContent);

    UISendMessage(static_cast<IMessage*>(m_pPanelHead), UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);
    UISendMessage(static_cast<IMessage*>(m_pPanelContent), UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);
    SAFE_RELEASE(pMapAttrib);

    m_pITabCtrlBase->AddChild(m_pPanelHead);
    m_pITabCtrlBase->AddChild(m_pPanelContent);

    m_pPanelHead->SetTransparent(true);

    return S_OK;
}   

void  TabCtrlBase::ResetAttribute()
{
    DO_PARENT_PROCESS(ITabCtrlBase, IPanel);

    m_nHeadHeight = 20;
    m_eHeadLayoutType = TABCTRL_HEAD_LAYOUT_TYPE_FIX;
}
void  TabCtrlBase::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(ITabCtrlBase, IPanel);

    const TCHAR*  szText = pMapAttr->GetAttr(XML_TABCTRL_HEAD_LAYOUT_TYPE, true);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE))
        {
            m_eHeadLayoutType = TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE;
        }
        else if (0 == _tcscmp(szText, XML_TABCTRL_HEAD_LAYOUT_TYPE_FIX))
        {
            m_eHeadLayoutType = TABCTRL_HEAD_LAYOUT_TYPE_FIX;
        }
        else if (0 == _tcscmp(szText, XML_TABCTRL_HEAD_LAYOUT_TYPE_Win32))
        {
            m_eHeadLayoutType = TABCTRL_HEAD_LAYOUT_TYPE_Win32;
        }
    }

    pMapAttr->GetAttr_int(XML_TABCTRL_HEAD_HEIGHT, true, &m_nHeadHeight);

    if (m_pPanelContent)
    {
        bool bConfigContentLayout = pMapAttr->HasAttrib(XML_TABCTRL_CONTENTPANEL_PREFIX XML_LAYOUT);
        m_pPanelContent->SetAttributeByPrefix(XML_TABCTRL_CONTENTPANEL_PREFIX, pMapAttr, bReload, true);

        if (!bConfigContentLayout)
        {
            m_pPanelContent->SetLayoutType(LAYOUT_TYPE_CARD);
        }
    }
}


void  TabCtrlBase::AddItem(int nId, IObject* pBtn, IObject* pContent)
{
    if (NULL == pBtn)
        return;

    TabCtrlItemBase* p = new TabCtrlItemBase;
    p->m_pBtn = pBtn;
    p->m_pContent = pContent;
    p->m_nId = nId;

    pBtn->SetNotify(m_pITabCtrlBase, 1);
    if (pContent)
    {
        pContent->SetNotify(m_pITabCtrlBase->GetWindowObject(), 0);
        pContent->SetVisible(false, false, false);
    }

    if (0 == m_nCount)
    {
        m_pFirstItem = m_pLastItem = p;
    }
    else
    {
        m_pLastItem->m_pNext = p;
        p->m_pPrev = m_pLastItem;
        m_pLastItem = p;
    }
    if (NULL == m_pCurItem)
    {
        SetCurItem(p);
    }
    m_nCount++;
}


void  TabCtrlBase::OnSize(UINT nType, int cx, int cy)
{
    CRect  rcClient;
    m_pITabCtrlBase->GetClientRect(&rcClient);
    cx = rcClient.Width();
    cy = rcClient.Height();

    if (m_pITabCtrlBase->TestStyleEx(TABCTRL_STYLE_BOTTOM))
    {
        m_pPanelHead->SetObjectPos(0, cy-m_nHeadHeight, cx, m_nHeadHeight, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
        m_pPanelContent->SetObjectPos(0,0, cx, cy-m_nHeadHeight, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
    }
    else
    {
        m_pPanelHead->SetObjectPos(0,0,cx, m_nHeadHeight, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
        m_pPanelContent->SetObjectPos(0,m_nHeadHeight,cx, cy-m_nHeadHeight, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
    }

    RelayoutButton(0,0,cx, m_nHeadHeight);
}
void  TabCtrlBase::RelayoutButton(int x, int y, int width, int height)
{
    switch (m_eHeadLayoutType)
    {
    case TABCTRL_HEAD_LAYOUT_TYPE_FIX:
        {
            TabCtrlItemBase*  p = m_pFirstItem;
            while (p)
            {
                SIZE  s = p->m_pBtn->GetDesiredSize();
                p->m_pBtn->SetObjectPos(x, height-s.cy, s.cx, s.cy, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
                x += s.cx;
                p = p->m_pNext;
            }
        }
        break;

    case TABCTRL_HEAD_LAYOUT_TYPE_AVERAGE:
        {
            if (0 == m_nCount)
                break;

            int nAverageWidth = width/m_nCount;
            int nDiff = width%m_nCount;  // 误差。导致没有占满整行。
            TabCtrlItemBase*  p = m_pFirstItem;

            while (p)
            {
                if (p == m_pLastItem)
                    p->m_pBtn->SetObjectPos(x, 0, nAverageWidth+nDiff, height, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
                else
                    p->m_pBtn->SetObjectPos(x, 0, nAverageWidth, height, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);

                x += nAverageWidth;
                p = p->m_pNext;
            }
        }
        break;

    default:
        {
            const int N = 3;
            x += N;

            TabCtrlItemBase* p = m_pFirstItem;
            while (p)
            {
                SIZE  s = p->m_pBtn->GetDesiredSize();

                if (s.cy > height)
                    s.cy = height;

                if (m_pCurItem == p)
                {
                    p->m_pBtn->SetObjectPos(x-N, height-s.cy-N, s.cx+N+N, s.cy+N, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
                }
                else
                {
                    p->m_pBtn->SetObjectPos(x, height-s.cy, s.cx, s.cy, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
                }
                
                x += s.cx;
                p = p->m_pNext;
            }
        }
        break;
    }
}

void  TabCtrlBase::OnBnClicked(IMessage*  pMsgFrom)
{
    TabCtrlItemBase*  pItem = this->FindItemByBtn(static_cast<IObject*>(pMsgFrom)); 
    if (NULL == pItem)
    {
        UI_LOG_WARN(_T("%s  FindItemByBtn Failed."), FUNC_NAME);
        return ;
    }

    this->SetCurItem(pItem);
}

void  TabCtrlBase::SetCurItem(TabCtrlItemBase*  pItem)
{
    if (m_pCurItem == pItem)
        return;

    if (m_pCurItem)
    {
        // Hide
        if (m_pCurItem->m_pBtn)
        {
            m_pCurItem->m_pBtn->SetSelected(false);
        }
        if (m_pCurItem->m_pContent)
        {
            bool bNeedUpdate = false;
            if (NULL == pItem || NULL == pItem->m_pContent)
                bNeedUpdate = true;

            m_pCurItem->m_pContent->SetVisible(false, bNeedUpdate, false);  // 
        }
    }

    m_pCurItem = pItem;

    if (m_pCurItem)
    {
        // Show
        if (m_pCurItem->m_pBtn)
        {
            m_pCurItem->m_pBtn->SetSelected(true);
        }
        if (m_pCurItem->m_pContent)
        {
            m_pCurItem->m_pContent->SetVisible(true, true, false);
        }
    }

    // TODO: 不支持
//     if (m_eHeadLayoutType == TABCTRL_HEAD_LAYOUT_TYPE_Win32)
//     {
//         // 将当前项挪到最前面来 
//         if (m_nCount > 1)
//         {
//             m_pCurItem->m_pBtn->MoveToAsLastChild();
//         }
//         RelayoutButton(0, 0, m_pPanelHead->GetHeight(), m_pPanelHead->GetHeight());
//         m_pPanelHead->UpdateObject();
//     }
}

TabCtrlItemBase*  TabCtrlBase::FindItemByBtn(IObject* pObj)
{
    if (NULL == pObj)
        return NULL;

    TabCtrlItemBase*  p = m_pFirstItem;
    while (p)
    {
        if (p->m_pBtn == pObj)
            return p;

        p = p->m_pNext;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////

TabCtrl::TabCtrl()
{
    m_pITabCtrl = NULL;
}
TabCtrl::~TabCtrl()
{

}
#include "UISDK\Control\Inc\Interface\ibutton.h"
void  TabCtrl::AddItem(const TCHAR*  szText, IObject* pContentObj)
{
    IButton*  pBtn = NULL;
    IButton::CreateInstance(m_pITabCtrl->GetUIApplication(), &pBtn);

    IMapAttribute*  pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);
    UISendMessage(pBtn, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)0);
    pMapAttrib->Release();

    pBtn->SetConfigWidth(100);
    pBtn->SetConfigHeight(20);
    m_pPanelHead->AddChild(pBtn);

    __super::AddItem(0, pBtn, NULL);

    RelayoutButton(0,0, m_pITabCtrl->GetWidth(), 20);
    pBtn->UpdateObject();
}