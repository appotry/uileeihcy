#include "stdafx.h"
#include "contactlist.h"
#include "layout\contactlistitemlayout.h"

ContactList::ContactList()
{
    m_pIContactList = NULL;
}

HRESULT  ContactList::FinalConstruct(UI::IUIApplication* p)
{
    DO_PARENT_PROCESS(IContactList, UI::ITreeView);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIContactList->SetLayout(new ContactListItemLayout());
    return S_OK;
}

IContactListGroupItem*  ContactList::InsertGroup(CONTACTLIST_GROUPITEM_INFO* pInfo)
{
    if (NULL == pInfo)
        return NULL;

    if (FindGroup(pInfo->lId))
        return NULL;

    IContactListGroupItem*  pItem = NULL;
    IContactListGroupItem::CreateInstance(m_pIContactList->GetUIApplication(), &pItem);

    m_pIContactList->AddItem(pItem, LISTITEM_OPFLAG_NOALL);
    pItem->SetId(pInfo->lId);

    pItem->Update(pInfo);
    return pItem;
}

IContactListGroupItem*  ContactList::FindGroup(long lId)
{
    UI::IListItemBase*  pItem = m_pIContactList->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemType() == CONTACTLIST_ITEM_TYPE_GROUP)
        {
            if (pItem->GetId() == lId)
                return static_cast<IContactListGroupItem*>(pItem);
        }

        pItem = pItem->GetNextItem();
    }
    
    return NULL;
}

IContactListContactItem*  ContactList::InsertContact(CONTACTLIST_CONTACTITEM_INFO* pInfo)
{
    if (NULL == pInfo)
        return NULL;

    IContactListGroupItem* pGroup = FindGroup(pInfo->lGroupId);
    if (!pGroup)
        return NULL;

    if (FindContact(pGroup, pInfo->lId))
        return NULL;

    IContactListContactItem*  pItem = NULL;
    IContactListContactItem::CreateInstance(m_pIContactList->GetUIApplication(), &pItem);

    m_pIContactList->InsertItem(static_cast<UI::IListItemBase*>(pItem), static_cast<UI::IListItemBase*>(pGroup), UITVI_LAST, LISTITEM_OPFLAG_NOALL);
    pItem->SetId(pInfo->lId);

    pItem->Update(pInfo);
    return pItem;
}
IContactListContactItem*  ContactList::FindContact(long lGroupId, long lId)
{
    IContactListGroupItem*  pGroup = FindGroup(lGroupId);
    if (!pGroup)
        return NULL;

    return FindContact(pGroup, lId);
    
}
IContactListContactItem*  ContactList::FindContact(IContactListGroupItem* pGroup, long lId)
{
    if (!pGroup)
        return NULL;

    UI::IListItemBase*  pItem = pGroup->GetChildItem();
    while (pItem)
    {
        if (pItem->GetItemType() == CONTACTLIST_ITEM_TYPE_CONTACT)
        {
            if (pItem->GetId() == lId)
                return static_cast<IContactListContactItem*>(pItem);
        }

        pItem = pItem->GetNextItem();
    }

    return NULL;
}
