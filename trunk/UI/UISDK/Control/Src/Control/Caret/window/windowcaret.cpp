#include "stdafx.h"
#include "windowcaret.h"
#include "3rd\gdiplus\gdiplusfix.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#pragma comment(lib, "gdiplus.lib")

namespace UI
{

/*

Hook API 常用的方法有：

1.  找到API函数在内存中的地址，改写函数头几个字节为JMP指令跳转到自己的代码，
       执行完毕再执行API开头几个字节的内容再跳回原地址。这种方法对CPU有较大的依赖性，
	   而且在多线程环境下可能出问题，当改写函数代码的时候有可能此函数正在被执行，
	   这样做可能导致程序出错。
2.  修改PE文件的IAT (Import Address Table)，使之指向自己的代码，
       这样EXE/DLL在调用系统API的时候便会调用你自己的函数

*/


//////////////////////////////////////////////////////////////////////////
// 使用API HOOK来修改RICHEDIT的一个默认行为

typedef HCURSOR (WINAPI *SetCursorFuncPtr)(HCURSOR hCursor);

char sz_asm_Origin_Instruction[5] = {0}; 
char sz_asm_Jmp_Instruction[5]    = {(char)0xe9}; 
SetCursorFuncPtr  pSetCursorAddr  = NULL; 
long g_lRef_Hooked_SetCursor = 0;   // 是否已经hook了

HCURSOR WINAPI Fixed_SetCursor(HCURSOR hCursor)
{ 
	if (NULL == hCursor)
	{
		return NULL;    // 禁用编辑框的输入时隐藏光标的功能
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

	ReadProcessMemory((void*)-1,  pSetCursorAddr, sz_asm_Origin_Instruction, 5, NULL); // 读出原来的前5个字节 
	WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Jmp_Instruction, 5, NULL);    // 写入我们处理后的5个字节 
}
void UnHookSetCursor()
{
	if ( 0 == (--g_lRef_Hooked_SetCursor) )
	{
		WriteProcessMemory((void*)-1, pSetCursorAddr, sz_asm_Origin_Instruction, 5, NULL); 
	}
}

//////////////////////////////////////////////////////////////////////////
// 光标模拟窗口

CaretWindow::CaretWindow(void)
{
    m_hCtrlWnd = NULL;
    m_pObject = NULL;
	m_ptLast.x = -1;
	m_ptLast.y = -1;
    m_bVisible = false;
    m_bShow = false;
    m_bTestEditVisibleEveryTime = true;

	m_pLayeredWnd = NULL;
	CaretWindow::GetWndClassInfo().m_wc.style = 0;
}

CaretWindow::~CaretWindow(void)
{
	SAFE_DELETE(m_pLayeredWnd);
    if (m_hWnd)
    {
        ::DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }
}
int CaretWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (NULL == m_pLayeredWnd)
	{
		m_pLayeredWnd = new CLayeredWindow;
	}
	m_pLayeredWnd->Attach(m_hWnd);
	m_pLayeredWnd->InitLayered();
	
	::HookSetCursor();
	return 0;
}
void CaretWindow::OnDestroy()
{
	::UnHookSetCursor();

    SyncWindowData data;
    data.m_hWnd = m_hWnd;
    data.m_nAnchorType = ANCHOR_CUSTOM;
    ::SendMessage(m_hCtrlWnd, UI_WM_SYNC_WINDOW, (WPARAM)REMOVE_SYNC_WINDOW, (LPARAM)&data);
}
void CaretWindow::OnPaint(HDC hDC)
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

        bool bVisible = m_bVisible;
        
        if (bVisible)
        {
            Gdiplus::Bitmap bmp(rc.Width(),rc.Height());
            Gdiplus::Graphics g(&bmp);
            Gdiplus::SolidBrush brush(Gdiplus::Color(254,0,0,0));
            g.FillRectangle(&brush, 0,0, rc.Width(), rc.Height());

            Gdiplus::Graphics gDC(hDC);
            gDC.DrawImage(&bmp, rcRender.left, rcRender.top, rcRender.Width(), rcRender.Height());
        }
        else
        {
            ::FillRect(hDC, &rcRender, (HBRUSH)::GetStockObject(BLACK_BRUSH));
        }
    }
}

