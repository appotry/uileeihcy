#include "stdafx.h"
#include "LyricDlg.h"

CLyricDlg::CLyricDlg()
{
	this->SetWindowResizeType(WRSB_ALL);
}

void CLyricDlg::OnPaint( HRDC hRDC )
{
	CRect rcClient;
	this->GetClientRect(&rcClient);

	rcClient.top += 20;
	DrawString(hRDC, _T("Ç§Ç§¾²ÌýDemo v0.6.60"), &rcClient, DT_SINGLELINE|DT_VCENTER|DT_CENTER, m_hFont, RGB(84,142,165) );
}
void CLyricDlg::OnClose()
{
	this->HideWindow();
}

void CLyricDlg::OnSwitchLayered()
{
	if( IsWindowLayered() )
	{
		this->SetWindowTransparentMaskType(WINDOW_TRANSPARENT_TYPE_MASKALPHA);
		this->SetWindowTransparentAlphaMask(1);
		this->SetWindowLayered(false);
	}
	else
	{
		this->SetWindowTransparentMaskType(WINDOW_TRANSPARENT_TYPE_LAYERED|WINDOW_TRANSPARENT_TYPE_MASKALPHA);
		this->SetWindowTransparentAlphaMask(230);
		this->SetWindowLayered(true);
	}

	// ²âÊÔ·Ö²ã´°¿ÚµÄ¿Ø¼þÒþ²Ø¹¦ÄÜ
	// 	Object* p = FindChildObject(_T("sys_close"));
	// 	if( NULL != p )
	// 	{
	// 		p->SetVisible(!p->IsVisible());
	// 	}
}
