#include "stdafx.h"
#include "listboxitem.h"
#include "listbox.h"

namespace UI
{
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
    if (m_pIListBoxItem->GetIListCtrlBase()->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE)) 
        return;
    
    SetMsgHandled(FALSE);
}

void ListBoxItem::OnLButtonUp(UINT nFlags, POINT point)
{
    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
    if (pListCtrl->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE))    // 以最后鼠标弹出来的那项作为被选中的项
    {
        IListItemBase* pNewHover = pListCtrl->HitTest(point);  // 重新取hover对象。因为当鼠标移到listbox外面时，会仍然保留最后一个hover item

        int nUpdateFlag = 0;
        if (pNewHover && pNewHover != pListCtrl->GetFirstSelectItem())
            pListCtrl->SelectItem(pListCtrl->GetHoverItem(), false);  // 因为即将要关闭了，所以不刷新
    }

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
    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (pTextRender)
    {
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIListBoxItem->GetText());
    }
}


}