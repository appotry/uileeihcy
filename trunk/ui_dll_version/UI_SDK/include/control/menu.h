#pragma once

namespace UI
{
	class MenuItem : public ListItemBase
	{
	public:
		MenuItem(ListCtrlBase* pCtrl);
		~MenuItem();

		const String& GetText() { return m_strText; }

		bool  IsSeperator() { return m_nFlag&MF_SEPARATOR ? true:false; }
		bool  IsPopup() { return m_nFlag&MF_POPUP ? true:false; }
		
		void  SetFlag(UINT nFlag){ m_nFlag = nFlag; }
		void  SetID(UINT nID) { m_nID = nID; }
		void  SetText(const String& str){ m_strText = str; }
		UINT  GetFlag() { return m_nFlag; }
		UINT  GetID() { return m_nID; }
		void  SetSubMenu(MenuBase* p) { m_pSubMenu = p; }

		virtual  bool    OnMouseEnter();
		virtual  bool    OnMouseLeave();
		void     OnTimer(UINT nIDEvent, TimerItem* pItem);

	protected:
		MenuBase*   m_pMenu;      // item �����˵�
		String      m_strText;
		MenuBase*   m_pSubMenu;   // �����һ��popup�˵����ó�Ա��ʾ���Ӳ˵�

		UINT        m_nFlag;
		int         m_nID;
	};



	class UIAPI MenuBase : public ListCtrlBase
	{
	public:
		MenuBase();
		~MenuBase();

		UI_BEGIN_MSG_MAP
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_TIMER(OnTimer)
			UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
			UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
		UI_END_MSG_MAP_CHAIN_PARENT(ListCtrlBase)

		virtual  HRESULT FinalConstruct();

		friend   class   MenuItem;
	protected:
		void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void     OnLButtonDown(UINT nFlags, POINT point);
		void     OnLButtonUp(UINT nFlags, POINT point);
		void     OnTimer(UINT_PTR nIDEvent, LPARAM lParam);

		void     SetTimer2PopupSubMenu(MenuItem* pItem);
		void     KillTimer2PopupSubMenu();
		int      PopupSubMenu(UINT nFlag, int x, int y, Message* pNotifyObj);

	public:
		int      GetMenuItemCount();
		int      TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		bool     AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem);

		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);

		void     OnDrawSeperatorItem(HRDC hRDC, ListItemBase* p, MenuItem* pMenu);
		void     OnDrawPopupItem(HRDC hRDC, ListItemBase* p, MenuItem* pMenu);
		void     OnDrawStringItem(HRDC hRDC, ListItemBase* p, MenuItem* pMenu);

		void     OnInitPopupControlWindow(Object* pObjMsgFrom);
		void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

		int      GetIconGutterWidth() { return m_nIconGutterWidth; }

	protected:
		PopupMenuWindow*  m_pPopupWrapWnd;
		UINT         m_nTimerIDPopupSubMenu;  // ��ʱ��ID�����ڵ����Ӵ���

		// �������
		RenderBase*  m_pSeperatorRender;

		int        m_nIconGutterWidth;    // �˵����ͼ���� �Ŀ��
		int        m_nTextMarginLeft;
		int        m_nTextMarginRight;
		int        m_nPopupTriangleWidth; // �˵��Ҳ൯���˵����͵�������
		int        m_nSeperatorHeight;

	
	
	};

	class UIAPI Menu : public MenuBase
	{
	public:
		UI_DECLARE_OBJECT( Menu, OBJ_CONTROL )

	public:
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);
		virtual  void ResetAttribute();
	};
}