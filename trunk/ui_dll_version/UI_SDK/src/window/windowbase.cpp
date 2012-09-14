#include "stdafx.h"

WindowBase::WindowBase()
{
	this->m_hWnd          = NULL;
	this->m_oldWndProc    = NULL;
	this->m_hRenderTarget = NULL;
	this->m_hFont         = NULL;

	this->m_bDoModal       = false;
	this->m_bEndModal      = false;
	this->m_lDoModalReturn = 0;

	this->m_MgrMouse.SetWindow( this );
	this->m_MgrMouse.SetKeyboardManager( &this->m_MgrKeyboard );
	this->m_MgrKeyboard.SetWindow( this );
}
WindowBase::~WindowBase()
{
	if( NULL != m_hFont )
	{
		::UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
}

void WindowBase::ResetAttribute()
{
	__super::ResetAttribute();

	if( NULL != m_hFont )
	{
		::UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
}
bool WindowBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Panel::SetAttribute( mapAttrib, bReload );
	if( false == bRet )
		return false;

	// 字体样式 
	if( this->m_mapAttribute.count( XML_FONT ) )
	{
		m_hFont = ::UI_GetFont( mapAttrib[XML_FONT], GetGraphicsRenderType(GetHWND()));
		m_mapAttribute.erase(XML_FONT);
	}

	// 创建默认字体
	if( NULL == m_hFont )
	{
		HFONT hFont = (HFONT)::SendMessage( m_hWnd, WM_GETFONT, 0,0 );
		if( NULL == hFont )
		{
			m_hFont = UI_GetDefaultFont(GetGraphicsRenderType(m_hWnd));
			if (NULL == m_hFont)
			{
				hFont = (HFONT)GetStockObject(SYSTEM_FONT);
			}
		}
		
		if (NULL == m_hFont && NULL != hFont)
		{
			UI_AttachFont(&m_hFont, hFont, GetGraphicsRenderType(m_hWnd));
		}
	}

	return true;
}



//	[public] bool Attach( HWND hWnd )
//
//	目的：和一个现有的窗口进行关联，对该窗口进行子类化操作
//
//	参数：
//		ID
//			[in]	UI窗口在xml中的ID，用于加载子控件
//
//		hWnd
//			[in]	要进行关联的窗口句柄
//	
//	返回：成功返回true，失败返回false
//
//	备注：调用该函数的情况是：有一个窗口（例如派生自CDialog，而不是一个UI的window类），
//		  但它却想使用我们的UI控件，这个时候需要采用这个
//
bool WindowBase::CreateUI( const String& ID, HWND hWnd )
{
	this->m_hWnd = hWnd;
	this->m_strID = ID;

	//	加载子控件
	bool bRet = ::UI_LoadLayout( this );
	if( false == bRet )
	{
		UI_LOG_FATAL( _T("Window::Create, 加载窗口(ID=%s)layout失败"), this->m_strID.c_str() );
		return false;
	}

	if( !IsChildWindow() )
	{
		g_pUIApplication->m_TopWindowMgr.AddTopWindowObject( this );
		::UI_UpdateTopWindowLayout( this );		
	}
	else
	{
		::UI_UpdateLayout(this);
	}

	//
	//	为了解决xp、win7上面的一个特性：只有在按了ALT键，或者TAB键之后，才会显示控件的focus rect
	//	在初始化后，主动将该特性清除。
	//	
	::PostMessage( m_hWnd, WM_CHANGEUISTATE, 
		MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);
	
	return true;
}
void WindowBase::DestroyUI()
{
	Panel::DestroyUI();

	UIASSERT( !IsWindow(m_hWnd) );          // 确保窗口已被销毁
	UIASSERT( NULL == this->m_oldWndProc ); // 确保已经取消子类化

	m_hWnd = NULL;
	if( NULL != m_hRenderTarget )
	{
		ReleaseHRDC(m_hRenderTarget);
		m_hRenderTarget = NULL;
	}
}

//
//	增加一个控件的通知者为自己，即当这个控件有事件发生时，将事件通知我
//
//	Parameter
//		idPath
//			[in]	要设置的控件ID，如window/panel/loginbtn，那么idPath应为 panel/loginbtn
//		nNotifyMapID
//			[in]	通知事件的处理ID，即ProcessMessage的第二个参数
//
// void WindowBase::Control_NotifyMe( const String&  idPath, int nNotifyMapID )
// {
// 	this->Control_NotifyMe( __super::GetChildObject( idPath ), nNotifyMapID );
// }
void WindowBase::Control_NotifyMe( Object*  pObj, int nNotifyMapID )
{
	if( NULL == pObj )
	{
		UI_LOG_ERROR( _T("Window::Control_NotifyMe, 无效的参数 pObj ") );
		return ;
	}

	pObj->AddNotify( this, nNotifyMapID );
}

WindowBase::operator HWND() const
{
	return this->m_hWnd;
}


//
//	[public] [virtual] void InvalidateObj( Object* pObj, bool bUpdateNow )
//
//	Parameters
//		pObj
//			[in] 要刷新哪个对象
//		bUpdateNow
//			[in] 是否立即刷新
//
//	Return
//		NA
//
//	Remark
//		在这里必须同时考虑各种WINDOW对象的刷新方式，例如分层窗口和普通窗口是不一样的。
//		为了防止普通窗口在需要一次刷新多个对象时，需要多次调用InvalidateObj方法去刷新，
//		如果这个窗口是一个分层窗口，只有当bUpdateNow = true的时候，才去处理。
//
//	See Also
//		LayeredWindow::InvalidateObject
//
void WindowBase::InvalidateObject( Object* pInvalidateObj, RECT* prc, bool bUpdateNow )
{
	if( NULL == pInvalidateObj || NULL == m_hRenderTarget )
		return;
 
	if( OBJ_HWNDHOST == pInvalidateObj->GetObjectType() )
	{
		// 系统控件的刷新由它自己本身负责
	}
	else if( OBJ_WINDOW == pInvalidateObj->GetObjectType() )
	{
		WindowBase* pWindow = (WindowBase*)pInvalidateObj;
		::InvalidateRect(pWindow->m_hWnd, prc, TRUE);
		if( bUpdateNow )
		{
			UpdateWindow(pWindow->m_hWnd);
		}
	}
	else
	{
		this->_InvalidateObject(pInvalidateObj, NULL);
	}
}

void WindowBase::InvalidateObjectBkgnd( Object* pInvalidateObj, RECT* prc, bool bUpdateNow )
{
	if( NULL == pInvalidateObj || NULL == m_hRenderTarget )
		return;

	if( OBJ_HWNDHOST == pInvalidateObj->GetObjectType() )
	{
		// 系统控件的刷新由它自己本身负责
	}
	else if( OBJ_WINDOW == pInvalidateObj->GetObjectType() )
	{
		WindowBase* pWindow = (WindowBase*)pInvalidateObj;
		::InvalidateRect(pWindow->m_hWnd, prc, TRUE);
		if( bUpdateNow )
		{
			UpdateWindow(pWindow->m_hWnd);
		}
	}
	else
	{
		this->_InvalidateObjectBkgnd(pInvalidateObj, NULL);
	}
}

void WindowBase::_InvalidateObject(Object* pInvalidateObj, HDC hDestDC)
{
	if (false ==BeginDraw(m_hRenderTarget, hDestDC))
		return;

	RenderOffsetClipHelper roc(this);
	pInvalidateObj->DrawObjectTransparentBkgnd(m_hRenderTarget, roc, pInvalidateObj->IsTransparent());
	
	pInvalidateObj->DrawObject(m_hRenderTarget, roc);
	roc.Reset(m_hRenderTarget);
	
	//////////////////////////////////////////////////////////////////////////
	//
	//  提交显示

	int nX = roc.m_rcClip.left + roc.m_ptOffset.x;
	int nY = roc.m_rcClip.top  + roc.m_ptOffset.y;
	int nW = roc.m_rcClip.Width();
	int nH = roc.m_rcClip.Height();
	EndDraw(m_hRenderTarget, nX, nY, nW, nH, nX, nY);
}

void WindowBase::_InvalidateObjectBkgnd(Object* pInvalidateObj, HDC hDestDC)
{
	if (false == BeginDraw(m_hRenderTarget, hDestDC))
		return;

	RenderOffsetClipHelper roc(this);
	pInvalidateObj->DrawObjectTransparentBkgnd(m_hRenderTarget, roc, true);
	roc.Reset(m_hRenderTarget);

	//////////////////////////////////////////////////////////////////////////
	//
	//  提交显示

	int nX = roc.m_rcClip.left + roc.m_ptOffset.x;
	int nY = roc.m_rcClip.top  + roc.m_ptOffset.y;
	int nW = roc.m_rcClip.Width();
	int nH = roc.m_rcClip.Height();
	EndDraw(m_hRenderTarget, nX, nY, nW, nH, nX, nY);
}

//
//	用于如LISTBOX ReDrawItem，只刷新一部分，而不是整个object
//
HRDC WindowBase::BeginDrawObject( Object* pInvalidateObj, HRGN& hClipRgn)
{
	if( NULL == pInvalidateObj )
		return NULL;

	if (false ==BeginDraw(m_hRenderTarget, NULL))
		return NULL;

	if( this->m_hRgn != NULL )
	{
		hClipRgn = m_hRgn;
	}
	else
	{
		hClipRgn = ::CreateRectRgn( pInvalidateObj->GetParentRectL(), pInvalidateObj->GetParentRectT(), 
			pInvalidateObj->GetParentRectR(), pInvalidateObj->GetParentRectB() );
	}
	SelectClipRgn( m_hRenderTarget, hClipRgn );
	SetViewportOrgEx(m_hRenderTarget, pInvalidateObj->GetParentRectL(), pInvalidateObj->GetParentRectT(), NULL );

	::UISendMessage(pInvalidateObj, WM_ERASEBKGND, (WPARAM)m_hRenderTarget, (LPARAM)1 );

	return m_hRenderTarget;
}
//
//	要提交到窗口上的区域，配合BeginDrawObject使用
//
void WindowBase::EndDrawObject( CRect* prcWindow, HRGN& hClipRgn )
{
	SetViewportOrgEx(m_hRenderTarget, 0,0 , NULL );

	if( m_hRgn != hClipRgn )
	{
		::DeleteObject(hClipRgn);
	}
	SelectClipRgn( m_hRenderTarget, NULL );

	EndDraw(m_hRenderTarget, 
		prcWindow->left, prcWindow->top,
		prcWindow->Width(), prcWindow->Height(),
		prcWindow->left, prcWindow->top);
}

bool WindowBase::Create( const String& ID, HWND hWndParent )
{
	// removed by libo 20120728 允许创建一个空的窗口，例如菜单窗口
// 	UIASSERT( ID != _T("") );
// 	if( _T("") == ID )
// 	{
// 		UI_LOG_FATAL( _T("Window::Create, 未指定窗口id") );
// 		return false;
// 	}

	__super::m_strID = ID;   // 提前给id赋值，便于日志输出

	//	创建窗口句柄
	CREATESTRUCT cs;
	::ZeroMemory(&cs, sizeof(CREATESTRUCT));
	DWORD  dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;

	cs.style     = WS_OVERLAPPEDWINDOW;
	cs.lpszClass = WND_CLASS_NAME;
	cs.lpszName  = _T("");
	cs.x = cs.y = 0;
	cs.cx = cs.cy = 300;//CW_USEDEFAULT;

	this->PreCreateWindow( cs );

	UI_AddCreateWndData(&m_thunk.cd, this);
	this->m_hWnd = ::CreateWindowEx( 
		cs.dwExStyle,
		cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy,
		hWndParent, 0, NULL, NULL );

	if( NULL == m_hWnd )
	{
		UI_LOG_FATAL( _T("Window::Create, CreateWindowEx失败") );
		return false;
	}
	return true;
}

void WindowBase::EndDialog(INT_PTR nResult)
{
	UIASSERT( this->m_bDoModal );

	// 让ModalWindow::DoModal里的消息循环能够退出
	this->m_lDoModalReturn = (long)nResult ;
	this->m_bEndModal = true;
	::PostMessage( this->m_hWnd, WM_NULL, 0,0 );
}
long WindowBase::DoModal( HINSTANCE hResInst, UINT nResID, const String& ID, HWND hWndParent )
{
#if 0
	UIASSERT( NULL == m_hWnd );

	UI_AddCreateWndData(&m_thunk.cd, (void*)this);
	m_strID = ID;

	return DialogBox(hResInst, MAKEINTRESOURCE(nResID), hWndParent, (DLGPROC)WindowBase::StartDialogProc);
#endif

	HWND hWnd = this->DoModeless(hResInst, nResID, ID, hWndParent);
	if( NULL == hWnd )
	{
		return -1;
	}

	return this->ModalLoop(hWndParent);
}


//
// 创建一个空的模态对话框
//
long  WindowBase::DoModal(  const String& ID, HWND hWndParent )
{

#if 0
	UIASSERT( NULL == m_hWnd );

	CREATESTRUCT cs;
	cs.style        = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
	cs.lpszClass    = WND_CLASS_NAME;
	cs.lpszName     = _T("");
	cs.x  = cs.y    = 0;
	cs.cx = cs.cy   = 100;//CW_USEDEFAULT;
	DWORD dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;
	this->PreCreateWindow( cs );

	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return -1;
	LPDLGTEMPLATE lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	lpdt->cdit = 0;  // Number of controls
	lpdt->style = cs.style;
	lpdt->dwExtendedStyle = dwStyleEx;
	lpdt->x = cs.x;
	lpdt->y = cs.y;
	lpdt->cx = cs.cx;
	lpdt->cy = cs.cy;

	LPWORD lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0;             // No menu
	*lpw++ = 0;             // Predefined dialog box class (by default)

	LPWSTR lpwsz = (LPWSTR)lpw;
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // 窗口的标题名称
	lpw += nchar;
	GlobalUnlock(hgbl); 

	UI_AddCreateWndData(&m_thunk.cd, this);
	m_strID = ID;

	long lRet = DialogBoxIndirect(g_pUIApplication->GetModuleInstance(), 
		(LPDLGTEMPLATE)hgbl, 
		hWndParent, 
		(DLGPROC)WindowBase::StartDialogProc); 

	GlobalFree(hgbl); 
	return lRet;
#endif

	HWND hWnd = this->DoModeless(ID, hWndParent);
	if( NULL == hWnd )
	{
		return -1;
	}

	return this->ModalLoop(hWndParent);
}

long WindowBase::ModalLoop(HWND hWndParent)
{
	this->ShowWindow();
	::UpdateWindow( this->m_hWnd );

	this->m_bDoModal = true;
	bool bEnableWindow = false;
	if( NULL != hWndParent && GetDesktopWindow() != hWndParent )
	{
		::EnableWindow( hWndParent, FALSE );
		bEnableWindow = true;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if( FALSE == UI_IsDialogMessage(&msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if( this->m_bEndModal )
		{
			this->m_bEndModal = false;
			break;
		}
	}

	if( bEnableWindow )
	{
		::EnableWindow( hWndParent, TRUE );
	}
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
	{
		::SetActiveWindow(hWndParent);  // 如果不调用该函数，将导致父窗口跑到屏幕Z次序的后面去了
	}
	::DestroyWindow( this->m_hWnd );    // 销毁窗口

	this->m_bDoModal = false;
	return this->m_lDoModalReturn;
}

BOOL WindowBase::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

HWND WindowBase::DoModeless( const String& ID, HWND hWndParent )
{
	UIASSERT( NULL == m_hWnd );

	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(CREATESTRUCT));
	cs.style        = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
	cs.lpszClass    = WND_CLASS_NAME;
	cs.lpszName     = _T("");
	cs.x  = cs.y    = 0;
	cs.cx = cs.cy   = 100;//CW_USEDEFAULT;
	cs.dwExStyle    = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;
	this->PreCreateWindow( cs );

	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
	if (!hgbl)
		return NULL;
	LPDLGTEMPLATE lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

	lpdt->cdit = 0;  // Number of controls
	lpdt->style = cs.style;
	lpdt->dwExtendedStyle = cs.dwExStyle;
	lpdt->x = cs.x;
	lpdt->y = cs.y;
	lpdt->cx = cs.cx;
	lpdt->cy = cs.cy;

	LPWORD lpw = (LPWORD)(lpdt + 1);
	*lpw++ = 0;             // No menu
	*lpw++ = 0;             // Predefined dialog box class (by default)

	LPWSTR lpwsz = (LPWSTR)lpw;
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // 窗口的标题名称
	lpw += nchar;
	GlobalUnlock(hgbl); 

	UI_AddCreateWndData(&m_thunk.cd, this);
	m_strID = ID;

	m_hWnd = CreateDialogIndirect(g_pUIApplication->GetModuleInstance(), 
		(LPDLGTEMPLATE)hgbl, 
		hWndParent, 
		(DLGPROC)WindowBase::StartDialogProc); 

	GlobalFree(hgbl); 
	return m_hWnd;
}
HWND WindowBase::DoModeless( HINSTANCE hResInst, UINT nResID, const String& ID, HWND hWndParent )
{
	UIASSERT( NULL == m_hWnd );

	UI_AddCreateWndData(&m_thunk.cd, (void*)this);
	m_strID = ID;

	m_hWnd = CreateDialog(hResInst, MAKEINTRESOURCE(nResID), hWndParent, (DLGPROC)WindowBase::StartDialogProc);
	return m_hWnd;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowBase::StartWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 获取this指针
	WindowBase* pThis = (WindowBase*)UI_ExtractCreateWndData();
	UIASSERT(NULL != pThis);
	
	if( NULL == pThis )
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, true);
}
//
//	[static] Dialog类型窗口的第一个窗口消息调用的窗口过程
//
LRESULT CALLBACK WindowBase::StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 获取this指针
	WindowBase* pThis = (WindowBase*)UI_ExtractCreateWndData();
	UIASSERT(NULL != pThis);

	if( NULL == pThis )
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, false);
//	return (UINT_PTR)FALSE;
}
//
//	由StartWindowProc/StartDialogProc调用，将窗口过程转换为类对象的一个方法
//
LRESULT WindowBase::StartProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bWindowOrDialog )
{
	// 子类化
	this->m_hWnd = hwnd;

	this->m_thunk.Init( &Window::ThunkWndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();

	if( bWindowOrDialog )
	{
		this->m_oldWndProc = ::DefWindowProc;
		::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);
	}
	else
	{
		this->m_oldWndProc = NULL;
		::SetWindowLong( m_hWnd, DWLP_DLGPROC, (LONG)(LONG_PTR)pProc);  
	}

	// 调用新的窗口过程 ThunkWndProc
	return pProc(hwnd, uMsg, wParam, lParam);
}

