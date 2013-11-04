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
//   ��������������������Ĵ��ڣ�������UI���ڣ���ȴ��ʹ��UI�Ŀؼ�
//	 �ṩ��������Ҫ��һЩ�������ԣ�������Ϊ���ṩUI�ؼ�ʹ�ã�Ҳ����ΪUI���ڵĻ���
//
//   libo 20110821 9:57 (hangzhou xingyao jiari jiudian 8332)

namespace UI
{
class RenderChain;
// 
//	�����ڴ������ڵ�ʱ�����ص�һ��������Ϣ�����������໯
//
class CREATE_WND_DATA  // ��ͬ�� ATL �е�_ATL_WIN_MODULE70
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

// 2013.4.22 ��windowbase�����޸�ΪWindowRenderLayer��������Panel���Ӷ��Ż������ֲ���Ⱦ����
// Ĭ��һ������ֻ��һ��render layer�������ڶ���������Ҫ����һ��render layerʱ��������
// ����һ��render layer���󣬸ö����봰�ڶ���û��ʲô��ϵ��ֻ�Ƕ���RenderChain���������ˡ�
class WindowBase : public WindowRenderLayer, public SyncWindowHelper<WindowBase>
                 , public IMKMgr   // ����Ŀǰû�н�mouse/keyboard�ϲ���������ʱ��window��ʵ������ӿ�
{
public:
    WindowBase();
    ~WindowBase();

	UI_DECLARE_OBJECT3(WindowBase, OBJ_WINDOW, _T("Kernel/Window"))

    // WndProc��ԭʼ��Ϣ����   // ����virtual��չ��
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

    // UI�¼�����Ϣ����
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

    // ��ȡ�������̣��Ȼ�ȡ�Լ���m_pTextRender�����û��������Լ���m_pWindow��GetRenderFont
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
	// public �麯������Ҫ�Ƿֲ㴰�ڵ�ʵ������ͨ���ڲ�һ��
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


	// �ڼ������Լ��Ŀؼ��Ͳ���֮�󱻵��ã��������ʱ�ȵ��ø���ĸ÷���
#if 0 // -- �ܹ�����
	virtual void  OnFinalMessage();
#endif
	virtual BOOL  PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet);
#pragma endregion
	
#pragma region // message handle
public:
	static LRESULT CALLBACK  StartWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK  StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );\
protected:
	static CREATE_WND_DATA  s_create_wnd_data;    // ��������ʱ�����ص�һ��������Ϣ����HWND->this
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
	HWND     m_hWnd;                // ���ھ��

protected:
    IWindowBase*   m_pIWindowBase;
	CWndProcThunk  m_thunk;                 // ATL�е�THUNK�����ڽ�һ�����ڹ��������Լ��ĳ�Ա����
	WNDPROC        m_oldWndProc;            // �ô��ڵ�ԭʼ���ڹ���

    WindowDragDropMgr  m_MgrDragDrop;       // ��ק������
	WindowMouseMgr     m_MgrMouse;          // �����Ϣ������
//	WindowKeyboardMgr  m_MgrKeyboard;       // ������Ϣ������

	CommitWindowBufferListenerMgr  m_MgrCWBL;   // ����ˢ�¼����߹���
	RenderChain*   m_pRenderChain;            // �����Ⱦ

	bool  m_bDoModal;              // ģ̬��������
	bool  m_bEndModal;
	INT_PTR  m_lDoModalReturn;

	int  m_nMinWidth;             // ���ڴ�С����
	int  m_nMinHeight;
	int  m_nMaxWidth;
	int  m_nMaxHeight;

	bool  m_bFirsetEraseBkgnd;    // TODO: �����һ����ʾ����ʱ������˸һ�º�ɫ������. �����Ż�������Ϣӳ������Ϊ��̬ӳ��Ļ���

    // ���ƣ�����ؼ�û����������ʱ��Ĭ��ȡ���ڵ����壬��m_pDefaultFont
    //
    // 1. ��xml�������˴�������ʱ��m_hFontΪxml�е����塣
    // 2. ��û����������ʱ��ʹ�ô������塣
    // 3. ����������Ҳûʱ��ʹ��UI Font Res Ĭ������
    // 4. �������ʹ��system default font.
	IRenderFont*  m_pDefaultFont;    

#pragma endregion

};



}

#endif // WINDOWBASE_H_A8C2427E_87C7_45e9_B6CE_AF3B4DBF49EF