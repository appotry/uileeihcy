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
	//	UIMSG_CB_SHOWDROPDOWN(OnCBShowDropDown)
	
	UIALT_MSG_MAP(1)
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)

	UI_BEGIN_CHAIN_ALL_MSG_MAP
		UICHAIN_MSG_MAP(Control)
	UI_END_CHAIN_ALL_MSG_MAP
	UI_END_MSG_MAP

	virtual bool SetAttribute( ATTRMAP& mapAttrib, bool bReload );
	virtual void ResetAttribute();
	virtual SIZE GetAutoSize( HRDC hRDC );

protected:
	void OnStateChanged(int nOld, int nNew);
	void OnEraseBkgnd(HRDC hRDC);
	void OnSize( UINT nType, int cx, int cy );
	void OnBtnLButtonDown(UINT nFlags, POINT point);

	void OnCBShowDropDown(BOOL bShow);
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