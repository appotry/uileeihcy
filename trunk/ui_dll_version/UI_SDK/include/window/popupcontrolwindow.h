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

			UIMESSAGE_HANDLER_EX(UI_WM_ENTERPOPUPLOOP, OnEnterPopupLoop)
			UIMESSAGE_HANDLER_EX(UI_WM_EXITPOPUPLOOP, OnExitPopupLoop)
		UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

		virtual void OnInitWindow();
		virtual void OnFinalMessage();
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

		virtual BOOL PreTranslatePopupMessage(MSG* pMsg);

		BOOL      OnEraseBkgnd(HRDC hRDC);
		void      DestroyPopupWindow();
		
		LRESULT   OnEnterPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT   OnExitPopupLoop(UINT uMsg, WPARAM wParam, LPARAM lParam);

		// ������������ڵ�ʱ�򣬲�Ҫȥ��ռ�����ڵĽ���
		// ��������ͬʱ�����������Ϣ�ͼ�����Ϣ��ȡ��������
		// WM_NCHITTEST���յ�
		int OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
		{
			return MA_NOACTIVATE;
		}

		// �����Ϣ��������������������/�������㴰�ڵ�ʱ��
		// ���������ص�֪ͨ�����ڵ�����Լ��Ķ��㴰��ȴ������
		void OnActivateApp(BOOL bActive, DWORD dwThreadID)
		{
			SetMsgHandled(FALSE);
			if (FALSE == bActive)
			{
				DestroyPopupWindow();
			}
		}

		void PopupLoop();

	protected:
		Object*   m_pObject;   // ����������װ�صĶ���ָ�룬��ListBox* Menu*
		bool      m_bExitLoop; // ���յ�WM_EXITPOPUPLOOP֮�󣬸ñ���Ϊtrue
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
		Object*    m_pBindObj;  // TODO: ����Ľ���ʹ������m_pBindObj�޹�ϵ
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