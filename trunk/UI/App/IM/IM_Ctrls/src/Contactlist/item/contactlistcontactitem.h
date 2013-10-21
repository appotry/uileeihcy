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
    UI::ITextRenderBase*  m_pTextRender1;   // ��ɫ�ı�
    UI::ITextRenderBase*  m_pTextRender2;   // ��ɫ�ı�
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
    POINT  m_ptLastLButtonDown;   // ���ڿ�����ק

    // ����
    UI::IPanel*   m_pPanelLeft;   // ������ͷ��
    UI::IPanel*   m_pPanelRight;  // ������High/Low Panel
    UI::IPanel*   m_pPanelHigh;
    UI::IPanel*   m_pPanelLow;
    UI::IPanel*   m_pPanelPlugin; 

    // Ԫ��
    UI::IPictureCtrl*  m_pImage_Portrait; // ͷ��
    UI::ILabel*   m_pString_NickName;     // ��ע����
    UI::ILabel*   m_pString_Remark;       // ��ע��������ʵ�ǳơ�ʹ�������豸��¼��
    UI::ILabel*   m_pString_Moodphrase;   // �������

    // ����Ԫ�����ü���
};