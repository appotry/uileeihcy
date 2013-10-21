#include "stdafx.h"
#include "contactlist.h"
#include "layout\bigfacelayout.h"
#include "layout\smallfacelayout.h"

ContactList::ContactList()
{
    m_pIContactList = NULL;
    m_eLayoutType = CONTACTLIST_LAYOUT_NONE;
}

HRESULT  ContactList::FinalConstruct(UI::IUIApplication* p)
{
    DO_PARENT_PROCESS(IContactList, UI::ITreeView);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIContactList->ModifyStyle(OBJECT_STYLE_RECEIVE_DRAGDROPEVENT, 0, false);
    SetLayoutType(CONTACTLIST_LAYOUT_BIGFACE);
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

void ContactList::OnDropTargetEvent( UI::DROPTARGETEVENT_TYPE eType, UI::DROPTARGETEVENT_DATA* pData )
{
    if (!pData)
        return;

    switch (eType)
    {
    case UI::_DragEnter:
    case UI::_DragOver:
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(m_pIContactList->GetHWND(), &pt);

            POINT ptClient;
            m_pIContactList->WindowPoint2ObjectClientPoint(&pt, &ptClient);

            CRect rcClient;
            m_pIContactList->GetClientRectAsWin32(&rcClient);
            rcClient.DeflateRect(10,10,10,10);

            bool bNeedScroll = true;
            if (ptClient.y < rcClient.top)
            {
                m_pIContactList->ScrollY(-10, true);
            }
            else if (ptClient.y > rcClient.bottom)
            {
                m_pIContactList->ScrollY(10, true);
            }
            else
            {
                bNeedScroll = false;
            }
            UISendMessage(m_pIContactList, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

            if (m_pIContactList->GetHoverItem())
            {
                UI::UIMSG  msg = *GetCurMsg();
                msg.pMsgTo = m_pIContactList->GetHoverItem();
                UISendMessage(&msg);
            }
        }
        break;

    case UI::_DragLeave:
        {
            UISendMessage(m_pIContactList, WM_MOUSELEAVE);
        }   
        break;

    case UI::_Drop:
        {
            if (m_pIContactList->GetHoverItem())
            {
                UI::UIMSG  msg = *GetCurMsg();
                msg.pMsgTo = m_pIContactList->GetHoverItem();
                UISendMessage(&msg);
            }
        }
        break;
    }
}

LRESULT  ContactList::OnGetLayoutType(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_eLayoutType;
}
CONTACTLIST_LAYOUT_TYPE  ContactList::GetLayoutType()
{
	return m_eLayoutType;
}
void  ContactList::SetLayoutType(CONTACTLIST_LAYOUT_TYPE eType)
{
    if (eType == m_eLayoutType)
        return;

    if (eType == CONTACTLIST_LAYOUT_BIGFACE)
    {
        m_pIContactList->SetLayout(new BigfaceLayout());
    }
    else if (eType == CONTACTLIST_LAYOUT_SMALLFACE)
    {
        m_pIContactList->SetLayout(new SmallfaceLayout());
    }
    else 
    {
        return;
    }

    m_eLayoutType = eType;

    UI::IListItemBase*  pItem = m_pIContactList->GetFirstItem();
    while (pItem)
    {
        UI::UISendMessage(pItem, UI_CONTACTLIST_MSG_LAYOUT_CHANGED, (WPARAM)m_eLayoutType);
        pItem = pItem->GetNextTreeItem();
    }
}