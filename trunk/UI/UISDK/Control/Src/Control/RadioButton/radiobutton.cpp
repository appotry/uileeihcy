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
//		������ע�͵���tab stop�ı仯����Ϊ��ǰ��û��ʵ��ʹ�÷�������л�ͬһ�����ڵ�
//		��������ͬʱҲ����ʹ�÷�������л�ͬһ�����ڵĶ���û��ʹ��TAB���л����ж���
//		���㡣
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

        // ȡ������radio��ť��ѡ��״̬

        bool bFind = false;
        if (!m_pIRadioButton->IsGroup()) // ���ǵ�һ��ѡ��.
        {
            IObject* pPrevRadioButton = m_pIRadioButton->GetPrevObject();  // ��ǰ����
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
            IObject* pNextRadioButton = m_pIRadioButton->GetNextObject();      // �������
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
	if (0 != lRet) // �ⲿ�Ѿ�������
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
        m_pIRadioButton->SetTransparent(true); // RadioĬ����͸����

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