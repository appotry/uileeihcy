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
namespace UI
{
class UIAPI RichEditBase : public Control
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
		UIMESSAGE_HANDLER_EX(WM_TIMER, OnForwardMessage)
		UIMSG_WM_SETFOCUS(OnSetFocus)
		UIMSG_WM_KILLFOCUS(OnKillFocus)
		UIMSG_WM_SIZE(OnSize) 

		// 在Scrollmgr处理之前先拦截UI_WM_GETSCROLLOFFSET
		// 因为richedit内部有自己的偏移信息，而不能使用UI控件的
		// 偏移信息，否则就会导致光标，内容显示错误。

		UIMESSAGE_HANDLER_EX(WM_VSCROLL, OnScroll)
		UIMESSAGE_HANDLER_EX(WM_HSCROLL, OnScroll)
		UIMESSAGE_HANDLER_EX(UI_WM_GETSCROLLOFFSET,OnGetScrollOffset)

		UICHAIN_MSG_MAP_MEMBER(m_MgrScrollbar) 
		UIMESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST, OnForwardMessage)

		UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

public:
	virtual  SIZE   GetAutoSize( HRDC hRDC );
	virtual  bool   SetAttribute(ATTRMAP& mapAttrib, bool bReload);
	virtual  void   ResetAttribute();

	WindowlessRichEdit& GetRichEdit() { return m_wrapRichEidt; }
	ScrollBarMgr&   GetScrollMgr() { return m_MgrScrollbar; }

protected:
	void     OnObjectLoaded();
	void     OnEraseBkgnd( HRDC hRDC );
	void     OnPaint( HRDC hRDC );

	LRESULT  OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT  OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void     OnSetFocus( Object* pOldFocusObj );
	void     OnKillFocus( Object* pNewFocusObj );
//	LRESULT  OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     OnSize( UINT nType, int cx, int cy );

protected:
	WindowlessRichEdit   m_wrapRichEidt;
	ScrollBarMgr  m_MgrScrollbar;

// 	friend class ITextHostImpl;
// 	friend class WindowlessRichEdit;
};


class UIAPI RichEdit : public RichEditBase
{
public:
	UI_DECLARE_OBJECT(RichEdit,OBJ_CONTROL);
};

}