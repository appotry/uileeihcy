#include "stdafx.h"
#include "windowbase.h"
#include <Uxtheme.h>

#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include "UISDK\Kernel\Src\Helper\topwindow\topwindowmanager.h"

namespace UI
{

WindowBase::WindowBase()
{
    m_pIWindowBase = NULL;
	this->m_hWnd = NULL;
	this->m_oldWndProc = NULL;
	this->m_pDefaultFont  = NULL;

	m_bFirsetEraseBkgnd = true;
	this->m_bDoModal = false;
	this->m_bEndModal = false;
	this->m_lDoModalReturn = 0;

	m_nMinWidth = m_nMinHeight = NDEF;
	m_nMaxWidth = m_nMaxHeight = NDEF;
    m_pRenderChain = NULL;
}
WindowBase::~WindowBase()
{
	UIASSERT( !IsWindow(m_hWnd) );          // 确保窗口已被销毁
	UIASSERT( NULL == this->m_oldWndProc ); // 确保已经取消子类化
	m_hWnd = NULL;

	SAFE_RELEASE(m_pDefaultFont);

    delete m_pRenderChain->GetIRenderChain();
    m_pRenderChain = NULL;
}

HRESULT  WindowBase::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = __super::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    this->m_MgrMouse.SetUIApplication(p, this);
    this->m_MgrCWBL.SetWindowPtr(this);
    this->m_MgrDragDrop.SetWindowBase(this);

    IRenderChain*  pIRenderChain = new IRenderChain;
    m_pRenderChain = pIRenderChain->GetImpl();
    m_pRenderChain->Init(this);

    return S_OK;
}

