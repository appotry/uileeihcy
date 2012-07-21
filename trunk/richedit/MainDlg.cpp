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

void CMainDlg::OnSize(UINT nType, CSize size)
{
	if(m_richedit.IsWindow())
	{
		m_richedit.SetWindowPos(NULL, 0,0, size.cx,size.cy,SWP_NOZORDER);
	}
}