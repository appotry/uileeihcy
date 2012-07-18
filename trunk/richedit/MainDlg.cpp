#include "StdAfx.h"
#include "MainDlg.h"

CMainDlg::CMainDlg(void)
{
}

CMainDlg::~CMainDlg(void)
{
}

void CMainDlg::OnClose()
{
	::PostQuitMessage(0);
}

BOOL CMainDlg::OnInitDialog(HWND wndFocus, LPARAM)
{
	CRect rc;
	this->GetClientRect(&rc);
	if (false == m_richedit.CreateControl(m_hWnd, rc, 1001))
		assert(0);
	
	m_richedit.ShowWindow(SW_SHOW);
	CenterWindow(NULL);
	return TRUE;
}