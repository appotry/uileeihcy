#include "stdafx.h"
#include "qqloginaccountlistboxlayout.h"

void  QQLoginAccountListBoxLayout::Release()
{
    delete this;
}

void QQLoginAccountListBoxLayout::Arrange(UI::IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;
    m_pIListCtrlBase->GetClientRect(&rcClient);

    int  nWidth = rcClient.Width();
    int  yTop = 0;
    int  nTotalHeight = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    IQQLoginAccountListItem* pItem = (IQQLoginAccountListItem*)m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (NULL == pItem)
        return;

    IQQLoginAccountListItem* pPrevItem = (IQQLoginAccountListItem*)pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, pItem->GetHeight());
        pItem->SetParentRect(&rc);

        yPrevItemBottom = rc.bottom;
        pItem = (IQQLoginAccountListItem*)pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemBottom = pPrevItem->GetParentRect()->bottom;
    }

    while (pItem)
    {
        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + pItem->GetHeight();

        rcItem.SetRect(0, yTop, nWidth ,yPrevItemBottom);
        pItem->SetParentRect(&rcItem);
        pItem = (IQQLoginAccountListItem*)pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nWidth;
        pSizeContent->cy = yPrevItemBottom;
    }
}

void  QQLoginAccountListBoxLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    int  nCount = m_pIListCtrlBase->GetItemCount();

    // 加上动画所需要的高度
    if (0 == nCount)
        return;

    if (nCount == 1)
    {
        pSize->cy = ITEM_HILIGHT_HEIGHT;
    }
    else if (nCount == 2)
    {
        pSize->cy = ITEM_HILIGHT_HEIGHT+ITEM_NEAR_HEIGHT;
    }
    else
    {
        pSize->cy = nCount*ITEM_NORMAL_HEIGHT;
        pSize->cy += ITEM_HILIGHT_HEIGHT + 2*ITEM_NEAR_HEIGHT - 3*ITEM_NORMAL_HEIGHT;
    }

    // 加上行间隙
    if (nCount > 1)
    {
        pSize->cy += (nCount-1) * m_pIListCtrlBase->GetVertSpacing();
    }

//    pSize->cx = m_pIListCtrlBase->GetMaxDesiredWidth();
}
