#include "StdAfx.h"
#include "control\CaretWindow.h"

#define USE_LAYERED_CARET_WINDOW  // <--  ��ʶ��괰���Ƿ�ʹ�÷ֲ㴰����ʵ�֣�����ʹ����ͨ������ʵ��

//////////////////////////////////////////////////////////////////////////
// ʹ��API HOOK���޸�RICHEDIT��һ��Ĭ����Ϊ

typedef HCURSOR (WINAPI *SetCursorFuncPtr)(HCURSOR hCursor);

char sz_asm_Origin_Instruction[5] = {0}; 
char sz_asm_Jmp_Instruction[5]    = {(char)0xe9}; 
SetCursorFuncPtr  pSetCursorAddr  = NULL; 
long g_lRef_Hooked_SetCursor = 0;   // �Ƿ��Ѿ�hook��

HCURSOR WINAPI Fixed_SetCursor(HCURSOR hCursor)
{ 
	if (NULL == hCursor)
	{
		return NULL;    // ���ñ༭�������ʱ���ع��Ĺ���
	}

	WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Origin_Instruction, 5, NULL); 
	HCURSOR h = ::SetCursor(hCursor); 
	WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Jmp_Instruction, 5, NULL); 
	return h; 
} 

void HookSetCursor()
{
	if (g_lRef_Hooked_SetCursor++ > 0)
	{
		return;
	}

	DWORD   dwJmpAddr = 0; 
	HMODULE hModule   = GetModuleHandle(_T("User32.Dll")); 
	pSetCursorAddr    = (SetCursorFuncPtr)GetProcAddress(hModule, "SetCursor"); 

	dwJmpAddr = (DWORD)Fixed_SetCursor - (DWORD)pSetCursorAddr - 5; 
	memcpy(sz_asm_Jmp_Instruction + 1, &dwJmpAddr, 4); 

	ReadProcessMemory((void*)-1,  pSetCursorAddr, sz_asm_Origin_Instruction, 5, NULL); // ����ԭ����ǰ5���ֽ� 
	WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Jmp_Instruction, 5, NULL);    // д�����Ǵ�����5���ֽ� 
}
void UnHookSetCursor()
{
	if ( 0 == (--g_lRef_Hooked_SetCursor) )
	{
		WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Origin_Instruction, 5, NULL); 
	}
}

//////////////////////////////////////////////////////////////////////////
// ���ģ�ⴰ��

CCaretWindow::CCaretWindow(void)
{
	m_ptOldPos.x = 0;
	m_ptOldPos.y = 0;

	CCaretWindow::GetWndClassInfo().m_wc.style = 0;
}

CCaretWindow::~CCaretWindow(void)
{
}

int CCaretWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef USE_LAYERED_CARET_WINDOW
	this->InitLayered();
#endif
	
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
		Gdiplus::SolidBrush brush(Gdiplus::Color(254,0,0,0));
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
	{
		::ShowWindow(m_hWnd, SW_HIDE);
	}
	else
	{
		if (FALSE == ::IsWindowVisible(GetParent()))  // �����������ˣ����Ҳ������ʾ
		{
			this->DestroyCaret();
		}
		
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	}
}	

void CCaretWindow::ShowCaret()
{
	if (NULL == m_hWnd)
		return;

	ShowWindow(SW_SHOWNOACTIVATE);
	SetTimer(1, GetCaretBlinkTime());

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

#ifdef USE_LAYERED_CARET_WINDOW
		this->Create(hWndParent, rc, 0, WS_POPUP|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);
		this->RedrawLayered();
#else
		this->Create(hWndParent, rc, 0, WS_POPUP|WS_DISABLED, WS_EX_TOOLWINDOW);
#endif
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

	KillTimer(1);
	this->DestroyWindow();
}

void CCaretWindow::SetCaretPos(int x, int y)
{
	if (NULL == m_hWnd)
		return;

	BOOL bRet = KillTimer(1);  // �������TRUE�����ʾ�������ʾ

	POINT pt = {x,y};
	::MapWindowPoints(GetParent(),NULL, &pt, 1);

	UINT nFlag = SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE;
	if (bRet) nFlag |= SWP_SHOWWINDOW;  // �ڹ����ʾǰ����Ҫ�Ӹñ�־���ڹ����ʾ��ÿ���ƶ�λ�ú������ʾ��꣬�Ա�ʶ�����λ�õĸı�

	SetWindowPos(NULL, pt.x, pt.y, 0,0, nFlag);
	
	if (bRet)                 // �ڹ����ʾǰ��û�ж�ʱ�������ûָ�
		SetTimer(1,GetCaretBlinkTime());

	m_ptOldPos.x = x;
	m_ptOldPos.y = y;
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

#ifdef _DEBUG
	SetLayered(true);
#else
	if(GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED)
		SetLayered(true);
	else
		SetLayered(false);
#endif

	if (m_bLayered)
	{
		if (NULL == hbmp)   // �����Ϊ�գ���ǰ��richedit����ѡ�����򣬲���ʾ���
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
// 	if (IsLayered())
// 	{
//		m_caretwnd.SetTextHost(pTextHost);
// 	}
}