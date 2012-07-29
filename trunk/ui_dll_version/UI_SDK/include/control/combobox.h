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
		UIMSG_BN_CLICKED2(&m_button, OnBtnClicked)
		UICHAIN_MSG_MAP(Control)
	UI_END_MSG_MAP

	virtual bool SetAttribute( ATTRMAP& mapAttrib, bool bReload );
	virtual void ResetAttribute();
	virtual SIZE GetAutoSize( HRDC hRDC );

protected:
	void OnStateChanged(int nOld, int nNew);
	void OnEraseBkgnd(HRDC hRDC);
	void OnSize( UINT nType, int cx, int cy );
	void OnBtnClicked( );

protected:
	Edit      m_edit;
	Button    m_button;
	ListBox   m_listbox;
};

class UIAPI Combobox : public ComboboxBase
{
public:
	UI_DECLARE_OBJECT(Combobox, OBJ_CONTROL);
};

}