//
//	[static] LRESULT CALLBACK ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//	被ATL的thunk替换过的窗口过程
//
//	Parameter：
//		hwnd
//			[in]	这里由于被替换过了，这里的hwnd是this指针
//
//		uMsg,wParam,lParam
//			[in]	消息信息
//
LRESULT  WindowBase::ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	Window* pThis = (Window*)hwnd;
	return pThis->WndProc( uMsg, wParam, lParam );
}

LRESULT WindowBase::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_oldWndProc )
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}

	return 0;
}

//
//	[private] LRESULT WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
//
//	窗口被子类化过之后的窗口过程
//
LRESULT	WindowBase::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes;
	UIMSG*  pOldMsg = m_pCurMsg;
	BOOL bRet = this->ProcessWindowMessage( m_hWnd, uMsg, wParam, lParam, lRes, 0 );  // 调用BEGIN_MSG_MAP消息映射列表
	if( bRet )
	{
		return lRes;
	}
	else
	{
		UIMSG  msg;
		msg.message = uMsg;
		msg.pObjMsgTo = this;
		msg.wParam  = wParam;
		msg.lParam  = lParam;

		// 如果这个消息被处理过了，直接返回，不用再调用旧的窗口过程了
		if( this->ProcessMessage( &msg ) )
		{
			if( NULL == m_oldWndProc )
			{
				switch(msg.message)
				{
				case WM_INITDIALOG: 
					if( NULL != GetKeyboardMgr().GetFocusObject() )
						lRes = FALSE;  // 不使用其焦点设置
					else
						lRes = msg.lRet;

					break;

				case WM_COMPAREITEM:
				case WM_VKEYTOITEM:
				case WM_CHARTOITEM:
				case WM_QUERYDRAGICON:
				case WM_CTLCOLORMSGBOX:
				case WM_CTLCOLOREDIT:
				case WM_CTLCOLORLISTBOX:
				case WM_CTLCOLORBTN:
				case WM_CTLCOLORDLG:
				case WM_CTLCOLORSCROLLBAR:
				case WM_CTLCOLORSTATIC:
					// return directly
					lRes = msg.lRet;
					break;

				default:
					SetWindowLong(m_hWnd, DWL_MSGRESULT, msg.lRet) ;
					lRes = TRUE;
				}
			}
			else
				lRes = msg.lRet;
		}
		else
			lRes = DefWindowProc( uMsg, wParam, lParam );
	}

	if (uMsg == WM_NCDESTROY)
	{
		// 注：为什么不在这里直接调用OnFinalMessage，却还要再加一个状态位？
		// 因为WM_NCDESTROY函数由DestroyWindow api触发，而DestroyWindow api
		// 可能位于任何一个当前窗口的消息响应中，因此当pOldMsg==NULL时，即表示
		// 没有消息嵌套了，在检查一次WINDOW_STYLE_DESTROYED标志即可。
		this->ModifyStyle(WINDOW_STYLE_DESTROYED,0);
	}
	if (m_nStyle&WINDOW_STYLE_DESTROYED && pOldMsg == NULL)
	{
		this->OnFinalMessage();
		this->ModifyStyle(0,WINDOW_STYLE_DESTROYED);
	}

	return lRes;
}

