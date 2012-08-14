#pragma once


class CLyricDlg : public UI::CustomWindow
{
public:
	CLyricDlg(){}

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_GETRENDERTYPE(OnGetRenderType)
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED( _T("switch_layered"), OnSwitchLayered )
		UICHAIN_MSG_MAP(CustomWindow)
		UI_END_MSG_MAP

public:
	void    OnPaint( HRDC );
	void    OnClose();
	void    OnSwitchLayered();
	LRESULT OnGetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }
};