#include "stdafx.h"

int  MenuBase::TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj)
{
	if (0 >= this->GetMenuItemCount())
		return -1;

	MenuWindowBase* pMenuWindow = new MenuWindowBase;
	if (nFlag & TPM_RETURNCMD)
	{
		pMenuWindow->DoModeless(_T(""), NULL);
	}
	return 0;
}

int MenuBase::GetMenuItemCount()
{
	return __super::GetItemCount();
}