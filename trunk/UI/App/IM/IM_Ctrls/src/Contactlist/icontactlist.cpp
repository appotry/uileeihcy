#include "stdafx.h"
#include "App\IM\IM_Ctrls\inc\icontactlist.h"
#include "App\IM\IM_Ctrls\src\Contactlist\contactlist.h"
#include "App\IM\IM_Ctrls\src\Contactlist\item\contactlistgroupitem.h"
#include "App\IM\IM_Ctrls\src\Contactlist\item\contactlistcontactitem.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(IContactList, ContactList, ITreeView)

UI_IMPLEMENT_Ixxx_INTERFACE2(IContactListContactItemShareData, ContactListContactItemShareData, IListItemTypeShareData);
UI_IMPLEMENT_Ixxx_INTERFACE2(IContactListGroupItemShareData, ContactListGroupItemShareData, IListItemTypeShareData);
UI_IMPLEMENT_Ixxx_INTERFACE2(IContactListGroupItem, ContactListGroupItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE2(IContactListContactItem, ContactListContactItem, IListItemBase)

//////////////////////////////////////////////////////////////////////////

IContactListGroupItem* IContactList::InsertGroup(CONTACTLIST_GROUPITEM_INFO* pInfo)
{ return m_pContactListImpl->InsertGroup(pInfo); }
IContactListGroupItem*  IContactList::FindGroup(long lId)
{ return m_pContactListImpl->FindGroup(lId); }

IContactListContactItem*  IContactList::InsertContact(CONTACTLIST_CONTACTITEM_INFO* pInfo)
{ return m_pContactListImpl->InsertContact(pInfo); }
IContactListContactItem*  IContactList::FindContact(long lGroupId, long lId)
{ return m_pContactListImpl->FindContact(lGroupId, lId); }
IContactListContactItem*  IContactList::FindContact(IContactListGroupItem* pGroup, long lId)
{ return m_pContactListImpl->FindContact(pGroup, lId); }

void  IContactList::SetLayoutType(CONTACTLIST_LAYOUT_TYPE eType)
{ return m_pContactListImpl->SetLayoutType(eType); }
CONTACTLIST_LAYOUT_TYPE  IContactList::GetLayoutType()
{ return m_pContactListImpl->GetLayoutType(); }

//////////////////////////////////////////////////////////////////////////

void  IContactListGroupItem::Update(CONTACTLIST_GROUPITEM_INFO* pInfo) { return m_pContactListGroupItemImpl->Update(pInfo); }

//////////////////////////////////////////////////////////////////////////

void  IContactListContactItem::Update(CONTACTLIST_CONTACTITEM_INFO* pInfo) { return m_pContactListContactItemImpl->Update(pInfo); }