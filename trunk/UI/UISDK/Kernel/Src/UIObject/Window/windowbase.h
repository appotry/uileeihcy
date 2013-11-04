#ifndef WINDOWBASE_H_A8C2427E_87C7_45e9_B6CE_AF3B4DBF49EF
#define WINDOWBASE_H_A8C2427E_87C7_45e9_B6CE_AF3B4DBF49EF

#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"
#include "UISDK\Kernel\Src\UIObject\Window\SyncWindowHelper.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Helper\commitbuffer\CommitWindowBufferListenerMgr.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\mousemanager.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\keyboardmanager.h"
#include "UISDK\Kernel\Src\RenderLayer\windowrenderlayet.h"
#include "UISDK\Kernel\Src\Helper\dragdrop\windowdragdropmgr.h"
#include "UISDK\Kernel\Inc\Interface\iwindowmousemgr.h"
//
//   这个窗口用于派生这样的窗口，它不是UI窗口，但却想使用UI的控件
//	 提供窗口所需要的一些基本属性，即可做为仅提供UI控件使用，也可做为UI窗口的基类
//
//   libo 20110821 9:57 (hangzhou xingyao jiari jiudian 8332)

namespace UI
{
class RenderChain;
// 
//	用于在创建窗口的时候，拦截第一个窗口消息，并进行子类化
//
class CREATE_WND_DATA  // 等同于 ATL 中的_ATL_WIN_MODULE70
{
public:
	CREATE_WND_DATA();
	~CREATE_WND_DATA();

public:
	void  AddCreateWndData(_AtlCreateWndData* pData, void* pThis);
	void* ExtractCreateWndData();

private:
	CRITICAL_SECTION   m_cs;
	_AtlCreateWndData* m_pCreateWndList;
};

// 2013.4.22 将windowbase基类修改为WindowRenderLayer，而不是Panel，从而优化整个分层渲染机制
// 默认一个窗口只有一个render layer，即窗口对象本身。当需要增加一个render layer时，将单独
// 创建一个render layer对象，该对象与窗口对象并没有什么关系，只是都被RenderChain管理起来了。
class WindowBase : public WindowRenderLayer, public SyncWindowHelper<WindowBase>
                 , public IMKMgr   // 由于目前没有将mouse/keyboard合并起来，暂时由window来实现这个接口
{
public:
    WindowBase();
    ~WindowBase();

	UI_DECLARE_OBJECT3(WindowBase, OBJ_WINDOW, _T("Kernel/Window"))

    // WndProc的原始消息处理   // 经过virtual扩展了
    VIRTUAL_BEGIN_MSG_MAP(WindowBase) 
        MESSAGE_HANDLER( WM_ERASEBKGND,           _OnEraseBkgnd )
        MESSAGE_HANDLER( WM_PAINT,                _OnPaint )
        MESSAGE_HANDLER( WM_SETCURSOR,            _OnSetCursor )
        
        MESSAGE_HANDLER( WM_MOUSEMOVE,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_MOUSELEAVE,           _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONDOWN,          _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONUP,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_RBUTTONDOWN,          _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_RBUTTONUP,            _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_LBUTTONDBLCLK,        _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_MOUSEWHEEL,           _OnHandleMouseMessage )
        MESSAGE_HANDLER( WM_CANCELMODE,           _OnHandleMouseMessage )

        MESSAGE_HANDLER( WM_CHAR,                 _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_KEYDOWN,              _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_KEYUP,                _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_SYSKEYDOWN,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_SYSKEYUP,             _OnHandleKeyBoardMessage )

        MESSAGE_HANDLER( WM_IME_STARTCOMPOSITION, _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_ENDCOMPOSITION,   _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_COMPOSITION,      _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_SETCONTEXT,       _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_NOTIFY,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_CONTROL,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_COMPOSITIONFULL,  _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_SELECT,           _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_CHAR,             _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_REQUEST,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_KEYDOWN,          _OnHandleKeyBoardMessage )
        MESSAGE_HANDLER( WM_IME_KEYUP,            _OnHandleKeyBoardMessage )
        
        MESSAGE_HANDLER( WM_GETMINMAXINFO,        _OnGetMinMaxInfo )
        MESSAGE_HANDLER( WM_WINDOWPOSCHANGING,    _OnWindowPosChanging )
        MESSAGE_HANDLER( UI_WM_SYNC_WINDOW,       _OnSyncWindow )
        MESSAGE_HANDLER( WM_ENTERSIZEMOVE,        _OnEnterSizeMove )
        MESSAGE_HANDLER( WM_EXITSIZEMOVE,         _OnExitSizeMove )
        MESSAGE_HANDLER( WM_SIZE,                 _OnSize )
        MESSAGE_HANDLER( WM_SETFOCUS,             _OnSetFocus )
        MESSAGE_HANDLER( WM_KILLFOCUS,            _OnKillFocus )
        MESSAGE_HANDLER( WM_MOUSEACTIVATE,        _OnMouseActive )
        MESSAGE_HANDLER( WM_CREATE,               _OnCreate )
        MESSAGE_HANDLER( WM_INITDIALOG,           _OnCreate )
        MESSAGE_HANDLER( WM_NCDESTROY,            _OnNcDestroy )
#if(_WIN32_WINNT >= 0x0501)
        MESSAGE_HANDLER( WM_THEMECHANGED,         _OnThemeChange )
#endif
    END_MSG_MAP()

    // UI事件的消息处理
    UI_BEGIN_MSG_MAP
        UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE( OnGetGraphicsRenderType )
        UIMESSAGE_HANDLER_EX( UI_WM_GET_MOUSEKEYBOARD_MANAGER, OnGetMouseKeyboardMgr )
        
        UIMSG_DM_GETDEFID( OnGetDefId )
        UIMSG_DM_SETDEFID( OnSetDefId )
        UIMSG_WM_GETDESIREDSIZE( OnGetDesiredSize )
        UIMSG_WM_QUERYINTERFACE( QueryInterface )
        UIMSG_WM_GETOBJECTINFO( OnGetObjectInfo )
        UIMSG_WM_SETATTRIBUTE( SetAttribute )
        UIMSG_WM_RESETATTRIBUTE( ResetAttribute )
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_FINALCONSTRUCT( FinalConstruct )
        UIMSG_WM_PRECREATEWINDOW( PreCreateWindow )
     UI_END_MSG_MAP_CHAIN_PARENT( WindowRenderLayer )

     IWindowBase* GetIWindowBase() { return m_pIWindowBase; }
     void SetIWindowBase(IWindowBase* p) { m_pIWindowBase = p; }

public:
	operator HWND() const;

    HWND  GetHWND() { return m_hWnd; }
    void  OnObjectDeleteInd(Object* p);
    void  OnObjectHideInd(Object* p);

    Object*  GetHoverObject();
    Object*  GetPressObject();

	bool  Create(IUIApplication* pUIApp, const TCHAR* szID,  HWND hWndParent = NULL);
	long  DoModal(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent );
	long  DoModal(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent);
	HWND  DoModeless(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndOnwer );
	HWND  DoModeless(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndOnwer);
	void  EndDialog(INT_PTR);
	void  Attach(IUIApplication* pUIApp, HWND hWnd, const TCHAR* szID);
	void  Detach();

	BOOL  IsChildWindow();
	void  ShowWindow();
	void  HideWindow();
    void  SetActive(bool b);
    bool  IsActive();
    bool  IsDoModal() { return m_bDoModal; }
    HRESULT  SetCanDrop(bool b) { return m_MgrDragDrop.SetCanDrop(b); }
    void  CenterWindow(HWND hWndCenter = NULL);
    void  UpdateDesktopLayout();

	void  CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow );
	void  CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient );
	void  Control_NotifyMe(const String&  idPath, int nNotifyMapID);
	void  Control_NotifyMe(Object*  pObj, int nNotifyMapID);

    WindowMouseMgr*  GetMouseMgr() { return &m_MgrMouse; }