LRESULT WindowBase::_OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if( 0 == HIWORD(lParam) )  // is menu popup
	{
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		return TRUE;
	}

	HWND hWnd = (HWND)wParam;
	if( 0 != wParam && hWnd !=  m_hWnd )
	{
		return DefWindowProc(uMsg,wParam,lParam);  // 鼠标位于窗口上的其它windows控件上面，不处理
	}  
	else if( hWnd == m_hWnd )  
	{
		if( LOWORD(lParam) != HTCLIENT )    // 用于鼠标位于system window的边缘时，应该调用默认的处理过程。包括当自己弹出一个模态框，返回HTERROR时
		{
			return DefWindowProc(uMsg,wParam,lParam);
		}

//      Removed 20120824 -- 在实现windowless richedit时，不能采用延时发送setcursor的办法，否则在
//      选择文本时鼠标样式一直闪烁。因此修改方案为：先发送，然后在WM_MOUSEMOVE中检测如果hover obj
//      发生了变化的话，重新再发送一次WM_SETCURSOR
//
//		return TRUE;   // 由于只有在MOUSEMOVE之后确定了HOVER、PRESS对象后，才可以去SETCURSOR，-- x 废弃
		// 因此在MOUSEMOVE之前的SETCURSOR消息需要被忽略，只处理在MOUSEMOVE后POST出来的        -- x 废弃
		// SETCURSOR消息，该消息wParam为0                                                     -- x 废弃
	}
	if( TRUE == this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam ) ) // 仅发给当前hover object
	{
		return TRUE;
	}

	// 让窗口去处理这个消息
	UIMSG  msg;
	msg.message = uMsg;
	msg.pObjMsgTo = this;
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	if( this->ProcessMessage( &msg ) )
		return msg.lRet;

	// 还是没有人处理
	::SetCursor( ::LoadCursor(NULL,IDC_ARROW) );
	return TRUE;
}

