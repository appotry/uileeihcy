#include "stdafx.h"
#undef  new

WindowBase::WindowBase()
{
	this->m_hWnd          = NULL;
	this->m_oldWndProc    = NULL;
	this->m_pFont         = NULL;
	this->m_hMemDC        = NULL;
	this->m_hMemBitmap    = NULL;
	this->m_hOldBitmap    = NULL;

	this->m_bDoModal       = false;
	this->m_bEndModal      = false;
	this->m_lDoModalReturn = 0;

	m_nMinWidth = m_nMinHeight = NDEF;
	m_nMaxWidth = m_nMaxHeight = NDEF;

	this->m_MgrMouse.SetWindow( this );
	this->m_MgrMouse.SetKeyboardManager( &this->m_MgrKeyboard );
	this->m_MgrKeyboard.SetWindow( this );
}
WindowBase::~WindowBase()
{
}

void WindowBase::ResetAttribute()
{
	__super::ResetAttribute();

	if (NULL != m_pFont)
	{
		::UI_ReleaseFont(m_pFont);
		m_pFont = NULL;
	}

	m_nMinWidth = m_nMinHeight = NDEF;
	m_nMaxWidth = m_nMaxHeight = NDEF;
	this->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT);  // ȡ��Panel�����е�͸������
}
bool WindowBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Panel::SetAttribute( mapAttrib, bReload );
	if( false == bRet )
		return false;

	// ������ʽ 
	ATTRMAP::iterator iter = m_mapAttribute.find(XML_FONT);
	if (m_mapAttribute.end() != iter)
	{
		m_pFont = ::UI_GetFont(iter->second, GetGraphicsRenderType(GetHWND()));
		m_mapAttribute.erase(iter);
	}

	// ����Ĭ������
	if( NULL == m_pFont )
	{
		HFONT hFont = (HFONT)::SendMessage( m_hWnd, WM_GETFONT, 0,0 );
		if( NULL == hFont )
		{
			m_pFont = UI_GetDefaultFont(GetGraphicsRenderType(m_hWnd));
			if (NULL == m_pFont)
			{
				hFont = (HFONT)GetStockObject(SYSTEM_FONT);
			}
		}
		
		if (NULL == m_pFont && NULL != hFont)
		{
			UI_AttachFont(&m_pFont, hFont, GetGraphicsRenderType(m_hWnd));
		}
	}


	iter = m_mapAttribute.find(XML_WINDOW_MAX_WIDTH);
	if (m_mapAttribute.end() != iter)
	{
		m_nMaxWidth = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(iter);
	}
	iter = m_mapAttribute.find(XML_WINDOW_MAX_HEIGHT);
	if (m_mapAttribute.end() != iter)
	{
		m_nMaxHeight = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(iter);
	}
	iter = m_mapAttribute.find(XML_WINDOW_MIN_WIDHT);
	if (m_mapAttribute.end() != iter)
	{
		m_nMinWidth = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(iter);
	}
	iter = m_mapAttribute.find(XML_WINDOW_MIN_HEIGHT);
	if (m_mapAttribute.end() != iter)
	{
		m_nMinHeight = _ttoi(iter->second.c_str());
		m_mapAttribute.erase(iter);
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

	if (!m_strID.empty())   // ����themetooltip
	{
		//	�����ӿؼ�
		bool bRet = ::UI_LoadLayout( this );
		if( false == bRet )
		{
			UI_LOG_FATAL( _T("Window::Create, ���ش���(ID=%s)layoutʧ��"), this->m_strID.c_str() );
			return false;
		}

		if( !IsChildWindow() )
		{
			::UI_UpdateTopWindowLayout( this );		
		}
		else
		{
			::UI_UpdateLayout(this);
		}
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
// 	if( NULL != m_hRenderTarget )
// 	{
// 		ReleaseHRDC(m_hRenderTarget);
// 		m_hRenderTarget = NULL;
// 	}
	this->DestroyDoubleBuffer();
	if (NULL != m_pFont)
	{
		::UI_ReleaseFont(m_pFont);
		m_pFont = NULL;
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
//		LayeredWindow::UpdateObject
//
void WindowBase::RedrawObject( Object* pRedrawObj, RECT* prc, bool bUpdateNow )
{
	if (NULL == pRedrawObj)
		return;
 
	if (OBJ_HWNDHOST == pRedrawObj->GetObjectType())
	{
		// ϵͳ�ؼ���ˢ�������Լ�������
	}
	else if (OBJ_WINDOW == pRedrawObj->GetObjectType())
	{
		WindowBase* pWindow = (WindowBase*)pRedrawObj;
		::InvalidateRect(pWindow->m_hWnd, prc, TRUE);
		if (bUpdateNow)
		{
			UpdateWindow(pWindow->m_hWnd);
		}
	}
	else
	{
		this->_InnerRedrawObject(pRedrawObj, NULL);
	}
}

void WindowBase::RedrawObjectBkgnd( Object* pInvalidateObj, RECT* prc, bool bUpdateNow )
{
#if 0 // TODO: RenderEngine Modify
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
#endif
}

void WindowBase::_InnerRedrawObject(Object* pInvalidateObj, HDC hDestDC)
{
	IRenderTarget*  pRenderTarget = CreateRenderTarget(m_hWnd);

	RenderOffsetClipHelper roc(this, false);
	pInvalidateObj->GetWindowRect(&roc.m_rcClip);

	if (pRenderTarget->BeginDraw(m_hMemDC, roc.m_rcClip))
	{
		if (this->IsTransparent())
		{
			pRenderTarget->Clear();
		}

		pInvalidateObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, pInvalidateObj->IsTransparent());
		pInvalidateObj->DrawObject(pRenderTarget, roc);
	
		pRenderTarget->EndDraw();
		this->CommitDoubleBuffet2Window(NULL, &roc.m_rcClip);
	}
	SAFE_RELEASE(pRenderTarget);
}

void WindowBase::_InnerRedrawObjectBkgnd(Object* pInvalidateObj, HDC hDestDC)
{
#if 0 // TODO: RenderEngine Modify
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
	EndDraw(m_hRenderTarget, nX, nY, nW, nH, nX, nY, true);
#endif
}

// ��ȡһ���ؼ��ڴ����ϵ�ͼ��
HBITMAP WindowBase::PaintObject(Object* pObj)
{
	if (NULL == pObj || NULL == m_hMemDC)
		return NULL;

	pObj->UpdateObject(true);

	CRect rcWindow;
	pObj->GetClientRectInWindow(&rcWindow);
	
	::SelectObject(m_hMemDC, m_hOldBitmap);
	Image image;
	image.Attach(m_hMemBitmap);
	HBITMAP hBitmap = image.CopyRect(&rcWindow);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, image.Detach());

	return hBitmap;
}

//
//	������LISTBOX ReDrawItem��ֻˢ��һ���֣�����������object
//
HRDC WindowBase::BeginRedrawObjectPart(Object* pRedrawObj, RECT* prc1, RECT* prc2)
{
	if (NULL == pRedrawObj)
		return NULL;

	IRenderTarget*  pRenderTarget = CreateRenderTarget(m_hWnd);

	RenderOffsetClipHelper roc(this, false);
	if (pRenderTarget->BeginDraw(m_hMemDC, prc1, prc2))
	{
		pRedrawObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, /*true*/pRedrawObj->IsTransparent());

		::UISendMessage(pRedrawObj, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)1);

		roc.DrawClient(pRenderTarget, pRedrawObj, false);
		roc.Scroll(pRenderTarget, pRedrawObj, false);
		roc.Update(pRenderTarget);

		return pRenderTarget;
	}

	SAFE_DELETE(pRenderTarget);
	return NULL;
}

