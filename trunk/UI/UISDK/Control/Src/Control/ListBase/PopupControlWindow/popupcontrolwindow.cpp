#include "stdafx.h"
#include "popupcontrolwindow.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Src\Control\Menu\menu.h"

namespace UI
{

PopupControlWindow::PopupControlWindow()
{
	m_pObject = NULL;
	m_bExitLoop = false;
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// 注：如果自己的父窗口不是桌面，则不会在任务栏上面显示图标
BOOL PopupControlWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    if (NULL == pcs->hwndParent)
    {
    	pcs->dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW;
    }
    else
    {
        pcs->dwExStyle |= WS_EX_NOACTIVATE;
    }
	pcs->lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	
    SetMsgHandled(FALSE);
    return TRUE;
}

void  PopupControlWindow::Create(IObject*  pBindObj, const TCHAR* szId, HWND hParentWnd)
{
    m_pObject = pBindObj;
    __super::Create(GetUIApplication(), _T(""), hParentWnd);  // 在创建完panel的layer之后再创建窗口，避免第一次响应_OnSize中没有layout

    IMapAttribute*  pMapAttrib = NULL;
    pBindObj->GetMapAttribute(&pMapAttrib);
    if (pMapAttrib)
    {
        IMapAttribute*  pMapAttribCopy = NULL;
        pMapAttrib->CreateCopy(&pMapAttribCopy);

        if (szId)
            this->SetID(szId);

        UISendMessage(this, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);

        SAFE_RELEASE(pMapAttribCopy);
        SAFE_RELEASE(pMapAttrib);
    }
}
 
void  PopupControlWindow::Show(POINT pt, BOOL bDoModal)
{
    if (NULL == m_pObject)
        return;

    m_bExitLoop = false;
    UISendMessage(m_pObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);

    SIZE size = m_pObject->GetDesiredSize();

    CRect  rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
    if (pt.x < rcWorkArea.left)
        pt.x = rcWorkArea.left;
    if (pt.x + size.cx > rcWorkArea.right)
        pt.x = rcWorkArea.right - size.cx;
    if (pt.y < rcWorkArea.top)
        pt.y = rcWorkArea.top;
    if (pt.y + size.cy > rcWorkArea.bottom)
        pt.y = rcWorkArea.bottom - size.cy;

    HWND hPopupWnd = GetHWND();
    ::ShowWindow(hPopupWnd, SW_SHOWNA);  // 放在SetWindowPos后面还是会导致黑一次
    ::SetWindowPos(hPopupWnd, NULL, pt.x, pt.y, size.cx, size.cy, SWP_NOZORDER/*|SWP_SHOWWINDOW*/|SWP_NOACTIVATE);
    ::UpdateWindow(hPopupWnd);

    IMessageFilterMgr* pMgr = NULL;
    GetUIApplication()->GetMessageFilterMgr(&pMgr);
    pMgr->AddMessageFilter(static_cast<IPreTranslateMessage*>(this));

    if (TRUE == bDoModal)
        GetUIApplication()->MsgHandleLoop(&m_bExitLoop);
}

// 如果是外部调用的话，则还需要移除pretranslatemessage?/?
void  PopupControlWindow::Hide()
{
    m_bExitLoop = true;

    if (!IsWindowVisible(GetHWND()))  // 已经隐藏了
        return ;

    ::ShowWindow(GetHWND(), SW_HIDE);

    if (m_pObject)
    {
        UISendMessage(m_pObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);  // 将该消息放在这里发送，保证每一个子菜单窗口销毁时都能收到该消息
    }

    // 通知对象窗口被销毁
    IMessageFilterMgr* pMgr = NULL;
    GetUIApplication()->GetMessageFilterMgr(&pMgr);
    pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));
}

void  PopupControlWindow::Destroy()
{
    ::DestroyWindow(GetHWND());
}

void PopupControlWindow::OnInitialize()
{
    __super::xProcessMessage(GetCurMsg(), 0, 0);
    SetWindowResizeType(WRSB_NONE);

    if (m_pObject)
    {
	    m_pObject->AddHook(this, 0, 1);
	    this->AddChild(m_pObject);
    }
}

