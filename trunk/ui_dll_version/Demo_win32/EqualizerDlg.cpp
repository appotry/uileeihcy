#include "stdafx.h"
#include "player.h"
#include "EqualizerDlg.h"
#include "EqualizerMgr.h"

CEqualizerDlg::CEqualizerDlg()
{
	m_pH = NULL;
	m_pL = NULL;
	m_pS = NULL;
}
void CEqualizerDlg::OnInitWindow( )
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
	UI_GetProjectSkinItemInfo(UI_GetSkinHandle(::UI_GetActiveSkinIndex()), &pInfo);
	if (NULL != pInfo)
	{
		m_pH->SetPos(pInfo->GetSkinHlsInfo()->h,false);
		m_pL->SetPos(pInfo->GetSkinHlsInfo()->l,false);
		m_pS->SetPos(pInfo->GetSkinHlsInfo()->s,false);
	}
}
void CEqualizerDlg::OnClose()
{
	this->HideWindow();
	GetEqualizerMgr()->OnEqualizerDlgHide();
}
void CEqualizerDlg::OnHLSChanged_H( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
	//	UI_ChangeSkinHLS(nPos, 0, 0, CHANGE_SKIN_HLS_FLAG_H);
		UI_ChangeSkinHLS(nPos, m_pL->GetPos(), m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
void CEqualizerDlg::OnHLSChanged_L( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
	//	UI_ChangeSkinHLS(0, nPos, 0, CHANGE_SKIN_HLS_FLAG_L);
		UI_ChangeSkinHLS(m_pH->GetPos(), nPos, m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
void CEqualizerDlg::OnHLSChanged_S( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
	{
	//	UI_ChangeSkinHLS(0, 0, nPos, CHANGE_SKIN_HLS_FLAG_S);
		UI_ChangeSkinHLS(m_pH->GetPos(), m_pL->GetPos(), nPos, CHANGE_SKIN_HLS_FLAG_HLS);
	}
}
