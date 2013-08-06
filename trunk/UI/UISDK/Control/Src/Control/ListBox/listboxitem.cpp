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
    // ����listitembase��OnLButtonDown��������굯��ʱѡ��
    if (m_pIListBoxItem->GetIListCtrlBase()->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE)) 
        return;
    
    SetMsgHandled(FALSE);
}

void ListBoxItem::OnLButtonUp(UINT nFlags, POINT point)
{
    IListCtrlBase*  pListCtrl = m_pIListBoxItem->GetIListCtrlBase();
    if (pListCtrl->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE))    // �������굯������������Ϊ��ѡ�е���
    {
        IListItemBase* pNewHover = pListCtrl->HitTest(point);  // ����ȡhover������Ϊ������Ƶ�listbox����ʱ������Ȼ�������һ��hover item

        int nUpdateFlag = 0;
        if (pNewHover && pNewHover != pListCtrl->GetFirstSelectItem())
            pListCtrl->SelectItem(pListCtrl->GetHoverItem(), false);  // ��Ϊ����Ҫ�ر��ˣ����Բ�ˢ��
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