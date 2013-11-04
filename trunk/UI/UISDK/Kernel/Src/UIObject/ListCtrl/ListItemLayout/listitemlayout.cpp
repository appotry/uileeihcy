#include "stdafx.h"
#include "listitemlayout.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"

namespace UI
{

void  ListCtrlLayoutMgr::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    if (NULL == m_pLayout || NULL == m_pIListCtrl)
        return;

    m_pLayout->Measure(pSize);

    pSize->cx += m_pIListCtrl->GetNonClientW();
    pSize->cy += m_pIListCtrl->GetNonClientH();

    *pSize = m_pIListCtrl->GetAdaptWidthHeight(pSize->cx, pSize->cy);
}

ListCtrlItemLayout1::ListCtrlItemLayout1()
{
    m_pIListCtrlBase = NULL;
}
void ListCtrlItemLayout1::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    int  nItemHeight = m_pIListCtrlBase->GetItemHeight();
    CRect  rcClient;
    CRect  rcItem;

    m_pIListCtrlBase->GetClientRect(&rcClient);

    int  nWidth = rcClient.Width();
    int  yTop = 0;
    int  yPrevItemBottom = 0;
    int  nVSpace = m_pIListCtrlBase->GetVertSpacing();

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (NULL == pIItem)
        return;

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, nItemHeight);
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
        yPrevItemBottom = yTop + nItemHeight;

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

void ListCtrlItemLayout1::Measure(SIZE* pSize)
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
    
        nVisibleCount ++;
        pItem = pItem->GetNextVisibleItem();
    }

    pSize->cx = nMaxWidth;
    pSize->cy = nVisibleCount*nItemHeight;

    // 加上行间隙
    if (nVisibleCount > 1)
        pSize->cy += (nVisibleCount-1) * m_pIListCtrlBase->GetVertSpacing();

}

//////////////////////////////////////////////////////////////////////////
ListCtrlItemLayout2::ListCtrlItemLayout2()
{
    m_pIListCtrlBase = NULL;
}
void  ListCtrlItemLayout2::Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent)
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

    ListItemBase* pItem = pIItem->GetImpl();
    ListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, nWidth, nItemHeight);
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
        yPrevItemBottom = yTop + nItemHeight;

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

void  ListCtrlItemLayout2::Measure(SIZE* pSize)
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

        nVisibleCount ++;
        pItem = pItem->GetNextVisibleItem();
    }

    pSize->cx = nMaxWidth;
    pSize->cy = nVisibleCount*nItemHeight;

    // 加上行间隙
    if (nVisibleCount > 1)
        pSize->cy += (nVisibleCount-1) * m_pIListCtrlBase->GetVertSpacing();
}

//////////////////////////////////////////////////////////////////////////

IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p)
{
    IListCtrlLayout* pLayout = NULL;
    switch (nType)
    {
    case LISTCTRL_ITEM_LAYOUT_TYPE_1:
        {
            pLayout = new ListCtrlItemLayout1();
            pLayout->SetIListCtrlBase(p);
        }
        break;

    case LISTCTRL_ITEM_LAYOUT_TYPE_2:
        {
            pLayout = new ListCtrlItemLayout2();
            pLayout->SetIListCtrlBase(p);
        }
        break;
    }

    return pLayout;
}
}