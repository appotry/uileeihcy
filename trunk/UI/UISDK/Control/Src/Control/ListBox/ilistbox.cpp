#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"
#include "UISDK\Control\Src\Control\ListBox\listboxitem.h"

namespace UI
{
    
UI_IMPLEMENT_Ixxx_INTERFACE2(IListBoxBase, ListBoxBase, IListCtrlBase)
UI_IMPLEMENT_Ixxx_INTERFACE2(IListBoxItem, ListBoxItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE(IListBox, ListBox, ListBoxBase)


void  IListBoxBase::SetBindObject(IObject* pCombobox)
{
    m_pListBoxBaseImpl->SetBindObject(pCombobox);
}

IListBoxItem*  IListBox::AddString(const TCHAR* szText, int nAddItemFlags)
{
    return m_pListBoxImpl->AddString(szText, nAddItemFlags);
}

bool  IListBoxBase::SetSel(int nIndex)
{
    return m_pListBoxBaseImpl->SetSel(nIndex);
}


}