#pragma once

namespace UI
{

	class UIAPI MenuBase : public ListBox
	{
	public:
		MenuBase();
		~MenuBase();

		int  GetMenuItemCount();
		int  TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);

//		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);
//		virtual  void OnDeleteItem( ListItemBase* p );

	protected:
		PopupMenuWindow*  m_pWrapWnd;
	};

	class UIAPI Menu : public MenuBase
	{

	};
}