void WindowBase::ResetAttribute()
{
    Panel::ResetAttribute();
	SAFE_RELEASE(m_pDefaultFont);

	m_nMinWidth = m_nMinHeight = NDEF;
	m_nMaxWidth = m_nMaxHeight = NDEF;
	this->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT, false);  // 取消Panel基本中的透明属性
}
void WindowBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	Panel::SetAttribute(pMapAttrib, bReload);

	IFontRes* pFontRes = GetUIApplication()->GetActiveSkinFontRes();
	if (NULL == pFontRes)
		return;

	// 字体样式 
    const TCHAR* szText = pMapAttrib->GetAttr(XML_FONT, true);
	if (szText)
		pFontRes->GetFont((BSTR)szText, GetRenderLibraryType(m_pIWindowBase), &m_pDefaultFont);

	// 创建默认字体
	if (NULL == m_pDefaultFont)
	{
        // GDI 窗口字体
		HFONT hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0,0);
		if (hFont)
        {
            UI_AttachFont(&m_pDefaultFont, hFont, GetRenderLibraryType(m_hWnd));
        }
        else
		{
            // UI Font Res Defualt Font
			pFontRes->GetDefaultFont(GetRenderLibraryType(m_hWnd), &m_pDefaultFont);
			if (NULL == m_pDefaultFont)
			{
                // System Default Font
				hFont = (HFONT)GetStockObject(SYSTEM_FONT);
                UI_AttachFont(&m_pDefaultFont, hFont, GetRenderLibraryType(m_hWnd));
			}
		}
	}

    pMapAttrib->GetAttr_int(XML_WINDOW_MAX_WIDTH,  true, &m_nMaxWidth);
    pMapAttrib->GetAttr_int(XML_WINDOW_MAX_HEIGHT, true, &m_nMaxHeight);
    pMapAttrib->GetAttr_int(XML_WINDOW_MIN_WIDHT,  true, &m_nMinWidth);
    pMapAttrib->GetAttr_int(XML_WINDOW_MIN_HEIGHT, true, &m_nMinHeight);

    if (false == bReload)
    {
        szText = pMapAttrib->GetAttr(XML_TEXT, true);;
        if (szText)
        {
            ::SetWindowText(m_hWnd, szText);
        }
        else if (m_pUIApplication->IsDesignMode())
        {
            ::SetWindowText(m_hWnd, _T(""));
        }
    }
}
void  WindowBase::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    __super::OnEditorGetAttrList(pData);

    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    IUIEditorGroupAttribute*  pWindowBaseGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, WindowBase::GetXmlName(), NULL);
    pEditor->CreateTextAttribute(pWindowBaseGroup, XML_FONT, szPrefix);
	pEditor->CreateTextAttribute(pWindowBaseGroup, XML_WINDOW_MAX_WIDTH, szPrefix);
	pEditor->CreateTextAttribute(pWindowBaseGroup, XML_WINDOW_MAX_HEIGHT, szPrefix);
	pEditor->CreateTextAttribute(pWindowBaseGroup, XML_WINDOW_MIN_WIDHT, szPrefix);
	pEditor->CreateTextAttribute(pWindowBaseGroup, XML_WINDOW_MIN_HEIGHT, szPrefix);
    pEditor->CreateTextAttribute(pWindowBaseGroup, XML_TEXT, szPrefix);
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
bool WindowBase::CreateUI(const TCHAR* szID, HWND hWnd)
{
	this->m_hWnd = hWnd;

	if (szID && _tcslen(szID)>0)   
	{
		//	加载子控件
		ILayoutManager* pLayoutMgr = m_pUIApplication->GetActiveSkinLayoutMgr();
		if (NULL == pLayoutMgr)
		{
			UI_LOG_ERROR(_T("%s GetLayoutManager Failed."), FUNC_NAME);
			return false;
		}

		if (FAILED(pLayoutMgr->LoadRootLayout(m_pIObject, szID)))
		{
			UI_LOG_FATAL(_T("%s, 加载窗口(ID=%s)layout失败"), FUNC_NAME, szID);
			return false;
		}
	}

	//
	//	为了解决xp、win7上面的一个特性：只有在按了ALT键，或者TAB键之后，才会显示控件的focus rect
	//	在初始化后，主动将该特性清除。
	//	
	// ::PostMessage(m_hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);
	
	// 为了实现列表框，树控件的theme绘制，否则画出来的效果不正确，就是一个黑边
	// http://msdn.microsoft.com/zh-cn/library/windows/desktop/bb759827(v=vs.85).aspx
	// http://www.codeproject.com/Articles/18858/Fully-themed-Windows-Vista-Controls
	// http://stackoverflow.com/questions/14039984/which-class-part-and-state-is-used-to-draw-selection

	// The following example code gives a list-view control the appearance of a Windows Explorer list: 
	SetWindowTheme(m_hWnd, L"explorer", NULL);

	return true;
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

	pObj->SetNotify(this->GetIMessage(), nNotifyMapID);
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
//		LayeredWindow::UpdateObject
//


// 获取一个控件在窗口上的图像
// 注：有可能一个对象是隐藏的，或者不在窗口
// HBITMAP WindowBase::PaintObject(Object* pObj)
// {
// 	if (NULL == pObj || NULL == GetRenderChainMemDC())
// 		return NULL;
// 
// 	pObj->UpdateObject(false);
// 
// 	CRect rcWindow;
// 	pObj->GetClientRectInWindow(&rcWindow);
// 	
// 	Image image;
// 	image.Create(rcWindow.Width(), rcWindow.Height(), 32, Image::createAlphaChannel);
// 	HDC hDC = image.BeginDrawToMyself();
// 	::BitBlt(hDC, 0,0, rcWindow.Width(), rcWindow.Height(), GetRenderChainMemDC(), rcWindow.left, rcWindow.top, SRCCOPY);
// 	image.EndDrawToMyself();
// 	return image.Detach();
// }
void  WindowBase::PaintWindow(HDC hDC)
{
    m_pRenderChain->OnWindowPaint(hDC);    
}

bool WindowBase::Create(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent)
{
	// removed by libo 20120728 允许创建一个空的窗口，例如菜单窗口
// 	UIASSERT( ID != _T("") );
// 	if( _T("") == ID )
// 	{
// 		UI_LOG_FATAL( _T("Window::Create, 未指定窗口id") );
// 		return false;
// 	}
	
	this->SetUIApplication(pUIApp);
    if (szID)
	__super::m_strID = szID;   // 提前给id赋值，便于日志输出

	//	创建窗口句柄
	CREATESTRUCT cs;
	::ZeroMemory(&cs, sizeof(CREATESTRUCT));
	DWORD  dwStyleEx = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;

    cs.hwndParent = hWndParent;
	cs.style     = WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
	cs.lpszClass = WND_CLASS_NAME;
	cs.lpszName  = _T("");
	cs.x = cs.y  = 0;
	cs.cx = cs.cy = 1; //CW_USEDEFAULT;

    UISendMessage(this, UI_WM_PRECREATEWINDOW, (WPARAM)&cs);
//	this->PreCreateWindow(cs);

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, this);
	this->m_hWnd = ::CreateWindowEx( 
		cs.dwExStyle,
		cs.lpszClass, cs.lpszName, cs.style,
		cs.x, cs.y, cs.cx, cs.cy,
		hWndParent, 0, NULL, NULL );

	if (NULL == m_hWnd)
	{
		UI_LOG_FATAL( _T("%s, CreateWindowEx失败"), FUNC_NAME);
		return false;
	}
	return true;
}

void WindowBase::Attach(IUIApplication* pUIApp, HWND hWnd, const TCHAR* szID)
{
	if (m_hWnd)
		return;

	this->SetUIApplication(pUIApp);

    if (szID)
	    m_strID = szID;   // 提前给id赋值，便于日志输出
    else
        m_strID.clear();

	m_hWnd = hWnd;

	this->ModifyStyle(WINDOW_STYLE_ATTACH, 0, 0);
	this->m_thunk.Init( &WindowBase::ThunkWndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();

	this->m_oldWndProc = (WNDPROC)::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);

	BOOL bHandled = FALSE;
	this->_OnCreate(0,0,0,bHandled);

	return;
}

