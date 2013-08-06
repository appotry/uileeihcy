#include "stdafx.h"
#include "listviewitemreportlayout.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

ListViewItemReportLayout::ListViewItemReportLayout()
{
    m_pListCtrlBase = NULL;
    m_nHeaderCtrlWidth = 0;
}
void  ListViewItemReportLayout::SetIListCtrlBase(IListCtrlBase* p)
{
    m_pListCtrlBase = p;
}

void ListViewItemReportLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    if (pSizeContent)  // 避免没有子结点时提前返回
        pSizeContent->cx = m_nHeaderCtrlWidth;

    int  nWidth = m_nHeaderCtrlWidth;
    int  yTop = 0;
    int  yPrevItemBottom = 0;
    int  nTotalHeight = 0;
    int  nItemHeight = m_pListCtrlBase->GetItemHeight();
    int  nVSpace = m_pListCtrlBase->GetVertSpacing();

    IListItemBase* pItem = m_pListCtrlBase->FindVisibleItemFrom(pStartToArrange);
    if (NULL == pItem)
        return;

    IListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, nItemHeight);
        pItem->SetParentRect(&rc);

        yPrevItemBottom = rc.bottom;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        CRect rcItem;
        pPrevItem->GetParentRect(&rcItem);
        yPrevItemBottom = rcItem.bottom;
    }
    
    while (pItem)
    {
        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + nItemHeight;

        CRect rc(0, yTop, nWidth, yPrevItemBottom);
        pItem->SetParentRect(&rc);

        nTotalHeight += rc.Height();
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nWidth;
        pSizeContent->cy = nTotalHeight;
    }
}
void ListViewItemReportLayout::Measure(SIZE* pSize)
{
    pSize->cy = 0;
    pSize->cx = m_nHeaderCtrlWidth;

    // 加上行间隙
    int nCount = m_pListCtrlBase->GetVisibleItemCount();
    pSize->cy = nCount * m_pListCtrlBase->GetItemHeight();
    if (nCount > 1)
    {
        pSize->cy += (nCount-1) * m_pListCtrlBase->GetVertSpacing();
    }
}

}