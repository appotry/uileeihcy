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

	// ������ʽ 
	if( this->m_mapAttribute.count( XML_FONT ) )
	{
		m_hFont = ::UI_GetFont( mapAttrib[XML_FONT], GetGraphicsRenderType(GetHWND()));
		m_mapAttribute.erase(XML_FONT);
	}

	// ����Ĭ������
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
//	Ŀ�ģ���һ�����еĴ��ڽ��й������Ըô��ڽ������໯����
//
//	������
//		ID
//			[in]	UI������xml�е�ID�����ڼ����ӿؼ�
//
//		hWnd
//			[in]	Ҫ���й����Ĵ��ھ��
//	
//	���أ��ɹ�����true��ʧ�ܷ���false
//
//	��ע�����øú���������ǣ���һ�����ڣ�����������CDialog��������һ��UI��window�ࣩ��
//		  ����ȴ��ʹ�����ǵ�UI�ؼ������ʱ����Ҫ�������
//
bool WindowBase::CreateUI( const String& ID, HWND hWnd )
{
	this->m_hWnd = hWnd;
	this->m_strID = ID;

	//	�����ӿؼ�
	bool bRet = ::UI_LoadLayout( this );
	if( false == bRet )
	{
		UI_LOG_FATAL( _T("Window::Create, ���ش���(ID=%s)layoutʧ��"), this->m_strID.c_str() );
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
	//	Ϊ�˽��xp��win7�����һ�����ԣ�ֻ���ڰ���ALT��������TAB��֮�󣬲Ż���ʾ�ؼ���focus rect
	//	�ڳ�ʼ���������������������
	//	
	::PostMessage( m_hWnd, WM_CHANGEUISTATE, 
		MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);
	
	return true;
}
void WindowBase::DestroyUI()
{
	Panel::DestroyUI();

	UIASSERT( !IsWindow(m_hWnd) );          // ȷ�������ѱ�����
	UIASSERT( NULL == this->m_oldWndProc ); // ȷ���Ѿ�ȡ�����໯

	m_hWnd = NULL;
	if( NULL != m_hRenderTarget )
	{
		ReleaseHRDC(m_hRenderTarget);
		m_hRenderTarget = NULL;
	}
}

//
//	����һ���ؼ���֪ͨ��Ϊ�Լ�����������ؼ����¼�����ʱ�����¼�֪ͨ��
//
//	Parameter
//		idPath
//			[in]	Ҫ���õĿؼ�ID����window/panel/loginbtn����ôidPathӦΪ panel/loginbtn
//		nNotifyMapID
//			[in]	֪ͨ�¼��Ĵ���ID����ProcessMessage�ĵڶ�������
//
// void WindowBase::Control_NotifyMe( const String&  idPath, int nNotifyMapID )
// {
// 	this->Control_NotifyMe( __super::GetChildObject( idPath ), nNotifyMapID );
// }
void WindowBase::Control_NotifyMe( Object*  pObj, int nNotifyMapID )
{
	if( NULL == pObj )
	{
		UI_LOG_ERROR( _T("Window::Control_NotifyMe, ��Ч�Ĳ��� pObj ") );
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
//			[in] Ҫˢ���ĸ�����
//		bUpdateNow
//			[in] �Ƿ�����ˢ��
//
//	Return
//		NA
//
//	Remark
//		���������ͬʱ���Ǹ���WINDOW�����ˢ�·�ʽ������ֲ㴰�ں���ͨ�����ǲ�һ���ġ�
//		Ϊ�˷�ֹ��ͨ��������Ҫһ��ˢ�¶������ʱ����Ҫ��ε���InvalidateObj����ȥˢ�£�
//		������������һ���ֲ㴰�ڣ�ֻ�е�bUpdateNow = true��ʱ�򣬲�ȥ����
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
		// ϵͳ�ؼ���ˢ�������Լ�������
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
		// ϵͳ�ؼ���ˢ�������Լ�������
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
	//  �ύ��ʾ

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
	//  �ύ��ʾ

	int nX = roc.m_rcClip.left + roc.m_ptOffset.x;
	int nY = roc.m_rcClip.top  + roc.m_ptOffset.y;
	int nW = roc.m_rcClip.Width();
	int nH = roc.m_rcClip.Height();
	EndDraw(m_hRenderTarget, nX, nY, nW, nH, nX, nY);
}

//
//	������LISTBOX ReDrawItem��ֻˢ��һ���֣�����������object
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
//	Ҫ�ύ�������ϵ��������BeginDrawObjectʹ��
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
	// removed by libo 20120728 ������һ���յĴ��ڣ�����˵�����
// 	UIASSERT( ID != _T("") );
// 	if( _T("") == ID )
// 	{
// 		UI_LOG_FATAL( _T("Window::Create, δָ������id") );
// 		return false;
// 	}

	__super::m_strID = ID;   // ��ǰ��id��ֵ��������־���

	//	�������ھ��
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
		UI_LOG_FATAL( _T("Window::Create, CreateWindowExʧ��") );
		return false;
	}
	return true;
}

