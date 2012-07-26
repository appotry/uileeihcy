#include "stdafx.h"

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
	SIZE s = m_pListBox->GetDesiredSize(NULL);
	
	CRect rcWindow;
	m_pBindOb->GetWindowRect(&rcWindow);
	::MapWindowPoints(m_pBindOb->GetHWND(), NULL, (LPPOINT)&rcWindow, 2);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, rcWindow.left, rcWindow.bottom, s.cx, s.cy, 0);
}