//    WindowKeyboardMgr*  GetKeyboardMgr() { return &m_MgrKeyboard; }
    Object*  GetObjectByCursorPos();
    Object*  GetObjectByPos(Object* pObjParent, POINT* pt, bool bSkinBuilderInvoke=false);

//     void  AddMouseMgr(MouseMgrBase* p);
//     void  AddKeyboardMgr(KeyboardMgrBase* p);
//     void  RemoveMouseMgr(MouseMgrBase* p);
//     void  RemoveKeyboardMgr(KeyboardMgrBase* p);

    void  SetFocusObject(Object* pObj);

    void  AddCommitWindowBufferListener(ICommitWindowBufferListener* p);
    void  RemoveCommitWindowBufferListener(ICommitWindowBufferListener* p);
    RenderChain*  GetRenderChain() { return m_pRenderChain; }
    IRenderChain*  GetIRenderChain();

    // 获取字体流程：先获取自己的m_pTextRender，如果没有则调用自己的m_pWindow的GetRenderFont
    IRenderFont*  GetWindowDefaultRenderFont() { return m_pDefaultFont; }

    void  PaintWindow(HDC hDC);

	void  SaveMemBitmap(TCHAR* szFile);
	void  DrawMemBitmap(HDC hDC, RECT* prc);
	HDC   GetRenderChainMemDC();

