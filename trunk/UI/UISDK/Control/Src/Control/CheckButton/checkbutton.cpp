#include "stdafx.h"
#include "checkbutton.h"

namespace UI
{

CheckButton::CheckButton()
{
    m_pICheckButton = NULL;
}

CheckButton::~CheckButton()
{
}

HRESULT  CheckButton::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = Button::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    m_pICheckButton->SetStyleEx(MAKECONTROLSUBTYPE(m_pICheckButton->GetStyleEx(), BUTTON_STYLE_CHECKBUTTON));
    return S_OK;
}

void CheckButton::OnClicked()
{
    // 在外部响应时能够正确调用IsChecked进行判断
    if (m_nCheckState & BST_CHECKED)
        m_nCheckState &= ~BST_CHECKED;
    else
        m_nCheckState |= BST_CHECKED;

    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)&pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pICheckButton;
    LRESULT lRet = m_pICheckButton->DoNotify(&msg);
    if (0 != lRet) // 外部已经处理了
    {
        return;
    }
}

void CheckButton::ResetAttribute()
{
	__super::ResetAttribute();
	m_nIconMarginText = 2;
}

void CheckButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    __super::SetAttribute(pMapAttrib, bReload);

    if (NULL == m_pICheckButton->GetBkRender())
    {
        m_pICheckButton->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, 0);  // check默认是透明的

        if (NULL == m_pICheckButton->GetForeRender())
        {
            IRenderBase* p = NULL;
            m_pICheckButton->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_CHECK_BUTTON, m_pICheckButton, &p);

            if (p)
            {
                m_pICheckButton->SetForegndRender(p);
                p->Release();
                m_nIconMarginText = 4;
            }
        }
    }
}
}