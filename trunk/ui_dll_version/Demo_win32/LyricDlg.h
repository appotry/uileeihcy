#pragma once


class CLyricDlg : public UI::CustomWindow
{
public:
	CLyricDlg();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_GETGRAPHICSRENDERTYPE(OnGetGraphicsRenderType)
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED( _T("switch_layered"), OnSwitchLayered )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_h"), OnHLSChanged_H )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_l"), OnHLSChanged_L )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_s"), OnHLSChanged_S )
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

public:
	void    OnPaint( HRDC );
	void    OnClose();
	void    OnSwitchLayered();
	LRESULT OnGetGraphicsRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }

	virtual void OnInitWindow();
	void    OnHLSChanged_H( int nPos, int nScrollType );
	void    OnHLSChanged_L( int nPos, int nScrollType );
	void    OnHLSChanged_S( int nPos, int nScrollType );

	SliderCtrl* m_pH;
	SliderCtrl* m_pL;
	SliderCtrl* m_pS;
};