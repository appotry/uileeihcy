#include "StdAfx.h"
#include "App\Player\Player_UI\UI\MainWindow.h"
#include "App\Player\Player_UI\Mgr\player.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
#include "App\Player\Player_UI\Mgr\Lyric\LyricMgr.h"
#include "App\Player\Player_UI\UI\OptionWindow.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "App\Player\Player_UI\Mgr\EqualizerMgr.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "App\Player\Player_Ctrls\Inc\imultilabel.h"


#define  TIMER_ID_RESTORE_STATUS_TEXT  1
#define  TIME_RESTORE_STATUS_TEXT      2000
#define  MENU_ID_SKIN_BASE 900

MainWindow::MainWindow(void)
{
	m_hWndPlayerList = NULL;
	m_hWndEqualizer = NULL;
    m_hWndLyric = NULL;
	m_pOptionWindow = NULL;

	m_pbtnStart = NULL;
	m_pbtnPause = NULL;
	m_pbtnStop = NULL;
	m_pbtnOpen = NULL;
	m_pbtnMute = NULL;
	m_pLabelPlaystatus = NULL;
	m_pLEDTime = NULL;
	m_pProgress = NULL;
	m_pVolume = NULL;
	m_pBtnPlaylist = NULL;
	m_pBtnLyric = NULL;
	m_pBtnEqualizer = NULL;
	m_pVisuallzationPic = NULL;
	m_pMultiLabelTitle = NULL;
    m_pChannelMode = NULL;

	m_pMainMenu = NULL;

    m_eLEDTimeShowType = LEDTimeShow_ELAPSE;
    m_nTimeElapse = 0;
// 	m_lSizeMove = 0;
// 	m_ptCursorSizeMove.x = m_ptCursorSizeMove.y = 0;
// 	m_ptWndPosSizeMove.x = m_ptWndPosSizeMove.y = 0;
}

MainWindow::~MainWindow(void)
{
	if (m_pOptionWindow)
	{
		DestroyWindow(m_pOptionWindow->GetHWND());
		SAFE_DELETE_Ixxx(m_pOptionWindow);
	}
	SAFE_DELETE_Ixxx(m_pMainMenu);
}

BOOL MainWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
	pcs->style |= WS_MINIMIZE;
	SetMsgHandled(FALSE);

    return TRUE;
}
void MainWindow::OnDestroy()
{
    GetMainMgr()->OnMainWindowDestroy();
	SetMsgHandled(FALSE);
	::PostQuitMessage(0);
}

void MainWindow::OnInitWindow()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);
    LoadCtrls();
   
	if (m_pbtnStop)
	{
		m_pbtnStop->SetEnable(false);
	}

	if (m_pbtnPause)
	{
		m_pbtnPause->SetVisible(false);
	}

	if (m_pVolume)
	{
		m_pVolume->SetPos(GetConfigData()->player.m_byteVolumn, false);
	}

	if (m_pbtnMute)
	{
		if (GetConfigData()->player.m_bMute)
			m_pbtnMute->SetChecked();
	}

	if (m_pLabelPlaystatus)
	{
		m_strStatusText = m_pLabelPlaystatus->GetText();
	}
	::SetWindowText(GetHWND(), _T("Player_Demo"));

// 	COptionWindow win;
// 	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),GetHWND());
}