//
//	Ҫ�ύ�������ϵ��������BeginDrawObjectʹ��
//  ����Ҫ�ύ���Rectʱ���Ƚ�bFinish����Ϊfalse�����һ������Ϊtrue�ͷ���Դ
//
void WindowBase::EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prc1, RECT* prc2)
{
	pRenderTarget->EndDraw();
	SAFE_RELEASE(pRenderTarget);

	if (NULL != prc1)
	{
		this->CommitDoubleBuffet2Window(NULL, prc1);
	}
	if (NULL != prc2)
	{
		this->CommitDoubleBuffet2Window(NULL, prc2);
	}
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

	cs.style     = WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
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
long WindowBase::DoModal(  const String& ID, HWND hWndParent )
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
		if (FALSE == UI_IsDialogMessage(&msg))
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

BOOL WindowBase::PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
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

	if (NULL != g_pUIApplication)
	{
		m_hWnd = CreateDialogIndirect(
					g_pUIApplication->GetModuleInstance(), 
					(LPDLGTEMPLATE)hgbl, 
					hWndParent, 
					(DLGPROC)WindowBase::StartDialogProc); 
	}
	if (NULL != m_hWnd)
	{
		// fixĬ�ϵ�#32770���ڹ����ڴ�С�ı�ʱ��ˢ�µ�����
		// TODO:?? Ϊʲô��THICKFRAME�Ĵ��ھ����Լ�ˢ��
		SetClassLong(m_hWnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS);
	}
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
	LRESULT lRet = 0;

	if (FALSE == pThis->PreTranslateMessage(uMsg, wParam, lParam, &lRet))
		lRet = pThis->WndProc( uMsg, wParam, lParam );

	return lRet;
}

