#pragma once
#include "App\IM\IM_Ctrls\inc\icontactlist.h"

namespace UI
{
    interface IPanel;
    interface ILabel;
    interface IPictureCtrl;
}

class ContactListContactItemShareData : public UI::MessageProxy
{
public:
    ContactListContactItemShareData();
    ~ContactListContactItemShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ContactListContactItemShareData, UI::IListItemTypeShareData)

    void  SetIContactListContactItemShareData(IContactListContactItemShareData* p)
    { m_pIContactListContactItemShareData = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
    IContactListContactItemShareData*  m_pIContactListContactItemShareData;
    UI::ITextRenderBase*  m_pTextRender1;   // 黑色文本
    UI::ITextRenderBase*  m_pTextRender2;   // 灰色文本
};

class ContactListContactItem : public UI::MessageProxy
{
public:
    ContactListContactItem();
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        UIMSG_WM_DROPTARGETEVENT(OnDropTargetEvent)
        UIMESSAGE_HANDLER_EX(UI_CONTACTLIST_MSG_LAYOUT_CHANGED, OnContactListLayoutChanged)
        UIMESSAGE_HANDLER_EX(UI_LISTITEM_MSG_DELAY_OP, OnDelayOp)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ContactListContactItem, UI::IListItemBase)

public:
    void  SetIContactListContactItem(IContactListContactItem* p) { m_pIContactListContactItem = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    void  Update(CONTACTLIST_CONTACTITEM_INFO* pInfo);

protected:
    HRESULT  FinalConstruct(UI::IUIApplication* p);
    void  OnInitialize();
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonDblClk(UINT nFlags, POINT point);
    void  GetDesiredSize(SIZE* pSize);
    void  OnDropTargetEvent(UI::DROPTARGETEVENT_TYPE eType, UI::DROPTARGETEVENT_DATA* pData);
    LRESULT  OnContactListLayoutChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnDelayOp(UINT, WPARAM, LPARAM);
private:
    void  InitInnerCtrl();
    void  UpdadteInnerCtrlLayout();
    void  UpdadteBigFaceLayout();
    void  UpdadteSmallFaceLayout();

    void  DoDrag(POINT pt);
    HBITMAP  CreateDragBitmap(int* pWidth, int* pHeight);

private:
    IContactListContactItem*  m_pIContactListContactItem;
    UI::ITreeView*  m_pTreeView;

    ContactListContactItemShareData*  m_pShareData;
    long   m_lSort;
    POINT  m_ptLastLButtonDown;   // 用于开启拖拽

    // 容器
    UI::IPanel*   m_pPanelLeft;   // 包含了头像
    UI::IPanel*   m_pPanelRight;  // 包含了High/Low Panel
    UI::IPanel*   m_pPanelHigh;
    UI::IPanel*   m_pPanelLow;
    UI::IPanel*   m_pPanelPlugin; 

    // 元素
    UI::IPictureCtrl*  m_pImage_Portrait; // 头像
    UI::ILabel*   m_pString_NickName;     // 备注姓名
    UI::ILabel*   m_pString_Remark;       // 备注，例如真实昵称、使用其它设备登录等
    UI::ILabel*   m_pString_Moodphrase;   // 心情短语

    // 其它元素懒得加了
};