void  WindowBase::Detach()
{
	if (NULL == m_hWnd)
		return;

	BOOL bHandled = FALSE;
	_OnNcDestroy(WM_NCDESTROY, 0, 0, bHandled);
	m_hWnd = NULL;
}

void WindowBase::EndDialog(INT_PTR nResult)
{
	UIASSERT( this->m_bDoModal );

	// 让ModalWindow::DoModal里的消息循环能够退出
	this->m_lDoModalReturn = (long)nResult ;
	this->m_bEndModal = true;
	::PostMessage( this->m_hWnd, WM_NULL, 0,0 );
}
long WindowBase::DoModal(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndParent )
{
#if 0
	UIASSERT( NULL == m_hWnd );

	UI_AddCreateWndData(&m_thunk.cd, (void*)this);
	m_strID = ID;

	return DialogBox(hResInst, MAKEINTRESOURCE(nResID), hWndParent, (DLGPROC)WindowBase::StartDialogProc);
#endif

	HWND hWnd = this->DoModeless(pUIApp, hResInst, nResID, szID, hWndParent);
	if( NULL == hWnd )
	{
		return -1;
	}

	return this->ModalLoop(hWndParent);
}


//
// 创建一个空的模态对话框
//
long WindowBase::DoModal(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndParent )
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

	long lRet = DialogBoxIndirect(/*m_pUIApplication->GetModuleInstance()*/g_hInstance, 
		(LPDLGTEMPLATE)hgbl, 
		hWndParent, 
		(DLGPROC)WindowBase::StartDialogProc); 

	GlobalFree(hgbl); 
	return lRet;
#endif
	HWND hWnd = this->DoModeless(pUIApp, szID, hWndParent);
	if( NULL == hWnd )
	{
		return -1;
	}

	return this->ModalLoop(hWndParent);
}

long WindowBase::ModalLoop(HWND hWndParent)
{
    // 给外部一个设置模态窗口显示的机会
    UIMSG  msg;
    msg.message = UI_WM_SHOWMODALWINDOW;
    msg.pMsgFrom = msg.pMsgTo = m_pIWindowBase;
    LRESULT lRet = UISendMessage(&msg);

    if(msg.bHandled && lRet)
    {

    }
    else
    {
    	this->ShowWindow();
	    ::UpdateWindow(this->m_hWnd);
    }
	
	this->m_bDoModal = true;
	bool bEnableWindow = false;
	if (hWndParent && GetDesktopWindow() != hWndParent && IsWindowEnabled(hWndParent))
	{
		::EnableWindow( hWndParent, FALSE );
		bEnableWindow = true;
	}

#if 0
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (FALSE == m_pUIApplication->IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (this->m_bEndModal)
		{
			this->m_bEndModal = false;
			break;
		}
	}
#else
	m_pUIApplication->MsgHandleLoop(&m_bEndModal);
	m_bEndModal = false;  // 还原，用于下次DoModal

#endif
	// hide the window before enabling the parent, etc.
	if (m_hWnd != NULL)
	{
		SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}

	if (bEnableWindow)
	{
		::EnableWindow( hWndParent, TRUE );
	}
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
	{
		::SetActiveWindow(hWndParent);  // 如果不调用该函数，将导致父窗口跑到屏幕Z次序的后面去了
	}
	::DestroyWindow(this->m_hWnd);    // 销毁窗口

	this->m_bDoModal = false;
	return this->m_lDoModalReturn;
}

BOOL WindowBase::PreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
	return FALSE;
}

