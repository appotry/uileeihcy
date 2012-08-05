#include "stdafx.h"


MenuBase::MenuBase()
{
	m_pWrapWnd = NULL;
}

MenuBase::~MenuBase()
{
	SAFE_DELETE(m_pWrapWnd);
}

int  MenuBase::TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

//	SAFE_DELETE(m_pWrapWnd);

	m_pWrapWnd = new PopupMenuWindow(this);
	m_pWrapWnd->Create(_T(""),NULL);
	::SetWindowPos(m_pWrapWnd->m_hWnd, NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

	return 0;
}

int MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}

SIZE MenuBase::OnMeasureItem( ListItemBase* p)
{
	SIZE s = {100,m_nItemHeight};
	return s;
}