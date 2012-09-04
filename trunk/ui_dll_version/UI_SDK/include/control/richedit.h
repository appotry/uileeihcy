#pragma once
#include "WindowlessRichEdit.h"

class RichEditBase : public Control
{
public:
	RichEditBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)

		UIMESSAGE_HANDLER_EX(WM_SETCURSOR, OnForwardMessage)
		UIMESSAGE_HANDLER_EX(WM_CHAR, OnForwardMessage)
		UIMESSAGE_HANDLER_EX(WM_KEYDOWN, OnForwardMessage)
		UIMESSAGE_HANDLER_EX(WM_KEYUP, OnForwardMessage)
		UIMSG_WM_SETFOCUS(OnSetFocus)
		UIMSG_WM_KILLFOCUS(OnKillFocus)
		UIMESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST, OnMouseRangeMsg)

		UIMESSAGE_HANDLER_EX(UI_WM_OBJECTLOADED, OnObjectLoaded)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

public:
//	virtual  HRESULT InitialConstruct();

	virtual  SIZE   GetAutoSize( HRDC hRDC );
	virtual  bool   SetAttribute(ATTRMAP& mapAttrib, bool bReload);
	virtual  void   ResetAttribute();

protected:
	LRESULT  OnObjectLoaded(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     OnEraseBkgnd( HRDC hRDC );
	void     OnPaint( HRDC hRDC );

	LRESULT  OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     OnSetFocus( Object* pOldFocusObj );
	void     OnKillFocus( Object* pNewFocusObj );
	LRESULT  OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	WindowlessRichEdit   m_wrapRichEidt;
};


class RichEdit : public RichEditBase
{
public:
	UI_DECLARE_OBJECT(RichEdit,OBJ_CONTROL);
};