// 该消息已在WM_PAINT中分发。
// 为了实现无闪烁的绘制，必须将所有的绘制代码放在一个地方处理，要么在WM_ERASEBKGND，要么在WM_PAINT
// 但WM_ERASEBKGND不是每次都会触发的（在处理异形窗口时出现该问题），因此考虑将绘制代码都放在WM_PAINT中处理
// 
// 如果 lParam == 0，则表示是系统自己发出来的消息，由于系统消息会导致和WM_PAINT DC不一致，从而产生闪烁
// 因此将WM_ERASEBKGND消息放在WM_PAINT中由我们自己发出，并且将lParam置为非0
LRESULT WindowBase::_OnEraseBkgnd( UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if( 0 == lParam )
	{
		bHandled = TRUE;
		return 1;   // 对于Dialog类型，需要返回1来阻止系统默认绘制
	}
	return 0;
}
LRESULT WindowBase::_OnPaint( UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	PAINTSTRUCT ps;
	HDC         hDC = NULL;
	HFONT       hOldFont = NULL;
	CRect       rect;

	if( NULL == wParam )
		hDC = ::BeginPaint(this->m_hWnd ,&ps);
	else
		hDC = (HDC)wParam;

	::GetClientRect( this->m_hWnd, &rect );
	int nWidth  = rect.Width();
	int nHeight = rect.Height();

	if (NULL == m_hRenderTarget)
	{
		m_hRenderTarget = CreateRenderTarget(m_hWnd, nWidth, nHeight );
		if( NULL == m_hRenderTarget )
			UI_LOG_ERROR(_T("WindowBase::WndProc CreateRenderTarget Failed."));
	}

	if (BeginDraw(m_hRenderTarget, hDC))
	{
		RenderOffsetClipHelper roc(this);
		this->DrawObject(m_hRenderTarget, roc);
		roc.Reset(m_hRenderTarget);
		EndDraw(m_hRenderTarget);
	}

	if( NULL == wParam )
		EndPaint(m_hWnd,&ps);

	return TRUE;
}

