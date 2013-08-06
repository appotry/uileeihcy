#include "stdafx.h"
#include "propertyctrlgroupitem.h"


namespace UI
{

PropertyCtrlGroupItem::PropertyCtrlGroupItem()
{
    m_pIPropertyCtrlGroupItem = NULL;
}
PropertyCtrlGroupItem::~PropertyCtrlGroupItem()
{

}

void  PropertyCtrlGroupItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlGroupItem->GetIListCtrlBase();

    // 背景
    Color c(PROPERTYCTRL_LINE_COLOR); 
    CRect rcParent;
    m_pIPropertyCtrlGroupItem->GetParentRect(&rcParent);
    pRenderTarget->FillRect(&rcParent, &c);

    CRect rcItem = rcParent;
    rcItem.left += pListCtrl->GetChildNodeIndent()* m_pIPropertyCtrlGroupItem->GetDepth();

    // 绘制展开收缩图标
    if (m_pShareData->nExpandIconWidth)
    {
        CRect rcExpandIcon = rcItem;
        rcExpandIcon.right = rcItem.left + m_pShareData->nExpandIconWidth;

        if (m_pShareData->pExpandIconRender && m_pIPropertyCtrlGroupItem->GetChildItem())
        {
            if (m_pIPropertyCtrlGroupItem->IsExpand())
                m_pShareData->pExpandIconRender->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL);
            else
                m_pShareData->pExpandIconRender->DrawState(pRenderTarget, &rcExpandIcon, TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL);
        }

        rcItem.left = rcExpandIcon.right;
    }

    // 绘制文本
    ITextRenderBase*  pTextRender = pListCtrl->GetTextRender();
    if (pTextRender)
        pTextRender->DrawState(pRenderTarget, &rcItem, 0, m_pIPropertyCtrlGroupItem->GetText());
}

}