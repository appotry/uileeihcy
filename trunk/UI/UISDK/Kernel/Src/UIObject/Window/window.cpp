#include "stdafx.h"
#include "window.h"

Window::Window()
{
    m_pIWindow = NULL;
	m_pBtnSystemMaximize = NULL;
	m_pBtnSystemRestore  = NULL;
	m_pBtnSystemMinimize = NULL;
	m_pBtnSystemClose = NULL;
}

Window::~Window(void)
{
}

void Window::OnInnerInitWindow()
{
	this->HandleSysBtnStyle();

	// 默认显示最大化按钮，隐藏还原按钮
    if (::IsZoomed(m_hWnd))
    {
	    if (m_pBtnSystemMaximize)
		    m_pBtnSystemMaximize->SetVisible(false, false);
    }
    else
    {
        if (m_pBtnSystemRestore)
            m_pBtnSystemRestore->SetVisible(false, false);
    }
}

void Window::_OnSkinChanged()
{
	SetMsgHandled(FALSE);
	this->HandleSysBtnStyle();
}

void Window::HandleSysBtnStyle()
{
    m_pBtnSystemMinimize = NULL;
    m_pBtnSystemRestore = NULL;
    m_pBtnSystemMaximize = NULL;
    m_pBtnSystemClose = NULL;

	Object* p = this->_findChildObjectItem(XML_SYS_BTN_MINIMIZE);  // 避免调用 FindChildObjectItem 失败时会写warn log
    if (p)
        m_pBtnSystemMinimize = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_RESTORE);
    if (p)
        m_pBtnSystemRestore = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_MAXIMIZE);
    if (p)
        m_pBtnSystemMaximize = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_CLOSE);
    if (p)
        m_pBtnSystemClose = p->GetIObject();

	// 去除这些按钮的TABSTOP属性
	if (m_pBtnSystemClose)
		m_pBtnSystemClose->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
	if (m_pBtnSystemRestore)
		m_pBtnSystemRestore->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
	if (m_pBtnSystemMinimize)
		m_pBtnSystemMinimize->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
	if (m_pBtnSystemMaximize)
		m_pBtnSystemMaximize->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);

	// 根据系统按钮的数据决定窗口样式
	UINT nAdd = 0, nRemove = 0;

	if (m_pBtnSystemMinimize)
	{
		nAdd |= WS_MINIMIZEBOX;
	}
	else
	{
		nRemove |= WS_MINIMIZEBOX;
	}

	if (m_pBtnSystemMaximize)
	{
		nAdd |= WS_MAXIMIZEBOX;
	}
	else
	{
		nRemove |= WS_MAXIMIZEBOX;
	}

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= nAdd;
	lStyle &= ~nRemove;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
//	SetWindowPos(m_hWnd, 0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
}
void Window::_OnLButtonDblClk(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (::IsZoomed(m_hWnd))
	{
		OnSysRestore();
	}
	else if (m_pBtnSystemMaximize)
	{
		OnSysMaximize();
	}
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               公共方法                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


void  Window::OnSysClose()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_CLOSE, 0 );
}
void  Window::OnSysMinimize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MINIMIZE, 0 );
}
void Window::OnSysMaximize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MAXIMIZE, 0 );
}
void Window::OnSysRestore()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_RESTORE, 0 );
}

void Window::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);
	if (SC_MAXIMIZE == nID)
	{
		// 隐藏显示最大化按钮，显示还原按钮
		if (m_pBtnSystemMaximize)
			m_pBtnSystemMaximize->SetVisible(false, false, false);

		if (m_pBtnSystemRestore)
			m_pBtnSystemRestore->SetVisible(true, true, true);
	}
	else if(SC_RESTORE == nID && !IsIconic(m_hWnd))  // Win7最大化时，再最小化，此时的还原是显示窗口
	{
		// 隐藏显示还原按钮，显示最大化按钮
		if (m_pBtnSystemRestore)
			m_pBtnSystemRestore->SetVisible(false, false, false);

		if (m_pBtnSystemMaximize)
			m_pBtnSystemMaximize->SetVisible(true, true, false);
	}
	else if(SC_CLOSE == nID)
	{
		// 不处理，先由外部处理WM_CLOSE消息
	}
}

void  Window::OnClose()
{
    SetMsgHandled(FALSE);  // 默认处理会调用DestroyWindow
    if (m_bDoModal)
    {
        EndDialog(IDCANCEL);
    }

    if (m_pUIApplication->IsDesignMode())   // 在编辑器中的窗口预览模式。该窗口由外部创建，销毁时通知外部释放
    {
        HWND hNotifyWnd = (HWND) m_pUserData;
        if (::IsWindow(hNotifyWnd))
            ::PostMessage(hNotifyWnd, UI_WM_PREVIEW_WINDOW_DESTROY, (WPARAM)m_pIWindow, 0);
    }
}