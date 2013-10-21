#include "stdafx.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountlistbox.h"
#include "QQLoginAccountComboBox.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

QQLoginAccountComboBox::QQLoginAccountComboBox()
{
    m_pIQQLoginAccountComboBox = NULL;
	m_pDropDownCtrl = NULL; 
}
QQLoginAccountComboBox::~QQLoginAccountComboBox()
{
    SAFE_DELETE_Ixxx(m_pDropDownCtrl);
}

HRESULT  QQLoginAccountComboBox::FinalConstruct(UI::IUIApplication* p)
{
    SetMsgHandled(FALSE);

	IQQLoginAccountListBox* pListBox = NULL;
    IQQLoginAccountListBox::CreateInstance(p, &pListBox);
    pListBox->SetID(COMBOBOX_LIST_ID);
    pListBox->ModifyStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE, 0, true);
	pListBox->SetBindObject(m_pIQQLoginAccountComboBox);
    pListBox->AddHook(m_pIQQLoginAccountComboBox, 0, COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID);
    pListBox->SetNotify(m_pIQQLoginAccountComboBox, COMBOBOX_LISTBOX_MSG_NOTIFY_MSG_ID);

	m_pDropDownCtrl = pListBox;
    m_pIQQLoginAccountComboBox->SetDropDownObjectPtr(pListBox);

	return S_OK;
}
HRESULT QQLoginAccountComboBox::OnDropDown(WPARAM, LPARAM)
{
    if (NULL == m_pDropDownCtrl)
        return FALSE;

    if (0 == m_pDropDownCtrl->GetItemCount())
        return FALSE;

    SetMsgHandled(FALSE);

    return TRUE;
}

void  QQLoginAccountComboBox::ResetAttribute()
{
    DO_PARENT_PROCESS(IQQLoginAccountComboBox, UI::IComboBoxBase);
    if (m_pDropDownCtrl)
        m_pDropDownCtrl->ModifyStyleEx(LISTCTRLBASE_STYLE_SORT_ASCEND|LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE, 0, false);
}


void  QQLoginAccountComboBox::SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(IQQLoginAccountComboBox, UI::IComboBoxBase);
    if (m_pDropDownCtrl)
        m_pDropDownCtrl->SetAttributeByPrefix(XML_COMBOBOX_LISTBOX_PRIFIX, pMapAttrib, bReload, true);
}

void QQLoginAccountComboBox::AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath)
{
	if (m_pDropDownCtrl) 
	    m_pDropDownCtrl->AddAccount(szId, szName, szFaceImgPath);
}

bool  QQLoginAccountComboBox::SetCurSel(int nIndex)
{
	if (NULL == m_pDropDownCtrl)
		return false;

	return m_pDropDownCtrl->SetSel(nIndex);
}


void QQLoginAccountComboBox::OnLCNSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem)
{
    SetMsgHandled(FALSE);
    
    UI::IEdit*  pEdit = m_pIQQLoginAccountComboBox->GetEdit();
    if (pEdit)
    {
        if (pSelItem)
        {
            pEdit->SetText(pSelItem->GetText());
            pEdit->SetSel(0,-1);
            if (!pEdit->IsFocus())
            {
                m_pIQQLoginAccountComboBox->GetWindowObject()->SetFocusObject(pEdit);
            }
        }
        else
        {
            pEdit->SetText(NULL);
        }
    }

    {
        UI::UIMSG   msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode    = UI_CBN_SELCHANGED;
        msg.wParam  = (WPARAM)pOldSelItem;
        msg.lParam  = (LPARAM)pSelItem;
        msg.pMsgFrom = m_pIQQLoginAccountComboBox;
        m_pIQQLoginAccountComboBox->DoNotify( &msg );
    }
}


LRESULT  QQLoginAccountComboBox::OnLCNClick(WPARAM, LPARAM)
{
    m_pIQQLoginAccountComboBox->CloseUp();
    return 0;
}