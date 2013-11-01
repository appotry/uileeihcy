#include "stdafx.h"
#include "listboxitem.h"
#include "listbox.h"

namespace UI
{

ListBoxItemShareData::ListBoxItemShareData()
{
    m_pIListBoxItemShareData = NULL;
    m_nTextIndent = 0;
}
void  ListBoxItemShareData::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    pMapAttr->GetAttr_int(XML_LISTBOX_TEXT_INDENT, true, &m_nTextIndent);
}

ListBoxItem::ListBoxItem()
{
    m_pIListBoxItem = NULL; 
    m_pShareData = NULL; 
}

void  ListBoxItem::OnInitialize()
{
    DO_PARENT_PROCESS(IListBoxItem, IListItemBase);

    m_pIListBoxItem->SetItemType(LISTITEM_TYPE_ListBoxItem);

    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
    IListBoxItemShareData*  pData = static_cast<IListBoxItemShareData*>(
        pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_ListBoxItem));

    if (!pData)
    {
        IListBoxItemShareData::CreateInstance(pListCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        pListCtrl->SetItemTypeShareData(LISTITEM_TYPE_ListBoxItem, pData);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }
}

void  ListBoxItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    ITextRenderBase* pTextRender = m_pIListBoxItem->GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        *pSize = pTextRender->GetDesiredSize(m_pIListBoxItem->GetText());
    }
}

void ListBoxItem::OnLButtonDown(UINT nFlags, POINT point)
{
    // 覆盖listitembase的OnLButtonDown，仅在鼠标弹起时选择
    if (m_pIListBoxItem->GetIListCtrlBase()->TestStyleEx(LISTCTRLBASE_STYLE_POPUPLISTBOX)) 
        return;
    
    SetMsgHandled(FALSE);
}

void ListBoxItem::OnLButtonUp(UINT nFlags, POINT point)
{
    // popuplistctrlmousemgr处理了，这里不用再处理
//     IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
//     if (pListCtrl->TestStyleEx(LISTCTRLBASE_STYLE_POPUPLISTBOX))    // 以最后鼠标弹出来的那项作为被选中的项
//     {
//         IListItemBase* pNewHover = pListCtrl->HitTest(point);  // 重新取hover对象。因为当鼠标移到listbox外面时，会仍然保留最后一个hover item
// 
//         int nUpdateFlag = 0;
//         if (pNewHover && pNewHover != pListCtrl->GetFirstSelectItem())
//             pListCtrl->SelectItem(pListCtrl->GetHoverItem(), false);  // 因为即将要关闭了，所以不刷新
//     }

    SetMsgHandled(FALSE);
}

void  ListBoxItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIListBoxItem->GetParentRect(&rcParent);

    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIListBoxItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    CRect rcText = rcParent;
    if (m_pShareData)
        rcText.left += m_pShareData->m_nTextIndent;

    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (pTextRender)
    {
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIListBoxItem->GetText());
    }
}


}