void  CaretWindow::OnMove(CPoint ptPos)
{   
    m_pLayeredWnd->RedrawLayered();
}
void  CaretWindow::OnTimer(UINT_PTR nIDEvent)
{
    m_bVisible = !m_bVisible;
    m_pLayeredWnd->RedrawLayered();
}	


void CaretWindow::Show(bool bRedraw)
{
	if (NULL == m_hWnd)
		return;

    if (!::IsWindowVisible(m_hWnd))
    {
	    ShowWindow(SW_SHOWNOACTIVATE);
    }
    m_bShow = true;
    m_bVisible = true;
    m_pLayeredWnd->RedrawLayered();

	SetTimer(1, GetCaretBlinkTime());
}
void CaretWindow::Hide(bool bRedraw)
{
	if (NULL == m_hWnd)
		return;

	KillTimer(1);
    m_bShow = false;

    m_bVisible = false;
}
BOOL CaretWindow::Create(IObject* pObj, HWND hWndParent, HBITMAP hbmp, int nWidth, int nHeight)
{
    m_pObject = pObj;
    m_hCtrlWnd = hWndParent;

    if (NULL == m_hWnd)
	{
		RECT rc = {0,0,nWidth, nHeight};
		::OffsetRect(&rc, 100,100);

        __super::Create(hWndParent, rc, 0, WS_POPUP|WS_DISABLED, WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);
		if (m_pLayeredWnd)
			m_pLayeredWnd->RedrawLayered();

		SyncWindowData data;
		data.m_hWnd = m_hWnd;
		data.m_nAnchorType = ANCHOR_CUSTOM;
		::SendMessage(hWndParent, UI_WM_SYNC_WINDOW, (WPARAM)ADD_SYNC_WINDOW, (LPARAM)&data);
	}
	else
	{
		this->SetWindowPos(hWndParent, 0,0, nWidth, nHeight, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSENDCHANGING);
	}

    return TRUE;
}
void CaretWindow::Destroy()
{
	if (NULL == m_hWnd)
		return;

	KillTimer(1);

	SyncWindowData data;
	data.m_hWnd = m_hWnd;
	::SendMessage(GetParent(), UI_WM_SYNC_WINDOW, (WPARAM)REMOVE_SYNC_WINDOW, (LPARAM)&data);

	this->DestroyWindow();
    m_bShow = false;
}

void CaretWindow::SetPos(int x, int y, bool bRedraw)
{
    _SetPos(x, y);
}

void  CaretWindow::_SetPos(int x, int y)
{
	if (NULL == m_hWnd)
		return;

    m_ptLast.x = x;
    m_ptLast.y = y;

    KillTimer(1);

    if (m_pObject)
    {
        POINT ptCtrl = m_pObject->GetRealPosInWindow();

        x += ptCtrl.x;
        y += ptCtrl.y;
    }

	POINT pt = {x,y};
	::MapWindowPoints(m_hCtrlWnd,NULL, &pt, 1);

    UINT nFlag = SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE;
	SetWindowPos(NULL, pt.x, pt.y, 0,0, nFlag);
	
	if (m_bShow) // 在光标显示前，没有定时器，不用恢复
		SetTimer(1,GetCaretBlinkTime());
}

LRESULT CaretWindow::OnSyncWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	HDWP hdwp = (HDWP)wParam;
	CRect* prc = (CRect*)lParam;  // 主窗口的新坐标

	// 计算client rect
	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;
	np.lppos = &wp;
	::CopyRect(&np.rgrc[0], prc);
	::SendMessage(m_hCtrlWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );
	CRect rcClient(np.rgrc[0]);

	POINT pt = {m_ptLast.x, m_ptLast.y};
    if (m_pObject)
    {
        POINT ptCtrl = m_pObject->GetRealPosInWindow();
        pt.x += ptCtrl.x;
        pt.y += ptCtrl.y;
    }

	pt.x += rcClient.left;
	pt.y += rcClient.top;

	UINT nFlag = SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE;
	hdwp = DeferWindowPos(hdwp,  NULL, pt.x, pt.y, 0,0, nFlag);

	return (LRESULT)hdwp;
}

void CaretWindow::OnWindowMove()
{
	this->SetPos(m_ptLast.x, m_ptLast.y, false);
}
void CaretWindow::OnControlMove()
{

}


}