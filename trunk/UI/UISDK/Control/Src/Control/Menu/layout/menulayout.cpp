#include "stdafx.h"
#include "menulayout.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{
MenuItemLayout::MenuItemLayout()
{
    m_pIListCtrlBase = NULL;
}
void  MenuItemLayout::SetIListCtrlBase(IListCtrlBase* p)
{
    m_pIListCtrlBase = p;
}

void MenuItemLayout::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    int  nItemHeight = m_pIListCtrlBase->GetItemHeight();

    CRect  rcItem;
    CRect  rcClient;
    m_pIListCtrlBase->GetClientRect(&rcClient);

    int  nWidth = rcClient.Width();
    int  nMaxDisiredWidth = 0; // m_pListCtrl->GetMaxDesiredWidth(NULL);
    {
        SIZE  sizeItem = {0, 0};
        IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
        while (pItem)
        {
            UISendMessage(pItem, UI_WM_GETDESIREDSIZE, (WPARAM)&sizeItem);
            if (sizeItem.cx > nMaxDisiredWidth)
                nMaxDisiredWidth = sizeItem.cx;

            pItem = pItem->GetNextVisibleItem();
        }
    }

    if (nWidth < nMaxDisiredWidth)
        nWidth = nMaxDisiredWidth;

    int  yTop = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (NULL == pIItem)
        return;

    IListItemBase*  pItem = pIItem;
    IListItemBase*  pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // ��һ��
    {
        SIZE s = {0};
        pItem->GetDesiredSize(&s);

        CRect rc(0, 0, nWidth, s.cy);
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
        SIZE s = {0};
        pItem->GetDesiredSize(&s);

        yTop = yPrevItemBottom + nVSpace;
        yPrevItemBottom = yTop + s.cy;

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
void MenuItemLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    int   nVisibleCount = 0;
    SIZE  sizeItem = {0, 0};
    int   nMaxWidth = 0;
    int   nItemHeight = m_pIListCtrlBase->GetItemHeight();

    IListItemBase* pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        pItem->GetDesiredSize(&sizeItem);
        if (sizeItem.cx > nMaxWidth)
            nMaxWidth = sizeItem.cx;

        pSize->cy += sizeItem.cy;

        nVisibleCount ++;
        pItem = pItem->GetNextVisibleItem();
    }

    pSize->cx = nMaxWidth;

    // �����м�϶
    if (nVisibleCount > 1)
        pSize->cy += (nVisibleCount-1) * m_pIListCtrlBase->GetVertSpacing();
}
}