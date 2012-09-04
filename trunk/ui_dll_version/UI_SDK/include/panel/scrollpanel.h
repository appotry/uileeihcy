#pragma once

namespace UI
{
	class UIAPI ScrollPanel : public Panel
	{
	public:
		ScrollPanel();
		~ScrollPanel();
		UI_DECLARE_OBJECT(ScrollPanel, OBJ_PANEL)

		UI_BEGIN_MSG_MAP
			UIMSG_WM_HITTEST(OnHitTest)
			UICHAIN_MSG_MAP_MEMBER(m_MgrScrollBar)
		UI_END_MSG_MAP_CHAIN_PARENT(Panel)

	public:
		virtual      void        ResetAttribute();
		virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload );

	protected:
		UINT         OnHitTest( POINT* pt );

	protected:
		ScrollBarMgr     m_MgrScrollBar;
	};
}