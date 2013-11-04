#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\propertyctrl.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\item\propertyctrlgroupitem.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\item\propertyctrledititem.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\item\propertyctrlcomboboxitem.h"


namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrl, PropertyCtrl, TreeView);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlEditItem, PropertyCtrlEditItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlGroupItem, PropertyCtrlGroupItem, NormalTreeItem);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlEditItemShareData, PropertyCtrlEditItemShareData, IListItemTypeShareData);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlComboBoxItem, PropertyCtrlComboBoxItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlComboBoxItemShareData, PropertyCtrlComboBoxItemShareData, IListItemTypeShareData);

IPropertyCtrlGroupItem*  IPropertyCtrl::InsertGroupItem(const TCHAR* szName, const TCHAR* szDesc, 
                                         IListItemBase* pParent, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertGroupItem(szName, szDesc, pParent, pInsertAfter, nInsertFlags);
}

IPropertyCtrlEditItem*   IPropertyCtrl::InsertEditProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey, 
                                            IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertEditProperty(szText, szValue, szDesc, szKey, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlComboBoxItem*   IPropertyCtrl::InsertBoolProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey,
												IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
	return m_pPropertyCtrlImpl->InsertBoolProperty(szText, szValue, szDesc, szKey, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlComboBoxItem*   IPropertyCtrl::InsertComboBoxProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey,
													IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
	return m_pPropertyCtrlImpl->InsertComboBoxProperty(szText, szValue, szDesc, szKey, pParentItem, pInsertAfter, nInsertFlags);
}

void  IPropertyCtrlEditItem::SetValueText(const TCHAR* szText)    { m_pPropertyCtrlEditItemImpl->SetValueText(szText); }
void  IPropertyCtrlEditItem::SetDefaultValueText(const TCHAR* szText)  { m_pPropertyCtrlEditItemImpl->SetDefaultValueText(szText); }
void  IPropertyCtrlEditItem::SetKeyText(const TCHAR* szText)      { m_pPropertyCtrlEditItemImpl->SetKeyText(szText); }

void  IPropertyCtrlComboBoxItem::SetValueText(const TCHAR* szText)    { m_pPropertyCtrlComboBoxItemImpl->SetValueText(szText); }
void  IPropertyCtrlComboBoxItem::SetDefaultValueText(const TCHAR* szText)  { m_pPropertyCtrlComboBoxItemImpl->SetDefaultValueText(szText); }
void  IPropertyCtrlComboBoxItem::SetKeyText(const TCHAR* szText)      { m_pPropertyCtrlComboBoxItemImpl->SetKeyText(szText); }
void  IPropertyCtrlComboBoxItem::AddOption(const TCHAR* szItemText, const TCHAR* szItemValue)  { m_pPropertyCtrlComboBoxItemImpl->AddOption(szItemText, szItemValue); }
}