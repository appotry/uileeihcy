#ifndef ICONTACTLIST_H_06DA650B_C3A2_4777_BF7A_A1D6CDD424B7
#define ICONTACTLIST_H_06DA650B_C3A2_4777_BF7A_A1D6CDD424B7
#include "UISDK\Control\Inc\Interface\itreeview.h"

// {32387088-7C27-4af7-85A5-1AE333AA6DC4}
static const GUID IID_UI_IContactList = 
{ 0x32387088, 0x7c27, 0x4af7, { 0x85, 0xa5, 0x1a, 0xe3, 0x33, 0xaa, 0x6d, 0xc4 } };

// {914A1560-868F-485e-8431-74D73D3F9BB0}
static const GUID CLSID_UI_ContactList = 
{ 0x914a1560, 0x868f, 0x485e, { 0x84, 0x31, 0x74, 0xd7, 0x3d, 0x3f, 0x9b, 0xb0 } };


// item通知消息定义

//
// 双击联系人
// message: UI_WM_NOTIFY
// code: UI_CONTACTLIST_NOTIFY_CONTACTITEM_DBCLICK
// wParam: IContactListContactItem
// lParam: NA.
// ret: NA.
//
#define UI_CONTACTLIST_NOTIFY_CONTACTITEM_DBCLICK  136011005  

// item类型定义
#define CONTACTLIST_ITEM_TYPE_GROUP     135302342
#define CONTACTLIST_ITEM_TYPE_CONTACT   135311433


//////////////////////////////////////////////////////////////////////////
// 联系人分组

#define CLGI_MASK_SORT    0x00000001
#define CLGI_MASK_TEXT    0x00000002

// 外部访问数据。内部并不保存该结构体
struct  CONTACTLIST_GROUPITEM_INFO
{
    UINT  nMask;
    UINT  nMask2;  // 保留。以防以后字段超出32种

    long  lId;
    long  lSort;

    TCHAR*  pszText;
};


class ContactListGroupItemShareData;
interface IContactListGroupItemShareData : public UI::IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IContactListGroupItemShareData, ContactListGroupItemShareData);
};

class ContactListGroupItem;
interface IM_CTRLS_API IContactListGroupItem : public UI::IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IContactListGroupItem, ContactListGroupItem);

    void  Update(CONTACTLIST_GROUPITEM_INFO* pInfo);
};

//////////////////////////////////////////////////////////////////////////
// 联系人


#define CLCI_MASK_SORT        0x00000001
#define CLCI_MASK_NICKNAME    0x00000002
#define CLCI_MASK_MOODPHRASE  0x00000004
#define CLCI_MASK_PORTRAIT    0x00000008

// 外部访问数据。内部并不保存该结构体
struct  CONTACTLIST_CONTACTITEM_INFO
{
    UINT  nMask;
    UINT  nMask2;  // 保留。以防以后字段超出32种

    long  lId;
    long  lGroupId;
    long  lSort;

    TCHAR*  pszNickName;  
    TCHAR*  pszMoodphrase;
    TCHAR*  pszPortraitPath;
};

class ContactListContactItemShareData;
interface IContactListContactItemShareData : public UI::IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IContactListContactItemShareData, ContactListContactItemShareData);
};

class ContactListContactItem;
interface IM_CTRLS_API IContactListContactItem : public UI::IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IContactListContactItem, ContactListContactItem);

    void  Update(CONTACTLIST_CONTACTITEM_INFO* pInfo);
};

//////////////////////////////////////////////////////////////////////////
// 联系人列表

class ContactList;
interface IM_CTRLS_API IContactList : public UI::ITreeView
{
    UI_DECLARE_Ixxx_INTERFACE(IContactList, ContactList);

    IContactListGroupItem*  InsertGroup(CONTACTLIST_GROUPITEM_INFO* pInfo);
    IContactListGroupItem*  FindGroup(long lId);

    IContactListContactItem*  InsertContact(CONTACTLIST_CONTACTITEM_INFO* pInfo);
    IContactListContactItem*  FindContact(long lGroupId, long lId);
    IContactListContactItem*  FindContact(IContactListGroupItem* pGroup, long lId);
};



#endif