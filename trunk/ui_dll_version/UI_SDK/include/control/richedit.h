#pragma once
#include "WindowlessRichEdit.h"

//
//	RichEdit编辑框控件
//
//  该类仅用于包装WindowlessRichEdit，为WindowlessRichEdit提供一个容器和消息转发器
//  为WindowlessRichEdit提供坐标范围、外部消息等。
//  RichEdit对应的方法和接口全部由WindowlessRichEdit实现，外部进行操作时，可直接获
//  取WindowlessRichEdit的指针进行调用。
//

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
		UIMESSAGE_HANDLER_EX(WM_WINDOWPOSCHANGED, OnForwardMessage)
		UIMSG_WM_SETFOCUS(OnSetFocus)
		UIMSG_WM_KILLFOCUS(OnKillFocus)
		UIMESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST, OnMouseRangeMsg)

		UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

public:
	virtual  SIZE   GetAutoSize( HRDC hRDC );
	virtual  bool   SetAttribute(ATTRMAP& mapAttrib, bool bReload);
	virtual  void   ResetAttribute();

	WindowlessRichEdit*  GetRichEdit() { return &m_wrapRichEidt; }

protected:
	void     OnObjectLoaded();
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