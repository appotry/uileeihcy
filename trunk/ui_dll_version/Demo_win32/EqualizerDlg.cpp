#include "stdafx.h"
#include "EqualizerDlg.h"


void CEqualizerDlg::OnInitWindow( )
{
	CustomWindow::OnInitWindow();

	SliderCtrl*  p = (SliderCtrl*)this->FindChildObject( _T("progress_hue1"));
	if( NULL != p )
	{
		p->SetRange(0,5);
		p->SetPos(3);
	}
	p = (SliderCtrl*)this->FindChildObject( _T("progress_hue2"));
	if( NULL != p )
	{
		p->SetRange(0,16);
		p->SetPos(8);
	}
}
void CEqualizerDlg::OnClose()
{
	this->HideWindow();
}
void CEqualizerDlg::OnHueChanged1( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinH(nPos*5);
}
void CEqualizerDlg::OnHueChanged2( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		UI_ChangeSkinH(100+nPos*5);
}
