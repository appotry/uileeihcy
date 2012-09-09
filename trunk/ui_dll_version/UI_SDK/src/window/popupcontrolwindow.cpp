#include "stdafx.h"

PopupControlWindow::PopupControlWindow(Object* pObj)
{
	m_pObject = pObj;
	m_bExitLoop = false;
}


void PopupControlWindow::OnFinalMessage()
{
	m_pObject = NULL;
	delete this;
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// 注：如果自己的父窗口不是桌面，则不会在任务栏上面显示图标
BOOL PopupControlWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW;
	cs.lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	return __super::PreCreateWindow(cs);
}

//
// 关闭/退出弹出窗口(菜单)的函数。目前会有两个地方调用到：
// 1. 鼠标点击了窗口外面
// 2. 控件内部自己调用，例如点击了其中一项。
//
// 注：这里不能使用SendMessage来销毁窗口
//     因为有可能MouseMgr::OnLButtonUp还可能会使用到m_pWindow句柄
//     
void PopupControlWindow::DestroyPopupWindow()
{
	// 退出消息循环，并销毁窗口
	::PostMessage(m_hWnd, UI_WM_EXITPOPUPLOOP, 0, 0);
}

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();

	m_pObject->AddHook(this, 0, 1);
	this->AddChild(m_pObject);

	// 由于Menu,listbox的需求不同，有可能需要调用同步SendMessage，因些将该消息的调用放到外面去调用
	// 准备进入消息循环
	//::PostMessage(m_hWnd, UI_WM_ENTERPOPUPLOOP, 0, 0);
}
void PopupControlWindow::OnDestroy()
{
	SetMsgHandled(FALSE);

	UISendMessage(m_pObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);  // 将该消息放在这里发送，保证每一个子菜单窗口销毁时都能收到该消息

	// 避免窗口在DestroyUI中delete child object
	// 同时避免在销毁root menu时，会先销毁子菜单的窗口，因此在这里将子菜单的parent也清理掉
	if (NULL != m_pObject)
	{
		m_pObject->RemoveHook(this);
		this->ClearTreeObject();
		m_pObject->ClearParentObject();

	}
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
		this->DestroyPopupWindow();
	}
}

LRESULT PopupControlWindow::OnEnterPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UISendMessage(m_pObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);

	PopupLoop();
	return 0;
}

LRESULT PopupControlWindow::OnExitPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 通知对象窗口被销毁
// 	this->ClearTreeObject();
// 	m_pObject->ClearParentObject();

	::DestroyWindow(m_hWnd);

	m_bExitLoop = true;

	return 0;
}



// 注：
//	这里的GetMessage其实是只能拦截PostMessage发送过来的消息，
//  对于其它SendMessage的消息是不能得知的。
//
void PopupControlWindow::PopupLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
		{
			::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			return;
		}

		// Removed <-- 不靠谱. 1>. 有可能该消息通过SendMessage发送 2>. 某些场景下面有问题
		// 因些增加了一个变量m_bExitLoop来设置是否退出本循环
// 		bool bBreak = false;
// 		if (UI_WM_EXITPOPUPLOOP == msg.message)  // 注：处理完该消息后，m_hWnd将为空，因此先判断再执行
// 		{
// 			if (msg.hwnd == m_hWnd)
// 				bBreak = true;
// 		}

		if (FALSE == this->PreTranslatePopupMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

// 		if (bBreak)
// 			break;
		if (m_bExitLoop)
		{
			m_bExitLoop = false;
			break;
		}
	}
}
BOOL PopupControlWindow::PreTranslatePopupMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		BOOL bHandle = FALSE;
		UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		return bHandle;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if (pMsg->hwnd != m_hWnd)
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
		::GetWindowRect(m_hWnd, &rcWindow);
		if (!PtInRect(&rcWindow, pMsg->pt))  // 鼠标在弹出窗口外面点击了，关闭当前窗口
		{
			this->DestroyPopupWindow();
			
			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			::SendMessage(pMsg->hwnd, WM_MOUSEMOVE, 0, pMsg->lParam);
		}
	}
	
	return FALSE;
}