void WindowBase::EndDialog(INT_PTR nResult)
{
	UIASSERT( this->m_bDoModal );

	// ��ModalWindow::DoModal�����Ϣѭ���ܹ��˳�
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
// ����һ���յ�ģ̬�Ի���
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
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // ���ڵı�������
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
		::SetActiveWindow(hWndParent);  // ��������øú����������¸������ܵ���ĻZ����ĺ���ȥ��
	}
	::DestroyWindow( this->m_hWnd );    // ���ٴ���

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
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // ���ڵı�������
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
//                               ��Ϣӳ��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowBase::StartWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// ��ȡthisָ��
	WindowBase* pThis = (WindowBase*)UI_ExtractCreateWndData();
	UIASSERT(NULL != pThis);
	
	if( NULL == pThis )
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, true);
}
//
//	[static] Dialog���ʹ��ڵĵ�һ��������Ϣ���õĴ��ڹ���
//
LRESULT CALLBACK WindowBase::StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// ��ȡthisָ��
	WindowBase* pThis = (WindowBase*)UI_ExtractCreateWndData();
	UIASSERT(NULL != pThis);

	if( NULL == pThis )
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, false);
//	return (UINT_PTR)FALSE;
}
//
//	��StartWindowProc/StartDialogProc���ã������ڹ���ת��Ϊ������һ������
//
LRESULT WindowBase::StartProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool bWindowOrDialog )
{
	// ���໯
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

	// �����µĴ��ڹ��� ThunkWndProc
	return pProc(hwnd, uMsg, wParam, lParam);
}