LRESULT WindowBase::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_oldWndProc )
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}
	else
	{
		if (WM_INPUTLANGCHANGEREQUEST == uMsg) // ???? ʲôԭ�� �ڶԻ����У����û��һ��EDIT���л����뷨û�з�Ӧ
		{                                      // ����Ϊû�н������Ϣ���ݸ�DefWindowProc
			return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}
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
		this->ModifyStyle(0,WINDOW_STYLE_DESTROYED);
		this->OnFinalMessage();
	}

	return lRes;
}

LRESULT WindowBase::_OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	if (0 == HIWORD(lParam))  // is menu popup
	{ 
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		return TRUE;
	}

	HWND hWnd = (HWND)wParam;
	if (0 != wParam && hWnd != m_hWnd)
	{
		return DefWindowProc(uMsg,wParam,lParam);  // ���λ�ڴ����ϵ�����windows�ؼ����棬������
	}  
	else if (hWnd == m_hWnd)  
	{
		if (LOWORD(lParam) != HTCLIENT)    // �������λ��system window�ı�Եʱ��Ӧ�õ���Ĭ�ϵĴ�����̡��������Լ�����һ��ģ̬�򣬷���HTERRORʱ
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
	if (TRUE == this->m_MgrMouse.HandleMessage(uMsg, wParam, lParam)) // ��������ǰhover object
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
	PAINTSTRUCT ps;
	HDC  hDC = NULL;

	if (NULL == wParam)
		hDC = ::BeginPaint(this->m_hWnd ,&ps);

	if (NULL == m_hMemDC)
	{
		if( NULL == wParam )
			EndPaint(m_hWnd,&ps);
		return 0;
	}
#if 0
	IRenderTarget* pRenderTarget = CreateRenderTarget(m_hWnd);
	if (pRenderTarget->BeginDraw(m_hMemDC, NULL))
	{
		this->OnDrawWindow(pRenderTarget);
		pRenderTarget->EndDraw();
	}
	pRenderTarget->Release();
#else

	HBITMAP hCurMemBitmap = (HBITMAP)::GetCurrentObject(m_hMemDC, OBJ_BITMAP);
	{
		BITMAP  bm;
		::GetObject(m_hMemBitmap, sizeof(bm), &bm);
		int a = 0;
	}

	DIBSECTION  dibSection;
	GetObject(hCurMemBitmap, sizeof(DIBSECTION), &dibSection);
	BYTE* pBits = (BYTE*)dibSection.dsBm.bmBits;
	pBits += (dibSection.dsBm.bmHeight-1)*dibSection.dsBm.bmWidthBytes;  // ��ָ���Ƶ���һ������λ��

	Gdiplus::Bitmap* m_pGdiplusMemBitmap = new Gdiplus::Bitmap(dibSection.dsBm.bmWidth, dibSection.dsBm.bmHeight, -dibSection.dsBm.bmWidthBytes, PixelFormat32bppARGB, (BYTE*)pBits);
	Gdiplus::Graphics* m_pGraphics = new Gdiplus::Graphics(m_pGdiplusMemBitmap);  // ʹ���ڴ�ͼƬ����������graphics���ٵ���gethdc��������HDCʹ��alphaͨ��

	HDC m_hGdiplusDC = m_pGraphics->GetHDC();
	SetBkMode(m_hGdiplusDC, TRANSPARENT);
	{
		HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(m_hGdiplusDC, OBJ_BITMAP);
		BITMAP  bm;
		::GetObject(hBitmap, sizeof(bm), &bm);
		int* p =(int* )bm.bmBits;
		*p = 0x000D0B0D;
		p++;
		*p = 0;

		int a = 0;
	}

// 	Image  image;
// 	image.Load(_T("E:\\���\\workingpath\\com.google.code\\trunk\\ui_dll_version\\UI_SDK\\bin\\mediaplay\\menu_check_bk.png"));
// 	image.Draw(m_hGdiplusDC, 0,0 );

// 	CRect rc; 
// 	::GetClientRect(m_hWnd, &rc);
// 	HBRUSH hBrush = ::CreateSolidBrush(0x000C0B0D);
// 	::FillRect(m_hGdiplusDC, &rc, hBrush/*(HBRUSH)::GetStockObject(WHITE_BRUSH)*/);
// 	::DeleteObject(hBrush);
// 	m_pGraphics->ReleaseHDC(m_hGdiplusDC);
// 	m_hGdiplusDC = NULL;
// 
// 	Image  image;
// 	image.Load(_T("D:\\menu_check_bk.png"));
// 	image.Draw(m_hGdiplusDC, 0,0);

	TextOut(m_hGdiplusDC, 0,0, L"a", 1);
	m_pGraphics->ReleaseHDC(m_hGdiplusDC);
	m_hGdiplusDC = NULL;

	SAFE_DELETE(m_pGdiplusMemBitmap);
	SAFE_DELETE(m_pGraphics);

#endif
	this->CommitDoubleBuffet2Window(hDC, NULL);

	if( NULL == wParam )
		EndPaint(m_hWnd,&ps);
	
	return 0;
}