LRESULT WindowBase::_OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if( SIZE_MINIMIZED != wParam && NULL != m_hRenderTarget )
	{
		//	修改缓冲背景图片大小
		ResizeRenderTarget(m_hRenderTarget, LOWORD(lParam), HIWORD(lParam));

		//
		// (只有是用户显式修改窗口大小（拖拽、最大化）时，才能将width/height 从auto修改为现有值cx/cy
		// 避免了因为 第一次显示窗口时发出的WM_SIZE 而造成错误。)  <-- 作废！因为没法处理 nType==0 RESTORE 的情况
		//
		//	改成忽略这些情况了，直接赋值！因此会造成窗口的width=auto/height=auto只是窗口创建后那一刻的事情了，之后
		//  只要响应过 WM_SIZE，就不会再是auto了
		//

		// WM_SIZE的参数是基于客户区大小的，UI_UpdateLayout，UI_UpdateTopWindowLayout需要的是整个窗口的大小
		if( NULL != m_pLayout )  // 窗口刚创建，还没有调用UI_LoadLayout
		{
			RECT rcWindow;
			::GetWindowRect(m_hWnd, &rcWindow);

			m_nConfigWidth = rcWindow.right-rcWindow.left;
			m_nConfigHeight = rcWindow.bottom-rcWindow.top;

			::GetClientRect(m_hWnd, &m_rcParent);
			::UI_UpdateLayout(this, FALSE);
		}
	}

	return 0;
}
LRESULT WindowBase::_OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if (!m_strID.empty())
	{
		this->CreateUI(m_strID, m_hWnd);
	}

	//
	//  给子类一个初始化的机会 (virtual)
	//  有可能m_strID为空（不加载资源，例如临时的popupconotrolwindow）
	//	因此没有将OnInitWindow放在CreateUI中执行
	//
	this->OnInitWindow();

	return 0;
}
LRESULT WindowBase::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if( !IsChildWindow() )
	{
		g_pUIApplication->m_TopWindowMgr.RemoveTopWindowObject( this );
	}

	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam );
	this->m_MgrKeyboard.HandleMessage( uMsg, wParam, lParam );

	if( NULL != m_oldWndProc )
	{
		::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
		m_oldWndProc = NULL;
	}
	m_hWnd = NULL;
	this->DestroyUI();

	return 0;
}
LRESULT WindowBase::_OnHandleMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam );
	if( NULL != m_MgrMouse.GetHoverObject() || NULL != m_MgrMouse.GetPressObject() )
	{
		bHandled = TRUE;
	}

	return 0;
}
LRESULT WindowBase::_OnHandleKeyboardMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	this->m_MgrKeyboard.HandleMessage( uMsg, wParam, lParam );
	return 1;
}
LRESULT WindowBase::_OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam );
	this->m_MgrKeyboard.HandleMessage( uMsg, wParam, lParam );

	UIMSG  msg;
	msg.message = uMsg;
	msg.pObjMsgTo = this;
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	this->ProcessMessage( &msg );

	return 1;    // 注：由于Dialog默认的WM_SETFOCUS会将焦点再设置到自己的第一个控件上面，因此如果在这里return 0或者bHandled = FALSE
	             //     将导致::SetFocus(m_hWnd)的焦点到窗口上面后，又被窗口自己设置到控件上面，导致SetFocus达不到我们的本意
}
LRESULT WindowBase::_OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam );
	this->m_MgrKeyboard.HandleMessage( uMsg, wParam, lParam );

	UIMSG  msg;
	msg.message = uMsg;
	msg.pObjMsgTo = this;
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	this->ProcessMessage( &msg );

	return 1 ;
}
LRESULT WindowBase::_OnThemeChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	
	Message::ForwardMessageToChildObject(this,uMsg,wParam,lParam);
	return 0;
}

