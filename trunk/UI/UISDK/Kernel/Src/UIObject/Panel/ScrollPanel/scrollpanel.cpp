#include "stdafx.h"
#include "scrollpanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

namespace UI
{

ScrollPanel::ScrollPanel()
{
    m_pIScrollPanel = NULL;
    m_pMgrScrollBar = NULL;
    m_sizeView.cx = m_sizeView.cy = NDEF;
}
ScrollPanel::~ScrollPanel()
{
    if (m_pMgrScrollBar)
    {
        delete m_pMgrScrollBar->GetIScrollBarMgr();
        m_pMgrScrollBar = NULL;
    }
}

HRESULT  ScrollPanel::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IScrollPanel, IPanel);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    IScrollBarManager* pScrollBarMgr = new IScrollBarManager;
    m_pMgrScrollBar = pScrollBarMgr->GetImpl();
    m_pMgrScrollBar->SetBindObject(m_pIScrollPanel);

    return S_OK;
}

void  ScrollPanel::ResetAttribute()
{
    Panel::ResetAttribute();

    this->ModifyStyle(CONTROL_STYLE_TABSTOP, 0, 0);  // 允许有焦点，这样才能响应mousewheel消息进行滚动

    m_sizeView.cx = m_sizeView.cy = NDEF;
    if (m_pMgrScrollBar)
        m_pMgrScrollBar->ResetAttribute();
}
void  ScrollPanel::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    Panel::SetAttribute(pMapAttrib, bReload);
    pMapAttrib->GetAttr_intarray(XML_SCROLLPANEL_VIEWSIZE, true, (int*)&m_sizeView, 2);

    if (m_pMgrScrollBar)
        m_pMgrScrollBar->SetAttribute(pMapAttrib, bReload);
}

//
// 1. 指定了viewSize，直接传递viewSize给滚动条作为range
// 2. 没有指定viewSize，则使用DesiredSize作为滚动条range
void  ScrollPanel::OnSize(UINT nType, int cx, int cy)
{
    Panel::OnSize(nType, cx, cy);

    SIZE sizeContent = {0, 0};
    if (m_sizeView.cx != NDEF && m_sizeView.cy != NDEF)
    {
        sizeContent.cx = m_sizeView.cx;
        sizeContent.cy = m_sizeView.cy;
    }
    else
    {
        ILayout* pLayout = m_pIScrollPanel->GetLayout();
        if (NULL == pLayout)
            return;

        sizeContent = pLayout->Measure();
    }

    CRect rcClient;
    m_pIScrollPanel->GetClientRect(&rcClient);
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };
    m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
}


BOOL  ScrollPanel::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    int nhScrollPos = 0, nvScrollPos = 0;
    int nhScrollPos2 = 0, nvScrollPos2 = 0;

    m_pMgrScrollBar->GetScrollPos(&nhScrollPos, &nvScrollPos);
    m_pMgrScrollBar->nvProcessMessage(GetCurMsg(), 0, false);
    m_pMgrScrollBar->GetScrollPos(&nhScrollPos2, &nvScrollPos2);

    if (nvScrollPos != nvScrollPos2 || nhScrollPos != nhScrollPos2)
    {
        this->UpdateObject();
    }

    return 0;
}

}