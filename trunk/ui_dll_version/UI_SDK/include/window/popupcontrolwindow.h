#pragma once

namespace UI
{
	class PopupControlWindow : public CustomWindow
	{
	public:
		UI_BEGIN_MSG_MAP
			UICHAIN_MSG_MAP(CustomWindow)
		UI_END_MSG_MAP

		virtual void OnInitWindow()
		{
			__super::OnInitWindow();
		}
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx )
		{
			return __super::PreCreateWindow(cs,dwStyleEx);
		}
	};

	class ListBox;
	class PopupListBoxWindow : public PopupControlWindow
	{
	public:
		PopupListBoxWindow(ListBox* pListBox, Object* pBindObj);
		
		UI_BEGIN_MSG_MAP
			UICHAIN_MSG_MAP(PopupControlWindow)
		UI_END_MSG_MAP

	protected:
		virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx );
		virtual void OnInitWindow();

	protected:
		ListBox*   m_pListBox;
		Object*    m_pBindOb;
	};
}