//
// ����DrawObject���ܶ���OnEndErasebkgnd����˽�DrawObject��дһ��ΪOnDrawWindow
// ��������һЩ���������ʱҲ������Ҫ���ƣ���ֲ㴰��...)
// TODO:  �ú����ı�Ҫ��
//
void WindowBase::OnDrawWindow(IRenderTarget* pRenderTarget)
{
	RenderOffsetClipHelper roc(this);

	if (this->IsVisible())
	{
		::UISendMessage(this, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)1 );  // ��lparam��Ϊ1����ԭʼ��Ϣ��������
		this->OnEndErasebkgnd();       // ��Ҫ������CustomWindow�����ô�������

		this->_drawNcChildObject(pRenderTarget, roc);

		roc.DrawClient(pRenderTarget, this, false);
		roc.Scroll(pRenderTarget, this, false);
		roc.Update(pRenderTarget);

		::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)1 );       // ��lparam��Ϊ1����ԭʼ��Ϣ��������
		this->_drawChildObject(pRenderTarget, roc);
	}
}

LRESULT WindowBase::_OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (SIZE_MINIMIZED != wParam)
	{
		this->CreateDoubleBuffer(LOWORD(lParam), HIWORD(lParam));

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

	this->CreateUI(m_strID, m_hWnd);

	//
	//  �п���m_strIDΪ�գ���������Դ��������ʱ��popupconotrolwindow��
	//	���û�н�AddTopWindowObject��OnInitWindow����CreateUI��ִ��
	//
	if (!IsChildWindow())
	{
		g_pUIApplication->m_TopWindowMgr.AddTopWindowObject( this );
	}

	//
	//  ������һ����ʼ���Ļ��� (virtual)
	//
	this->OnInitWindow();

	return 0;
}
LRESULT WindowBase::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	SyncWindowHelper<WindowBase>::_OnNcDestroy();

	if (!IsChildWindow())
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
	bHandled = FALSE;
// 	if (uMsg == WM_SYSKEYDOWN && wParam == VK_F4) // �ؼ����������Ϣ��ֱ�ӹرմ���
// 		return 0;

	LRESULT bRet = this->m_MgrKeyboard.HandleMessage( uMsg, wParam, lParam );
	if (bRet)
	{
		bHandled = TRUE;
	}
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

