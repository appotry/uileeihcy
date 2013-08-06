#include "stdafx.h"
#include "tabctrlbutton.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Control\Src\Renderbase\themerender.h"

namespace UI
{

TabCtrlButton::TabCtrlButton()
{
    m_pITabCtrlButton = NULL;
}
TabCtrlButton::~TabCtrlButton()
{
    
}

HRESULT  TabCtrlButton::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = __super::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    m_pITabCtrlButton->SetStyleEx(MAKECONTROLSUBTYPE(m_pITabCtrlButton->GetStyleEx(), BUTTON_STYLE_TABCTRLBUTTON));
    return S_OK;
}

void  TabCtrlButton::OnStateChanged(UINT nMask)
{
//     if (nOld & OSB_SELECTED  &&
//         nNew & OSB_SELECTED)
//         return;
//     
    m_pITabCtrlButton->UpdateObject();
}

void  TabCtrlButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    __super::SetAttribute(pMapAttrib, bReload);

    IUIApplication*  pUIApp = m_pITabCtrlButton->GetUIApplication();

    if (NULL == m_pITabCtrlButton->GetBkRender())
    {
        IRenderBase*  pRenderBase = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_TABCTRL_BUTTON_BKGND, m_pITabCtrlButton, &pRenderBase);
        if (pRenderBase)
        {
            TabCtrlButtonBkThemeRender* pTabCtrlBtn = (TabCtrlButtonBkThemeRender*)
                pRenderBase->QueryInterface(IID_UI_TabCtrlButtonBkThemeRender);
            if (pTabCtrlBtn)
            {
                pTabCtrlBtn->SetDirection(1);
            }

            m_pITabCtrlButton->SetBkgndRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
    }
}

void  TabCtrlButton::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    CRect rc(0,0, m_pITabCtrlButton->GetWidth(), m_pITabCtrlButton->GetHeight());

    IRenderBase* pBkgndRender = m_pITabCtrlButton->GetBkRender();
    if (pBkgndRender)
    {
        bool  bDisable = !m_pITabCtrlButton->IsEnable();
        bool  bHover   = m_pITabCtrlButton->IsHover();
        bool  bSelected = m_pITabCtrlButton->IsSelected();

        if (bDisable)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, TABCTRLBUTTON_BKGND_RENDER_STATE_DISABLE);
        }
        else if (bSelected)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, TABCTRLBUTTON_BKGND_RENDER_STATE_SELECTED);
        }
        else if (bHover)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, TABCTRLBUTTON_BKGND_RENDER_STATE_HOVER);
        }
        else 
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, TABCTRLBUTTON_BKGND_RENDER_STATE_NORMAL);
        }
    }

//     if (m_pITabCtrlButton->IsFocus())
//     {
//         ::InflateRect(&rc, -3, -3 );
//         pRenderTarget->DrawFocusRect(&rc);
//     }
}


void  TabCtrlButton::OnLButtonDown(UINT nFlags, POINT point)
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_BN_CLICKED;
    msg.pMsgFrom = m_pITabCtrlButton;

    m_pITabCtrlButton->DoNotify(&msg, 0);
}

}