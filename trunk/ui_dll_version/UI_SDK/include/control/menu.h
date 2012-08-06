#pragma once

namespace UI
{
	class MenuListItem : public ListItemBase
	{
	public:
		MenuListItem(ListCtrlBase* pCtrl);

		const String& GetText() { return m_strText; }
		bool  IsSeperator() { return m_nFlag&MF_SEPARATOR ? true:false; }
		bool  IsPopup() { return m_nFlag&MF_SEPARATOR ? true:false; }
		void  SetFlag(UINT nFlag){ m_nFlag = nFlag; }
		void  SetText(const String& str){ m_strText = str; }

	protected:
		String    m_strText;
		UINT      m_nFlag;
		int       m_nID;
	};



	class UIAPI MenuBase : public ListCtrlBase
	{
	public:
		MenuBase();
		~MenuBase();

		int    GetMenuItemCount();
		int    TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		bool   AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem);

//		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);
//		virtual  void OnDeleteItem( ListItemBase* p );

	protected:
		PopupMenuWindow*  m_pWrapWnd;

		int        m_nItemHeight;
		int        m_nSeperatorHeight;
	};

	class UIAPI Menu : public MenuBase
	{
	public:

	};
}