#include "stdafx.h"
#include "radiobutton.h"

namespace UI
{

RadioButton::RadioButton()
{
    m_pIRadioButton = NULL;
}

HRESULT  RadioButton::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = Button::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    UINT nStyleEx = m_pIRadioButton->GetStyleEx();
    m_pIRadioButton->SetStyleEx(MAKECONTROLSUBTYPE(nStyleEx, BUTTON_STYLE_RADIOBUTTON));
    return S_OK;
}

//
//	TODO:
//		在这里注释掉了tab stop的变化，因为当前还没有实现使用方向键来切换同一个组内的
//		其它对象。同时也觉得使用方向键来切换同一个组内的对象没有使用TAB键切换所有对象
//		方便。
//
void RadioButton::OnClicked()
{
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode    = UI_BN_CLICKED;
    msg.wParam  = NULL; // (WPARAM)pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pIRadioButton;

    if (m_nCheckState & BST_CHECKED)
    {
    }
    else
    {
        this->SetCheck(BST_CHECKED);
        //		this->SetTabstop(true);

        // 取消其它radio按钮的选择状态

        bool bFind = false;
        if (!m_pIRadioButton->IsGroup()) // 不是第一个选项.
        {
            IObject* pPrevRadioButton = m_pIRadioButton->GetPrevObject();  // 往前遍历
            while (1)
            {
                if (NULL == pPrevRadioButton || (IObject*)m_pIRadioButton == pPrevRadioButton)
                    break;

                if (pPrevRadioButton->GetObjectExtentType() != CONTROL_RADIOBUTTON)
                    break;

                if ((static_cast<IButtonBase*>(pPrevRadioButton))->IsChecked())
                {
                    bFind = true;
                    (static_cast<IButtonBase*>(pPrevRadioButton))->SetCheck(BST_UNCHECKED);
                    //					((ButtonBase*)pPrevRadioButton)->SetTabstop(false);
                    break;
                }

                if (pPrevRadioButton->IsGroup())
                    break;

                pPrevRadioButton = pPrevRadioButton->GetPrevObject();
            }

        }

        if (!bFind)
        {
            IObject* pNextRadioButton = m_pIRadioButton->GetNextObject();      // 往后遍历
            while (1)
            {
                if (NULL == pNextRadioButton || (IObject*)m_pIRadioButton == pNextRadioButton)
                    break;

                if (pNextRadioButton->IsGroup() )
                    break;

                if (pNextRadioButton->GetObjectExtentType() != CONTROL_RADIOBUTTON)
                    break;

                if ((static_cast<IButtonBase*>(pNextRadioButton))->IsChecked())
                {
                    bFind = true;
                    (static_cast<IButtonBase*>(pNextRadioButton))->SetCheck(BST_UNCHECKED);
                    //					((ButtonBase*)pPrevRadioButton)->SetTabstop(false);
                    break;
                }

                pNextRadioButton = pNextRadioButton->GetNextObject();
            }
        }
    }

	LRESULT lRet = m_pIRadioButton->DoNotify(&msg);
	if (0 != lRet) // 外部已经处理了
		return;
}

void RadioButton::ResetAttribute()
{
    __super::ResetAttribute();
    m_nIconMarginText = 2;
}

void RadioButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    __super::SetAttribute(pMapAttrib, bReload);

    if (NULL == m_pIRadioButton->GetBkRender())
    {
        m_pIRadioButton->SetTransparent(true); // Radio默认是透明的

        if (NULL == m_pIRadioButton->GetForeRender())
        {
            IRenderBase* p = NULL;
            m_pIRadioButton->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_RADIO_BUTTON, m_pIRadioButton, &p);

            if (p)
            {
                m_pIRadioButton->SetForegndRender(p);
                p->Release();
                m_nIconMarginText = 3;
            }
        }
    }
}


}