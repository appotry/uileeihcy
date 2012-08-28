#include "stdafx.h"

PopupControlWindow::PopupControlWindow(Object* pObj)
{
	m_pObject = pObj;
}


void PopupControlWindow::OnFinalMessage()
{
	m_pObject->ClearTreeObject();
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

void PopupControlWindow::DestroyPopupWindow()
{
	// 退出消息循环，并销毁窗口
	::PostMessage(m_hWnd, UI_WM_EXITPOPUPLOOP, 0, 0);
}

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();

	// 清除原窗口上面的hover、press对象
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);

	m_pObject->AddHook(this, 0, 1);
	this->AddChild(m_pObject);

	// 准备进入消息循环
	::PostMessage(m_hWnd, UI_WM_ENTERPOPUPLOOP, 0, 0);
}

LRESULT PopupControlWindow::OnEnterPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UISendMessage(m_pObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);

	PopupLoop();
	return 0;
}

LRESULT PopupControlWindow::OnExitPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->ClearTreeObject();
	::DestroyWindow(m_hWnd);

	// 通知对象窗口被销毁
	UISendMessage(m_pObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);

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

		bool bBreak = false;
		if (UI_WM_EXITPOPUPLOOP == msg.message)  // 注：处理完该消息后，m_hWnd将为空，因此先判断再执行
		{
			if (msg.hwnd == m_hWnd)
				bBreak = true;
		}

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
// 			POINT pt;
// 			GetCursorPos(&pt);
			HWND hWnd = GetActiveWindow();
//			MapWindowPoints(NULL, hWnd, &pt, 1);
			::SendMessage(GetActiveWindow(), WM_MOUSEMOVE, 0, pMsg->lParam/*MAKELPARAM(pt.x, pt.y)*/);
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

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindObj->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindObj->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupListBoxWindow");
	this->SetAttribute(map_temp,false);  // 初始化一些默认变量，如窗口字体

	TextRenderBase* pTextRender = this->m_pListBox->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

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

	this->SetObjectPos(0, 0, rc.Width(), rc.Height(), SWP_NOMOVE);
}

void PopupMenuWindow::OnFinalMessage()
{
	__super::OnFinalMessage();
}