//
//	[static] LRESULT CALLBACK ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//	��ATL��thunk�滻���Ĵ��ڹ���
//
//	Parameter��
//		hwnd
//			[in]	�������ڱ��滻���ˣ������hwnd��thisָ��
//
//		uMsg,wParam,lParam
//			[in]	��Ϣ��Ϣ
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
//	���ڱ����໯��֮��Ĵ��ڹ���
//
LRESULT	WindowBase::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes;
	UIMSG*  pOldMsg = m_pCurMsg;
	BOOL bRet = this->ProcessWindowMessage( m_hWnd, uMsg, wParam, lParam, lRes, 0 );  // ����BEGIN_MSG_MAP��Ϣӳ���б�
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

		// ��������Ϣ��������ˣ�ֱ�ӷ��أ������ٵ��þɵĴ��ڹ�����
		if( this->ProcessMessage( &msg ) )
		{
			if( NULL == m_oldWndProc )
			{
				switch(msg.message)
				{
				case WM_INITDIALOG: 
					if( NULL != GetKeyboardMgr().GetFocusObject() )
						lRes = FALSE;  // ��ʹ���佹������
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
		// ע��Ϊʲô��������ֱ�ӵ���OnFinalMessage��ȴ��Ҫ�ټ�һ��״̬λ��
		// ��ΪWM_NCDESTROY������DestroyWindow api��������DestroyWindow api
		// ����λ���κ�һ����ǰ���ڵ���Ϣ��Ӧ�У���˵�pOldMsg==NULLʱ������ʾ
		// û����ϢǶ���ˣ��ڼ��һ��WINDOW_STYLE_DESTROYED��־���ɡ�
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
		return DefWindowProc(uMsg,wParam,lParam);  // ���λ�ڴ����ϵ�����windows�ؼ����棬������
	}  
	else if( hWnd == m_hWnd )  
	{
		if( LOWORD(lParam) != HTCLIENT )    // �������λ��system window�ı�Եʱ��Ӧ�õ���Ĭ�ϵĴ�����̡��������Լ�����һ��ģ̬�򣬷���HTERRORʱ
		{
			return DefWindowProc(uMsg,wParam,lParam);
		}

//      Removed 20120824 -- ��ʵ��windowless richeditʱ�����ܲ�����ʱ����setcursor�İ취��������
//      ѡ���ı�ʱ�����ʽһֱ��˸������޸ķ���Ϊ���ȷ��ͣ�Ȼ����WM_MOUSEMOVE�м�����hover obj
//      �����˱仯�Ļ��������ٷ���һ��WM_SETCURSOR
//
//		return TRUE;   // ����ֻ����MOUSEMOVE֮��ȷ����HOVER��PRESS����󣬲ſ���ȥSETCURSOR��-- x ����
		// �����MOUSEMOVE֮ǰ��SETCURSOR��Ϣ��Ҫ�����ԣ�ֻ������MOUSEMOVE��POST������        -- x ����
		// SETCURSOR��Ϣ������ϢwParamΪ0                                                     -- x ����
	}
	if( TRUE == this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam ) ) // ��������ǰhover object
	{
		return TRUE;
	}

	// �ô���ȥ���������Ϣ
	UIMSG  msg;
	msg.message = uMsg;
	msg.pObjMsgTo = this;
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	if( this->ProcessMessage( &msg ) )
		return msg.lRet;

	// ����û���˴���
	::SetCursor( ::LoadCursor(NULL,IDC_ARROW) );
	return TRUE;
}

