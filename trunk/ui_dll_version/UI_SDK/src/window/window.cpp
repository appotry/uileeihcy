#include "stdafx.h"

Window::Window(void)
{
	m_pBtnSystemMaximize = NULL;
	m_pBtnSystemRestore  = NULL;
	m_pBtnSystemMinimize = NULL;
}

Window::~Window(void)
{
}

void Window::OnInitWindow()
{	
	__super::OnInitWindow();

	this->HandleSysBtnStyle();

	// 默认显示最大化按钮，隐藏还原按钮
	if (NULL != m_pBtnSystemRestore)
		m_pBtnSystemRestore->SetVisible(false, false);
}

void Window::_OnSkinChanged()
{
	SetMsgHandled(FALSE);
	this->HandleSysBtnStyle();
}

void Window::HandleSysBtnStyle()
{
	m_pBtnSystemMinimize = this->FindChildObject(XML_SYS_BTN_MINIMIZE);
	m_pBtnSystemRestore = this->FindChildObject(XML_SYS_BTN_RESTORE);
	m_pBtnSystemMaximize = this->FindChildObject(XML_SYS_BTN_MAXIMIZE);

	UINT nAdd = 0, nRemove = 0;

	if (NULL != m_pBtnSystemMinimize)
	{
		nAdd |= WS_MINIMIZEBOX;
	}
	else
	{
		nRemove |= WS_MINIMIZEBOX;
	}

	if (NULL != m_pBtnSystemMaximize)
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
	else if (NULL != m_pBtnSystemMaximize)
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
		if (NULL != m_pBtnSystemMaximize)
			m_pBtnSystemMaximize->SetVisible(false, false, false);

		if (NULL != m_pBtnSystemRestore)
			m_pBtnSystemRestore->SetVisible(true, true, true);
	}
	else if(SC_RESTORE == nID)
	{
		// 隐藏显示还原按钮，显示最大化按钮
		if (NULL != m_pBtnSystemRestore)
			m_pBtnSystemRestore->SetVisible(false, false, false);

		if (NULL != m_pBtnSystemMaximize)
			m_pBtnSystemMaximize->SetVisible(true, true, false);
	}
	else if(SC_CLOSE == nID)
	{
		if (m_bDoModal)
			EndDialog(IDCANCEL);
	}
}