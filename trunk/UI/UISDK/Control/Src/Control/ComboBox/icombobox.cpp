#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Control\Src\Control\ComboBox\combobox.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IComboBoxBase, ComboBoxBase, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(IComboBox, ComboBox, ComboBoxBase)

const TCHAR*  IComboBoxBase::GetText()
{
	return m_pComboBoxBaseImpl->GetText(); 
}
void  IComboBoxBase::SetText(const TCHAR* szText)
{
	m_pComboBoxBaseImpl->SetText(szText);
}
void  IComboBoxBase::SetDropDownObjectPtr(IObject* p) { m_pComboBoxBaseImpl->SetDropDownObjectPtr(p); }
void  IComboBoxBase::CloseUp() { m_pComboBoxBaseImpl->CloseUp(); }
IEdit*  IComboBoxBase::GetEdit() { return m_pComboBoxBaseImpl->GetEdit(); }

bool  IComboBox::AddString(const TCHAR* szText, int nAddItemFlags)
{
    return m_pComboBoxImpl->AddString(szText, nAddItemFlags);
}
IListBoxItem*  IComboBox::AddStringEx(const TCHAR* szText, int nAddItemFlags)
{
    return m_pComboBoxImpl->AddStringEx(szText, nAddItemFlags);
}
bool  IComboBox::SetCurSel(int nIndex)
{
    return m_pComboBoxImpl->SetCurSel(nIndex);
}
IListBox*  IComboBox::GetListBox()
{
    return m_pComboBoxImpl->GetListBox();
}
void  IComboBox::ResetContent()
{
	return m_pComboBoxImpl->ResetContent();
}

void  IComboBox::SetReadOnly(bool b)
{
    m_pComboBoxImpl->SetReadOnly(b);
}
bool  IComboBox::IsReadOnly()
{   
    return m_pComboBoxImpl->IsReadOnly();
}
}