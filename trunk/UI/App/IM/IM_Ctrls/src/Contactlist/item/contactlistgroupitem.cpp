#include "stdafx.h"
#include "contactlistgroupitem.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"

ContactListGroupItemShareData::ContactListGroupItemShareData()
{
    m_nExpandIconItemWidth = 0;
    m_nExpandIconItemHeight = 0;
    m_nExpandIconItemCount = 0;
    m_pIContactListGroupItemShareData = NULL;
}

ContactListGroupItem::ContactListGroupItem()
{
    m_pIContactListGroupItem = NULL;
    m_lSort = 0;
    m_pTreeView = NULL;
    m_pShareData = NULL;
    m_nCurExpandIconIndex = 0;
}


HRESULT  ContactListGroupItem::FinalConstruct(UI::IUIApplication* p)
{
    DO_PARENT_PROCESS(IContactListGroupItem, UI::IListItemBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIContactListGroupItem->SetItemType(CONTACTLIST_ITEM_TYPE_GROUP);

    return S_OK;
}

void  ContactListGroupItem::OnInitialize()
{
    UI::IListCtrlBase*  pCtrlBase = m_pIContactListGroupItem->GetIListCtrlBase();
    m_pTreeView = static_cast<UI::ITreeView*>(pCtrlBase);

    IContactListGroupItemShareData* pData = (IContactListGroupItemShareData*)m_pTreeView->GetItemTypeShareData(CONTACTLIST_ITEM_TYPE_GROUP);
    if (!pData)
    {
        IContactListGroupItemShareData::CreateInstance(pCtrlBase->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        m_pTreeView->SetItemTypeShareData(CONTACTLIST_ITEM_TYPE_GROUP, pData);

        UI::IRenderBase*  pRenderBase = m_pTreeView->GetExpandIconRender();
        if (pRenderBase)
        {
            UI::IImageListRender* pImagelistRender = (UI::IImageListRender*)pRenderBase->QueryInterface(UI::uiiidof(IImageListRender));
            if (pImagelistRender)
            {
                m_pShareData->m_nExpandIconItemWidth = pImagelistRender->GetItemWidth();
                m_pShareData->m_nExpandIconItemHeight = pImagelistRender->GetItemHeight();
                m_pShareData->m_nExpandIconItemCount = pImagelistRender->GetItemCount();
            }
        }
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }
    m_nCurExpandIconIndex = m_pIContactListGroupItem->IsExpand()?m_pShareData->m_nExpandIconItemCount-1:0;
}

void  ContactListGroupItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    CRect  rcItem;
    m_pIContactListGroupItem->GetParentRect(&rcItem);

    // 绘制展开收缩图标
    CRect rcExpandIcon = rcItem;
    rcExpandIcon.left += 5; // icon/edge 间距
    rcExpandIcon.right = rcItem.left + m_pShareData->m_nExpandIconItemWidth;

    if (m_pTreeView->GetExpandIconRender())
    {
        m_pTreeView->GetExpandIconRender()->DrawState(pRenderTarget, &rcExpandIcon, m_nCurExpandIconIndex);
    }

     // 绘制文本
    rcItem.left = rcExpandIcon.right + 10; // text/icon 间距
    if (NULL != m_pTreeView->GetTextRender())
    {
        String  strText = m_pIContactListGroupItem->GetText();
        strText.append(_T(" [0/0]"));
        m_pTreeView->GetTextRender()->DrawState(pRenderTarget, &rcItem, 0, strText.c_str());
    }
}

void  ContactListGroupItem::Update(CONTACTLIST_GROUPITEM_INFO* pInfo)
{
    if (NULL == pInfo)
        return;

    if (pInfo->nMask & CLGI_MASK_SORT)
    {
        m_lSort = pInfo->lSort;       
    }
    if (pInfo->nMask & CLGI_MASK_TEXT)
    {
        m_pIContactListGroupItem->SetText(pInfo->pszText);
    }
}

void  ContactListGroupItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_pTreeView->ToggleItemExpand(m_pIContactListGroupItem, true);
}
void  ContactListGroupItem::OnLButtonDblClk(UINT nFlags, POINT point)
{
    m_pTreeView->ToggleItemExpand(m_pIContactListGroupItem, true);
}

LRESULT  ContactListGroupItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_COLLAPSED)
    {
        bool bExpand = m_pIContactListGroupItem->IsExpand();
        UI::IUIApplication*  pUIApp = m_pTreeView->GetUIApplication();
        
        pUIApp->KillTimerById(0, m_pIContactListGroupItem);
        pUIApp->KillTimerById(1, m_pIContactListGroupItem);

        if (bExpand)
        {
            m_pTreeView->GetUIApplication()->SetTimerById(20, 0, m_pIContactListGroupItem);
        }
        else
        {
            m_pTreeView->GetUIApplication()->SetTimerById(20, 1, m_pIContactListGroupItem);
        }
    }
    return 0;
}

void  ContactListGroupItem::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    UI::TimerItem* pItem = (UI::TimerItem*)lParam;
    if (0 == pItem->nId)
    {
        m_nCurExpandIconIndex++;
        if ((m_nCurExpandIconIndex+1) >= m_pShareData->m_nExpandIconItemCount)
        {
            m_pTreeView->GetUIApplication()->KillTimer(nIDEvent);
        }
        UI::IListItemBase* pArray[1] = {static_cast<UI::IListItemBase*>(m_pIContactListGroupItem)};
        m_pTreeView->RedrawItem(pArray, 1);
    }
    else if (1 == pItem->nId)
    {
        m_nCurExpandIconIndex--;
        if ((m_nCurExpandIconIndex) < 0)
        {
            m_nCurExpandIconIndex = 0;
            m_pTreeView->GetUIApplication()->KillTimer(nIDEvent);
            return;
        }
        UI::IListItemBase* pArray[1] = {static_cast<UI::IListItemBase*>(m_pIContactListGroupItem)};
        m_pTreeView->RedrawItem(pArray, 1);
    }
}