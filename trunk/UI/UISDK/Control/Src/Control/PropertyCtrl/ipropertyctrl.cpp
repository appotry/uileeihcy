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
                                         IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertGroupItem(szName, szDesc, pParent, pInsertAfter, nInsertFlags);
}

IPropertyCtrlEditItem*   IPropertyCtrl::InsertEditProperty(const TCHAR* szKey, const TCHAR* szValue, const TCHAR* szDesc, 
                                            IListItemBase* pParentItem, IListItemBase* pInsertAfter, int nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertEditProperty(szKey, szValue, szDesc, pParentItem, pInsertAfter, nInsertFlags);
}


void  IPropertyCtrlEditItem::SetValueText(const TCHAR* szText)    { m_pPropertyCtrlEditItemImpl->SetValueText(szText); }
}