void  MainWindow::LoadCtrls()
{
    m_pbtnStart        = (UI::IButton*)this->FindChildObject(_T("btn_play") );
    m_pbtnPause        = (UI::IButton*)this->FindChildObject(_T("btn_pause") );
    m_pbtnStop         = (UI::IButton*)this->FindChildObject(_T("btn_stop") );
    m_pbtnOpen         = (UI::IButton*)this->FindChildObject(_T("btn_open") );
    m_pBtnPlaylist     = (UI::ICheckButton*)this->FindChildObject(_T("btn_playlist") );
    m_pBtnLyric        = (UI::ICheckButton*)this->FindChildObject(_T("btn_lyric") );
    m_pBtnEqualizer    = (UI::ICheckButton*)this->FindChildObject(_T("btn_equalizer") );
    m_pbtnMute         = (UI::ICheckButton*)this->FindChildObject(_T("btn_mute") );
    m_pLabelPlaystatus = (UI::ILabel*)this->FindChildObject(_T("label_playstatus"));
    m_pProgress        = (UI::ISliderCtrl*)this->FindChildObject(_T("progress_music"));
    m_pVolume          = (UI::ISliderCtrl*)this->FindChildObject(_T("progress_voice"));
    m_pLEDTime         = (UI::ILEDCtrl*)this->FindChildObject(_T("led_time"));
    m_pVisuallzationPic= (UI::IPictureCtrl*)this->FindChildObject(_T("visualization"));
    m_pMultiLabelTitle = (IMultiLabel*)this->FindChildObject(_T("label_title"));
    m_pChannelMode     = (UI::ILabel*)this->FindChildObject(_T("label_channel_mode"));
}
void  MainWindow::LoadMainMenu()
{
    UI::UILoadMenu(GetUIApplication(), _T("main_menu"), &m_pMainMenu);
	if (NULL == m_pMainMenu)
		return;

	UI::IMenu* pSkinMenuItem = m_pMainMenu->GetSubMenuByPos(12);
	if (pSkinMenuItem)
	{
		// 皮肤列表
		long lCount = g_pSkinMgr->GetSkinCount();
        UI::ISkinRes*  pActiveSkin = g_pSkinMgr->GetActiveSkin();

		for (int i = 0; i < lCount; i++)
		{
            UI::ISkinRes*  pSkinRes = g_pSkinMgr->GetSkinResByIndex(i);

            UI::IListItemBase* pMenuItem = pSkinMenuItem->AppendString(pSkinRes->GetName(), i+MENU_ID_SKIN_BASE);
            if (pMenuItem && pActiveSkin == pSkinRes)
                pMenuItem->SetChecked(true);
		}
	}
}

void MainWindow::OnPaint(UI::IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget || NULL == m_pVisuallzationPic)
		return;

	CRect rcClient;
	m_pVisuallzationPic->GetClientRectInWindow(&rcClient);

	HBITMAP hBitmap = GetMainMgr()->GetVisualSnapshot();
	if (hBitmap)
	{
		HDC hDC = pRenderTarget->GetHDC();
        UI::IImage image;
		image.Attach(hBitmap);
		image.BitBlt(hDC, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), 0,0);
		image.Detach();
		pRenderTarget->ReleaseHDC(hDC);
	}
	else
	{
	}
	GetMainMgr()->ReleaseVisualSnapshot();
}