LRESULT WindowBase::_OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	MINMAXINFO* pInfo = (MINMAXINFO*)lParam;

	pInfo->ptMaxPosition.x = -m_rcBorder.left;
	pInfo->ptMaxPosition.y = -m_rcBorder.top;

	CRect  rcWorkArea;
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWorkArea, NULL);

	if (NDEF != m_nMaxWidth)
		pInfo->ptMaxSize.x = pInfo->ptMaxTrackSize.x = m_nMaxWidth;
	else
		pInfo->ptMaxSize.x = pInfo->ptMaxTrackSize.x = rcWorkArea.Width() +m_rcBorder.left+m_rcBorder.right;

	if (NDEF != m_nMaxHeight)
		pInfo->ptMaxSize.y = pInfo->ptMaxTrackSize.y = m_nMaxHeight;
	else
		pInfo->ptMaxSize.y = pInfo->ptMaxTrackSize.y = rcWorkArea.Height() +m_rcBorder.top+m_rcBorder.bottom;

	

	if (NDEF != m_nMinWidth)
	{
		pInfo->ptMinTrackSize.x = m_nMinWidth;
	}
	if (NDEF != m_nMinHeight)
	{
		pInfo->ptMinTrackSize.y = m_nMinHeight;
	}
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
	switch(wParam)
	{
	case ADD_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
			this->AddAnchorItem(data);
		}
		break;
	case MODIFY_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
			this->ModifyAnchorItem(data);
		}
		break;
	case REMOVE_SYNC_WINDOW:
		{
			const SyncWindowData& data = *(SyncWindowData*)(lParam);
			this->RemoveAnchorItem(data.m_hWnd);
		}
		break;
	case ADD_SYNC_WINDOW_RESULT:
		{
			this->OnAddAnchorItem((HWND)lParam);
		}
		break;
	case MODIFY_SYNC_WINDOW_RESULT:
		{
			this->OnModifyAnchorItem((HWND)lParam);
		}
		break;
	case REMOVE_SYNC_WINDOW_RESULT:
		{
			this->OnRemoveAnchorItem((HWND)lParam);
		}
		break;
	case HOST_WINDOW_DESTROYED:
		{
			this->OnRemoveAnchorItem((HWND)lParam);
		}
		break;
	case HOST_WINDOW_POSCHANGING:
		{
			this->OnHostWindowPosChanging();
		}
		break;
	}
	return 0;
}
LRESULT WindowBase::_OnEnterSizeMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	SyncWindowHelper<WindowBase>::_OnEnterSizeMove();
	return 0;
}
LRESULT WindowBase::_OnExitSizeMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	SyncWindowHelper<WindowBase>::_OnExitSizeMove();
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

LRESULT WindowBase::OnGetGraphicsRenderType()
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
#if 0 // TODO: RenderEngine Modify
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
#endif
}

//
//	����˫��������
//
void WindowBase::CreateDoubleBuffer(int nWidth, int nHeight)
{
	this->DestroyDoubleBuffer();

	m_hMemDC = ::CreateCompatibleDC(NULL);
	::SetBkMode(m_hMemDC, TRANSPARENT);

	Image image;
	image.Create( nWidth, nHeight, 32, Image::createAlphaChannel );
	m_hMemBitmap = image.Detach();
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hMemBitmap);
}
//
//	�ͷ�˫��������
//
void WindowBase::DestroyDoubleBuffer()
{
	if( NULL != m_hMemDC )
	{
		::SelectObject(m_hMemDC, m_hOldBitmap);
		::DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}

	SAFE_DELETE_GDIOBJECT(m_hMemBitmap);
}

//
//	��˫���������ύ������DC��
//
void WindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit)
{
	HDC hDC = hDCWnd;
	if (NULL == hDC)
	{
		hDC = GetDC(m_hWnd);
	}

	CRect rcCommit;
	if (NULL == prcCommit)
		::GetClientRect(m_hWnd, &rcCommit);
	else
		rcCommit.CopyRect(prcCommit);

	::BitBlt(hDC, rcCommit.left, rcCommit.top, rcCommit.Width(), rcCommit.Height(),
			 m_hMemDC, rcCommit.left, rcCommit.top, SRCCOPY);

	if (NULL == hDCWnd)
	{
		ReleaseDC(m_hWnd, hDC);
	}
}

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
	return m_pFont;
}