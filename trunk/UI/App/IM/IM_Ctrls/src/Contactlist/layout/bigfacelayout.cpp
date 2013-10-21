#include "stdafx.h"
#include "BigfaceLayout.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"

void  BigfaceLayout::Release()
{
    delete this;
}

void BigfaceLayout::Arrange(UI::IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;
    m_pIListCtrlBase->GetClientRect(&rcClient);

    int  nWidth = rcClient.Width();
    int  yTop = 0;
    int  nTotalHeight = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    UI::IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (NULL == pItem)
        return;

    UI::IListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, GetItemHeight(pItem));
        pItem->SetParentRect(&rc);

        yPrevItemBottom = rc.bottom;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemBottom = pPrevItem->GetParentRect()->bottom;
    }

    while (pItem)
    {
        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + GetItemHeight(pItem);

        rcItem.SetRect(0, yTop, nWidth ,yPrevItemBottom);
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = nWidth;
        pSizeContent->cy = yPrevItemBottom;
    }
}

void  BigfaceLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    int  nCount = m_pIListCtrlBase->GetItemCount();

    // 加上动画所需要的高度
    if (0 == nCount)
        return;

    UI::IListItemBase*  pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        SIZE s = {0,0};
        pItem->GetDesiredSize(&s);
        pSize->cy += s.cy;

        pItem = pItem->GetNextVisibleItem();
    }
}


int  BigfaceLayout::GetItemHeight(UI::IListItemBase*  pItem)
{
    long  lItemType = pItem->GetItemType();
    if (lItemType == CONTACTLIST_ITEM_TYPE_CONTACT)
        return 50;

    return 20;
}