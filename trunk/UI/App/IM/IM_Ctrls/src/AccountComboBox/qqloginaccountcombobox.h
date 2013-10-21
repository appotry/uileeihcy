#pragma once
#include "App\IM\IM_Ctrls\inc\iqqloginaccountcombobox.h"

class QQLoginAccountComboBox : public UI::MessageProxy
{
public:
	QQLoginAccountComboBox();
    ~QQLoginAccountComboBox();
    UI_DECLARE_OBJECT3(QQLoginAccountComboBox, UI::OBJ_CONTROL|UI::CONTROL_COMBOBOX, _T("IMCtrl"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_NOTIFY(UI_CBN_DROPDOWN, OnDropDown)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UIALT_MSG_MAP(COMBOBOX_LISTBOX_MSG_NOTIFY_MSG_ID)  // ListBox Notify
        UIMSG_LCN_SELCHANGED(OnLCNSelChanged)
        UIMSG_WM_NOTIFY(UI_LCN_CLICK, OnLCNClick)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(QQLoginAccountComboBox, UI::IComboBoxBase)

    void  SetIQQLoginAccountComboBox(IQQLoginAccountComboBox* p)
    { m_pIQQLoginAccountComboBox = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }

public:
	void  AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath);
    bool  SetCurSel(int nIndex);

protected:
    HRESULT  FinalConstruct(UI::IUIApplication* p);
    void  SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();
    HRESULT  OnDropDown(WPARAM, LPARAM);
    void  OnLCNSelChanged(UI::IMessage* pObjMsgFrom, UI::IListItemBase* pOldSelItem, UI::IListItemBase* pSelItem);
    LRESULT  OnLCNClick(WPARAM, LPARAM);
protected:
    IQQLoginAccountComboBox*  m_pIQQLoginAccountComboBox;
	IQQLoginAccountListBox*   m_pDropDownCtrl;
};

