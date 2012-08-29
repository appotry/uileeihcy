#pragma once

namespace UI
{
	class MenuItem : public ListItemBase
	{
	public:
		MenuItem(ListCtrlBase* pCtrl);

		const String& GetText() { return m_strText; }
		bool  IsSeperator() { return m_nFlag&MF_SEPARATOR ? true:false; }
		bool  IsPopup() { return m_nFlag&MF_SEPARATOR ? true:false; }
		void  SetFlag(UINT nFlag){ m_nFlag = nFlag; }
		void  SetID(UINT nID) { m_nID = nID; }
		void  SetText(const String& str){ m_strText = str; }
		UINT  GetFlag() { return m_nFlag; }
		UINT  GetID() { return m_nID; }

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
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
			UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
			UICHAIN_MSG_MAP(ListCtrlBase)
		UI_END_MSG_MAP

		virtual  HRESULT FinalConstruct();

	protected:
		void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void     OnLButtonDown(UINT nFlags, POINT point);
		void     OnLButtonUp(UINT nFlags, POINT point);

	public:
		int      GetMenuItemCount();
		int      TrackPopupMenu(UINT nFlag, int x, int y, Message* pNotifyObj);
		bool     AppendMenu(UINT uFlags, UINT_PTR uIDNewItem, TCHAR* lpNewItem);
	

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
		RenderBase*       m_pSeperatorRender;

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