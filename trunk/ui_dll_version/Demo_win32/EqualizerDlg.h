#pragma once


class CEqualizerDlg : public UI::CustomWindow
{
public:
	UI_BEGIN_MSG_MAP
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_TRBN_POSCHANGED( _T("progress_hue1"), OnHueChanged1 )
		UIMSG_TRBN_POSCHANGED( _T("progress_hue2"), OnHueChanged2 )
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

	virtual void OnInitWindow( );

	void    OnClose();
	void    OnHueChanged1( int nPos, int nScrollType );
	void    OnHueChanged2( int nPos, int nScrollType );
};