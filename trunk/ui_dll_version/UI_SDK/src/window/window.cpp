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

	// Ĭ����ʾ��󻯰�ť�����ػ�ԭ��ť
	Button* pButton = (Button*)this->FindChildObject(_T("sys_restore"));
	if (NULL != pButton)
		pButton->SetVisible(false, false);
}
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��������                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��Ϣӳ��                               //
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

	// ������ʾ��󻯰�ť����ʾ��ԭ��ť
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

	// ������ʾ��ԭ��ť����ʾ��󻯰�ť
	Button* pButtonMax = (Button*)this->FindChildObject(_T("sys_maximize"));
	if (NULL != pButtonMax)
		pButtonMax->SetVisible(true, false);

	Button* pBtnRestore = (Button*)this->FindChildObject(_T("sys_restore"));
	if (NULL != pBtnRestore)
		pBtnRestore->SetVisible(false, true);
}