LRESULT WindowBase::_OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	SyncWindowHelper<WindowBase>::_OnWindowPosChanging((LPWINDOWPOS)lParam);
	return 0;
}
LRESULT WindowBase::_OnSyncWindow( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	const SyncWindowData& data = *(SyncWindowData*)(lParam);
	switch(wParam)
	{
	case ADD_SYNC_WINDOW:
		this->AddAnchorItem(data);
		break;
	case MODIFY_SYNC_WINDOW:
		this->ModifyAnchorItem(data);
		break;
	case REMOVE_SYNC_WINDOW:
		this->RemoveAnchorItem(data.m_hWnd);
		break;
	}
	return 0;
}
BOOL WindowBase::OnEraseBkgnd(HRDC hRDC)
{
	BOOL bRet = __super::OnEraseBkgnd(hRDC);  // 如果m_pEraseBkgndRender没有处理，在这里调用系统过程
	if( FALSE == bRet )
	{
		if( NULL == m_oldWndProc )   // Dialog类型，直接填充系统背景色
		{
			CRect rc;
			::GetClientRect(m_hWnd,&rc);
			FillRect( hRDC, &rc, ::GetSysColor(COLOR_BTNFACE) );
		}
		else                         // Window类型，直接调用系统过程
		{
			HDC hDC = GetHDC(hRDC);
			bRet = (BOOL)/*this->*/DefWindowProc(WM_ERASEBKGND, (WPARAM)hDC, 1 );  // 与原始消息进行区分
			ReleaseHDC(hRDC,hDC);
		}
	}	

	return bRet;
}

