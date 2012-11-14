#pragma once

namespace UI
{
#define COMBOBOX_BUTTON_ID _T("combobox_button")
#define COMBOBOX_EDIT_ID   _T("combobox_edit")
#define COMBOBOX_LIST_ID   _T("combobox_list")


class UIAPI ComboboxBase : public Control
{
public:
	ComboboxBase();
	~ComboboxBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED(OnStateChanged)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
		UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
		UIMSG_WM_STYLECHANGED(OnStyleChanged)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)

	UIALT_MSG_MAP(1)  // Button hook
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)

	UIALT_MSG_MAP(2)  // ListBox Notify
		UIMSG_LCN_SELCHANGED(OnLCNSelChanged)

	UI_END_MSG_MAP_CHAIN_PARENT(Control)

	virtual bool SetAttribute( ATTRMAP& mapAttrib, bool bReload );
	virtual void ResetAttribute();
	virtual SIZE GetAutoSize( HRDC hRDC );

public:
	bool     AddString(const String& strText, bool bUpdate=true);
	ListBoxItem* AddStringEx(const String& strText, bool bUpdate=true);
	bool     SetCurSel(int nIndex);
	ListBox* GetListBox() { return m_listbox; }
	Edit*    GetEdit()    { return m_edit; }
	void     SetComboboxStyleType(int n);

protected:
	void     OnStateChanged(int nOld, int nNew);
	void     OnEraseBkgnd(HRDC hRDC);
	void     OnSize( UINT nType, int cx, int cy );
	void     OnLButtonDown(UINT nFlags, POINT point);
	void     OnBtnLButtonDown(UINT nFlags, POINT point);
	void     OnLCNSelChanged(Message* pObjMsgFrom, ListItemBase* pOldSelItem, ListItemBase* pSelItem);
	void     OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	
	void     OnInitPopupControlWindow(Object* pObjMsgFrom);
	void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

protected:
	Edit*      m_edit;
	Button*    m_button;
	ListBox*   m_listbox;
};

class UIAPI Combobox : public ComboboxBase
{
public:
	UI_DECLARE_OBJECT(Combobox, OBJ_CONTROL);
};

}