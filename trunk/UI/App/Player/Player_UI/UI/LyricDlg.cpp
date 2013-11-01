#include "stdafx.h"
#include "LyricDlg.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "App\Player\Player_UI\Mgr\Lyric\LyricMgr.h"
#include "App\Player\Player_Ctrls\Inc\ilyricctrl.h"

CLyricDlg::CLyricDlg()
{
    m_pILyricCtrl = NULL;
    m_pLyricMgr = NULL;
}

void CLyricDlg::OnInitWindow()
{
    ICustomWindow::nvProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);

    this->LoadCtrls();
}

void  CLyricDlg::OnDestroy()
{
    SetMsgHandled(FALSE);
}
void  CLyricDlg::OnSkinChanged()
{   
    this->LoadCtrls();
}
void  CLyricDlg::LoadCtrls()
{
    m_pILyricCtrl = (ILyricCtrl*)this->FindChildObject(_T("lyricctrl"));
}

void CLyricDlg::OnClose()
{
	this->HideWindow();
    GetLyricMgr()->OnLyricDlgHide();
}

void CLyricDlg::OnSwitchLayered()
{
	if (IsWindowLayered())
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
}

void  CLyricDlg::OnLyricLoad(IPlayerListItemInfo*  pItem, IBuffer* pBuffer)
{
    if (NULL == pItem || NULL == pBuffer)
        return;

    if (m_pILyricCtrl)
    {
        // 将utf8转成unicode
        const char* szBuffer = (const char*)pBuffer->Get();

        int nSize = (int)strlen(szBuffer)+1;
        wchar_t* psz = new wchar_t[nSize];
        memset(psz, 0, nSize*sizeof(wchar_t));
        int nRet = UI::Util::UTF82U(szBuffer, psz, nSize);

        m_pILyricCtrl->LoadLyric(psz);

//        m_strLyric = psz;
        SAFE_ARRAY_DELETE(psz);
    }
    
    ::InvalidateRect(GetHWND(), NULL, TRUE);
}

void  CLyricDlg::OnMp3Start(IPlayerListItemInfo*  pItem)
{
    if (m_pILyricCtrl)
    {
        m_pILyricCtrl->Start();
    }
}
void  CLyricDlg::OnMp3Stop()
{
    if (m_pILyricCtrl)
    {
        m_pILyricCtrl->Stop();
    }
}

void  CLyricDlg::OnMp3ProgressInd(double dSeconds, double dPercent)
{
    if (m_pILyricCtrl)
    {
        m_pILyricCtrl->OnProgressInd((int)(dSeconds*1000.0));
    }
}

void  CLyricDlg::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    if (NULL == pRenderTarget)
        return ;
}

#if 0

m_pH = NULL;
m_pL = NULL;
m_pS = NULL;


m_pH = (UI::ISliderCtrl*)this->FindChildObject( _T("progress_hls_h"));
if (m_pH)
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
m_pL = (UI::ISliderCtrl*)this->FindChildObject( _T("progress_hls_l"));
if (m_pL)
{
    m_pL->SetRange(-100,100);
}
m_pS = (UI::ISliderCtrl*)this->FindChildObject( _T("progress_hls_s"));
if (m_pS)
{
    m_pS->SetRange(-100,100);
}
#if 0 // -- 架构改造
UI::ISkinConfigRes* pSkinConfigRes = NULL;
g_pUIApp->GetActiveSkinConfigRes(&pSkinConfigRes);
if (pSkinConfigRes)
{
    if (m_pH)
        m_pH->SetPos(pSkinConfigRes->GetSkinHlsInfo()->h,false);
    if (m_pL)
        m_pL->SetPos(pSkinConfigRes->GetSkinHlsInfo()->l,false);
    if (m_pS)
        m_pS->SetPos(pSkinConfigRes->GetSkinHlsInfo()->s,false);
}
#endif

void CLyricDlg::OnHLSChanged_H( int nPos, int nScrollType )
{
    if( nScrollType != SB_ENDSCROLL )
    {
        //	UI_ChangeSkinHLS(nPos, 0, 0, CHANGE_SKIN_HLS_FLAG_H);
        g_pSkinMgr->ChangeSkinHLS(nPos, m_pL->GetPos(), m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
    }
}
void CLyricDlg::OnHLSChanged_L( int nPos, int nScrollType )
{
    if( nScrollType != SB_ENDSCROLL )
    {
        //	UI_ChangeSkinHLS(0, nPos, 0, CHANGE_SKIN_HLS_FLAG_L);
        g_pSkinMgr->ChangeSkinHLS(m_pH->GetPos(), nPos, m_pS->GetPos(), CHANGE_SKIN_HLS_FLAG_HLS);
    }
}
void CLyricDlg::OnHLSChanged_S( int nPos, int nScrollType )
{
    if( nScrollType != SB_ENDSCROLL )
    {
        //	UI_ChangeSkinHLS(0, 0, nPos, CHANGE_SKIN_HLS_FLAG_S);
        g_pSkinMgr->ChangeSkinHLS(m_pH->GetPos(), m_pL->GetPos(), nPos, CHANGE_SKIN_HLS_FLAG_HLS);
    }
}

void CLyricDlg::OnLButtonDblClk(UINT nFlags, POINT point)
{
    g_pSkinMgr->ChangeSkinHLS(0, 0, 0, CHANGE_SKIN_HLS_FLAG_HLS);
    if (m_pH)
        m_pH->SetPos(0);
    if (m_pL)
        m_pL->SetPos(0);
    if (m_pS)
        m_pS->SetPos(0);
}

#endif