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
	UIASSERT( !IsWindow(m_hWnd) );          // ȷ�������ѱ�����
	UIASSERT( NULL == this->m_oldWndProc ); // ȷ���Ѿ�ȡ�����໯
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
	this->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT, false);  // ȡ��Panel�����е�͸������
}
void WindowBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	Panel::SetAttribute(pMapAttrib, bReload);

	IFontRes* pFontRes = GetUIApplication()->GetActiveSkinFontRes();
	if (NULL == pFontRes)
		return;

	// ������ʽ 
    const TCHAR* szText = pMapAttrib->GetAttr(XML_FONT, true);
	if (szText)
		pFontRes->GetFont((BSTR)szText, GetRenderLibraryType(m_pIWindowBase), &m_pDefaultFont);

	// ����Ĭ������
	if (NULL == m_pDefaultFont)
	{
        // GDI ��������
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
bool WindowBase::CreateUI(const TCHAR* szID, HWND hWnd)
{
	this->m_hWnd = hWnd;

	if (szID && _tcslen(szID)>0)   
	{
		//	�����ӿؼ�
		ILayoutManager* pLayoutMgr = m_pUIApplication->GetActiveSkinLayoutMgr();
		if (NULL == pLayoutMgr)
		{
			UI_LOG_ERROR(_T("%s GetLayoutManager Failed."), FUNC_NAME);
			return false;
		}

		if (FAILED(pLayoutMgr->LoadRootLayout(m_pIObject, szID)))
		{
			UI_LOG_FATAL(_T("%s, ���ش���(ID=%s)layoutʧ��"), FUNC_NAME, szID);
			return false;
		}
	}

	//
	//	Ϊ�˽��xp��win7�����һ�����ԣ�ֻ���ڰ���ALT��������TAB��֮�󣬲Ż���ʾ�ؼ���focus rect
	//	�ڳ�ʼ���������������������
	//	
	// ::PostMessage(m_hWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);
	
	// Ϊ��ʵ���б�����ؼ���theme���ƣ����򻭳�����Ч������ȷ������һ���ڱ�
	// http://msdn.microsoft.com/zh-cn/library/windows/desktop/bb759827(v=vs.85).aspx
	// http://www.codeproject.com/Articles/18858/Fully-themed-Windows-Vista-Controls
	// http://stackoverflow.com/questions/14039984/which-class-part-and-state-is-used-to-draw-selection

	// The following example code gives a list-view control the appearance of a Windows Explorer list: 
	SetWindowTheme(m_hWnd, L"explorer", NULL);

	return true;
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


// ��ȡһ���ؼ��ڴ����ϵ�ͼ��
// ע���п���һ�����������صģ����߲��ڴ���
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
	// removed by libo 20120728 ������һ���յĴ��ڣ�����˵�����
// 	UIASSERT( ID != _T("") );
// 	if( _T("") == ID )
// 	{
// 		UI_LOG_FATAL( _T("Window::Create, δָ������id") );
// 		return false;
// 	}
	
	this->SetUIApplication(pUIApp);
    if (szID)
	__super::m_strID = szID;   // ��ǰ��id��ֵ��������־���

	//	�������ھ��
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
		UI_LOG_FATAL( _T("%s, CreateWindowExʧ��"), FUNC_NAME);
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
	    m_strID = szID;   // ��ǰ��id��ֵ��������־���
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

	// ��ModalWindow::DoModal�����Ϣѭ���ܹ��˳�
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
// ����һ���յ�ģ̬�Ի���
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
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // ���ڵı�������
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
    // ���ⲿһ������ģ̬������ʾ�Ļ���
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
	m_bEndModal = false;  // ��ԭ�������´�DoModal

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
		::SetActiveWindow(hWndParent);  // ��������øú����������¸������ܵ���ĻZ����ĺ���ȥ��
	}
	::DestroyWindow(this->m_hWnd);    // ���ٴ���

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
	int nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "", -1, lpwsz, 50);  // ���ڵı�������
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
		// fixĬ�ϵ�#32770���ڹ����ڴ�С�ı�ʱ��ˢ�µ�����
		// TODO:?? Ϊʲô��THICKFRAME�Ĵ��ھ����Լ�ˢ��
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

    // ע�� hWndParent����Owner��������parent
	m_hWnd = CreateDialog(hResInst, MAKEINTRESOURCE(nResID), hWndOnwer, (DLGPROC)WindowBase::StartDialogProc);
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
	WindowBase* pThis = (WindowBase*)s_create_wnd_data.ExtractCreateWndData();
	UIASSERT(NULL != pThis);
	
	if (NULL == pThis)
		return 0;

	return pThis->StartProc(hwnd,uMsg,wParam,lParam, true);
}
//
//	[static] Dialog���ʹ��ڵĵ�һ��������Ϣ���õĴ��ڹ���
//
LRESULT CALLBACK WindowBase::StartDialogProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// ��ȡthisָ��
	WindowBase* pThis = (WindowBase*)s_create_wnd_data.ExtractCreateWndData();
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
//	���ڱ����໯��֮��Ĵ��ڹ���
//
LRESULT	WindowBase::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes;
	UIMSG*  pOldMsg = m_pCurMsg;
	BOOL bHandled = this->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lRes, 0);  // ����BEGIN_MSG_MAP��Ϣӳ���б�
	if (bHandled)
	{
		return WndProc_GetRetValue(uMsg, wParam, lParam, bHandled, lRes);
	}
    
    // ֱ�ӷ�����ǰ���ڴ���
    UIMSG  msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam = wParam;
	msg.lParam = lParam;

	// ��������Ϣ��������ˣ�ֱ�ӷ��أ������ٵ��þɵĴ��ڹ�����
	//if (static_cast<IMessage*>(this)->ProcessMessage(&msg, 0))
    UISendMessage(&msg);
    lRes = WndProc_GetRetValue(uMsg, wParam, lParam,  msg.bHandled, msg.lRet);

	if (uMsg == WM_NCDESTROY)
	{
		// ע��Ϊʲô��������ֱ�ӵ���OnFinalMessage��ȴ��Ҫ�ټ�һ��״̬λ��
		// ��ΪWM_NCDESTROY������DestroyWindow api��������DestroyWindow api
		// ����λ���κ�һ����ǰ���ڵ���Ϣ��Ӧ�У���˵�pOldMsg==NULLʱ������ʾ
		// û����ϢǶ���ˣ��ڼ��һ��WINDOW_STYLE_DESTROYED��־���ɡ�
		this->ModifyStyle(WINDOW_STYLE_DESTROYED, 0, 0);
	}
	if (m_nStyle & WINDOW_STYLE_DESTROYED && pOldMsg == NULL)
	{
		this->ModifyStyle(0, WINDOW_STYLE_DESTROYED, 0);
#if 0 // -- �ܹ�����
		this->OnFinalMessage();
#endif
	}

	return lRes;
}