void MainWindow::OnBnClickPlay()
{
	GetMainMgr()->Play();
}
void MainWindow::OnMp3Start(IPlayerListItemInfo* pItemInfo)
{
	if(NULL != m_pbtnStart && NULL != m_pbtnPause)
	{
		m_pbtnStart->SetVisible(false, false);
		m_pbtnPause->SetVisible(true, true);
	}

	if (m_pbtnStop)
		m_pbtnStop->SetEnable(true);

	if (m_pLabelPlaystatus)
	{
		m_strStatusText = _T("状态:播放");
		m_pLabelPlaystatus->SetText(m_strStatusText.c_str());
	}

	::SetWindowText(GetHWND(), pItemInfo->GetDisplayText());
	if (m_pMultiLabelTitle)
    {
        TCHAR  szFormat[32] = _T("");
        String  strText;
        const TCHAR* szText = NULL;

        strText.append(_T("标题: "));
        szText = pItemInfo->GetTitle();
        if (szText && _tcslen(szText) > 0)
        {
            strText.append(szText);
        }
        else
        {
            strText.append(pItemInfo->GetFileName());
        }
        szText = pItemInfo->GetArtist();
        if (szText && _tcslen(szText) > 0)
        {
            strText.append(_T("\n艺术家: "));
            strText.append(szText);
        }

        szText = pItemInfo->GetAlbum();
        if (szText && _tcslen(szText) > 0)
        {
            strText.append(_T("\n专辑: "));
            strText.append(szText);
        }

        strText.append(_T("\n格式:"));
        int nSample = pItemInfo->GetSample();
        if (0 != nSample)
        {
            _stprintf(szFormat, _T(" %dkHZ"), nSample/1000);
            strText.append(szFormat);
        }
        int nBitrate = pItemInfo->GetBitrate();
        if (0 != nBitrate)
        {
            _stprintf(szFormat, _T(" %dK"), nBitrate/1000);
            strText.append(szFormat);
        }

        strText.append(_T("\n长度: "));

        int nDuration = pItemInfo->GetDuration();
        if (nDuration > 0)
        {
            nDuration = nDuration/1000;
            int n1 = nDuration%60;
            int n2 = nDuration/60;
            _stprintf(szFormat, _T("%02d:%02d"), n2, n1);
            strText.append(szFormat);
        }

		m_pMultiLabelTitle->SetContent(strText.c_str(), _T('\n'));
        m_pMultiLabelTitle->StartAnimateSwitch(5000);
        m_pMultiLabelTitle->UpdateObject();
    }

    if (m_pChannelMode)
    {
        mp3_channel_mode eChannelMode = pItemInfo->GetChannelMode();
        if (CHANNEL_MODE_Single == eChannelMode)
        {
            m_pChannelMode->SetText(_T("单声道"));
        }
        else
        {   
            m_pChannelMode->SetText(_T("立体声"));
        }
    }
}
void MainWindow::OnMp3Continue()
{
	if( NULL != m_pbtnStart && NULL != m_pbtnPause )
	{
		m_pbtnStart->SetVisible(false, false);
		m_pbtnPause->SetVisible(true, true);
	}

	if( NULL != m_pbtnStop )
		m_pbtnStop->SetEnable( true );

	if( NULL != m_pLabelPlaystatus )
	{
		m_strStatusText = _T("状态:播放");
		m_pLabelPlaystatus->SetText(m_strStatusText.c_str());
	}
}


void MainWindow::OnBnClickPause()
{
	GetMainMgr()->Pause();
}
void MainWindow::OnMp3Pause()
{
	if( NULL != m_pbtnStart && NULL != m_pbtnPause )
	{
		m_pbtnStart->SetVisible(true, false);
		m_pbtnPause->SetVisible(false, false);
	}

	if( NULL != m_pLabelPlaystatus )
	{
		m_strStatusText = _T("状态:暂停");
		m_pLabelPlaystatus->SetText(m_strStatusText.c_str());
	}
}

void MainWindow::OnBnClickStop()
{
	GetMainMgr()->Stop();
}
void MainWindow::OnMp3Stop()
{
	if (m_pbtnStart && m_pbtnPause)
	{
		m_pbtnStart->SetVisible(true, false);
		m_pbtnPause->SetVisible(false, false);
	}
	if( NULL != m_pbtnStop )
	{
		m_pbtnStop->SetEnable(false);
	}

	this->UpdateLayout(false);

	if (m_pLabelPlaystatus)
	{
		m_strStatusText = _T("状态:停止");
		m_pLabelPlaystatus->SetText(m_strStatusText.c_str(), false);
	}
	if (m_pProgress)
	{
		m_pProgress->SetPos(0, false);
	}

    m_nTimeElapse = 0;
	if (m_pLEDTime)
	{
		m_pLEDTime->SetText(_T(" 00:00"), false);
	}

	this->UpdateObject();
}

void  MainWindow::OnMp3Finish(bool bHaveNext)
{
    if (bHaveNext)
        return;  // 什么也不干

    this->OnMp3Stop();

    if (m_pMultiLabelTitle)
    {
        m_pMultiLabelTitle->SetContent(NULL);
        m_pMultiLabelTitle->StopAnimateSwitch();
    }
}

