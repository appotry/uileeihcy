#include "stdafx.h"
#include "compoundbutton.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

namespace UI
{

CompoundButton::CompoundButton()
{
    m_pICompoundButton = NULL;
	m_pLayout = NULL; 

}
CompoundButton::~CompoundButton()
{
	SAFE_RELEASE(this->m_pLayout);
}

void CompoundButton::ResetAttribute()
{
	__super::ResetAttribute();
	SAFE_RELEASE(m_pLayout);
}

void CompoundButton::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    ButtonBase::SetAttribute(pMapAttrib, bReload);

	SAFE_RELEASE(m_pLayout);
	//m_pLayout = LayoutManagerFactory::GetLayoutFromAttribute(pMatAttrib, this, true);
    const TCHAR* szText = pMapAttrib->GetAttr(XML_LAYOUT, true);
    m_pICompoundButton->GetUIApplication()->CreateLayoutByName((BSTR)szText, m_pICompoundButton, true, &m_pLayout);
    if (m_pLayout)
        m_pLayout->SetAttribute(pMapAttrib);
}

void  CompoundButton::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    
    if (m_pLayout)
        m_pLayout->Arrange(NULL, false);
}

void  CompoundButton::GetDesiredSize(SIZE* pSize)
{
	UIASSERT(this->m_pLayout != NULL);

	// ��ȡ�Ӷ�������Ҫ�Ŀռ�
	if (this->m_pLayout != NULL)
		*pSize = this->m_pLayout->Measure();
}

void CompoundButton::DrawFocus(IRenderTarget* pRenderTarget)
{
// 	CRect rc;
// 	rc.SetRect(0,0, GetWidth(), GetHeight());
// 	::InflateRect( &rc, -3, -3 );
// 
// 	pRenderTarget->DrawFocusRect(&rc);
}

void CompoundButton::OnObjectLoaded()
{
	SetMsgHandled(FALSE);

	IObject* pObj = NULL;
	while (pObj = m_pICompoundButton->EnumChildObject(pObj))
	{
		pObj->AddHook(m_pICompoundButton, 0, 1);  // ����hover״̬�ı�
	}
}

// �������Ƶ��˸��ϰ�ť�ϵ��ӿؼ����棬���ʱ�򸴺ϰ�ť�ǲ�Ӧ��ȥˢ�µ�
// �����ʱ��Ҳ��ȷ��������ʲô������״̬�ı䣬���ͨ���ӳ�ˢ�������������
void CompoundButton::OnStateChanged2(UINT nMask)
{
	UIMSG msg;
	msg.message = UI_WM_REDRAWOBJECT;
	msg.pMsgTo = m_pICompoundButton;
	UIPostMessage(m_pICompoundButton->GetUIApplication(), &msg);
}

void CompoundButton::OnRedrawObject()
{
	m_pICompoundButton->UpdateObject();
}

// ���λ�ڸ��Ͽؼ����Ӷ�������ʱ�����Ͽؼ�Ҳ��hover״̬��ʾ
void CompoundButton::OnChildObjStateChanged(UINT nMask)
{
	SetMsgHandled(FALSE);

    IObject* pObj = static_cast<IObject*>(GetCurMsg()->pMsgTo);
    if (nMask & (OSB_HOVER | OSB_FORCEHOVER | OSB_PRESS | OSB_FORCEPRESS | OSB_FOCUS))
    {
        m_pICompoundButton->SetForceHover(pObj->IsHover()||pObj->IsPress() || pObj->IsFocus(), false);

        // ��Ϊ�ӿؼ����ڱ�HOOK��ûˢ���ˣ���Combobox��ˢ��
        UIMSG msg;
        msg.pMsgFrom = m_pICompoundButton;
        msg.pMsgTo = m_pICompoundButton;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEHOVER;
        UIPostMessage(m_pICompoundButton->GetUIApplication(), &msg);  // �п����Ǵ��Ӷ������Ƶ�COMBOBOX�����Ҫ�ӳ�ˢ��

        SetMsgHandled(TRUE);
    }

// 	bool OldHover = (nOld&(OSB_HOVER | OSB_FORCEHOVER | OSB_PRESS | OSB_FORCEPRESS)) == 0?false:true;
// 	bool bNewHover = (nNew&(OSB_HOVER | OSB_FORCEHOVER | OSB_PRESS | OSB_FORCEPRESS)) == 0?false:true;
// 
// 	int nOldStateBit = m_pICompoundButton->GetStateBit();
// 	if (bNewHover && !OldHover)
// 	{
// 		m_pICompoundButton->SetForceHover(true);
// 	}
// 	else if (!bNewHover && OldHover)
// 	{
// 		m_pICompoundButton->SetForceHover(false);
// 	}
// 	if (m_pICompoundButton->GetStateBit() != nOldStateBit)
// 	{
// 		UIMSG msg;
// 		msg.pMsgFrom = m_pICompoundButton;
// 		msg.pMsgTo = m_pICompoundButton;
// 		msg.message = UI_WM_STATECHANGED;
// 		msg.wParam = nOldStateBit;
// 		msg.lParam = m_pICompoundButton->GetStateBit();
// 		UIPostMessage(m_pICompoundButton->GetUIApplication(), &msg);
// 
// 		SetMsgHandled(TRUE);
// 	}
}
LRESULT CompoundButton::OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pLayout;
}


}