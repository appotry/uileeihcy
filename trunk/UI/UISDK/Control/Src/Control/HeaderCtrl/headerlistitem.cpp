#include "stdafx.h"
#include "headerlistitem.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"


namespace UI
{
HeaderListItem::HeaderListItem()
{
    m_pIHeaderListItem = NULL;
    m_pHeaderCtrl = NULL;
    m_nTextAlign = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
}
HeaderListItem::~HeaderListItem()
{

}

void  HeaderListItem::OnInitialize()
{
    SetMsgHandled(FALSE);

    IHeaderListCtrl* pHeaderCtrl = (IHeaderListCtrl*)m_pIHeaderListItem->GetIListCtrlBase()->QueryInterface(uiiidof(IHeaderListCtrl));
    if (pHeaderCtrl)
        m_pHeaderCtrl = pHeaderCtrl->GetImpl();
}

void  HeaderListItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    UINT nRenderState = 0;
    CRect  rcItem;
    m_pIHeaderListItem->GetParentRect(&rcItem);
    IRenderBase* pRender = m_pIHeaderListItem->GetIListCtrlBase()->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIHeaderListItem->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcItem, nRenderState);
    }

    CRect rcText = rcItem;
    ITextRenderBase* pTextRender = m_pIHeaderListItem->GetIListCtrlBase()->GetTextRender();
    if (pTextRender)
    {
        rcText.left += HEADER_DIVIDER_EXTENT_SIZE;
        rcText.right -= HEADER_DIVIDER_EXTENT_SIZE;

        // 产生两像素按下的效果
        if (nRenderState & RENDER_STATE_PRESS)
        {
            rcText.top += 2;
            rcText.left += 2;
        }

        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIHeaderListItem->GetText(), m_nTextAlign);
    }
}

LRESULT  HeaderListItem::OnGetColumnInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ColumnInfo* p = (ColumnInfo*)wParam;
    p->nFormat = m_nTextAlign;
    p->nIndentLeft = HEADER_DIVIDER_EXTENT_SIZE;
    p->nIndentRight = HEADER_DIVIDER_EXTENT_SIZE;

    if (m_pIHeaderListItem->IsVisible())
    {
        CRect rc;
        m_pIHeaderListItem->GetParentRect(&rc);
        p->nWidth = rc.Width();
    }
    else
    {
        p->nWidth = 0;
    }
    p->pHeaderItem = m_pIHeaderListItem;

    return 0;
}

}