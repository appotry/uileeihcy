#pragma once

namespace UI
{
	class PopupControlWindow : public CustomWindow
	{
	public:
		PopupControlWindow(Object* pObj);

		UI_BEGIN_MSG_MAP
			//UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
			UIMSG_WM_MOUSEACTIVATE(OnMouseActivate)
			UIMSG_WM_ACTIVATEAPP(OnActivateApp)
			UIMSG_WM_DESTROY(OnDestroy)
			UIMESSAGE_HANDLER_EX(UI_WM_ENTERPOPUPLOOP, OnEnterPopupLoop)
			UIMESSAGE_HANDLER_EX(UI_WM_EXITPOPUPLOOP, OnExitPopupLoop)
		UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

	protected:
		virtual void OnInitWindow();
		virtual void OnFinalMessage();
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

		virtual BOOL PreTranslatePopupMessage(MSG* pMsg);

		BOOL      OnEraseBkgnd(HRDC hRDC);
		void      OnDestroy();
		
		LRESULT   OnEnterPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT   OnExitPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam);

		int       OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
		void      OnActivateApp(BOOL bActive, DWORD dwThreadID);

	protected:
		void      PopupLoop();

	public:
		void      DestroyPopupWindow();

	protected:
		Object*   m_pObject;   // 弹出窗口中装载的对象指针，如ListBox* Menu*
		bool      m_bExitLoop; // 在收到WM_EXITPOPUPLOOP之后，该变量为true
	};

	class ListBox;
	class PopupListBoxWindow : public PopupControlWindow
	{
	public:
		PopupListBoxWindow(ListBox* pListBox, Object* pBindObj);
		
		UI_BEGIN_MSG_MAP
			UIMSG_WM_KEYDOWN(OnKeyDown)

		UIALT_MSG_MAP(1)
		
		UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)

	protected:
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
		virtual void OnInitWindow();
		virtual void OnFinalMessage();

		void    OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void    OnListBoxSize(UINT nType, int cx, int cy);
	protected:
		ListBox*   m_pListBox;
		Object*    m_pBindObj;  // TODO: 后面改进，使该类与m_pBindObj无关系
	};


	class MenuBase;
	class PopupMenuWindow : public PopupControlWindow
	{
	public:
		PopupMenuWindow(MenuBase* pMenu);

		UI_BEGIN_MSG_MAP
			UIMSG_WM_PAINT(OnPaint)
		UIALT_MSG_MAP(1)

		UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)

	protected:

		virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
		virtual BOOL PreTranslatePopupMessage(MSG* pMsg);
		virtual void OnInitWindow();
		virtual void OnFinalMessage();

		void    OnPaint(HRDC hRDC)
		{
			SetMsgHandled(FALSE);
			int a = 0;
			return;
		}
	protected:
		MenuBase*   m_pMenu;
	};

}