void PopupControlWindow::OnDestroy()
{
	SetMsgHandled(FALSE);

	// 避免窗口在DestroyChildObject中delete child object
	// 同时避免在销毁root menu时，会先销毁子菜单的窗口，因此在这里将子菜单的parent也清理掉
	if (m_pObject)
	{
		m_pObject->RemoveHook(this);
		this->Standalone();
		m_pObject->SetParentObjectDirect(NULL);
	}
}

// Popup控件不参与换肤，目前
void  PopupControlWindow::OnSkinChanging(BOOL* pbChange)
{
    *pbChange = FALSE;
}

// 在鼠标点击本窗口的时候，不要去抢占主窗口的焦点
// 但是这样同时导致了鼠标消息和键盘消息获取不进来了
// WM_NCHITTEST能收到
int PopupControlWindow::OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}
// 这个消息是用于针对鼠标点击了桌面/其它顶层窗口的时候，
// 将窗口隐藏的通知。对于点击了自己的顶层窗口却处理不了
void PopupControlWindow::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	SetMsgHandled(FALSE);
	if (FALSE == bActive)
	{
		this->Hide();
	}
}



// 注：
//	这里的GetMessage其实是只能拦截PostMessage发送过来的消息，
//  对于其它SendMessage的消息是不能得知的。
//

BOOL PopupControlWindow::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
		{
			this->Hide();
			return TRUE;
		}

		BOOL bHandle = FALSE;
		UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		//return bHandle;
        return TRUE;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if (pMsg->hwnd != GetHWND())
		{
			return TRUE;  // 窗口外的鼠标移动事件忽略
		}
		else
		{
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN   == pMsg->message ||
		WM_LBUTTONDBLCLK == pMsg->message ||
		WM_RBUTTONDOWN   == pMsg->message ||
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDBLCLK == pMsg->message )
	{
		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
		if (!PtInRect(&rcWindow, pMsg->pt))  // 鼠标在弹出窗口外面点击了，关闭当前窗口
		{
			this->Hide();
			
			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

            return TRUE;
		}
	}
	
	return FALSE;
}

BOOL PopupControlWindow::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	return TRUE;
}

LRESULT PopupControlWindow::OnGetGraphicsRenderType()
{
	if (m_pObject)
	{
		// 注：这里没有去调用GetGraphicsRenderType，因为GetGraphicsRenderType中m_pObject又会反调用m_pWindow->GetGraphicsRenderLibraryType
		//     有可能导致死嵌套

		GRAPHICS_RENDER_LIBRARY_TYPE e = (GRAPHICS_RENDER_LIBRARY_TYPE)UISendMessage( m_pObject, UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE );
		if (GRAPHICS_RENDER_LIBRARY_TYPE_AUTO == e)
		{
			e = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		}
		return e;
	}
	return  GRAPHICS_RENDER_LIBRARY_TYPE_AUTO;
}


PopupListBoxWindow::PopupListBoxWindow()
{
}

BOOL PopupListBoxWindow::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (pMsg->message != WM_KEYDOWN)
			return FALSE;

		UINT nChar = pMsg->wParam;
		if (VK_ESCAPE == nChar)
		{
			this->Hide();
			return TRUE;
		}

		// 只处理一些基本按键，其它键盘消息还是传递给当前FOCUS COMBOBOX'S EDIT
		if (Util::IsKeyDown(VK_SHIFT))  // SHIFT + xxx 由EDIT处理
			return FALSE;

		if (nChar == VK_UP    ||
			nChar == VK_DOWN  ||
			nChar == VK_HOME  ||
			nChar == VK_END   ||
			nChar == VK_PRIOR ||
			nChar == VK_NEXT)
		{
			BOOL bHandle = FALSE;
			UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
			return TRUE;/*bHandle*/; // 有可能传给了其实控件
		}

		return FALSE;
	}

	return __super::PreTranslateMessage(pMsg);
}

void PopupListBoxWindow::OnInitialize()
{
	__super::OnInitialize();

	// 清除原窗口上面的hover、press对象
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
}

void PopupListBoxWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetMsgHandled(FALSE);

	if (VK_DOWN == nChar || VK_UP == nChar) 
	{
		SetMsgHandled(TRUE);
		m_pObject->ProcessMessage(GetCurMsg(), 0);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
PopupMenuWindow::PopupMenuWindow()
{
	m_pMenu = NULL;
	m_ptLastMousePos.x = m_ptLastMousePos.y = 0;
    m_pLastHoverMenu = NULL;
}
void PopupMenuWindow::OnInitialize()
{
	__super::OnInitialize();
    if (m_pObject)
    {
        IMenu* p = (IMenu*)m_pObject->QueryInterface(uiiidof(IMenu));
        if (p)
            m_pMenu = p->GetImpl();
    }

    if (m_pMenu)
    {
	    // 清除原窗口上面的hover、press对象
	    if (m_pMenu->GetRootMenu() == m_pMenu) // 子菜单弹出来时,activeWindow是父菜单，因此不需要再发送mouseleave消息
	    {
		    HWND hWnd = GetActiveWindow();
		    ::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	    }
    }
}


//
//	注：只有根菜单才会有MenuLoop，子菜单没有
//
BOOL PopupMenuWindow::PreTranslateMessage(MSG* pMsg)
{
    if (NULL == m_pMenu)
        return FALSE;

    HWND hWnd = GetHWND();
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		if (WM_SYSKEYDOWN == pMsg->message && Util::IsKeyDown(VK_MENU))
		{
			this->Hide();   // 按下ALT键时，销毁整个菜单。(ESC只销毁last menu)
			return TRUE;
		}

		BOOL bHandle = FALSE;
        Menu* pMenu = m_pMenu->GetLastMenu();
        if (pMenu)
    		UISendMessage(pMenu->GetIMenu(), pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);

		return bHandle;
	}

	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
        // 屏蔽菜单外的鼠标消息（仿系统）,但当鼠标离开菜单的时候，得补发一个MOUSELEAVE事件...(TODO: 是不是我哪搞错了)
        Menu*  pMenuHover = m_pMenu->GetMenuByHWND(pMsg->hwnd);
		if (NULL == pMenuHover)
		{
            if (m_pLastHoverMenu)
            {
                ::PostMessage(m_pLastHoverMenu->GetIMenu()->GetHWND(), WM_MOUSELEAVE, 0, 0);
            }
            m_pLastHoverMenu = NULL;
			return TRUE;  // 窗口外的鼠标移动事件忽略
		}
		else
		{
            m_pLastHoverMenu = pMenuHover;
			POINT ptNow = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)};
			if (ptNow.x == m_ptLastMousePos.x && ptNow.y == m_ptLastMousePos.y)  // 可能是由于子菜单的显示发出来的MOUSEMOVE，忽略
			{
				UI_LOG_DEBUG(_T("%s point repeat"),FUNC_NAME);
				return TRUE;
			}

			m_ptLastMousePos = ptNow;
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN   == pMsg->message ||
		WM_LBUTTONDBLCLK == pMsg->message ||
		WM_RBUTTONDOWN   == pMsg->message ||
		WM_RBUTTONDBLCLK == pMsg->message ||
		WM_MBUTTONDOWN   == pMsg->message ||
		WM_MBUTTONDBLCLK == pMsg->message ||
		WM_XBUTTONDOWN   == pMsg->message ||
		WM_XBUTTONDBLCLK == pMsg->message ||
		WM_NCLBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDOWN == pMsg->message ||
		WM_NCRBUTTONDBLCLK == pMsg->message)
	{
		RECT rcWindow;
		::GetWindowRect(GetHWND(), &rcWindow);
		if (NULL == m_pMenu->GetMenuByPos(pMsg->pt))  // 鼠标在弹出窗口外面点击了，关闭当前窗口
		{
			this->Hide();

			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			
			HWND hWnd = GetActiveWindow();
			::SendMessage(hWnd, WM_MOUSEMOVE, 0, pMsg->lParam);
            
            // 给当前鼠标点击的窗口发送一个鼠标消息，用于解决在弹出菜单后，
            // 直接点击另一个窗口上的按钮会没有反应
            if (pMsg->hwnd != hWnd)
                ::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);

            return TRUE;
		}
	}

	return FALSE;
}
}