HWND WindowBase::DoModeless(IUIApplication* pUIApp, const TCHAR* szID, HWND hWndOnwer)
{
	UIASSERT(NULL == m_hWnd);
	this->SetUIApplication(pUIApp);

	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(CREATESTRUCT));
	cs.style        = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
	cs.lpszClass    = WND_CLASS_NAME;
	cs.lpszName     = _T("");
	cs.x  = cs.y    = 0;
	cs.cx = cs.cy   = 100;//CW_USEDEFAULT;
	cs.dwExStyle    = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE;
    UISendMessage(this, UI_WM_PRECREATEWINDOW, (WPARAM)&cs);

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

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, this);
    if (szID)
	m_strID = szID;

	if (m_pUIApplication)
	{
		m_hWnd = CreateDialogIndirect(
					/*UIApplication::GetModuleInstance()*/g_hInstance, 
					(LPDLGTEMPLATE)hgbl, 
					hWndOnwer, 
					(DLGPROC)WindowBase::StartDialogProc); 
	}
	if (m_hWnd)
	{
		// fix默认的#32770窗口过程在大小改变时不刷新的问题
		// TODO:?? 为什么带THICKFRAME的窗口就能自己刷新
		SetClassLong(m_hWnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS);
	}
	GlobalFree(hgbl); 
	return m_hWnd;
}
HWND WindowBase::DoModeless(IUIApplication* pUIApp, HINSTANCE hResInst, UINT nResID, const TCHAR* szID, HWND hWndOnwer )
{
	UIASSERT( NULL == m_hWnd );
	this->SetUIApplication(pUIApp);

	s_create_wnd_data.AddCreateWndData(&m_thunk.cd, (void*)this);
    if (szID)
	m_strID = szID;

    // 注： hWndParent仅是Owner，并不是parent
	m_hWnd = CreateDialog(hResInst, MAKEINTRESOURCE(nResID), hWndOnwer, (DLGPROC)WindowBase::StartDialogProc);
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
	WindowBase* pThis = (WindowBase*)s_create_wnd_data.ExtractCreateWndData();
	UIASSERT(NULL != pThis);
	
	if (NULL == pThis)
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, true);
}
//
//	[static] Dialog类型窗口的第一个窗口消息调用的窗口过程
//
LRESULT CALLBACK WindowBase::StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 获取this指针
	WindowBase* pThis = (WindowBase*)s_create_wnd_data.ExtractCreateWndData();
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

	this->m_thunk.Init( &WindowBase::ThunkWndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();

	if (bWindowOrDialog)
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
	WindowBase* pThis = (WindowBase*)hwnd;
	LRESULT lRet = 0;

	if (FALSE == pThis->PreTranslateMessage(uMsg, wParam, lParam, &lRet))
		lRet = pThis->WndProc( uMsg, wParam, lParam );

	return lRet;
}

LRESULT WindowBase::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (m_oldWndProc)
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
	BOOL bHandled = this->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lRes, 0);  // 调用BEGIN_MSG_MAP消息映射列表
	if (bHandled)
	{
		return WndProc_GetRetValue(uMsg, wParam, lParam, bHandled, lRes);
	}
    
    // 直接发给当前窗口处理
    UIMSG  msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam = wParam;
	msg.lParam = lParam;

	// 如果这个消息被处理过了，直接返回，不用再调用旧的窗口过程了
	//if (static_cast<IMessage*>(this)->ProcessMessage(&msg, 0))
    UISendMessage(&msg);
    lRes = WndProc_GetRetValue(uMsg, wParam, lParam,  msg.bHandled, msg.lRet);

	if (uMsg == WM_NCDESTROY)
	{
		// 注：为什么不在这里直接调用OnFinalMessage，却还要再加一个状态位？
		// 因为WM_NCDESTROY函数由DestroyWindow api触发，而DestroyWindow api
		// 可能位于任何一个当前窗口的消息响应中，因此当pOldMsg==NULL时，即表示
		// 没有消息嵌套了，在检查一次WINDOW_STYLE_DESTROYED标志即可。
		this->ModifyStyle(WINDOW_STYLE_DESTROYED, 0, 0);
	}
	if (m_nStyle & WINDOW_STYLE_DESTROYED && pOldMsg == NULL)
	{
		this->ModifyStyle(0, WINDOW_STYLE_DESTROYED, 0);
#if 0 // -- 架构改造
		this->OnFinalMessage();
#endif
	}

	return lRes;
}

// 设置对话框的DialogProc返回值，见MSDN中对DialogProc返回值的说明
LRESULT  WindowBase::WndProc_GetRetValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled, LRESULT lRet)
{
    LRESULT lResult = 0; 

    if (NULL == m_oldWndProc)  // Dialog窗口过程
    {
        switch(uMsg)
        {
        case WM_INITDIALOG: 
            if (GetMouseMgr()->GetFocusObject())
                lResult = FALSE;  // 不使用其焦点设置
            else
                lResult = lRet;
            break;

        case WM_CHARTOITEM:
        case WM_COMPAREITEM:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORMSGBOX:
        case WM_QUERYDRAGICON:
        case WM_VKEYTOITEM:
            // return directly，这几个消息不按DialogProc返回值规定走
            lResult = lRet;
            break;

        default:
            SetWindowLong(m_hWnd, DWL_MSGRESULT, lRet) ;
            lResult = bHandled ? 1:0;
            break;
        }
    }
    else
    {
        if (bHandled)
        {
            lResult = lRet;
        }
        else
        {
            lResult = DefWindowProc(uMsg, wParam, lParam);
        }
    }
    return lResult;
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
		return DefWindowProc(uMsg,wParam,lParam);  // 鼠标位于窗口上的其它windows控件上面，不处理
	}  
	else if (hWnd == m_hWnd)  
	{
		if (LOWORD(lParam) != HTCLIENT)    // 用于鼠标位于system window的边缘时，应该调用默认的处理过程。包括当自己弹出一个模态框，返回HTERROR时
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
	if (TRUE == this->m_MgrMouse.HandleMessage(uMsg, wParam, lParam, &bHandled)) // 仅发给当前hover object
	{
		return TRUE;
	}

	// 让窗口去处理这个消息
	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	if ((this->GetIMessage())->ProcessMessage(&msg, 0)) 
		return msg.lRet;

	// 还是没有人处理
	::SetCursor(::LoadCursor(NULL,IDC_ARROW));
	return TRUE;
}