protected:
	bool  CreateUI(const TCHAR* szID, HWND hWnd);
	long  ModalLoop(HWND hWndParent);

#pragma region // virtual function
public:
	// public 虚函数，主要是分层窗口的实现与普通窗口不一致
	virtual void CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount=1);
    virtual void OnInnerInitWindow(){}
	
protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    BOOL  PreCreateWindow(CREATESTRUCT* pcs) { return TRUE; }
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnSetDefId(IObject* pButton);
    IObject*  OnGetDefId();
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);


	// 在加载完自己的控件和布局之后被调用，子类调用时先调用父类的该方法
#if 0 // -- 架构改造
	virtual void  OnFinalMessage();
#endif
	virtual BOOL  PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
#pragma endregion
	
#pragma region // message handle
public:
	static LRESULT CALLBACK  StartWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK  StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );\
protected:
	static CREATE_WND_DATA  s_create_wnd_data;    // 创建窗口时，拦截第一个窗口消息，将HWND->this
	static LRESULT CALLBACK ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  StartProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bWindowOrDialog );
	LRESULT  WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    LRESULT  WndProc_GetRetValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled, LRESULT lRet);

protected:
	LRESULT  _OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnHandleMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnHandleKeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT  _OnMouseActive( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnThemeChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnSyncWindow( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnEnterSizeMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnExitSizeMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	void  OnEraseBkgnd(IRenderTarget*);
	LRESULT  OnGetGraphicsRenderType();
    void  OnGetDesiredSize(SIZE* pSize);
    LRESULT  OnGetMouseKeyboardMgr(UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma endregion


#pragma region // member var
public:
	HWND     m_hWnd;                // 窗口句柄

protected:
    IWindowBase*   m_pIWindowBase;
	CWndProcThunk  m_thunk;                 // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC        m_oldWndProc;            // 该窗口的原始窗口过程

    WindowDragDropMgr  m_MgrDragDrop;       // 拖拽管理器
	WindowMouseMgr     m_MgrMouse;          // 鼠标消息处理器
//	WindowKeyboardMgr  m_MgrKeyboard;       // 键盘消息处理器

	CommitWindowBufferListenerMgr  m_MgrCWBL;   // 窗口刷新监听者管理
	RenderChain*   m_pRenderChain;            // 多层渲染

	bool  m_bDoModal;              // 模态窗口数据
	bool  m_bEndModal;
	INT_PTR  m_lDoModalReturn;

	int  m_nMinWidth;             // 窗口大小限制
	int  m_nMinHeight;
	int  m_nMaxWidth;
	int  m_nMaxHeight;

	bool  m_bFirsetEraseBkgnd;    // TODO: 解决第一次显示窗口时，会闪烁一下黑色的问题. 考虑优化，将消息映射做成为动态映射的机制

    // 机制：如果控件没有配置字体时，默认取窗口的字体，即m_pDefaultFont
    //
    // 1. 当xml中配置了窗口字体时，m_hFont为xml中的字体。
    // 2. 当没有配置字体时，使用窗口字体。
    // 3. 当窗口字体也没时，使用UI Font Res 默认字体
    // 4. 最后则尝试使用system default font.
	IRenderFont*  m_pDefaultFont;    

#pragma endregion

};



}

#endif // WINDOWBASE_H_A8C2427E_87C7_45e9_B6CE_AF3B4DBF49EF