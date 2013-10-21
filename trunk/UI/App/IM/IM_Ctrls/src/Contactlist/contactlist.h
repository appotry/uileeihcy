#pragma once
#include "App\IM\IM_Ctrls\inc\icontactlist.h"


class ContactList : public UI::MessageProxy
{
public:
    ContactList();
    
    UI_DECLARE_OBJECT3(ContactList, UI::OBJ_CONTROL|UI::CONTROL_LISTBOX, _T("IMCtrl"));

    UI_BEGIN_MSG_MAP
        UIMESSAGE_HANDLER_EX(UI_CONTACTLIST_MSG_GET_LAYOUT_TYPE, OnGetLayoutType)
        UIMSG_WM_DROPTARGETEVENT(OnDropTargetEvent)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ContactList, UI::ITreeView)

public:
    void  SetIContactList(IContactList* p) { m_pIContactList = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    IContactList*  GetIContactList() { return m_pIContactList; }

    IContactListGroupItem*  InsertGroup(CONTACTLIST_GROUPITEM_INFO* pInfo);
    IContactListGroupItem*  FindGroup(long lId);

    IContactListContactItem*  InsertContact(CONTACTLIST_CONTACTITEM_INFO* pInfo);
    IContactListContactItem*  FindContact(long lGroupId, long lId);
    IContactListContactItem*  FindContact(IContactListGroupItem* pGroup, long lId);
    LRESULT  OnGetLayoutType(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void  SetLayoutType(CONTACTLIST_LAYOUT_TYPE eType);
	CONTACTLIST_LAYOUT_TYPE  GetLayoutType();
protected:
    HRESULT  FinalConstruct(UI::IUIApplication* p);
    void  OnDropTargetEvent(UI::DROPTARGETEVENT_TYPE eType, UI::DROPTARGETEVENT_DATA* pData);

protected:
    IContactList*  m_pIContactList;
    CONTACTLIST_LAYOUT_TYPE  m_eLayoutType;
};