void  MainWindow::OnLEDTimeClicked()
{
    if (LEDTimeShow_ELAPSE == m_eLEDTimeShowType)
        m_eLEDTimeShowType = LEDTimeShow_REMAIN;
    else
        m_eLEDTimeShowType = LEDTimeShow_ELAPSE;

    this->UpdateLEDTimeValue();
}
void MainWindow::OnBnClickMute()
{
	if( NULL == m_pbtnMute )
		return;

	bool bMute = !m_pbtnMute->IsChecked();
	GetMainMgr()->SetMute(bMute);
}

void MainWindow::OnBnClickPrev()
{
	GetMainMgr()->PlayPrev();
}
void MainWindow::OnBnClickNext()
{
	GetMainMgr()->PlayNext();
}

void MainWindow::OnBnClickOpen()
{
// 	COptionWindow win;
// 	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),GetHWND());
// 	return;

	CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0*.wma\0*.wma\0*.wav\0*.wav\0*.mp3;*.wma;*.wav;\0*.wma;*.mp3;*.wav\0*.*\0*.*\0\0"));
	if(IDCANCEL == dlg.DoModal())
		return;

	GetMainMgr()->Play(GetPlayerListMgr()->AddFile(dlg.m_szFileName));
}

void MainWindow::OnBnClickPlaylist()
{
	HWND hWnd = ::GetPlayerListMgr()->ToggleShowPlayerListDlg();
	if (NULL == m_hWndPlayerList)
	{
		this->SetPlayerListDlgHandle(hWnd);
	}
}
void MainWindow::SetPlayerListDlgHandle(HWND hWnd)
{
	HWND hOldValue = m_hWndPlayerList;
	m_hWndPlayerList = hWnd;

	if (NULL == hOldValue)
	{
        UI::SyncWindowData data;
		data.m_hWnd = hWnd;
		//data.m_nAnchorType = SYNC_OUT_BOTTOM;
// 		data.m_rcAnchorData.Width = -1;
// 		data.m_rcAnchorData.Height = -1;
		// TODO: TESET
		data.m_nAnchorType = ANCHOR_LEFT|ANCHOR_OUT_BOTTOM|ANCHOR_RIGHT;
		data.m_rcAnchorData.Height = -1;
		data.m_rcAnchorData.xOffset2 = 0;
		
		this->AddAnchorItem(data);
	}
}

void MainWindow::SetEqualizerDlgHandle(HWND hWnd)
{
	HWND hOldValue = m_hWndEqualizer;
	m_hWndEqualizer = hWnd;

	if (NULL == hOldValue)
	{
        UI::SyncWindowData data;
		data.m_hWnd = hWnd;
		data.m_nAnchorType = ANCHOR_OUT_RIGHT|ANCHOR_OUT_BOTTOM;
		data.m_rcAnchorData.Width = data.m_rcAnchorData.Height = -1;
		this->AddAnchorItem(data);
	}
}

void  MainWindow::SetLyricDlgHandle(HWND hWnd)
{
    HWND hOldValue = m_hWndLyric;
    m_hWndLyric = hWnd;

    if (NULL == hOldValue)
    {
        UI::SyncWindowData data;
        data.m_hWnd = hWnd;
        data.m_nAnchorType = ANCHOR_OUT_RIGHT|ANCHOR_TOP;
        data.m_rcAnchorData.Width = data.m_rcAnchorData.Height = -1;
        this->AddAnchorItem(data);
    }
}

void MainWindow::OnBnClickLyric()
{
    ::GetLyricMgr()->ToggleShowLyricDlg();
}

void MainWindow::OnBnClickEqualizer()
{
	::GetEqualizerMgr()->ToggleShowEqualizerDlg();
}

#if 0
void MainWindow::OnLButtonDown(UINT nFlags, POINT point)
{
	m_lSizeMove = 1;
	m_ptCursorSizeMove = point;
	::SetCapture(GetHWND());
}
void MainWindow::OnLButtonUp(UINT nFlags, POINT point)
{
	if (2 == m_lSizeMove)
	{
		this->OnExitSizeMove();
	}
	m_lSizeMove = 0;
	::ReleaseCapture();
}

