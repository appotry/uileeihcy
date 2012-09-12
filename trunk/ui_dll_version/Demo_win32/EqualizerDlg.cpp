#include "stdafx.h"
#include "EqualizerDlg.h"


void CEqualizerDlg::OnInitWindow( )
{
	CustomWindow::OnInitWindow();

	SliderCtrl*  p = (SliderCtrl*)this->FindChildObject( _T("progress_hue1"));
	if( NULL != p )
	{
		p->SetRange(MIN_HUE_VALUE,MAX_HUE_VALUE);
		p->SetPos(0);
	}
	p = (SliderCtrl*)this->FindChildObject( _T("progress_hue2"));
	if( NULL != p )
	{
		p->SetRange(-100,100);
		p->SetPos(0);
	}
}
void CEqualizerDlg::OnClose()
{
	this->HideWindow();
}
void CEqualizerDlg::OnHueChanged1( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinHLS(nPos, 0, 0, CHANGE_SKIN_HLS_FLAG_H);
}
void CEqualizerDlg::OnHueChanged2( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinHLS(0, nPos, 0, CHANGE_SKIN_HLS_FLAG_L);
}