// ����Ϣ����WM_PAINT�зַ���
// Ϊ��ʵ������˸�Ļ��ƣ����뽫���еĻ��ƴ������һ���ط�����Ҫô��WM_ERASEBKGND��Ҫô��WM_PAINT
// ��WM_ERASEBKGND����ÿ�ζ��ᴥ���ģ��ڴ������δ���ʱ���ָ����⣩����˿��ǽ����ƴ��붼����WM_PAINT�д���
// 
// ��� lParam == 0�����ʾ��ϵͳ�Լ�����������Ϣ������ϵͳ��Ϣ�ᵼ�º�WM_PAINT DC��һ�£��Ӷ�������˸
// ��˽�WM_ERASEBKGND��Ϣ����WM_PAINT���������Լ����������ҽ�lParam��Ϊ��0
LRESULT WindowBase::_OnEraseBkgnd( UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if( 0 == lParam )
	{
		bHandled = TRUE;
		return 1;   // ����Dialog���ͣ���Ҫ����1����ֹϵͳĬ�ϻ���
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
		//	�޸Ļ��屳��ͼƬ��С
		ResizeRenderTarget(m_hRenderTarget, LOWORD(lParam), HIWORD(lParam));

		//
		// (ֻ�����û���ʽ�޸Ĵ��ڴ�С����ק����󻯣�ʱ�����ܽ�width/height ��auto�޸�Ϊ����ֵcx/cy
		// ��������Ϊ ��һ����ʾ����ʱ������WM_SIZE ����ɴ���)  <-- ���ϣ���Ϊû������ nType==0 RESTORE �����
		//
		//	�ĳɺ�����Щ����ˣ�ֱ�Ӹ�ֵ����˻���ɴ��ڵ�width=auto/height=autoֻ�Ǵ��ڴ�������һ�̵������ˣ�֮��
		//  ֻҪ��Ӧ�� WM_SIZE���Ͳ�������auto��
		//

		// WM_SIZE�Ĳ����ǻ��ڿͻ�����С�ģ�UI_UpdateLayout��UI_UpdateTopWindowLayout��Ҫ�����������ڵĴ�С
		if( NULL != m_pLayout )  // ���ڸմ�������û�е���UI_LoadLayout
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
	//  ������һ����ʼ���Ļ��� (virtual)
	//  �п���m_strIDΪ�գ���������Դ��������ʱ��popupconotrolwindow��
	//	���û�н�OnInitWindow����CreateUI��ִ��
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

	return 1;    // ע������DialogĬ�ϵ�WM_SETFOCUS�Ὣ���������õ��Լ��ĵ�һ���ؼ����棬������������return 0����bHandled = FALSE
	             //     ������::SetFocus(m_hWnd)�Ľ��㵽����������ֱ������Լ����õ��ؼ����棬����SetFocus�ﲻ�����ǵı���
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
	BOOL bRet = __super::OnEraseBkgnd(hRDC);  // ���m_pEraseBkgndRenderû�д������������ϵͳ����
	if( FALSE == bRet )
	{
		if( NULL == m_oldWndProc )   // Dialog���ͣ�ֱ�����ϵͳ����ɫ
		{
			CRect rc;
			::GetClientRect(m_hWnd,&rc);
			FillRect( hRDC, &rc, ::GetSysColor(COLOR_BTNFACE) );
		}
		else                         // Window���ͣ�ֱ�ӵ���ϵͳ����
		{
			HDC hDC = GetHDC(hRDC);
			bRet = (BOOL)/*this->*/DefWindowProc(WM_ERASEBKGND, (WPARAM)hDC, 1 );  // ��ԭʼ��Ϣ��������
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
//  ������OnCreate/OnInitDialog���������ú��������Ǵ��ڻ��ǶԻ��򶼻ᱻ����
//	�������������ظú�����ʱ�򣬱���ͬʱȥ���ø���ĸú�����
//
void WindowBase::OnInitWindow( )
{
	// TODO ����DDX_DDV
}

//
//  ����Ӧ�괰�ڵ�WM_NCDESTROY�󣬻ᴥ���ú�����
//  ��Ҫ��������Ϣ��Ӧ�е���delete this; �������ٸú����е���
//
void WindowBase::OnFinalMessage()
{
	
}

//
//	�����������ͷ����ı�ʱ(�ֲ㴰��<->��ͨ����)�����´���render target
//
void WindowBase::ReCreateRenderTarget()
{
	if( NULL != m_hRenderTarget )     // ��Ҫ���´���һ�������´������͵�RenderTarget
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
//	[static] Window���ʹ��ڵĵ�һ��������Ϣ���õĴ��ڹ���
//
//
//	�����֪һ�����ڵ�client�����С����������ڵ�window�����С
//
//	����
//		sizeClient
//			[in]	�ô��ڵĿͻ�����С
//		pSizeWindow
//			[out]	���ش��ڵ�window��С
//
void WindowBase::CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow )
{
	if( NULL == pSizeWindow )
		return;

	CRect  tempRcWindow, tempRcClient;

	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// ͨ�� WM_NCCALCSIZE ��Ϣ����ϵͳ��������һ��window rect ��Ӧ��client rect�Ƕ���
	np.lppos = &wp;
	::GetWindowRect( this->m_hWnd, &np.rgrc[0] );
	tempRcWindow.CopyRect(&np.rgrc[0]);
	::SendMessage( this->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );
	tempRcClient.CopyRect(&np.rgrc[0] );

	pSizeWindow->cx = sizeClient.cx + tempRcWindow.Width() - tempRcClient.Width();
	pSizeWindow->cy = sizeClient.cy + tempRcWindow.Height() - tempRcClient.Height();
}

//
//	�����֪һ�����ڵ�window�����С����������ڵ�client�����С
//
//	����
//		rcWindow
//			[in]	�ô��ڵĿʹ�С
//		rcClient
//			[out]	���ش��ڵ�client��С��Ĭ��left=0,top=0����ֻ���ش��ڴ�С��
//
void WindowBase::CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient )
{
	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// ͨ�� WM_NCCALCSIZE ��Ϣ����ϵͳ��������һ��window rect ��Ӧ��client rect�Ƕ���
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