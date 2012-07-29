#include "stdafx.h"

void PopupControlWindow::OnInitWindow()
{
	__super::OnInitWindow();
}
void PopupControlWindow::OnFinalMessage()
{
	delete this;
}
BOOL PopupControlWindow::PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx )
{
	dwStyleEx = 0;//(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
	cs.lpszClass = WND_POPUP_CONTROL_NAME;
	return __super::PreCreateWindow(cs,dwStyleEx);
}
void PopupControlWindow::OnKillFocus( Object* pNewFocusObj )
{
	this->DestroyPopupWindow();
}

void PopupControlWindow::DestroyPopupWindow()
{
	::PostMessage(m_hWnd, UI_WM_DESTROYPOPUPWINDOW, 0, 0);
}
LRESULT PopupControlWindow::__OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	this->ClearTreeObject();
	::DestroyWindow(m_hWnd);
	return 0;
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

BOOL PopupListBoxWindow::PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx )
{
	if (NULL == m_pListBox || NULL == m_pBindOb)
		return FALSE;

	return __super::PreCreateWindow(cs,dwStyleEx);
}
void PopupListBoxWindow::OnInitWindow()
{
	m_pListBox->AddHook(this, 0, ALT_MSG_ID_LISTBOX);
	this->AddChild(m_pListBox);

	CRect rc;
	this->m_pListBox->GetWindowRect(&rc);
	
	CRect rcWindow;
	m_pBindOb->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindOb->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	ATTRMAP map_temp;
	map_temp[XML_ID] = _T("PopupListBoxWindow");
	this->SetAttribute(map_temp,false);  // 初始化一些默认变量

	bool bNeedReCalcListBoxRect = false;
	if (0 == rc.Width())
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
	UI_LOG_DEBUG(_T("AAA"));
	return;
}