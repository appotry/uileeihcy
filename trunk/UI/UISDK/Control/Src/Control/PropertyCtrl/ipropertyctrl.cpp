#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\propertyctrl.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\item\propertyctrlgroupitem.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\item\propertyctrledititem.h"


namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrl, PropertyCtrl, TreeView);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlEditItem, PropertyCtrlEditItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlGroupItem, PropertyCtrlGroupItem, NormalTreeItem);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPropertyCtrlEditItemShareData, PropertyCtrlEditItemShareData, IListItemTypeShareData);

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


void  IPropertyCtrlEditItem::SetValueText(const TCHAR* szText)    { m_pPropertyCtrlEditItemImpl->SetValueText(szText); }
void  IPropertyCtrlEditItem::SetDefaultValueText(const TCHAR* szText)  { m_pPropertyCtrlEditItemImpl->SetDefaultValueText(szText); }
void  IPropertyCtrlEditItem::SetKeyText(const TCHAR* szText)      { m_pPropertyCtrlEditItemImpl->SetKeyText(szText); }
}