#pragma once

namespace UI
{
	class MenuItem : public ListItemBase
	{
	public:
		MenuItem(ListCtrlBase* pCtrl);
		~MenuItem();

		const String& GetText() { return m_strText; }

		bool  IsSeparator() { return m_nFlag&MF_SEPARATOR ? true:false; }
		bool  IsPopup() { return m_nFlag&MF_POPUP ? true:false; }
		bool  IsDisable() { return m_nFlag&MF_DISABLED ? true:false; }
		
		void  SetFlag(UINT nFlag){ m_nFlag = nFlag; }
		void  SetID(UINT nID) { m_nID = nID; }
		void  SetText(const String& str){ m_strText = str; }
		UINT  GetFlag() { return m_nFlag; }
		UINT  GetID() { return m_nID; }
		void  SetSubMenu(MenuBase* p) { m_pSubMenu = p; }
		MenuBase* GetSubMenu() { return m_pSubMenu; }

		virtual  bool    OnMouseEnter();
		virtual  bool    OnMouseLeave();

	protected:
		MenuBase*   m_pMenu;      // item 所属菜单
		String      m_strText;
		MenuBase*   m_pSubMenu;   // 如果是一个popup菜单，该成员表示其子菜单

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

		void     ShowPopupSubMenu(MenuItem* pItem);
		void     HidePopupSubMenu();
		int      PopupSubMenu(MenuItem* pSubMenu);
		int      PopupAsSubMenu(MenuBase* pParentMenu, MenuItem* pItem);
		void     DestroyPopupWindow();

		bool     IsItemHilight(MenuItem* p);

	public:
		int      GetMenuItemCount();
		int      TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		bool     AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem);
		HWND     GetPopupWindowHandle();

		MenuBase* GetPrevMenu() { return m_pPrevMenu; }
		MenuBase* GetNextMenu() { return m_pNextMenu; }
		MenuBase* GetRootMenu();
		MenuBase* GetLastMenu();
		MenuBase* GetMenuByHWND(HWND hWnd);

		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);

		void     OnDrawSeperatorItem(HRDC hRDC, MenuItem* pMenu);
		void     OnDrawPopupItem(HRDC hRDC,  MenuItem* pMenu);
		void     OnDrawStringItem(HRDC hRDC, MenuItem* pMenu);

		void     OnInitPopupControlWindow(Object* pObjMsgFrom);
		void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

		int      GetIconGutterWidth() { return m_nIconGutterWidth; }

	protected:
		PopupMenuWindow*  m_pPopupWrapWnd;
		MenuBase*    m_pNextMenu;             // 
		MenuBase*    m_pPrevMenu;             // 
		UINT         m_nTimerIDShowPopupSubMenu;  // 计时器ID，用于弹出子窗口
		UINT         m_nTimerIDHidePopupSubMenu;  // 计时器ID，用于关闭子窗口

		// 绘制相关
		RenderBase*  m_pSeperatorRender;
		RenderBase*  m_pPopupTriangleRender;

		int        m_nIconGutterWidth;    // 菜单左侧图标列 的宽度
		int        m_nTextMarginLeft;
		int        m_nTextMarginRight;
		int        m_nPopupTriangleWidth; // 菜单右侧弹出菜单类型的三角形
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