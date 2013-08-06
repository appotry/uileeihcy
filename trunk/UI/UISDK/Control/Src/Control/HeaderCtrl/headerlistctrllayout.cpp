#include "stdafx.h"
#include "headerlistctrllayout.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistitem.h"

namespace UI
{
void  HeaderListItemLayout::Release()
{
    delete this;
}

void HeaderListItemLayout::Arrange(UI::IListItemBase* pStartToArrange, SIZE* pSizeContent)
{
    CRect  rcClient;
    CRect  rcItem;
    m_pIListCtrlBase->GetClientRect(&rcClient);

    int  nHeight = rcClient.Height();
    int  xLeft = 0;
    int  nTotalWidth = 0;
    int  yPrevItemRight = 0;
    int  nHSpace = m_pIListCtrlBase->GetHorzSpacing();

    IListItemBase* pIItem = m_pIListCtrlBase->FindVisibleItemFrom(pStartToArrange); // m_pListCtrl->GetFirstItem();
    if (NULL == pIItem)
        return;
    IListItemBase* pItem = pIItem;

    IListItemBase* pPrevItem = pItem->GetPrevVisibleItem();
    if (NULL == pPrevItem)   // 第一个
    {
        CRect rc(0, 0, GetItemWidth(pItem), nHeight);
        pItem->SetParentRect(&rc);

        yPrevItemRight = rc.right;
        pItem = pItem->GetNextVisibleItem();
    }
    else
    {
        yPrevItemRight = pPrevItem->GetParentRect()->right;
    }

    while (pItem)
    {
        xLeft = yPrevItemRight + nHSpace;
        yPrevItemRight = xLeft + GetItemWidth(pItem);

        rcItem.SetRect(xLeft, 0, yPrevItemRight, nHeight);
        pItem->SetParentRect(&rcItem);
        pItem = pItem->GetNextVisibleItem();
    }

    if (pSizeContent)
    {
        pSizeContent->cx = yPrevItemRight;
        pSizeContent->cy = nHeight;
    }
}

void  HeaderListItemLayout::Measure(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    int  nCount = m_pIListCtrlBase->GetItemCount();

    // 加上动画所需要的高度
    if (0 == nCount)
        return;

    int  nHSpace = m_pIListCtrlBase->GetHorzSpacing();

    UI::IListItemBase*  pItem = m_pIListCtrlBase->FindVisibleItemFrom(NULL);
    while (pItem)
    {
        SIZE s = {0,0};
        pItem->GetDesiredSize(&s);
        pSize->cx += s.cx;
        pSize->cx += nHSpace;

        pItem = pItem->GetNextVisibleItem();
    }
}


int  HeaderListItemLayout::GetItemWidth(IListItemBase*  pItem)
{
    SIZE s = {0,0};

    UIMSG  msg;
    msg.pMsgTo = pItem;
    msg.message = UI_WM_GETDESIREDSIZE;
    msg.wParam = (WPARAM)&s;
    UISendMessage(&msg);

    return s.cx;
}
}