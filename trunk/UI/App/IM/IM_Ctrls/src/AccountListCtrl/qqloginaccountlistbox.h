#pragma once
#include "App\IM\IM_Ctrls\inc\iqqloginaccountlistbox.h"

namespace UI
{
    interface  IStoryboard;
    interface  IButton;
}

class QQLoginAccountListBox : public UI::MessageProxy/*ListBox*/
{
public:
	QQLoginAccountListBox();
	~QQLoginAccountListBox();

	UI_DECLARE_OBJECT3(QQLoginAccountListBox, UI::OBJ_CONTROL|UI::CONTROL_LISTBOX, _T("IMCtrl"))

	UI_BEGIN_MSG_MAP
        UIMSG_WM_NOTIFY(UI_LCN_DRAWITEM, OnDrawItem)
        UIMSG_WM_NOTIFY(UI_LCN_HOVERITEMCHANGED, OnItemChanged)
        UIMSG_WM_NOTIFY(UI_LCN_SELCHANGED, OnItemChanged)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_NOTIFY(UI_LCN_POST_ITEMREMOVE, OnItemRemoved)
        UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupWindow)
        UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UIALT_MSG_MAP(1)
        UIMSG_BN_CLICKED2(m_pEraseBtn, OnEraseBtnClick)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(QQLoginAccountListBox, UI::IListBoxBase)

    void  SetIQQLoginAccountListBox(IQQLoginAccountListBox* p) {
        m_pIQQLoginAccountListBox = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
    void  SetBindObject(UI::IObject* pBindObj) { m_pBindObj = pBindObj; }

protected:
	void  ResetAttribute();
    void  SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload);
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    HRESULT  FinalConstruct(UI::IUIApplication* p);
    void  OnInitPopupWindow();
    void  OnUnInitPopupControlWindow();

    LRESULT  OnDrawItem(WPARAM, LPARAM);
    LRESULT  OnItemChanged(WPARAM, LPARAM);
    LRESULT  OnItemRemoved(WPARAM, LPARAM);
	void  HandleItemChanged(IQQLoginAccountListItem* pOld, IQQLoginAccountListItem* pNew, bool bAnimate=true);
    
    void  OnEraseBtnClick();
public:
	IQQLoginAccountListItem*  AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath);

protected:
    IQQLoginAccountListBox*  m_pIQQLoginAccountListBox;

    UI::IObject*  m_pBindObj;
	UI::ITextRenderBase*  m_pNameTextRender;
	UI::IButton*  m_pEraseBtn;
};