// ���öԻ����DialogProc����ֵ����MSDN�ж�DialogProc����ֵ��˵��
LRESULT  WindowBase::WndProc_GetRetValue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled, LRESULT lRet)
{
    LRESULT lResult = 0; 

    if (NULL == m_oldWndProc)  // Dialog���ڹ���
    {
        switch(uMsg)
        {
        case WM_INITDIALOG: 
            if (GetMouseMgr()->GetFocusObject())
                lResult = FALSE;  // ��ʹ���佹������
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
            // return directly���⼸����Ϣ����DialogProc����ֵ�涨��
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
	if (TRUE == this->m_MgrMouse.HandleMessage(uMsg, wParam, lParam, &bHandled)) // ��������ǰhover object
	{
		return TRUE;
	}

	// �ô���ȥ���������Ϣ
	UIMSG msg;
	msg.message = uMsg;
	msg.pMsgTo = this->GetIMessage();
	msg.wParam  = wParam;
	msg.lParam  = lParam;
	if ((this->GetIMessage())->ProcessMessage(&msg, 0)) 
		return msg.lRet;

	// ����û���˴���
	::SetCursor(::LoadCursor(NULL,IDC_ARROW));
	return TRUE;
}

// ����Ϣ����WM_PAINT�зַ���
// Ϊ��ʵ������˸�Ļ��ƣ����뽫���еĻ��ƴ������һ���ط�����Ҫô��WM_ERASEBKGND��Ҫô��WM_PAINT
// ��WM_ERASEBKGND����ÿ�ζ��ᴥ���ģ��ڴ������δ���ʱ���ָ����⣩����˿��ǽ����ƴ��붼����WM_PAINT�д���
// 
// ��� lParam == 0�����ʾ��ϵͳ�Լ�����������Ϣ������ϵͳ��Ϣ�ᵼ�º�WM_PAINT DC��һ�£��Ӷ�������˸
// ��˽�WM_ERASEBKGND��Ϣ����WM_PAINT���������Լ����������ҽ�lParam��Ϊ��0
LRESULT WindowBase::_OnEraseBkgnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (0 == lParam)
	{
		bHandled = TRUE;
		if (m_bFirsetEraseBkgnd)
		{
			m_bFirsetEraseBkgnd = false;

			// TODO: ���ʲôҲ�������ᵼ�´��ڵ�һ����ʾʱ����������ʾһ�κ�ɫ������combobox.listbox/menu
			// ���������ܻ��ǻᵼ�±����˸���⣬��Ϊ������ʾ�Ľ��治һ�����ǰ�ɫ��
			DefWindowProc(uMsg, wParam, lParam); 
		}
		
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

	PaintWindow(hDC);

	if(NULL == wParam)
		EndPaint(m_hWnd,&ps);
	
	return 1;  //  ��_OnPaint�з���0���ᵼ��dialog���͵Ĵ��ڣ����������ڸ��Ǻ��Ƴ���ˢ���쳣!!!
}

LRESULT WindowBase::_OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (SIZE_MINIMIZED == wParam)
    {
        return 0;
    }
	
	//
	// (ֻ�����û���ʽ�޸Ĵ��ڴ�С����ק����󻯣�ʱ�����ܽ�width/height ��auto�޸�Ϊ����ֵcx/cy
	// ��������Ϊ ��һ����ʾ����ʱ������WM_SIZE ����ɴ���)  <-- ���ϣ���Ϊû������ nType==0 RESTORE �����
	//
	//	�ĳɺ�����Щ����ˣ�ֱ�Ӹ�ֵ����˻���ɴ��ڵ�width=auto/height=autoֻ�Ǵ��ڴ�������һ�̵������ˣ�֮��
	//  ֻҪ��Ӧ�� WM_SIZE���Ͳ�������auto��
	//

	// WM_SIZE�Ĳ����ǻ��ڿͻ�����С�ģ�UI_UpdateLayout��UI_UpdateTopWindowLayout��Ҫ�����������ڵĴ�С
	if (m_pLayout)  // ���ڸմ�������û�е���UI_LoadLayout
	{
		RECT rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
        ::GetClientRect(m_hWnd, &m_rcParent);

        bHandled = TRUE;
        m_pRenderChain->OnWindowResize(wParam, LOWORD(lParam), HIWORD(lParam));  // �ڴ���ˢ��֮ǰ���´��ڻ�������С
        
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
	//  �п���m_strIDΪ�գ���������Դ��������ʱ��popupconotrolwindow��
	//	���û�н�AddTopWindowObject��OnInitWindow����CreateUI��ִ��
	//
	// if (!IsChildWindow()) -- �Ӵ��ڣ���Ҳ��һ��UI���ڣ���ôҲά������
	{
		ITopWindowManager* pTopWndMgr = m_pUIApplication->GetTopWindowMgr();
		if (pTopWndMgr)
			pTopWndMgr->GetImpl()->AddTopWindowObject(this);
	}

    // ����Ĭ�϶���
    m_MgrMouse.SetDefaultObject(m_MgrMouse.GetOriginDefaultObject(), false);

    // ����
    if (m_nStyle&WINDOW_STYLE_ATTACH)  // attach�Ĵ���ֱ��ʹ���ⲿ�Ĵ�С
    {
        ::GetClientRect(m_hWnd, &m_rcParent);

        // ��ΪAttach���Ĵ��ڳ�ʼ��ʱ�Ѿ��ղ���WM_SIZE�ˣ�����Լ��ٷ�һ�Σ�
        // ֪ͨ����RenderTarget����������һЩˢ�½�ʧ��
        BOOL bHandled = FALSE;
        this->_OnSize(WM_SIZE, 0, MAKELPARAM(m_rcParent.Width(), m_rcParent.Height()), bHandled);

        this->UpdateLayout(true); 

        CRect rcWindow;    // �����ʱ����GetDesiredSize��ȥ�������ڴ�С��
        ::GetWindowRect(m_hWnd, &rcWindow);
		SetConfigWidth(rcWindow.Width());
		SetConfigHeight(rcWindow.Height());
    }
    else
    {
        DesktopLayout dl;
        dl.Arrange(this);
    }

    //  ������һ����ʼ���Ļ��� (virtual)�������������/��ԭ��ť��״̬
    this->OnInnerInitWindow();

    // ��ֹ��ʵ����ʾ����ʱ������ʾ��һЩ��ʼ����ˢ�µ����ݡ�ע������ֻ����һ��layer
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
	
	this->DestroyChildObject();   // ɾ���Ӷ���
	this->ResetAttribute();       // �ͷ��Լ���һЩ�ڴ�����

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

	return 1;    // ע������DialogĬ�ϵ�WM_SETFOCUS�Ὣ���������õ��Լ��ĵ�һ���ؼ����棬������������return 0����bHandled = FALSE
	             //     ������::SetFocus(m_hWnd)�Ľ��㵽����������ֱ������Լ����õ��ؼ����棬����SetFocus�ﲻ�����ǵı���
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

// ���ڽ�����WindowBase��һ���Ӵ��ڣ�����һ����ͨ���㴰���ϣ���ô���WindowBase
// ���޷��õ����㡣�����������ӦMouseActive��Ϣ�����Լ�SetFocus
// ������ϵͳ��ť�ؼ�����δ���ĵ���õ�����Ͳ��ö�֪�ˣ���WS_TABSTOP����ԭ���Թ��ˣ�û�á�
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
		__super::OnEraseBkgnd(pRenderTarget);  // ���m_pBkgndRenderû�д������������ϵͳ����
	}
	else
	{
		if (NULL == m_oldWndProc)   // Dialog���ͣ�ֱ�����ϵͳ����ɫ
		{
			CRect rc;
			::GetClientRect(m_hWnd,&rc);

            Color c(::GetSysColor(COLOR_BTNFACE));
            c.a = 255;
			pRenderTarget->FillRect(&rc, &c);
		}
		else                         // Window���ͣ�ֱ�ӵ���ϵͳ����
		{
			HDC hDC = pRenderTarget->GetBindHDC();
			DefWindowProc(WM_ERASEBKGND, (WPARAM)hDC, 1);  // ��ԭʼ��Ϣ��������
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
//  ����Ӧ�괰�ڵ�WM_NCDESTROY�󣬻ᴥ���ú�����
//  ��Ҫ��������Ϣ��Ӧ�е���delete this; �������ٸú����е���
//
// void WindowBase::OnFinalMessage()
// {
// 	
// }

//
//	��˫���������ύ������DC��
//
void WindowBase::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount)
{
	HDC hDC = hDCWnd;
	if (NULL == hDC)
    {
        if (NULL == m_hWnd)  // �����������ˡ�
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

    // �����ȡ������client��С����Ҫת����window��С
    this->CalcWindowSizeByClientSize(*pSize, pSize);
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

// ʹ�����沼�����¸��´���λ�ô�С����Ҫ������UIEditor���޸���
// ���ڶ���Ĵ�С����ʹ��Object::UpdateLayout�������²����Ӷ��������
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

// ���ڴ�λͼ��ͼ��ָ������
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
// PS: 20130108 ��ʹ��CommitWindowBufferListenerMgr��ʵ��
//
// ���ڽ��EDIT�ؼ��й������ʾ������£�����ˢ�´��ڱ���������
// ������ʾ���ݱ��ƻ���������ع��ʱ�������������ʾ�����ˡ�
// �����EDIT::OnEraseBkgnd��Ϊ�������ϸñ�ʶ����WindowBase::CommitDoubleBuffet2Window
// �м��ñ�ʶ������ñ�ʶΪtrue���������ع�꣬���ύ���ݣ��ٽ������ʾ������
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

// ��ȡ��ǰ����µĶ��� 
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

// ������ˢ��
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