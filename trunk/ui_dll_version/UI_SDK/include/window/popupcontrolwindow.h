#pragma once

namespace UI
{
	class PopupControlWindow : public CustomWindow
	{
	public:
		UI_BEGIN_MSG_MAP
			UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
			//UIMSG_WM_KILLFOCUS(OnKillFocus)
			MESSAGE_HANDLER_EX(UI_WM_DESTROYPOPUPWINDOW, __OnDestroyPopupWindow)
			MESSAGE_HANDLER_EX(WM_MOUSEACTIVATE, OnMouseActivate)
			//MESSAGE_HANDLER_EX(WM_ACTIVATEAPP, OnActivateApp)

			UICHAIN_MSG_MAP(CustomWindow)
		UI_END_MSG_MAP

		virtual void OnInitWindow();
		virtual void OnFinalMessage();

		virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx );
		void    OnKillFocus( Object* pNewFocusObj );
		BOOL    OnEraseBkgnd(HRDC hRDC);
		void    DestroyPopupWindow();
		LRESULT __OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return MA_NOACTIVATE;
		}
		LRESULT OnActivateApp(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			SetMsgHandled(FALSE);
			DestroyPopupWindow();
			return 0;
		}
		
		
		
	};

	class ListBox;
#define ALT_MSG_ID_LISTBOX 1
	class PopupListBoxWindow : public PopupControlWindow
	{
	public:
		PopupListBoxWindow(ListBox* pListBox, Object* pBindObj);
		
		UI_BEGIN_MSG_MAP
			UIMSG_WM_KEYDOWN(OnKeyDown)
			MESSAGE_HANDLER_EX(UI_WM_DESTROYPOPUPWINDOW, OnDestroyPopupWindow)


		UIALT_MSG_MAP(ALT_MSG_ID_LISTBOX)
			UIMSG_WM_SIZE(OnSize)
		UI_BEGIN_CHAIN_ALL_MSG_MAP
			UICHAIN_MSG_MAP(PopupControlWindow)
		UI_END_CHAIN_ALL_MSG_MAP
		UI_END_MSG_MAP

	protected:
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx );
		virtual void OnInitWindow();

		void    OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void    OnListBoxSize(UINT nType, int cx, int cy);
		LRESULT OnDestroyPopupWindow(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		ListBox*   m_pListBox;
		Object*    m_pBindOb;
	};
}