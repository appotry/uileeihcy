#pragma once


class CEqualizerDlg : public UI::CustomWindow
{
public:
	CEqualizerDlg();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_TRBN_POSCHANGED( _T("progress_hls_h"), OnHLSChanged_H )
		UIMSG_TRBN_POSCHANGED( _T("progress_hls_l"), OnHLSChanged_L )
		UIMSG_TRBN_POSCHANGED( _T("progress_hls_s"), OnHLSChanged_S )
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

	virtual void OnInitWindow( );

	void    OnClose();
	void    OnHLSChanged_H( int nPos, int nScrollType );
	void    OnHLSChanged_L( int nPos, int nScrollType );
	void    OnHLSChanged_S( int nPos, int nScrollType );

	SliderCtrl* m_pH;
	SliderCtrl* m_pL;
	SliderCtrl* m_pS;
};
