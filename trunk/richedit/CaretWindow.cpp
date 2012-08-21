#include "StdAfx.h"
#include "CaretWindow.h"
#include "TextServ.h"

typedef HCURSOR (WINAPI *pHook_SetCursor)(HCURSOR hCursor);

char szOriginSetCursor[5] = {0}; 
char szJmpSetCursor[5] = {(char)0xe9}; 
pHook_SetCursor pSetCursorAddr = NULL; 

HCURSOR WINAPI Hook_SetCursor(HCURSOR hCursor)
{ 
	if( NULL == hCursor)
	{
		::OutputDebugString(_T("Hook_SetCursor\n"));
		return NULL;
	}

	WriteProcessMemory((void*)-1, pSetCursorAddr, szOriginSetCursor, 5, NULL); 
	HCURSOR h = SetCursor(hCursor); 
	WriteProcessMemory((void*)-1, pSetCursorAddr, szJmpSetCursor, 5, NULL); 
	return h; 
} 

void HookSetCursor()
{
	DWORD   dwJmpAddr = 0; 
	HMODULE hModule = GetModuleHandle(_T("User32.Dll")); 
	pSetCursorAddr = (pHook_SetCursor)GetProcAddress(hModule, "SetCursor"); 
	dwJmpAddr = (DWORD)Hook_SetCursor - (DWORD)pSetCursorAddr - 5; 
	memcpy(szJmpSetCursor + 1, &dwJmpAddr, 4); 
	ReadProcessMemory((void*)-1, pSetCursorAddr, szOriginSetCursor, 5, NULL);//读出原来的前5个字节 
	WriteProcessMemory((void*)-1, pSetCursorAddr, szJmpSetCursor, 5, NULL);//写入我们处理后的5个字节 
}
void UnHookSetCursor()
{
	WriteProcessMemory((void*)-1, pSetCursorAddr, szOriginSetCursor, 5, NULL); 
}

CCaretWindow::CCaretWindow(void)
{
	m_pTextHost = NULL;	
	m_ptOldPos.x = 0;
	m_ptOldPos.y = 0;

	CCaretWindow::GetWndClassInfo().m_wc.style = 0;
}

CCaretWindow::~CCaretWindow(void)
{
	m_pTextHost = NULL;
}

int CCaretWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	this->InitLayered();
	
	::HookSetCursor();
	return 0;
}
void CCaretWindow::OnDestroy()
{
	::UnHookSetCursor();
}
void CCaretWindow::OnPaint(HDC hDC)
{
	CRect  rc;
	this->GetClientRect(&rc);

	if (NULL == hDC)
	{
		PAINTSTRUCT  ps;
		HDC hDC = ::BeginPaint(m_hWnd, &ps);
		::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
		::EndPaint(m_hWnd, &ps);
	}
	else
	{
		CRect  rcRender(0,0, rc.Width(), rc.Height());

		Gdiplus::Bitmap bmp(rc.Width(),rc.Height());
		Gdiplus::Graphics g(&bmp);
		Gdiplus::SolidBrush brush(Gdiplus::Color(192,0,0,0));
		g.FillRectangle(&brush, 0,0, rc.Width(), rc.Height());

// 		HDC hTempDC = g.GetHDC();
// 		g.ReleaseHDC(hTempDC);

		Gdiplus::Graphics gDC(hDC);
		gDC.DrawImage(&bmp, rcRender.left, rcRender.top, rcRender.Width(), rcRender.Height());
	}
}

void CCaretWindow::OnTimer(UINT_PTR nIDEvent)
{
	if (IsWindowVisible())
		::ShowWindow(m_hWnd, SW_HIDE);
	else
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
}	

void CCaretWindow::ShowCaret()
{
	if (NULL == m_hWnd)
		return;

	ShowWindow(SW_SHOWNOACTIVATE);
	SetTimer(1,GetCaretBlinkTime());

}
void CCaretWindow::HideCaret()
{
	if (NULL == m_hWnd)
		return;

	KillTimer(1);
	ShowWindow(SW_HIDE);
}
void CCaretWindow::CreateCaret(HWND hWndParent, int nWidth, int nHeight)
{
	if (NULL == m_hWnd)
	{
		RECT rc = {0,0,nWidth, nHeight};
		::OffsetRect(&rc, 100,100);
		this->Create(hWndParent, rc, 0, WS_POPUP|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);
	}
	else
	{
		this->SetWindowPos(hWndParent, 0,0, nWidth, nHeight, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSENDCHANGING);
	}
}
void CCaretWindow::DestroyCaret()
{
	if (NULL == m_hWnd)
		return;

	this->DestroyWindow();

}

void CCaretWindow::SetCaretPos(int x, int y)
{
	if (NULL == m_hWnd)
		return;

	POINT pt = {x,y};
	::MapWindowPoints(GetParent(),NULL, &pt, 1);

// 	if (NULL == GetCursor())
// 	{
// 		g_b = true;
// 	}
	SetWindowPos(NULL, pt.x, pt.y, 0,0, SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
// 	if (b)
// 	{
// 	}
	
	m_ptOldPos.x = x;
	m_ptOldPos.y = y;
	this->RedrawLayered();
}

void CCaretWindow::OnWindowMove()
{
	this->SetCaretPos(m_ptOldPos.x, m_ptOldPos.y);
}
void CCaretWindow::OnControlMove()
{

}

//////////////////////////////////////////////////////////////////////////

void CCaret::CreateCaret(HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
	m_hWnd = hWndParent;

	if (m_bLayered)
	{
		if (NULL == hbmp)   // 如果不为空，当前在richedit中有选中区域，不显示光标
		{
			m_caretwnd.CreateCaret(hWndParent, nWidth, nHeight);
		}
		else
		{
			m_caretwnd.DestroyCaret();
		}
	}
	else
	{
		::CreateCaret(hWndParent, hbmp, nWidth, nHeight);
	}
}
void CCaret::DestroyCaret()
{
	if (m_bLayered)
	{
		m_caretwnd.DestroyCaret();
	}
	else
	{
		::DestroyCaret();
	}
	m_hWnd = NULL;
}
void CCaret::SetCaretPos(int x, int y)
{
	if (m_bLayered)
	{
		m_caretwnd.SetCaretPos(x, y);
	}
	else
	{
		::SetCaretPos(x, y);
	}
}
void CCaret::ShowCaret()
{
	if (m_bLayered)
	{
		m_caretwnd.ShowCaret();
	}
	else
	{
		::ShowCaret(m_hWnd);
	}

}
void CCaret::HideCaret()
{
	if (m_bLayered)
	{
		m_caretwnd.HideCaret();
	}
	else
	{
		::HideCaret(m_hWnd);
	}
}

void CCaret::OnWindowMove()
{
	if (IsLayered())
	{
		m_caretwnd.OnWindowMove();
	}
}
void CCaret::OnControlMove()
{
	
}

void CCaret::SetTextHost(ITextHost* pTextHost)
{	
	if (IsLayered())
	{
		m_caretwnd.SetTextHost(pTextHost);
	}
}