// 该消息已在WM_PAINT中分发。
// 为了实现无闪烁的绘制，必须将所有的绘制代码放在一个地方处理，要么在WM_ERASEBKGND，要么在WM_PAINT
// 但WM_ERASEBKGND不是每次都会触发的（在处理异形窗口时出现该问题），因此考虑将绘制代码都放在WM_PAINT中处理
// 
// 如果 lParam == 0，则表示是系统自己发出来的消息，由于系统消息会导致和WM_PAINT DC不一致，从而产生闪烁
// 因此将WM_ERASEBKGND消息放在WM_PAINT中由我们自己发出，并且将lParam置为非0
LRESULT WindowBase::_OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (0 == lParam)
	{
		bHandled = TRUE;
		if (m_bFirsetEraseBkgnd)
		{
			m_bFirsetEraseBkgnd = false;

			// TODO: 如果什么也不做，会导致窗口第一次显示时，窗口先显示一次黑色，例如combobox.listbox/menu
			// 但这样可能还是会导致别的闪烁问题，因为最终显示的界面不一定就是白色的
			DefWindowProc(uMsg, wParam, lParam); 
		}
		
		return 1;   // 对于Dialog类型，需要返回1来阻止系统默认绘制
	}
	return 0;
}
LRESULT WindowBase::_OnPaint( UINT uMsg, WPARAM wParam,LPARAM lParam, BOOL& bHandled )
{
	PAINTSTRUCT ps;
	HDC  hDC = NULL;

	if (NULL == wParam)
		hDC = ::BeginPaint(this->m_hWnd ,&ps);

	PaintWindow(hDC);

	if(NULL == wParam)
		EndPaint(m_hWnd,&ps);
	
	return 1;  //  在_OnPaint中返回0，会导致dialog类型的窗口，被其它窗口覆盖后移出来刷新异常!!!
}

LRESULT WindowBase::_OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (SIZE_MINIMIZED == wParam)
    {
        return 0;
    }
	
	//
	// (只有是用户显式修改窗口大小（拖拽、最大化）时，才能将width/height 从auto修改为现有值cx/cy
	// 避免了因为 第一次显示窗口时发出的WM_SIZE 而造成错误。)  <-- 作废！因为没法处理 nType==0 RESTORE 的情况
	//
	//	改成忽略这些情况了，直接赋值！因此会造成窗口的width=auto/height=auto只是窗口创建后那一刻的事情了，之后
	//  只要响应过 WM_SIZE，就不会再是auto了
	//

	// WM_SIZE的参数是基于客户区大小的，UI_UpdateLayout，UI_UpdateTopWindowLayout需要的是整个窗口的大小
	if (m_pLayout)  // 窗口刚创建，还没有调用UI_LoadLayout
	{
		RECT rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
        ::GetClientRect(m_hWnd, &m_rcParent);

        bHandled = TRUE;
        m_pRenderChain->OnWindowResize(wParam, LOWORD(lParam), HIWORD(lParam));  // 在窗口刷新之前更新窗口缓冲区大小
        
        if (IsWindowVisible(m_hWnd))
            m_pRenderChain->OnWindowPaint(NULL);
	}

	return 0;
}
LRESULT WindowBase::_OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if (false == this->CreateUI(m_strID.c_str(), m_hWnd))
	{
		UI_LOG_ERROR(_T("%s CreateUI failed. id=%s"), FUNC_NAME, m_strID.c_str());
		return 0;
	}

	//
	//  有可能m_strID为空（不加载资源，例如临时的popupconotrolwindow）
	//	因此没有将AddTopWindowObject、OnInitWindow放在CreateUI中执行
	//
	// if (!IsChildWindow()) -- 子窗口，但也是一个UI窗口，那么也维护起来
	{
		ITopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
		if (pTopWndMgr)
			pTopWndMgr->GetImpl()->AddTopWindowObject(this);
	}

    // 设置默认对象
    m_MgrMouse.SetDefaultObject(m_MgrMouse.GetOriginDefaultObject(), false);

    // 布局
    if (m_nStyle&WINDOW_STYLE_ATTACH)  // attach的窗口直接使用外部的大小
    {
        ::GetClientRect(m_hWnd, &m_rcParent);

        // 因为Attach到的窗口初始化时已经收不到WM_SIZE了，因此自己再发一次，
        // 通知创建RenderTarget，否则后面的一些刷新将失败
        BOOL bHandled = FALSE;
        this->_OnSize(WM_SIZE, 0, MAKELPARAM(m_rcParent.Width(), m_rcParent.Height()), bHandled);

        this->UpdateLayout(true); 

        CRect rcWindow;    // 避免此时调用GetDesiredSize又去测量窗口大小了
        ::GetWindowRect(m_hWnd, &rcWindow);
		SetConfigWidth(rcWindow.Width());
		SetConfigHeight(rcWindow.Height());
    }
    else
    {
        DesktopLayout dl;
        dl.Arrange(this);
    }

    //  给子类一个初始化的机会 (virtual)，例如设置最大化/还原按钮的状态
    this->OnInnerInitWindow();

    // 防止在实现显示动画时，先显示了一些初始化中刷新的内容。注：不能只限制一个layer
    m_pRenderChain->SetCanCommit(false);  
    UISendMessage(this, UI_WM_INITIALIZE);
    m_pRenderChain->SetCanCommit(true);
                                
	return 0;
}
LRESULT WindowBase::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	SyncWindowHelper<WindowBase>::_OnNcDestroy();

