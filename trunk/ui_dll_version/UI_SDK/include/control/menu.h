#pragma once

namespace UI
{
	class MenuItemData
	{
	public:
		MenuItemData();

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

		UI_BEGIN_MSG_MAP
		//	UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
			UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
			UICHAIN_MSG_MAP(ListCtrlBase)
		UI_END_MSG_MAP

	public:
		int      GetMenuItemCount();
		int      TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		bool     AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem);

		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);
		virtual  void OnDeleteItem( ListItemBase* p );

		void     OnDrawSeperatorItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData);
		void     OnDrawPopupItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData);
		void     OnDrawStringItem(HRDC hRDC, ListItemBase* p, MenuItemData* pMenuData);

		void     OnInitPopupControlWindow(Object* pObjMsgFrom);
		void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

	protected:
		PopupMenuWindow*  m_pPopupWrapWnd;

		int        m_nItemHeight;
		int        m_nSeperatorHeight;
	};

	class UIAPI Menu : public MenuBase
	{
	public:
		UI_DECLARE_OBJECT( Menu, OBJ_CONTROL )

	};
}