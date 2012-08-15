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
	this->DestroyWindow();
}
void CMainDlg::OnDestroy()
{
	::PostQuitMessage(0);
}

BOOL CMainDlg::OnInitDialog(HWND wndFocus, LPARAM)
{
	CRect rc;
	this->GetClientRect(&rc);
	rc.top = rc.bottom/2;
// 	if (false == m_richedit.CreateControl(m_hWnd, rc, 1001))
// 	{
// 		assert(0);
// 	}
// 	m_richedit.ShowWindow(SW_SHOW);

	CRect rcWindowless(100,100,400,400);
	m_windowlessRichedit.Create(m_hWnd, rcWindowless);

	::SetFocus(m_hWnd);
	CenterWindow(NULL);
	return FALSE;
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	if(m_richedit.IsWindow())
	{
		m_richedit.SetWindowPos(NULL, 0,size.cy/2, size.cx,size.cy/2,SWP_NOZORDER);
	}
}
void CMainDlg::OnPaint(HDC)
{
	PAINTSTRUCT  ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);
	m_windowlessRichedit.Draw(hDC);
	::EndPaint(m_hWnd, &ps);
}