//	if (!IsChildWindow())
	{
		ITopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
		if (pTopWndMgr)
			pTopWndMgr->GetImpl()->RemoveTopWindowObject(this);
	}

    this->m_MgrDragDrop.OnWindowDestroy();
	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam, &bHandled);

	if (m_oldWndProc)
	{
		::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
		m_oldWndProc = NULL;
	}
	m_hWnd = NULL;
	
	this->DestroyChildObject();   // 删除子对象
	this->ResetAttribute();       // 释放自己的一些内存数据

	return 0;
}
LRESULT WindowBase::_OnHandleMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LRESULT lRet = this->m_MgrMouse.HandleMouseMessage(uMsg, wParam, lParam, &bHandled);

    if (bHandled)
        return lRet;

	if (m_MgrMouse.GetHoverObject() || NULL != m_MgrMouse.GetPressObject())
	{
		bHandled = TRUE;
	}
	return 0;
}
LRESULT  WindowBase::_OnHandleKeyBoardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    return this->m_MgrMouse.HandleKeyboardMessage( uMsg, wParam, lParam, &bHandled );
}

LRESULT WindowBase::_OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;


	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam, &bHandled);

	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam  = wParam;
	msg.lParam  = lParam;
    UISendMessage(&msg);

	return 1;    // 注：由于Dialog默认的WM_SETFOCUS会将焦点再设置到自己的第一个控件上面，因此如果在这里return 0或者bHandled = FALSE
	             //     将导致::SetFocus(m_hWnd)的焦点到窗口上面后，又被窗口自己设置到控件上面，导致SetFocus达不到我们的本意
}
LRESULT WindowBase::_OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	this->m_MgrMouse.HandleMessage( uMsg, wParam, lParam, &bHandled);

	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam = wParam;
	msg.lParam = lParam;
//	this->ProcessMessage( &msg );
    UISendMessage(&msg);

	return 1 ;
}

// 用于解决如果WindowBase是一个子窗口，放在一个普通顶层窗口上，那么点击WindowBase
// 将无法得到焦点。因此在这里响应MouseActive消息，将自己SetFocus
// 至少像系统按钮控件是如何处理的点击得到焦点就不得而知了，但WS_TABSTOP不是原因，试过了，没用。
LRESULT WindowBase::_OnMouseActive( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    if (IsChildWindow() && ::GetFocus() != m_hWnd)
    {
        ::SetFocus(m_hWnd);
    }
    return 0;
}

LRESULT WindowBase::_OnThemeChange( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	
    UIMSG  msg;
    msg.message = uMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;

	Message::ForwardMessageToChildObject(this, &msg);
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
	SyncWindowHelper<WindowBase>::_OnWindowPosChanging((LPWINDOWPOS)lParam, bHandled);
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

void WindowBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	if (m_pBkgndRender || NULL != m_pForegndRender)
	{
		__super::OnEraseBkgnd(pRenderTarget);  // 如果m_pBkgndRender没有处理，在这里调用系统过程
	}
	else
	{
		if (NULL == m_oldWndProc)   // Dialog类型，直接填充系统背景色
		{
			CRect rc;
			::GetClientRect(m_hWnd,&rc);

            Color c(::GetSysColor(COLOR_BTNFACE));
            c.a = 255;
			pRenderTarget->FillRect(&rc, &c);
		}
		else                         // Window类型，直接调用系统过程
		{
			HDC hDC = pRenderTarget->GetBindHDC();
			DefWindowProc(WM_ERASEBKGND, (WPARAM)hDC, 1);  // 与原始消息进行区分
		}
	}	
}

LRESULT WindowBase::OnGetGraphicsRenderType()
{
	return  /*GRAPHICS_RENDER_TYPE_GDIPLUS*/ GRAPHICS_RENDER_LIBRARY_TYPE_AUTO;
}

LRESULT  WindowBase::OnGetMouseKeyboardMgr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)static_cast<IMKMgr*>(this);
}

