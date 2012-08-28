#include "stdafx.h"

PopupControlWindow::PopupControlWindow(Object* pObj)
{
	m_pObject = pObj;
}


void PopupControlWindow::OnFinalMessage()
{
	m_pObject->ClearTreeObject();
	m_pObject = NULL;
	delete this;
}

//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
// ע������Լ��ĸ����ڲ������棬�򲻻���������������ʾͼ��
BOOL PopupControlWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW;
	cs.lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;
	return __super::PreCreateWindow(cs);
}

void PopupControlWindow::DestroyPopupWindow()
{
	// �˳���Ϣѭ���������ٴ���
	::PostMessage(m_hWnd, UI_WM_EXITPOPUPLOOP, 0, 0);
}

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();

	// ���ԭ���������hover��press����
	HWND hWnd = GetActiveWindow();
	::PostMessage(hWnd,WM_MOUSELEAVE,0,0);

	m_pObject->AddHook(this, 0, 1);
	this->AddChild(m_pObject);

	// ׼��������Ϣѭ��
	::PostMessage(m_hWnd, UI_WM_ENTERPOPUPLOOP, 0, 0);
}

LRESULT PopupControlWindow::OnEnterPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UISendMessage(m_pObject, UI_WM_INITPOPUPCONTROLWINDOW, 0,0,0, this);

	PopupLoop();
	return 0;
}

LRESULT PopupControlWindow::OnExitPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->ClearTreeObject();
	::DestroyWindow(m_hWnd);

	// ֪ͨ���󴰿ڱ�����
	UISendMessage(m_pObject, UI_WM_UNINITPOPUPCONTROLWINDOW, 0,0,0, this);

	return 0;
}



// ע��
//	�����GetMessage��ʵ��ֻ������PostMessage���͹�������Ϣ��
//  ��������SendMessage����Ϣ�ǲ��ܵ�֪�ġ�
//
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
		if (UI_WM_EXITPOPUPLOOP == msg.message)  // ע�����������Ϣ��m_hWnd��Ϊ�գ�������ж���ִ��
		{
			if (msg.hwnd == m_hWnd)
				bBreak = true;
		}

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
		UISendMessage(this->m_pObject, pMsg->message, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandle);
		return bHandle;
	}
	
	if (WM_MOUSEMOVE == pMsg->message ||
		WM_NCMOUSEMOVE == pMsg->message )
	{
		if (pMsg->hwnd != m_hWnd)
		{
			return TRUE;  // �����������ƶ��¼�����
		}
		else
		{
			return FALSE;
		}
	}
	else if (WM_LBUTTONDOWN   == pMsg->message ||
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
		RECT rcWindow;
		::GetWindowRect(m_hWnd, &rcWindow);
		if (!PtInRect(&rcWindow, pMsg->pt))  // ����ڵ��������������ˣ��رյ�ǰ����
		{
			this->DestroyPopupWindow();
			
			// ��ԭ���ڷ���һ������ƶ���Ϣ������hover����
			// ����ᵼ��popupwnd��ʧ��ԭ�������ֱ�ӵ���޷�Ӧ
			// ���ߵ��´��ڽ��յ�lbuttondown֮ǰ����û�и���hover����
// 			POINT pt;
// 			GetCursorPos(&pt);
			HWND hWnd = GetActiveWindow();
//			MapWindowPoints(NULL, hWnd, &pt, 1);
			::SendMessage(GetActiveWindow(), WM_MOUSEMOVE, 0, pMsg->lParam/*MAKELPARAM(pt.x, pt.y)*/);
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

PopupListBoxWindow::PopupListBoxWindow(ListBox* pListBox, Object* pBindObj) : PopupControlWindow(pListBox)
{
	m_pBindObj = pBindObj;
	m_pListBox = pListBox;
}

BOOL PopupListBoxWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pListBox || NULL == m_pBindObj)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	cs.lpszClass = WND_POPUP_CONTROL_NAME;  // ������Ӱ
	return bRet;
}
void PopupListBoxWindow::OnInitWindow()
{
	__super::OnInitWindow();

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindObj->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindObj->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupListBoxWindow");
	this->SetAttribute(map_temp,false);  // ��ʼ��һЩĬ�ϱ������細������

	TextRenderBase* pTextRender = this->m_pListBox->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	if (rc.Width() < m_pBindObj->GetWidth())  // �������б�Ŀ������Ϊ��С��comboboxһ��
	{
		rc.right = rc.left + m_pBindObj->GetWidth();
		m_pListBox->SetObjectPos(0,0,rc.Width(), rc.Height(),SWP_NOMOVE|SWP_NOREDRAW);
		m_pListBox->UpdateItemRect(NULL);
	}
	this->SetObjectPos(rcWindow.left, rcWindow.bottom, rc.Width(), rc.Height(), 0);
}

void  PopupListBoxWindow::OnListBoxSize(UINT nType, int cx, int cy)
{
	SetMsgHandled(FALSE);
}

void  PopupListBoxWindow::OnFinalMessage()
{
	__super::OnFinalMessage();
}

void PopupListBoxWindow::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	SetMsgHandled(FALSE);

	if (VK_DOWN == nChar || VK_UP == nChar) 
	{
		SetMsgHandled(TRUE);
		m_pListBox->ProcessMessage(m_pCurMsg,0);
	}
	return;
}


//////////////////////////////////////////////////////////////////////////

PopupMenuWindow::PopupMenuWindow(MenuBase* pMenu) : PopupControlWindow(pMenu)
{
	m_pMenu = pMenu;
}
BOOL PopupMenuWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	if (NULL == m_pMenu)
		return FALSE;

	BOOL bRet = __super::PreCreateWindow(cs);
	return bRet;
}
void PopupMenuWindow::OnInitWindow()
{
	__super::OnInitWindow();

	CRect rc;
	this->m_pMenu->GetWindowRect(&rc);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupMenuWindow");
	this->SetAttribute(map_temp,false);  // ��ʼ��һЩĬ�ϱ������細������

	TextRenderBase* pTextRender = this->m_pMenu->GetTextRender();
	if (NULL != pTextRender)
	{
		pTextRender->SetHRFont(this->GetHRFONT());
		pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER);
	}

	this->SetObjectPos(0, 0, rc.Width(), rc.Height(), SWP_NOMOVE);
}

void PopupMenuWindow::OnFinalMessage()
{
	__super::OnFinalMessage();
}