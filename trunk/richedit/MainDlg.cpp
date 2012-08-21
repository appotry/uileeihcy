#include "StdAfx.h"
#include "MainDlg.h"
#define WINDOWLESS_RICHEDIT_RECT  100,100,300,200


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
	this->InitLayered(240);

	CRect rc;
	this->GetClientRect(&rc);
	rc.top = rc.bottom/2;
// 	if (false == m_richedit.CreateControl(m_hWnd, rc, 1001))
// 	{
// 		assert(0);
// 	}
// 	m_richedit.ShowWindow(SW_SHOW);

	CRect rcWindowless(WINDOWLESS_RICHEDIT_RECT);
	m_windowlessRichedit.Create(m_hWnd, rcWindowless);

	::SetFocus(m_hWnd);
	CenterWindow(NULL);

	this->RedrawLayered();

	CRect rcWindow;
	this->GetWindowRect(&rcWindow);

	SetTimer(1, 1000);
	return FALSE;
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	if(m_richedit.IsWindow())
	{
		m_richedit.SetWindowPos(NULL, 0,size.cy/2, size.cx,size.cy/2,SWP_NOZORDER);
	}
}
UINT CMainDlg::OnNcHitTest(POINT point)
{
	::ScreenToClient(m_hWnd, &point);
	if (point.y < 30)
		return HTCAPTION;

	else
		SetMsgHandled(FALSE);

	return 0;
}

void CMainDlg::OnPaint(HDC hDC)
{
	if (NULL == hDC)
	{
		PAINTSTRUCT  ps;
		HDC hDC = ::BeginPaint(m_hWnd, &ps);
		m_windowlessRichedit.Draw(hDC);
		::EndPaint(m_hWnd, &ps);
	}
	else  /// ·Ö²ã´°¿Ú»æÖÆ
	{
		CRect rc;
		::GetWindowRect(m_hWnd, &rc);

		Gdiplus::Bitmap bmp(rc.Width(),rc.Height());
		Gdiplus::Graphics g(&bmp);

		Gdiplus::SolidBrush brush(Gdiplus::Color(255,240,240,240));
		g.FillRectangle(&brush, 0,0, rc.Width(), rc.Height());

		Gdiplus::Pen pen(Gdiplus::Color(255,0,0,0));
		CRect rcBorder(WINDOWLESS_RICHEDIT_RECT);
		::InflateRect(&rcBorder,1,1);
		g.DrawRectangle(&pen, rcBorder.left, rcBorder.top, rcBorder.Width(), rcBorder.Height());

		HDC hTempDC = g.GetHDC();
		m_windowlessRichedit.Draw(hTempDC);
		g.ReleaseHDC(hTempDC);

		Gdiplus::Graphics gDC(hDC);
		gDC.DrawImage(&bmp, 0,0, rc.Width(), rc.Height());
	}
}

LRESULT CMainDlg::OnInvalidate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	this->RedrawLayered();
	return 0;
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
// 	int nRander = (rand()%500);
// 	HDWP h = BeginDeferWindowPos(3);
// 	::DeferWindowPos(h, m_hWnd,0,0,0,0,0,SWP_NOSENDCHANGING|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
// 	::DeferWindowPos(h, m_hWnd,0,nRander,nRander,nRander,nRander,SWP_NOACTIVATE|SWP_NOZORDER);
// 	::DeferWindowPos(h, m_hWnd,0,0,0,0,0,SWP_NOSENDCHANGING|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
// 	::EndDeferWindowPos(h);
// 
// //	::SetWindowPos(m_hWnd,HWND_BOTTOM,nRander,nRander,nRander,nRander,SWP_NOMOVE|SWP_NOSIZE|SWP_DEFERERASE|SWP_NOACTIVATE);
// 
// 	CRect  rcWindow;
// 	this->GetWindowRect(&rcWindow);
// 	TCHAR szInfo[MAX_PATH];
// 	_stprintf(szInfo, _T("window rect: %d,%d,%d,%d\n"), rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height());
// 	::OutputDebugString(szInfo);
}