//
//  在响应完窗口的WM_NCDESTROY后，会触发该函数。
//  不要尝试在消息响应中调用delete this; 但可以再该函数中调用
//
// void WindowBase::OnFinalMessage()
// {
// 	
// }

//
//	将双缓存数据提交到窗口DC上
//
void WindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount)
{
	HDC hDC = hDCWnd;
	if (NULL == hDC)
    {
        if (NULL == m_hWnd)  // 窗口已销毁了。
            return;

		hDC = GetDC(m_hWnd);
    }

	CRect rcCommit;
	if (NULL == prcCommit)
	{
		::GetClientRect(m_hWnd, &rcCommit);
		nRectCount = 1;
	}
// 	else
// 		rcCommit.CopyRect(prcCommit);

    HDC hMemDC = m_pRenderChain->GetMemoryDC();
	m_MgrCWBL.DoPre(hDC, hMemDC, prcCommit, nRectCount);
	for (int i = 0; i < nRectCount; i++)
	{
		rcCommit.CopyRect(&prcCommit[i]);

		::BitBlt(hDC, rcCommit.left, rcCommit.top, 
            rcCommit.Width(), rcCommit.Height(), 
            hMemDC, rcCommit.left, rcCommit.top,
            SRCCOPY);
	}
	m_MgrCWBL.DoPost(hDC, hMemDC, prcCommit, nRectCount);

	if (NULL == hDCWnd)
	{
		ReleaseDC(m_hWnd, hDC);
	}
}

void WindowBase::OnGetDesiredSize(SIZE* pSize)
{
    if (NULL == pSize)
        return;

	Panel::OnGetDesiredSize(pSize);

    // 这里获取到的是client大小，需要转换成window大小
    this->CalcWindowSizeByClientSize(*pSize, pSize);
}

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

