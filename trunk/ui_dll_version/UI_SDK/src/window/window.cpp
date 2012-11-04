#include "stdafx.h"

Window::Window(void)
{
}

Window::~Window(void)
{
}

void Window::OnInitWindow()
{	
	__super::OnInitWindow();

	// 默认显示最大化按钮，隐藏还原按钮
	Button* pButton = (Button*)this->FindChildObject(_T("sys_restore"));
	if (NULL != pButton)
		pButton->SetVisible(false, false);
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
	if (m_bDoModal)
		EndDialog(IDCANCEL);
	else
		::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_CLOSE, 0 );
	
}
void  Window::OnSysMinimize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MINIMIZE, 0 );
}
void Window::OnSysMaximize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MAXIMIZE, 0 );

	// 隐藏显示最大化按钮，显示还原按钮
	Button* pBtnRestore = (Button*)this->FindChildObject(_T("sys_restore"));
	if (NULL != pBtnRestore)
		pBtnRestore->SetVisible(true, false);

	Button* pButtonMax = (Button*)this->FindChildObject(_T("sys_maximize"));
	if (NULL != pButtonMax)
		pButtonMax->SetVisible(false, true);
}
void Window::OnSysRestore()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_RESTORE, 0 );

	// 隐藏显示还原按钮，显示最大化按钮
	Button* pButtonMax = (Button*)this->FindChildObject(_T("sys_maximize"));
	if (NULL != pButtonMax)
		pButtonMax->SetVisible(true, false);

	Button* pBtnRestore = (Button*)this->FindChildObject(_T("sys_restore"));
	if (NULL != pBtnRestore)
		pBtnRestore->SetVisible(false, true);
}

