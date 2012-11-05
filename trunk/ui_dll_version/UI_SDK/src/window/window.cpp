#include "stdafx.h"

Window::Window(void)
{
	m_pBtnSystemMaximize = NULL;
	m_pBtnSystemRestore  = NULL;
}

Window::~Window(void)
{
}

void Window::OnInitWindow()
{	
	__super::OnInitWindow();

	m_pBtnSystemRestore = this->FindChildObject(XML_SYS_BTN_RESTORE);
	m_pBtnSystemMaximize = this->FindChildObject(XML_SYS_BTN_MAXIMIZE);

	// Ĭ����ʾ��󻯰�ť�����ػ�ԭ��ť
	if (NULL != m_pBtnSystemRestore)
		m_pBtnSystemRestore->SetVisible(false, false);
}

void Window::_OnSkinChanged()
{
	SetMsgHandled(FALSE);
	m_pBtnSystemRestore = this->FindChildObject(XML_SYS_BTN_RESTORE);
	m_pBtnSystemMaximize = this->FindChildObject(XML_SYS_BTN_MAXIMIZE);
}

void Window::_OnLButtonDblClk(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (::IsZoomed(m_hWnd))
	{
		OnSysRestore();
	}
	else
	{
		OnSysMaximize();
	}
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
	if (NULL != m_pBtnSystemRestore)
		m_pBtnSystemRestore->SetVisible(true, false);

	if (NULL != m_pBtnSystemMaximize)
		m_pBtnSystemMaximize->SetVisible(false, true);
}
void Window::OnSysRestore()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_RESTORE, 0 );

	// ������ʾ��ԭ��ť����ʾ��󻯰�ť
	if (NULL != m_pBtnSystemMaximize)
		m_pBtnSystemMaximize->SetVisible(true, false);

	if (NULL != m_pBtnSystemRestore)
		m_pBtnSystemRestore->SetVisible(false, true);
}

