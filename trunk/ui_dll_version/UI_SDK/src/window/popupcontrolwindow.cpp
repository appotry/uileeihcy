#include "stdafx.h"

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();
	::PostMessage(m_hWnd, UI_WM_BEGINPOPUPLOOP, 0, 0);
	SetCapture(m_hWnd);

	m_bDestroying = false;
}
void PopupControlWindow::OnFinalMessage()
{
	delete this;
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// ע������Լ��ĸ����ڲ������棬�򲻻���������������ʾͼ�꣬�������û�����WS_EX_TOOLWINDOW����
BOOL PopupControlWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE;
	cs.lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	return __super::PreCreateWindow(cs);
}

void PopupControlWindow::DestroyPopupWindow()
{
	m_bDestroying = true;
	::PostMessage(m_hWnd, UI_WM_DESTROYPOPUPWINDOW, 0, 0);
}
LRESULT PopupControlWindow::OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->ClearTreeObject();
	::DestroyWindow(m_hWnd);
	return 0;
}

LRESULT PopupControlWindow::OnBeginPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PopupLoop();
	return 0;
}

// ע��
//	�����GetMessage��ʵ��ֻ������PostMessage���͹�������Ϣ��
//  ��������SendMessage����Ϣ�ǲ�����֪�ġ�

void PopupControlWindow::PopupLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
		{
			::PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			return;
		}

		bool bBreak = false;
		if (msg.hwnd == m_hWnd && UI_WM_DESTROYPOPUPWINDOW == msg.message)  // ע�����������Ϣ��m_hWnd��Ϊ��
			bBreak = true;

		if (FALSE == this->PreTranslatePopupMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (bBreak)
			break;
	}

}
BOOL PopupControlWindow::PreTranslatePopupMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && WM_KEYLAST >= pMsg->message)
	{
		BOOL bHandle = FALSE;
		UISendMessage(this, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		return bHandle;
	}
	
	if (pMsg->hwnd != m_hWnd)
	{
		if (WM_LBUTTONDOWN   == pMsg->message ||
			WM_LBUTTONDBLCLK == pMsg->message ||
			WM_RBUTTONDOWN   == pMsg->message ||
			WM_RBUTTONDBLCLK == pMsg->message ||
			WM_MBUTTONDOWN   == pMsg->message ||
			WM_MBUTTONDBLCLK == pMsg->message ||
			WM_XBUTTONDOWN   == pMsg->message ||
			WM_XBUTTONDBLCLK == pMsg->message ||
			WM_NCLBUTTONDOWN == pMsg->message ||
			WM_NCRBUTTONDOWN == pMsg->message ||
			WM_NCRBUTTONDBLCLK == pMsg->message )
		{
			this->DestroyPopupWindow();
			return FALSE;
		}
	}
	
	return FALSE;
}

BOOL PopupControlWindow::OnEraseBkgnd(HRDC hRDC)
{
	CRect rcClient;
	this->GetClientRect(&rcClient);

	Rectangle(hRDC, &rcClient, RGB(0,0,0), RGB(255,255,255) );
	return TRUE;
}

PopupListBoxWindow::PopupListBoxWindow(ListBox* pListBox, Object* pBindObj)
{
	m_pBindOb = pBindObj;
	m_pListBox = pListBox;
}

BOOL PopupListBoxWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pListBox || NULL == m_pBindOb)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	cs.lpszClass = WND_POPUP_CONTROL_NAME;  // ������Ӱ
	return bRet;
}
void PopupListBoxWindow::OnInitWindow()
{
	__super::OnInitWindow();

	m_pListBox->AddHook(this, 0, ALT_MSG_ID_LISTBOX);
	this->AddChild(m_pListBox);

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindOb->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindOb->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupListBoxWindow");
	this->SetAttribute(map_temp,false);  // ��ʼ��һЩĬ�ϱ������細������

	TextRenderBase* pTextRender = this->m_pListBox->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	bool bNeedReCalcListBoxRect = false;
	if (rc.Width() < m_pBindOb->GetWidth())  // �������б�Ŀ������Ϊ��С��comboboxһ��
	{
		rc.right = rc.left + m_pBindOb->GetWidth();
		m_pListBox->SetObjectPos(0,0,rc.Width(), rc.Height(),SWP_NOMOVE|SWP_NOREDRAW);
		m_pListBox->UpdateItemRect(NULL);
	}
	this->SetObjectPos(rcWindow.left, rcWindow.bottom, rc.Width(), rc.Height(), 0);
}

void  PopupListBoxWindow::OnListBoxSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
}

LRESULT PopupListBoxWindow::OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	m_pListBox->ClearTreeObject();
	return 0;
}

void PopupListBoxWindow::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	return;
}