BOOL PopupControlWindow::OnEraseBkgnd(HRDC hRDC)
{
	CRect rcClient;
	this->GetClientRect(&rcClient);

	Rectangle(hRDC, &rcClient, RGB(0,0,0), RGB(255,255,255) );
	return TRUE;
}

PopupListBoxWindow::PopupListBoxWindow(ListBox* pListBox, Object* pBindObj) : PopupControlWindow(pListBox)
{
	m_pBindObj = pBindObj;
	m_pListBox = pListBox;
}

BOOL PopupListBoxWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pListBox || NULL == m_pBindObj)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	cs.lpszClass = WND_POPUP_CONTROL_NAME;  // 不加阴影
	return bRet;
}
void PopupListBoxWindow::OnInitWindow()
{
	__super::OnInitWindow();

	// 清除原窗口上面的hover、press对象
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindObj->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindObj->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

// 	ATTRMAP map_temp;
// 	map_temp[XML_ID] = _T("PopupListBoxWindow");
// 	this->SetAttribute(map_temp,false);  // 初始化一些默认变量，如窗口字体

// 	TextRenderBase* pTextRender = this->m_pListBox->GetTextRender();
// 	if (NULL != pTextRender)
// 	{
// 		pTextRender->SetHRFont(this->GetHRFONT());
// 		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
// 	}

	if (rc.Width() < m_pBindObj->GetWidth())  // 将下拉列表的宽度限制为最小与combobox一致
	{
		rc.right = rc.left + m_pBindObj->GetWidth();
		m_pListBox->SetObjectPos(0,0,rc.Width(), rc.Height(),SWP_NOMOVE|SWP_NOREDRAW);
		m_pListBox->UpdateItemRect(NULL);
	}
	this->SetObjectPos(rcWindow.left, rcWindow.bottom, rc.Width(), rc.Height(), 0);
}

void  PopupListBoxWindow::OnListBoxSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
}

void  PopupListBoxWindow::OnFinalMessage()
{
	__super::OnFinalMessage();
}

void PopupListBoxWindow::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	SetMsgHandled(FALSE);

	if (VK_DOWN == nChar || VK_UP == nChar) 
	{
		SetMsgHandled(TRUE);
		m_pListBox->ProcessMessage(m_pCurMsg,0);
	}
	return;
}


//////////////////////////////////////////////////////////////////////////

PopupMenuWindow::PopupMenuWindow(MenuBase* pMenu) : PopupControlWindow(pMenu)
{
	m_pMenu = pMenu;
}
BOOL PopupMenuWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pMenu)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	return bRet;
}
void PopupMenuWindow::OnInitWindow()
{
	__super::OnInitWindow();

	// 清除原窗口上面的hover、press对象
	if (m_pMenu->GetRootMenu() == m_pMenu) // 子菜单弹出来时,activeWindow是父菜单，因此不需要再发送mouseleave消息
	{
		HWND hWnd = GetActiveWindow();
		::PostMessage(hWnd,WM_MOUSELEAVE,0,0);
	}

	CRect rc;
	this->m_pMenu->GetWindowRect(&rc);

	this->SetObjectPos(0, 0, rc.Width(), rc.Height(), SWP_NOMOVE);
}

void PopupMenuWindow::OnFinalMessage()
{
	__super::OnFinalMessage();
}

//
//	注：只有根菜单才会有MenuLoop，子菜单没有
//
BOOL PopupMenuWindow::PreTranslatePopupMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		BOOL bHandle = FALSE;
		UISendMessage(m_pMenu->GetLastMenu(), pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		return bHandle;
	}

	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if ( NULL == m_pMenu->GetMenuByHWND(pMsg->hwnd) )
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
		::GetWindowRect(m_hWnd, &rcWindow);
		if (NULL == m_pMenu->GetMenuByPos(pMsg->pt))  // 鼠标在弹出窗口外面点击了，关闭当前窗口
		{
			this->DestroyPopupWindow();

			// 给原窗口发送一个鼠标移动消息，重置hover对象。
			// 否则会导致popupwnd消失后，原窗口鼠标直接点击无反应
			// 或者导致窗口接收到lbuttondown之前，还没有更新hover对象
			
			HWND hWnd = GetActiveWindow();
			::SendMessage(GetActiveWindow(), WM_MOUSEMOVE, 0, pMsg->lParam);
		}
	}

	return FALSE;
}