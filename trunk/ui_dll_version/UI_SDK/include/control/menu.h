#pragma once

namespace UI
{
	class MenuItem : public ListItemBase
	{
	public:
		MenuItem(ListCtrlBase* pCtrl);
		~MenuItem();

		const String& GetText() { return m_strText; }

		bool  IsSeparator()    { return m_nFlag&MF_SEPARATOR   ? true:false; }
		bool  IsPopup()        { return m_nFlag&MF_POPUP       ? true:false; }
		bool  IsDisable()      { return m_nFlag&MF_DISABLED    ? true:false; }
		bool  IsChecked()      { return m_nFlag&MF_CHECKED     ? true:false; }
		bool  IsRadioChecked() { return m_nFlag&MFT_RADIOCHECK ? true:false; }
		
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
			UIMSG_WM_MOUSEMOVE(OnMouseMove)
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_TIMER(OnTimer)
			UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
			UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
			UIMSG_WM_GETRENDERTYPE(OnGetRenderType)
			UIMSG_WM_THEMECHANGED(OnThemeChanged)
		UI_END_MSG_MAP_CHAIN_PARENT(ListCtrlBase)

		friend   class   MenuItem;
		friend   class   PopupMenuWindow;

	protected:
		void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void     OnLButtonDown(UINT nFlags, POINT point);
		void     OnLButtonUp(UINT nFlags, POINT point);
		void     OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
		LRESULT  OnGetRenderType();
		void     OnMouseMove(UINT nFlags, CPoint point);
		void     OnSubMenuMouseMove(MenuBase* pSubMenu);
		void     SetReturnCmd(UINT n) { m_nRetCmd = n; }
		void     OnThemeChanged();

		void     ShowPopupSubMenu(MenuItem* pItem);
		void     HidePopupSubMenu();
		int      PopupSubMenu(MenuItem* pSubMenu);
		int      PopupAsSubMenu(UINT nFlags, MenuBase* pParentMenu, MenuItem* pItem);
		void     DestroyPopupWindow();
		bool     IsItemHilight(MenuItem* p);

		MenuBase* GetPrevMenu() { return m_pPrevMenu; }
		MenuBase* GetNextMenu() { return m_pNextMenu; }
		MenuBase* GetRootMenu();
		MenuBase* GetLastMenu();
		MenuBase* GetMenuByHWND(HWND hWnd);
		MenuBase* GetMenuByPos(POINT ptScreen);
		HWND      GetPopupWindowHandle();
		MenuItem* GetNextSelectableItem(MenuItem* pItem);
		MenuItem* GetPrevSelectableItem(MenuItem* pItem);
	private:
		MenuItem* _GetNextSelectableItem(MenuItem* pItem);
		MenuItem* _GetPrevSelectableItem(MenuItem* pItem);

	public:
		int       GetMenuItemCount();
		int       TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		MenuItem* AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem);
		MenuBase* GetSubMenuByPos(int nPos);
		MenuBase* GetSubMenuByID(int nID);
		MenuItem* GetMenuItemByPos(int nPos);
		MenuItem* GetMenuItemByID(int nID);
		MenuItem* GetMenuItemBySubMenu(MenuBase* pSubMenu);
		PopupMenuWindow* GetPopupLoopMenuWindow() { return GetRootMenu()->m_pPopupWrapWnd; }

		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);

		void     OnDrawSeperatorItem(HRDC hRDC, MenuItem* pMenu);
		void     OnDrawPopupItem(HRDC hRDC,  MenuItem* pMenu);
		void     OnDrawStringItem(HRDC hRDC, MenuItem* pMenu);

		void     OnInitPopupControlWindow(Object* pObjMsgFrom);
		void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

		int      GetIconGutterWidth() { return m_nIconGutterWidth; }

	protected:
		PopupMenuWindow*  m_pPopupWrapWnd;    // ���ڰ�װ�˵��Ĵ���
		MenuBase*    m_pNextMenu;             // ��ǰ�����Ĳ˵��б��У��Լ����Ӳ˵�
		MenuBase*    m_pPrevMenu;             // ��ǰ�����Ĳ˵��б��У��Լ��ĸ��˵�
		UINT         m_nTimerIDShowPopupSubMenu;  // ��ʱ��ID�����ڵ����Ӵ���
		UINT         m_nTimerIDHidePopupSubMenu;  // ��ʱ��ID�����ڹر��Ӵ���
		UINT         m_nTrackPopupMenuFlag;   // �˵�������ʽ������ָ��TPM_RETURNCMD
		UINT         m_nRetCmd;               // ��ָ��TPM_RETURNCMD����ʱ�����ص�ID
		bool         m_bLayered;              // �Ƿ�ʹ�÷ֲ���ʽ

		// �������
		RenderBase*  m_pSeperatorRender;
		RenderBase*  m_pPopupTriangleRender;
		RenderBase*  m_pRadioIconRender;
		RenderBase*  m_pCheckIconRender;

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