// CommitWindowBufferListenerMgr&  WindowBase::GetCommitWindowBufferListenerMgr()
// {
// 	return m_MgrCWBL;
// }
void  WindowBase::AddCommitWindowBufferListener(ICommitWindowBufferListener* p) 
{ 
    m_MgrCWBL.AddListener(p); 
}
void  WindowBase::RemoveCommitWindowBufferListener(ICommitWindowBufferListener* p) 
{ 
    m_MgrCWBL.RemoveListener(p); 
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

void  WindowBase::CenterWindow(HWND hWndCenter)
{
    // Copy  CWindowImpl::CenterWindow()
    CWindow wnd;
    wnd.Attach(m_hWnd);
    wnd.CenterWindow(hWndCenter);
    wnd.Detach();
}

// 使用桌面布局重新更新窗口位置大小，主要用于在UIEditor中修改了
// 窗口对象的大小。而使用Object::UpdateLayout仅能重新布局子对象的问题
void  WindowBase::UpdateDesktopLayout()
{
    DesktopLayout dl;
    dl.Arrange(this);
}

void WindowBase::SaveMemBitmap(TCHAR* szFile)
{
	UIASSERT(0 && _T("TODO:"));
	return;
#ifdef _DEBUGx
	if (NULL == m_hMemBitmap)
		return;

	::SelectObject(m_hMemDC, m_hOldBitmap);
	Image image;
	image.Attach(m_hMemBitmap);
//	image.ForceUseAlpha();
	image.Save(szFile, Gdiplus::ImageFormatBMP);
	image.Detach();
	::SelectObject(m_hMemDC, m_hMemBitmap);
#endif
}

HDC  WindowBase::GetRenderChainMemDC() 
{
    return m_pRenderChain->GetMemoryDC(); 
}
IRenderChain*  WindowBase::GetIRenderChain() 
{
    return m_pRenderChain->GetIRenderChain(); 
}

// 将内存位图绘图到指定区域
void WindowBase::DrawMemBitmap(HDC hDC, RECT* prc)
{
    HDC hMemDC = m_pRenderChain->GetMemoryDC();
    Image*  pBuffer = m_pRenderChain->GetMemoryBuffer();
	if (NULL == pBuffer || NULL == hMemDC)
		return;

	if (this->IsTransparent()) 
	{
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(hDC, prc->left, prc->top, LPRECTW(prc), LPRECTH(prc), hMemDC, 0, 0, pBuffer->GetWidth(), pBuffer->GetHeight(), bf);
	}
	else
	{
        HRGN hRgn = CreateRectRgn(0,0,0,0);
		int nRet = ::GetWindowRgn(m_hWnd, hRgn);
        if (ERROR == nRet)
        {
            BitBlt(hDC, prc->left, prc->top, LPRECTW(prc), LPRECTH(prc), hMemDC, 0, 0, SRCCOPY);
        }
        else
        {
		    ::SelectClipRgn(hDC, hRgn);
		    ::OffsetClipRgn(hDC, prc->left, prc->top);
    		
            BitBlt(hDC, prc->left, prc->top, LPRECTW(prc), LPRECTH(prc), hMemDC, 0, 0, SRCCOPY);

		    ::SelectClipRgn(hDC, NULL);
        }
        SAFE_DELETE_GDIOBJECT(hRgn);
	}
}



void WindowBase::SetActive(bool b)
{
	if (b)
		this->setStateBit( WSB_ACTIVE );
	else
		this->clearStateBit( WSB_ACTIVE );
}
bool WindowBase::IsActive()
{
	return this->testStateBit( WSB_ACTIVE );
}

//
// PS: 20130108 已使用CommitWindowBufferListenerMgr来实现
//
// 用于解决EDIT控件中光标在显示的情况下，重新刷新窗口背景将导致
// 光标的显示数据被破坏。最后隐藏光标时，反而将光标显示出来了。
// 因此在EDIT::OnEraseBkgnd中为窗口置上该标识，在WindowBase::CommitDoubleBuffet2Window
// 中检查该标识，如果该标识为true，则先隐藏光标，再提交数据，再将光标显示出来。
//
// void WindowBase::SetCaretBlinking(bool b)
// {
// 	if (b)
// 		this->setStateBit(WSB_CARETBLINKING);
// 	else
// 		this->clearStateBit(WSB_CARETBLINKING);
// }
// bool WindowBase::IsCaretBlinking(bool bClear)
// {
// 	bool b = this->testStateBit(WSB_CARETBLINKING);
// 	
// 	if (b && bClear)
// 		this->clearStateBit(WSB_CARETBLINKING);
// 
// 	return b;
// }


#if 1   // CreateData

CREATE_WND_DATA WindowBase::s_create_wnd_data;
CREATE_WND_DATA::CREATE_WND_DATA()
{
	m_pCreateWndList = NULL;
	::InitializeCriticalSection(&m_cs);
}
CREATE_WND_DATA::~CREATE_WND_DATA()
{
	::DeleteCriticalSection(&m_cs);
}

void  CREATE_WND_DATA::AddCreateWndData(_AtlCreateWndData* pData, void* pThis)
{
	UIASSERT(pData != NULL && pThis != NULL);

	pData->m_pThis = pThis;
	pData->m_dwThreadID = ::GetCurrentThreadId();

	EnterCriticalSection(&m_cs);

	pData->m_pNext = this->m_pCreateWndList;
	this->m_pCreateWndList = pData;

	LeaveCriticalSection(&m_cs);
}
void*  CREATE_WND_DATA::ExtractCreateWndData()
{
	void* pv = NULL;
	EnterCriticalSection(&m_cs);

	_AtlCreateWndData* pEntry = this->m_pCreateWndList;
	if(pEntry != NULL)
	{
		DWORD dwThreadID = ::GetCurrentThreadId();
		_AtlCreateWndData* pPrev = NULL;
		while(pEntry != NULL)
		{
			if(pEntry->m_dwThreadID == dwThreadID)
			{
				if(pPrev == NULL)
					this->m_pCreateWndList = pEntry->m_pNext;
				else
					pPrev->m_pNext = pEntry->m_pNext;
				pv = pEntry->m_pThis;
				break;
			}
			pPrev = pEntry;
			pEntry = pEntry->m_pNext;
		}
	}

	LeaveCriticalSection(&m_cs);
	return pv;
}

void WindowBase::OnObjectDeleteInd(Object* p)
{
    m_MgrMouse.ObjectDeleteInd(p);
}
void WindowBase::OnObjectHideInd(Object* p)
{
    m_MgrMouse.ObjectHideInd(p);
}
void  WindowBase::SetFocusObject(Object* pObj)
{
    m_MgrMouse.SetFocusObject(pObj);
}

// 获取当前鼠标下的对象 
Object*  WindowBase::GetObjectByCursorPos()
{
    POINT  pt;
    GetCursorPos(&pt);
    ::ScreenToClient(m_hWnd, &pt);

    return m_MgrMouse.GetObjectByPos(this, &pt);
}
Object*  WindowBase::GetObjectByPos(Object* pObjParent, POINT* pt, bool bSkinBuilderInvoke)
{
    if (bSkinBuilderInvoke)
        return m_MgrMouse.GetObjectByPos_UIEditor(pObjParent, pt);
    else
        return m_MgrMouse.GetObjectByPos(pObjParent, pt);
}
#endif

// 不负责刷新
void WindowBase::OnSetDefId(IObject* pButton)
{
    if (pButton)
        m_MgrMouse.SetOriginDefaultObject(pButton->GetImpl());
    else
        m_MgrMouse.SetOriginDefaultObject(NULL);
}

IObject* WindowBase::OnGetDefId()
{
    Object* pObj = m_MgrMouse.GetOriginDefaultObject();
    if (pObj)
    {
        return pObj->GetIObject();
    }
    else
    {
        return NULL;
    }
}

}