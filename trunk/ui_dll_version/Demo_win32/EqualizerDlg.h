#pragma once


class CEqualizerDlg : public UI::CustomWindow
{
public:
	CEqualizerDlg();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_h"), OnHLSChanged_H )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_l"), OnHLSChanged_L )
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_s"), OnHLSChanged_S )
		UIMSG_TRBN_POSCHANGED( m_pBalance, OnBalanceChanged )
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

	virtual void OnInitWindow( );

	void    OnClose();
	void    OnHLSChanged_H( int nPos, int nScrollType );
	void    OnHLSChanged_L( int nPos, int nScrollType );
	void    OnHLSChanged_S( int nPos, int nScrollType );

	void    OnBalanceChanged( int nPos, int nScrollType );

	SliderCtrl* m_pH;
	SliderCtrl* m_pL;
	SliderCtrl* m_pS;

	SliderCtrl* m_pBalance;
	SliderCtrl* m_p3DSound;  // 暂不支持杜比环绕立体声，没有设备
};
