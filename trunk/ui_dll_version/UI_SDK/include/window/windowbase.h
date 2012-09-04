#pragma once

//
//   这个窗口用于派生这样的窗口，它不是UI窗口，但却想使用UI的控件
//	 提供窗口所需要的一些基本属性，即可做为仅提供UI控件使用，也可做为UI窗口的基类
//
//   libo 20110821 9:57 (hangzhou yingyao jiari jiudian 8332)

namespace UI
{
class UIAPI WindowBase : public Panel
{
public:
	UI_DECLARE_OBJECT( WindowBase, OBJ_WINDOW )
	WindowBase();
	~WindowBase();
	virtual  void ObjectMustCreateByUIObjCreator(){};   // TODO: 如果需要使用FinalConstruct的窗口可自己去由UIObjCreator实现

public:
	bool              Create( const String& ID,  HWND hWndParent = NULL );
	long              DoModal( const String& ID, HWND hWndParent );
	long              DoModal( HINSTANCE hResInst, UINT nResID, const String& ID, HWND hWndParent );
	HWND              DoModeless( const String& ID, HWND hWndParent );
	HWND              DoModeless( HINSTANCE hResInst, UINT nResID, const String& ID, HWND hWndParent );
	void              EndDialog(INT_PTR);

	static LRESULT    CALLBACK StartWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT    CALLBACK StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT    CALLBACK ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT           StartProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bWindowOrDialog );
	LRESULT		      WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT           DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	bool              CreateUI( const String& ID, HWND hWnd );
	virtual void      DestroyUI();
	long              ModalLoop(HWND hWndParent);
	BOOL              PreTranslateMessage(MSG* pMsg);

public:
	virtual           void InvalidateObject( Object* pObj, RECT* prc, bool bUpdateNow );
	virtual           void InvalidateObjectBkgnd( Object* pObj, RECT* prc, bool bUpdateNow );
	
	virtual           HRDC BeginDrawObject( Object* pInvalidateObj, HRGN& hClipRgn);
	virtual           void EndDrawObject( CRect* prcWindow, HRGN& hClipRgn );
protected:
	void              _InvalidateObject(Object* pInvalidateObj, HDC hDestDC);
	void              _InvalidateObjectBkgnd(Object* pInvalidateObj, HDC hDestDC);

public:
	// 一些公共方法
	void			  CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow );
	void              CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient );
	BOOL              IsChildWindow();
	void              ShowWindow();
	void              HideWindow();

	operator          HWND() const;
	MouseManager&     GetMouseMgr();
	KeyboardManager&  GetKeyboardMgr();
	Object*           GetHoverObject();
	Object*           GetPressObject();
	HRFONT            GetHRFONT();

	void              Control_NotifyMe( const String&  idPath, int nNotifyMapID );
	void              Control_NotifyMe( Object*  pObj, int nNotifyMapID );

protected:
	// object 虚函数
	virtual           void ResetAttribute();
	virtual           bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	// 自己的虚函数
	virtual           BOOL PreCreateWindow( CREATESTRUCT& cs );

	// 在加载完自己的控件和布局之后被调用，子类调用时先调用父类的该方法
	virtual           void OnInitWindow();
	virtual           void OnFinalMessage();

	void              ReCreateRenderTarget();

public:
	// WndProc的原始消息处理
	BEGIN_MSG_MAP(WindowBase)
		MESSAGE_HANDLER( WM_SETCURSOR,     _OnSetCursor )
		MESSAGE_HANDLER( WM_MOUSEMOVE,     _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_ERASEBKGND,    _OnEraseBkgnd )
		MESSAGE_HANDLER( WM_PAINT,         _OnPaint )
		MESSAGE_HANDLER( WM_MOUSELEAVE,    _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_LBUTTONDOWN,   _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_LBUTTONUP,     _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_RBUTTONDOWN,   _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_RBUTTONUP,     _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_LBUTTONDBLCLK, _OnHandleMouseMessage )
		MESSAGE_HANDLER( WM_SIZE,          _OnSize )
		MESSAGE_HANDLER( WM_CHAR,          _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_KEYDOWN,       _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_KEYUP,         _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_SYSKEYDOWN,    _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_SYSKEYUP,      _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_MOUSEWHEEL,    _OnHandleKeyboardMessage )
		MESSAGE_HANDLER( WM_SETFOCUS,      _OnSetFocus )
		MESSAGE_HANDLER( WM_KILLFOCUS,     _OnKillFocus )
#if(_WIN32_WINNT >= 0x0501)
		MESSAGE_HANDLER( WM_THEMECHANGED,  _OnThemeChange )
#endif
		MESSAGE_HANDLER( WM_CREATE,        _OnCreate )
		MESSAGE_HANDLER( WM_INITDIALOG,    _OnCreate )
		MESSAGE_HANDLER( WM_NCDESTROY,     _OnNcDestroy )
	END_MSG_MAP()

	// UI事件的消息处理
	UI_BEGIN_MSG_MAP
		UIMSG_WM_GETRENDERTYPE( OnGetRenderType )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

protected:
	LRESULT           _OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnHandleMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnHandleKeyboardMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT           _OnThemeChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	
	BOOL              OnEraseBkgnd(HRDC);
	LRESULT           OnGetRenderType();

public:
	HWND              m_hWnd;                // 窗口句柄
	HRDC              m_hRenderTarget;       // 双缓冲DC
	HRFONT            m_hFont;            

protected:
	CWndProcThunk     m_thunk;               // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC           m_oldWndProc;          // 该窗口的原始窗口过程

	MouseManager      m_MgrMouse;            // 鼠标消息处理器
	KeyboardManager   m_MgrKeyboard;         // 键盘消息处理器

	bool              m_bDoModal;
	bool              m_bEndModal;
	INT_PTR           m_lDoModalReturn;
};


}

