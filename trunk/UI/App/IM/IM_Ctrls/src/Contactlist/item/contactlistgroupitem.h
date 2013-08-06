#pragma once
#include "App\IM\IM_Ctrls\inc\icontactlist.h"

class ContactListGroupItemShareData : public UI::MessageProxy
{
public:
    ContactListGroupItemShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ContactListGroupItemShareData, UI::IListItemTypeShareData)

    void  SetIContactListGroupItemShareData(IContactListGroupItemShareData* p) 
    { m_pIContactListGroupItemShareData = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
    IContactListGroupItemShareData*  m_pIContactListGroupItemShareData;
    int  m_nExpandIconItemWidth;
    int  m_nExpandIconItemHeight;
    int  m_nExpandIconItemCount;
};

class ContactListGroupItem : public UI::MessageProxy
{
public:
    ContactListGroupItem();
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        UIMESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnStateChanged)
        UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ContactListGroupItem, UI::IListItemBase)

public:
    void  SetIContactListGroupItem(IContactListGroupItem* p) { m_pIContactListGroupItem = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    void  Update(CONTACTLIST_GROUPITEM_INFO* pInfo);

protected:
    HRESULT  FinalConstruct(UI::IUIApplication* p);
    void  OnInitialize();
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonDblClk(UINT nFlags, POINT point);
    LRESULT  OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);

private:
    IContactListGroupItem*  m_pIContactListGroupItem;
    UI::ITreeView*  m_pTreeView;

    ContactListGroupItemShareData*  m_pShareData;
    long  m_lSort;
    int   m_nCurExpandIconIndex;  // ¶¯»­Ë÷Òý
};