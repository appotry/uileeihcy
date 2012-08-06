#include "stdafx.h"

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();
	::PostMessage(m_hWnd, UI_WM_BEGINPOPUPLOOP, 0, 0);

	m_bDestroying = false;
}
void PopupControlWindow::OnFinalMessage()
{
	delete this;
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// 注：如果自己的父窗口不是桌面，则不会在任务栏上面显示图标，因此这里没有添加WS_EX_TOOLWINDOW属性
BOOL PopupControlWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE;
	cs.lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	return __super::PreCreateWindow(cs);
}

void PopupControlWindow::DestroyPopupWindow()
{
	m_bDestroying = true;
	::PostMessage(m_hWnd, UI_WM_DESTROYPOPUPWINDOW, 0, 0);
}
LRESULT PopupControlWindow::OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->ClearTreeObject();
	::DestroyWindow(m_hWnd);
	return 0;
}

LRESULT PopupControlWindow::OnBeginPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PopupLoop();
	return 0;
}

// 注：
//	这里的GetMessage其实是只能拦截PostMessage发送过来的消息，
//  对于其它SendMessage的消息是不法得知的。

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

		bool bBreak = false;
		if (msg.hwnd == m_hWnd && UI_WM_DESTROYPOPUPWINDOW == msg.message)  // 注：处理完该消息后，m_hWnd将为空
			bBreak = true;

		if (FALSE == this->PreTranslatePopupMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (bBreak)
			break;
	}

}
BOOL PopupControlWindow::PreTranslatePopupMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		BOOL bHandle = FALSE;
		UISendMessage(this, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		return bHandle;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message ||
 		/*WM_MOUSELEAVE == pMsg->message ||  */
 		WM_NCMOUSELEAVE == pMsg->message
		)
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

PopupListBoxWindow::PopupListBoxWindow(ListBox* pListBox, Object* pBindObj)
{
	m_pBindOb = pBindObj;
	m_pListBox = pListBox;
}

BOOL PopupListBoxWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pListBox || NULL == m_pBindOb)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	cs.lpszClass = WND_POPUP_CONTROL_NAME;  // 不加阴影
	return bRet;
}
void PopupListBoxWindow::OnInitWindow()
{
	__super::OnInitWindow();

	m_pListBox->AddHook(this, 0, ALT_MSG_ID_LISTBOX);
	this->AddChild(m_pListBox);

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindOb->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindOb->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupListBoxWindow");
	this->SetAttribute(map_temp,false);  // 初始化一些默认变量，如窗口字体

	TextRenderBase* pTextRender = this->m_pListBox->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	if (rc.Width() < m_pBindOb->GetWidth())  // 将下拉列表的宽度限制为最小与combobox一致
	{
		rc.right = rc.left + m_pBindOb->GetWidth();
		m_pListBox->SetObjectPos(0,0,rc.Width(), rc.Height(),SWP_NOMOVE|SWP_NOREDRAW);
		m_pListBox->UpdateItemRect(NULL);
	}
	this->SetObjectPos(rcWindow.left, rcWindow.bottom, rc.Width(), rc.Height(), 0);
}

void  PopupListBoxWindow::OnListBoxSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
}

LRESULT PopupListBoxWindow::OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	m_pListBox->ClearTreeObject();

//	UISendMessage(m_pBindOb, UI_WM_EXIT_
	return 0;
}

void PopupListBoxWindow::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	return;
}


//////////////////////////////////////////////////////////////////////////

PopupMenuWindow::PopupMenuWindow(MenuBase* pMenu)
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

	m_pMenu->AddHook(this, 0, ALT_MSG_ID_MENU);
	this->AddChild(m_pMenu);

	CRect rc;
	this->m_pMenu->GetWindowRect(&rc);


	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupMenuWindow");
	this->SetAttribute(map_temp,false);  // 初始化一些默认变量，如窗口字体

	TextRenderBase* pTextRender = this->m_pMenu->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}


	this->SetObjectPos(0, 0, 100,100/*rc.Width(), rc.Height()*/, SWP_NOMOVE);
}
LRESULT PopupMenuWindow::OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	m_pMenu->ClearTreeObject();
	return 0;
}