#pragma once

namespace UI
{
	class MenuWindowBase : public WindowBase
	{
	public:
		UI_BEGIN_MSG_MAP
			UICHAIN_MSG_MAP(WindowBase)
		UI_END_MSG_MAP

		//virtual void OnInitWindow();
		//virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx );
	};

	class UIAPI MenuBase : public ListBoxBase
	{
	public:
		int  GetMenuItemCount();
		int  TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
	};

	class UIAPI Menu : public MenuBase
	{

	};
}