LRESULT WindowBase::OnGetRenderType()
{
	return  /*GRAPHICS_RENDER_TYPE_GDIPLUS*/ GRAPHICS_RENDER_TYPE_AUTO;
}

BOOL WindowBase::PreCreateWindow( CREATESTRUCT& cs )
{
	return TRUE;
}

//
//  类似于OnCreate/OnInitDialog函数，但该函数无论是窗口还是对话框都会被触发
//	所有子类在重载该函数的时候，必须同时去调用父类的该函数。
//
void WindowBase::OnInitWindow( )
{
	// TODO 数据DDX_DDV
}

//
//  在响应完窗口的WM_NCDESTROY后，会触发该函数。
//  不要尝试在消息响应中调用delete this; 但可以再该函数中调用
//
void WindowBase::OnFinalMessage()
{
	
}

//
//	当窗口有类型发生改变时(分层窗口<->普通窗口)，重新创建render target
//
void WindowBase::ReCreateRenderTarget()
{
	if( NULL != m_hRenderTarget )     // 需要重新创建一个基于新窗口类型的RenderTarget
	{
		ReleaseHRDC(m_hRenderTarget);
		m_hRenderTarget = NULL;
	}

	if (NULL == m_hRenderTarget)
	{
		m_hRenderTarget = CreateRenderTarget(m_hWnd, GetWidth(), GetHeight() );
		if( NULL == m_hRenderTarget )
		{
			UI_LOG_ERROR(_T("WindowBase::WndProc CreateRenderTarget Failed."));
		}
	}
}

