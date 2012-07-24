#pragma once

namespace UI
{

class UIAPI ComboboxBase : public Control
{
public:
	ComboboxBase();
	~ComboboxBase();

	UI_BEGIN_MSG_MAP
		
		UIMSG_WM_SIZE(OnSize)
		UICHAIN_MSG_MAP(Control)
	UI_END_MSG_MAP

	virtual bool SetAttribute( ATTRMAP& mapAttrib, bool bReload );
	virtual void ResetAttribute();
	virtual SIZE GetAutoSize( HRDC hRDC );

protected:
	void OnEraseBkgnd(HRDC hRDC);
	void OnSize( UINT nType, int cx, int cy );

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