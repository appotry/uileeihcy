#pragma once
#include "WindowlessRichEdit.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

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
interface IScrollBarManager;

class  RichEdit : public MessageProxy
{
public:
	RichEdit();
    ~RichEdit();
    UI_DECLARE_OBJECT3(RichEdit, OBJ_CONTROL|CONTROL_RICHEDIT, _T("UICtrl/Control"));

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT( OnPaint )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMESSAGE_HANDLER_EX( UI_WM_REDRAWOBJECT, OnRedrawObject )

		UIMESSAGE_HANDLER_EX( WM_SETCURSOR, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_CHAR, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_KEYDOWN, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_KEYUP, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_WINDOWPOSCHANGED, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_TIMER, OnForwardMessage )
		UIMSG_WM_VISIBLE_CHANGED( OnVisibleChanged )
		UIMSG_WM_SHOWWINDOW( OnShowWindow )
		UIMSG_WM_SETFOCUS( OnSetFocus )
		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UIMSG_WM_SIZE( OnSize ) 

		// 在Scrollmgr处理之前先拦截UI_WM_GETSCROLLOFFSET
		// 因为richedit内部有自己的偏移信息，而不能使用UI控件的
		// 偏移信息，否则就会导致光标，内容显示错误。

		UIMESSAGE_HANDLER_EX( WM_VSCROLL, OnScroll )
		UIMESSAGE_HANDLER_EX( WM_HSCROLL, OnScroll )
		UIMESSAGE_HANDLER_EX( UI_WM_GETSCROLLOFFSET,OnGetScrollOffset )
        UIMESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnForwardMessage )

        UIMESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_ENDCOMPOSITION,   OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_COMPOSITION,      OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_SETCONTEXT,       OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_NOTIFY,           OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_CONTROL,          OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_COMPOSITIONFULL,  OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_SELECT,           OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_CHAR,             OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_REQUEST,          OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_KEYDOWN,          OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_KEYUP,            OnForwardMessage )


	//	UICHAIN_MSG_MAP_POINT_MEMBER(m_pMgrScrollbar)  
		UIMSG_WM_RBUTTONDOWN( OnRButtonDown )
		UIMESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST,WM_MOUSELAST, OnForwardMessage )

        UIMESSAGE_HANDLER_EX( UI_WM_WINDOWLAYEREDCHANGED, OnWindowLayeredChanged )
		UIMSG_WM_OBJECTLOADED( OnObjectLoaded )
        UIMSG_WM_QUERYINTERFACE( QueryInterface )
        UIMSG_WM_GETOBJECTINFO( OnGetObjectInfo ) 
        UIMSG_WM_RESETATTRIBUTE( ResetAttribute )
        UIMSG_WM_SETATTRIBUTE( SetAttribute )
        UIMSG_WM_FINALCONSTRUCT( FinalConstruct )
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx( RichEdit, IControl )

    void  SetIRichEdit(IRichEdit* p) { m_pIRichEdit = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IRichEdit*  GetIRichEdit() { return m_pIRichEdit; }

public:
    // 接口转发
    bool  IsNeedFixGdiAlpha() { return m_bNeedFixGdiAlpha; } // WindowlessRE中CreateCaret需要使用

	WindowlessRichEdit& GetRichEdit() { return m_wrapRichEidt; }
	IScrollBarManager*  GetScrollMgr() { return m_pMgrScrollBar; }
    CCaret*  GetCaret() { return &m_caret; }

    HRESULT  OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled);

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void     SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void     ResetAttribute();

	void     OnObjectLoaded();
	void     OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void     OnPaint(IRenderTarget* pRenderTarget);
	LRESULT  OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT  OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT  OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void     OnSetFocus(IObject* pOldFocusObj);
	void     OnKillFocus(IObject* pNewFocusObj);
//	LRESULT  OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     OnSize(UINT nType, int cx, int cy);
	void     OnRButtonDown(UINT nFlags, CPoint point);
	
	void     OnShowWindow(BOOL bShow, UINT nStatus);
	void     OnVisibleChanged(BOOL bVisible, IObject* pParent);
    LRESULT  OnWindowLayeredChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    IRichEdit*  m_pIRichEdit;

	WindowlessRichEdit   m_wrapRichEidt;
	IScrollBarManager*   m_pMgrScrollBar;
    CCaret  m_caret;      // 光标

	CRect  m_rcInvalidate;

    bool   m_bNeedFixGdiAlpha;   // 在分层窗口上面需要带alpha通道。窗口风格改变时，会收到UI_WM_WINDOWLAYEREDCHANGED通知
// 	friend class ITextHostImpl;
// 	friend class WindowlessRichEdit;
};

}