//
//	[static] Window类型窗口的第一个窗口消息调用的窗口过程
//
//
//	如果已知一个窗口的client区域大小，求这个窗口的window区域大小
//
//	参数
//		sizeClient
//			[in]	该窗口的客户区大小
//		pSizeWindow
//			[out]	返回窗口的window大小
//
void WindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow )
{
	if( NULL == pSizeWindow )
		return;

	CRect  tempRcWindow, tempRcClient;

	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// 通过 WM_NCCALCSIZE 消息，让系统告诉我们一个window rect 对应的client rect是多少
	np.lppos = &wp;
	::GetWindowRect( this->m_hWnd, &np.rgrc[0] );
	tempRcWindow.CopyRect(&np.rgrc[0]);
	::SendMessage( this->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );
	tempRcClient.CopyRect(&np.rgrc[0] );

	pSizeWindow->cx = sizeClient.cx + tempRcWindow.Width() - tempRcClient.Width();
	pSizeWindow->cy = sizeClient.cy + tempRcWindow.Height() - tempRcClient.Height();
}

//
//	如果已知一个窗口的window区域大小，求这个窗口的client区域大小
//
//	参数
//		rcWindow
//			[in]	该窗口的客大小
//		rcClient
//			[out]	返回窗口的client大小（默认left=0,top=0，即只返回窗口大小）
//
void WindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient )
{
	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// 通过 WM_NCCALCSIZE 消息，让系统告诉我们一个window rect 对应的client rect是多少
	np.lppos = &wp;
	::CopyRect( &np.rgrc[0], rcWindow );
	::SendMessage( this->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );

	rcClient->left   = rcClient->top = 0;
	rcClient->right  = np.rgrc[0].right - np.rgrc[0].left;
	rcClient->bottom = np.rgrc[0].bottom - np.rgrc[0].top;
}

BOOL WindowBase::IsChildWindow()
{
	LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE );
	if( lStyle & WS_CHILD )
		return TRUE;
	else
		return FALSE;
}

MouseManager& WindowBase::GetMouseMgr()
{
	return m_MgrMouse;
}
KeyboardManager&  WindowBase::GetKeyboardMgr()
{
	return m_MgrKeyboard;
}
Object* WindowBase::GetHoverObject()
{
	return m_MgrMouse.GetHoverObject();
}
Object* WindowBase::GetPressObject()
{
	return m_MgrMouse.GetPressObject();
}

void WindowBase::ShowWindow()
{
	::ShowWindow( m_hWnd, SW_SHOW );
}
void WindowBase::HideWindow()
{
	::ShowWindow( m_hWnd, SW_HIDE );
}

HRFONT WindowBase::GetHRFONT()
{
	return m_hFont;
}