void MainWindow::OnMouseMove(UINT nFlags, POINT point)
{
	if( 0 != m_lSizeMove )
	{
		if( 1 == m_lSizeMove )
		{
			this->OnEnterSizeMove();
		}

		// 计算新的窗口位置
		POINT ptCursor;
		::GetCursorPos(&ptCursor);

		POINT ptNew = { m_ptWndPosSizeMove.x + ptCursor.x - m_ptCursorSizeMove.x,
			m_ptWndPosSizeMove.y + ptCursor.y - m_ptCursorSizeMove.y };

		// 计算需要同步移动的窗口数量 
		int nCount = 1;
		if (::IsWindowVisible(m_hWndPlayerList))
		{ nCount++; }
		if( m_pEqualizerDlg != NULL && ::IsWindowVisible(m_pEqualizerDlg->GetHWND()) )
		{ nCount++; }
		if( m_pLyricDlg != NULL && ::IsWindowVisible(m_pLyricDlg->GetHWND()) )
		{ nCount++; }

		HDWP hdwp = BeginDeferWindowPos(nCount);
		
		hdwp = DeferWindowPos(hdwp, GetHWND(), NULL, ptNew.x, ptNew.y, 0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		if(::IsWindowVisible(m_hWndPlayerList) )
		{
			hdwp = DeferWindowPos( hdwp, m_hWndPlayerList, NULL, 
				ptNew.x, 
				ptNew.y + GetHeight(), 
				0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		}
		if( NULL != m_pEqualizerDlg && ::IsWindowVisible(m_pEqualizerDlg->GetHWND()) )
		{
			hdwp = DeferWindowPos( hdwp, m_pEqualizerDlg->GetHWND(), NULL, 
				ptNew.x + GetWidth(), 
				ptNew.y + GetHeight(), 
				0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		}
		if( NULL != m_pLyricDlg && ::IsWindowVisible(m_pLyricDlg->GetHWND()) )
		{
			hdwp = DeferWindowPos( hdwp, m_pLyricDlg->GetHWND(), NULL, 
				ptNew.x+GetWidth(), 
				ptNew.y, 
				0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		}
		EndDeferWindowPos(hdwp);
	}
}

void MainWindow::OnEnterSizeMove()
{
	m_lSizeMove = 2;

	RECT rc;
	::GetWindowRect(GetHWND(), &rc);
	m_ptWndPosSizeMove.x = rc.left;
	m_ptWndPosSizeMove.y = rc.top;

	::MapWindowPoints(GetHWND(), NULL, &m_ptCursorSizeMove, 1);

	RECT rcWorkArea;
	::SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rcWorkArea, 0);
	ClipCursor(&rcWorkArea);
}

void MainWindow::OnExitSizeMove()
{
	m_lSizeMove = 0;
	::ClipCursor(NULL);
}
void MainWindow::OnCancelMode()
{
	this->OnExitSizeMove();
}
#endif

void MainWindow::OnSysCommand(UINT nID, CPoint lParam)
{
	SetMsgHandled(FALSE);
	if (SC_CLOSE == nID)
	{
		this->HideAllAnchorItem();  // 瞬间隐藏所有窗口
	}
}

void MainWindow::OnContextMenu( HWND wnd, POINT point )
{
	if (this->GetHoverObject() != NULL)
		return;

// #ifdef _DEBUG
// 	 	COptionWindow win;
// 	 	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),GetHWND());
// 		return;
// #endif

#if 0 // 手动创建菜单 
	int nMenuID = 100;

	// Menu是自销毁的
	Menu* pMenu = NULL;
	UICreateInstance(&pMenu);

	Menu* pSubMenu = NULL;
	UICreateInstance(&pSubMenu);
	pSubMenu->AppendMenu(MF_STRING,nMenuID++, _T("Test"));

	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("千千选项"));
	pMenu->AppendMenu(MF_POPUP, (UINT_PTR)pSubMenu,  _T("相关链接"));
	pMenu->AppendMenu(MF_SEPARATOR, 0, NULL);

	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("播放控制"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("音量控制"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("播放模式"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("播放曲目"));
	pMenu->AppendMenu(MF_SEPARATOR, 0, NULL);

	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("视觉外观"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("歌词秀"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("均衡器"));
	pMenu->AppendMenu(MF_SEPARATOR, 0, NULL);

	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("皮肤"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("透明"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("查看"));
	pMenu->AppendMenu(MF_SEPARATOR, 0, NULL);

	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("最小化"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("全屏显示"));
	pMenu->AppendMenu(MF_STRING, nMenuID++, _T("退出"));

 	POINT pt;
 	GetCursorPos(&pt);
 	pMenu->TrackPopupMenu(0,pt.x,pt.y, static_cast<Message*>(this));
	SAFE_DELETE(pMenu);
	SAFE_DELETE(pSubMenu);

#else  // 从配置文件中加载

	if (NULL == m_pMainMenu)
	{
		// 加载主菜单
		LoadMainMenu();
	}

	POINT pt;
	GetCursorPos(&pt);

	m_pMainMenu->TrackPopupMenu(0,pt.x,pt.y, static_cast<IMessage*>(this));
#endif
}

void MainWindow::OnMenuClick(UINT nMenuID)
{
    if (NULL == m_pMainMenu)
        return;

	switch (nMenuID)
	{
	case 1001:
		{		
			if (NULL == m_pOptionWindow)
			{
                COptionWindow::CreateInstance(g_pUIApp, &m_pOptionWindow);
				m_pOptionWindow->DoModeless(g_pUIApp, g_hInstance, COptionWindow::IDD, _T("OptionWindow"),GetHWND());
			}
			m_pOptionWindow->ShowWindow();
//			win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),GetHWND());
		}		
		break;

	case 1014:
		{
			::PostMessage(GetHWND(), WM_SYSCOMMAND,SC_CLOSE, 0 );
		}
		break;
	}

	UINT nID = nMenuID;
	if (nID >= MENU_ID_SKIN_BASE &&  nID < (MENU_ID_SKIN_BASE+g_pSkinMgr->GetSkinCount()))
	{
        UI::IMenu*  pChangeSkinMenu = m_pMainMenu->GetSubMenuById(1009);
        if (pChangeSkinMenu)
        {
            UI::IListItemBase* pMenuItem = pChangeSkinMenu->GetItemById(nID);
            if (pMenuItem && !pMenuItem->IsChecked())
            {
                int nNewActiveIndex = nMenuID-MENU_ID_SKIN_BASE;
                UI::ISkinRes* pSkinRes = g_pSkinMgr->GetSkinResByIndex(nNewActiveIndex);
                g_pSkinMgr->ChangeSkin(pSkinRes, false);
            }
		}
	}
// 菜单将被销毁，并重新创建
// 	MenuBase* pSkinMenuItem = m_pMainMenu->GetSubMenuByPos(12);
// 	if (pSkinMenuItem)
// 	{
// 		MenuItem* p = (MenuItem*)pSkinMenuItem->GetFirstItem();
// 		int i = 0; 
// 		while (p)
// 		{
// 			if (i == nNewActiveIndex)
// 			{
// 				p->SetFlag(p->GetFlag()|MF_CHECKED);
// 			}
// 			else
// 			{
// 				p->SetFlag(p->GetFlag()&~MF_CHECKED);
// 			}
// 			i++;
// 			p = (MenuItem*)p->GetNextItem();
// 		}
// 	}
}

void MainWindow::OnMusicProgressPosChanged( int nPos, int nScrollType )
{
	if (SB_ENDSCROLL == nScrollType)
		GetMainMgr()->SetPlayProgressPercent(nPos);

//	UI_ChangeSkinH(nPos);
}
void MainWindow::OnVolumnChanged( int nPos, int nScrollType )
{
	if (nScrollType != SB_ENDSCROLL)
	{
		if (GetMainMgr()->SetVolumn(nPos))
		{
			if (m_pLabelPlaystatus)
			{
				TCHAR szInfo[16] = _T("");
				_stprintf(szInfo, _T("音量:%d%%"), nPos);
				m_pLabelPlaystatus->SetText(szInfo);
			}
		}
	}
	else
	{
		KillTimer(GetHWND(), TIMER_ID_RESTORE_STATUS_TEXT);
		SetTimer(GetHWND(), TIMER_ID_RESTORE_STATUS_TEXT, TIME_RESTORE_STATUS_TEXT, NULL);
		//m_pLabelPlaystatus->SetText(m_strStatusText);
	}
}


void MainWindow::OnMp3ProgressInd(double dSeconds, double dPercent)
{
    m_nTimeElapse = int(dSeconds*1000);
    UpdateLEDTimeValue();

	if (::GetCapture() == GetHWND())  // 正在拖拽过程中，不能再去设置它的位置
	{
        UI::IObject* pObj = this->GetPressObject();
		if (pObj)
		{
			if (pObj == m_pProgress || pObj->GetParentObject() == m_pProgress)
				return;
		}
	}

	if (m_pProgress)
	{
		m_pProgress->SetPos((int)(double)(dPercent*100));
	}
}

void  MainWindow::UpdateLEDTimeValue()
{
    IPlayerListItemInfo* pCurPlay = GetPlayerListMgr()->GetCurPlayingItem();
    if (NULL == pCurPlay)
        return;

    // 转换为时间
    TCHAR  cPrefix = _T(' ');
    int  nShowTime = 0;
    if (LEDTimeShow_ELAPSE == m_eLEDTimeShowType)
    {
        nShowTime = m_nTimeElapse/1000;
    }
    else
    {
        int nDuration = pCurPlay->GetDuration();
        if (0 == nDuration)
        {
            nShowTime = (int)m_nTimeElapse/1000;
        }
        else
        {
            nShowTime = (nDuration - m_nTimeElapse)/1000;
            cPrefix = _T('-');
        }
    }

    int h = nShowTime/3600;
    int temp = nShowTime%3600;
    int m = temp/60;
    int s = temp%60;

    TCHAR szTime[32] = _T("");
    if (h > 0)
    {
        _stprintf(szTime, _T("%c%02d:%02d:%02d"), cPrefix, h,m,s );
    }
    else
    {
        _stprintf(szTime, _T("%c%02d:%02d"), cPrefix, m,s );
    }

    if (m_pLEDTime)
    {
        m_pLEDTime->SetText(szTime);
    }
}

void MainWindow::OnBalanceChanged(long lPercent, bool bFinish)
{
	if (bFinish)
	{
//		m_pLabelPlaystatus->SetText(m_strStatusText);
		KillTimer(GetHWND(), TIMER_ID_RESTORE_STATUS_TEXT);
		SetTimer(GetHWND(), TIMER_ID_RESTORE_STATUS_TEXT, TIME_RESTORE_STATUS_TEXT, NULL);
	}
	else
	{

		TCHAR szInfo[16] = _T("");
		if (lPercent < 0)
		{
			_stprintf(szInfo, _T("偏左:%d%%"), -lPercent);
		}
		else if (lPercent > 0)
		{
			_stprintf(szInfo, _T("偏右:%d%%"), lPercent);
		}
		else 
		{
			_stprintf(szInfo, _T("平衡:居中"));
		}
		m_pLabelPlaystatus->SetText(szInfo);
	}
		
}
// void MainWindow::OnMp3VolumeInd(long lVolumn)
// {
// 	if( ::GetCapture() == GetHWND() )  // 正在拖拽过程中
// 	{
// 		Object* pObj = this->GetPressObject();
// 		if( NULL != pObj )
// 		{
// 			if( pObj == m_pVolume || pObj->GetParentObject() == m_pVolume )
// 			{
// 				return;
// 			}
// 		}
// 	}
// 
// 	int percent = (10000 + lVolumn) / 100;
// 	if( NULL != m_pVolume )
// 	{
// 		m_pVolume->SetPos(percent);
// 	}
// }

void MainWindow::OnPlayerListDlgCreated(HWND hWnd)
{
	this->SetPlayerListDlgHandle(hWnd);
}
void MainWindow::OnPlayerListDlgVisibleChanged(HWND hWnd, BOOL bVisible)
{
	if (m_pBtnPlaylist)
	{
		m_pBtnPlaylist->SetCheck(bVisible?BST_CHECKED:BST_UNCHECKED);
		m_pBtnPlaylist->UpdateObject();
	}
}
void MainWindow::OnEqualizerDlgCreated(HWND hWnd)
{
	this->SetEqualizerDlgHandle(hWnd);
}
void MainWindow::OnEqualizerDlgVisibleChanged(HWND wParam, BOOL bVisible)
{
	if (m_pBtnEqualizer)
	{
		m_pBtnEqualizer->SetCheck(bVisible?BST_CHECKED:BST_UNCHECKED);
		m_pBtnEqualizer->UpdateObject();
	}
}

void  MainWindow::OnLyricDlgCreated(HWND hWnd)
{
    this->SetLyricDlgHandle(hWnd);
}

void  MainWindow::OnLyricDlgVisibleChanged(HWND wParam, BOOL bVisible)
{
    if (m_pBtnLyric)
    {
        m_pBtnLyric->SetCheck(bVisible?BST_CHECKED:BST_UNCHECKED);
        m_pBtnLyric->UpdateObject();
    }
}

HBITMAP MainWindow::GetVisualizationInfo(RECT* prc)
{
	if (m_pVisuallzationPic)
	{
		CRect rc;
		m_pVisuallzationPic->GetClientRectInWindow(&rc);
		::CopyRect(prc, &rc);
        
		HBITMAP hBitmap = this->PaintObject(m_pVisuallzationPic);
// 		Image image;
// 		image.Attach(hBitmap);
// 		image.Save(L"C:\\adf.png", Gdiplus::ImageFormatPNG);
// 		image.Detach();
		return hBitmap;
	}
	return NULL;
}

void MainWindow::OnSkinHLSChanged()
{
#if 0 // -- 架构改造
	if (m_pVisuallzationPic)
	{
		HBITMAP hBitmap = this->PaintObject(m_pVisuallzationPic);
		GetMainMgr()->SetVisualizationBkgndBmp(hBitmap);
	}
#endif
}
void MainWindow::OnSkinChanged()
{
    LoadCtrls();

	if (m_pVisuallzationPic)
	{
		GetMainMgr()->InitVisualization();
	}

    UI::ISkinRes* pSkinRes = g_pSkinMgr->GetActiveSkin();
    if (pSkinRes)
    {
        GetMainMgr()->OnSkinChanged(pSkinRes->GetName());
    }
	SAFE_DELETE_Ixxx(m_pMainMenu);
}

void MainWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	if (nIDEvent == TIMER_ID_RESTORE_STATUS_TEXT)
	{
		KillTimer(GetHWND(), nIDEvent);
		if (m_pLabelPlaystatus)
		{
			m_pLabelPlaystatus->SetText(m_strStatusText.c_str());
		}
	}
}
BOOL MainWindow::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
	if (m_pVolume)
	{
		int nPos = m_pVolume->GetPos();
		if (zDelta>0)
		{
			nPos += 3;
		}
		else
		{
			nPos -= 3;
		}
		if (nPos < 0)
			nPos = 0;
		if (nPos > 100)
			nPos = 100;
		m_pVolume->SetPos(nPos);

		this->OnVolumnChanged(nPos, SB_ENDSCROLL+1);
		this->OnVolumnChanged(nPos, SB_ENDSCROLL);
	}
	return TRUE;
}
void MainWindow::OnVisualiazationTypeChanged(int nType)
{
	if (m_pVisuallzationPic)
	{
		m_pVisuallzationPic->UpdateObject();
	}
}

// 获取当前播放进度
int MainWindow::GetCurrentPlayingPercent()
{
	if (NULL == m_pProgress)
		return 0;

	return m_pProgress->GetPos();
}
void MainWindow::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
	for (int i = 0; i < nCount; i++)
	{
	}
}