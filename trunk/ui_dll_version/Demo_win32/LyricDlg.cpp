#include "stdafx.h"
#include "LyricDlg.h"

CLyricDlg::CLyricDlg()
{
	m_pH = NULL;
	m_pL = NULL;
	m_pS = NULL;

	this->SetWindowResizeType(WRSB_ALL);
}

void CLyricDlg::OnInitWindow()
{
	CustomWindow::OnInitWindow();

	m_pH = (SliderCtrl*)this->FindChildObject( _T("progress_hls_h"));
	if( NULL != m_pH )
	{
		m_pH->SetRange(-120,120);
		// 		srand( (unsigned)time( NULL ) );
		// 		int n = rand()%240-120;
		// 		m_pH->SetPos(n);
		// 		if (0 != n)
		// 		{
		// 			UI_ChangeSkinHLS(n, 0,0, CHANGE_SKIN_HLS_FLAG_H);
		// 		}
	}
	m_pL = (SliderCtrl*)this->FindChildObject( _T("progress_hls_l"));
	if( NULL != m_pL )
	{
		m_pL->SetRange(-100,100);
	}
	m_pS = (SliderCtrl*)this->FindChildObject( _T("progress_hls_s"));
	if( NULL != m_pS )
	{
		m_pS->SetRange(-100,100);
	}

	IProjectSkinItemInfo* pInfo = NULL;
	UI_GetProjectSkinItemInfo((HSKIN)NULL, &pInfo);
	if (NULL != pInfo)
	{
		if (NULL != m_pH)
			m_pH->SetPos(pInfo->GetSkinHlsInfo()->h,false);
		if (NULL != m_pL)
			m_pL->SetPos(pInfo->GetSkinHlsInfo()->l,false);
		if (NULL != m_pS)
			m_pS->SetPos(pInfo->GetSkinHlsInfo()->s,false);
	}
}

void CLyricDlg::OnPaint( HRDC hRDC )
{
	CRect rcClient;
	this->GetClientRectAsWin32(&rcClient);
	rcClient.top += 60;

	DrawString(hRDC, _T("Ç§Ç§¾²ÌýDemo v0.6.200"), &rcClient, DT_SINGLELINE|DT_CENTER, m_hFont, RGB(84,142,165) );
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



void CLyricDlg::OnHLSChanged_H( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
		//	UI_ChangeSkinHLS(nPos, 0, 0, CHANGE_SKIN_HLS_FLAG_H);
		UI_ChangeSkinHLS(nPos, m_pL->GetPos(), m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
void CLyricDlg::OnHLSChanged_L( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
		//	UI_ChangeSkinHLS(0, nPos, 0, CHANGE_SKIN_HLS_FLAG_L);
		UI_ChangeSkinHLS(m_pH->GetPos(), nPos, m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
void CLyricDlg::OnHLSChanged_S( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
		//	UI_ChangeSkinHLS(0, 0, nPos, CHANGE_SKIN_HLS_FLAG_S);
		UI_ChangeSkinHLS(m_pH->GetPos(), m_pL->GetPos(), nPos, CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
