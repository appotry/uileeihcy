#include "StdAfx.h"
#include "MainWindow.h"
#include "player.h"
#include "PlayerListMgr.h"
#include "OptionWindow.h"
#include "MainMgr.h"
#include "EqualizerMgr.h"

MainWindow::MainWindow(void)
{
	m_hWndPlayerList = NULL;
	m_hWndEqualizer = NULL;
	m_pLyricDlg = NULL;

	m_pbtnStart = NULL;
	m_pbtnPause = NULL;
	m_pbtnStop = NULL;
	m_pbtnOpen = NULL;
	m_pbtnMute = NULL;
	m_pLabelPlaystatus = NULL;
	m_pLabelTime = NULL;
	m_pLEDTime = NULL;
	m_pProgress = NULL;
	m_pVolume = NULL;
	m_pBtnPlaylist = NULL;
	m_pBtnLyric = NULL;
	m_pBtnEqualizer = NULL;

// 	m_lSizeMove = 0;
// 	m_ptCursorSizeMove.x = m_ptCursorSizeMove.y = 0;
// 	m_ptWndPosSizeMove.x = m_ptWndPosSizeMove.y = 0;
}

MainWindow::~MainWindow(void)
{
	if( NULL != m_pLyricDlg )
	{
		::DestroyWindow(m_pLyricDlg->m_hWnd);
		delete m_pLyricDlg;
		m_pLyricDlg = NULL;
	}
}

BOOL MainWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= WS_MINIMIZE;
	return CustomWindow::PreCreateWindow(cs);;

}
void MainWindow::OnDestroy()
{
	SetMsgHandled(FALSE);
	::PostQuitMessage(0);
}

void MainWindow::OnInitWindow()
{
	CustomWindow::OnInitWindow();

	m_pbtnStart        = (Button*)this->FindChildObject(_T("btn_play") );
	m_pbtnPause        = (Button*)this->FindChildObject(_T("btn_pause") );
	m_pbtnStop         = (Button*)this->FindChildObject(_T("btn_stop") );
	m_pbtnOpen         = (Button*)this->FindChildObject(_T("btn_open") );
	m_pBtnPlaylist     = (CheckButton*)this->FindChildObject(_T("btn_playlist") );
	m_pBtnLyric        = (CheckButton*)this->FindChildObject(_T("btn_lyric") );
	m_pBtnEqualizer    = (CheckButton*)this->FindChildObject(_T("btn_equalizer") );
	m_pbtnMute         = (CheckButton*)this->FindChildObject(_T("btn_mute") );
	m_pLabelPlaystatus = (Label*)this->FindChildObject(_T("label_playstatus"));
	m_pProgress        = (SliderCtrl*)this->FindChildObject(_T("progress_music"));
	m_pVolume          = (SliderCtrl*)this->FindChildObject(_T("progress_voice"));
	m_pLabelTime       = (Label*)this->FindChildObject(_T("label_time"));
	m_pLEDTime         = (LEDCtrl*)this->FindChildObject(_T("led_time"));

	if( NULL != m_pbtnStop )
	{
		m_pbtnStop->SetEnable(false);
	}

	if( NULL != m_pbtnPause )
	{
		m_pbtnPause->SetVisible(false);
	}

	::SetWindowText(m_hWnd, _T("Player_Demo"));
// 	COptionWindow win;
// 	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
}

void MainWindow::OnBnClickPlay()
{
	GetMainMgr()->Play();
}
void MainWindow::OnMp3Start(PlayerListItemInfo* pItemInfo)
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
		m_pLabelPlaystatus->SetText(_T("状态:播放"));
	}

	::SetWindowText(m_hWnd, pItemInfo->GetFileName().c_str());
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
		m_pLabelPlaystatus->SetText(_T("状态:播放"));
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
		m_pLabelPlaystatus->SetText(_T("状态:暂停"));
	}
}

void MainWindow::OnBnClickStop()
{
	GetMainMgr()->Stop();
}
void MainWindow::OnMp3Stop()
{
	if( NULL != m_pbtnStart && NULL != m_pbtnPause )
	{
		m_pbtnStart->SetVisible(true);
		m_pbtnPause->SetVisible(false);
	}
	if( NULL != m_pbtnStop )
	{
		m_pbtnStop->SetEnable(false);
	}

	UI_UpdateLayout(this);

	if( NULL != m_pLabelPlaystatus )
	{
		m_pLabelPlaystatus->SetText(_T("状态:停止"));
	}
	if( NULL != m_pProgress )
	{
		m_pProgress->SetPos(0);
	}

	if (NULL != m_pLEDTime)
	{
		m_pLEDTime->SetText(_T(" 00:00"));
	}
	if (NULL != m_pLabelTime)
	{
		m_pLabelTime->SetText(_T(" 00:00"));
	}
}

void MainWindow::OnBnClickMute()
{
	if( NULL == m_pbtnMute )
		return;

	if( m_pbtnMute->IsChecked() )
		mp3_mute(false);
	else
		mp3_mute(true);
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
	COptionWindow win;
	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
	return;

	CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0\0"));
	if(IDCANCEL == dlg.DoModal())
		return;

	OnBnClickStop();
	::mp3_set_file(dlg.m_szFileName);
	OnBnClickPlay();
}

void MainWindow::OnBnClickPlaylist()
{
	HWND hWnd = ::GetPlayerListMgr()->ToggleShowPlayerListDlg(m_hWnd);
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
		SyncWindowData data;
		data.m_hWnd = hWnd;
		data.m_nAnchorType = SYNC_OUT_BOTTOM;
		data.m_rcAnchorData.Width = -1;
		data.m_rcAnchorData.Height = -1;
		this->AddAnchorItem(data);
	}
}

void MainWindow::SetEqualizerDlgHandle(HWND hWnd)
{
	HWND hOldValue = m_hWndEqualizer;
	m_hWndEqualizer = hWnd;

	if (NULL == hOldValue)
	{
		SyncWindowData data;
		data.m_hWnd = hWnd;
		data.m_nAnchorType = SYNC_OUT_RIGHT|SYNC_OUT_BOTTOM;
		data.m_rcAnchorData.Width = data.m_rcAnchorData.Height = -1;
		this->AddAnchorItem(data);
	}
}

void MainWindow::OnBnClickLyric()
{
	if( NULL == m_pLyricDlg )
	{
		m_pLyricDlg = new CLyricDlg;
		m_pLyricDlg->Create(_T("lyricdlg"), m_hWnd);

		RECT  rc;
		::GetWindowRect(m_hWnd, &rc);
		::SetWindowPos( m_pLyricDlg->m_hWnd, NULL, rc.right, rc.top, 
			rc.right-rc.left, rc.bottom-rc.top
			, SWP_NOZORDER|SWP_NOSIZE );

		SyncWindowData data;
		data.m_hWnd = m_pLyricDlg->m_hWnd;
		data.m_nAnchorType = SYNC_OUT_RIGHT;
		data.m_rcAnchorData.Width = data.m_rcAnchorData.Height = -1;
		this->AddAnchorItem(data);
	}

	if(m_pLyricDlg->IsVisible())
	{
		::ShowWindow(m_pLyricDlg->m_hWnd, SW_HIDE);
	}
	else
	{
		::ShowWindow(m_pLyricDlg->m_hWnd,SW_SHOW);
	}
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
	::SetCapture(m_hWnd);
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
		if( m_pEqualizerDlg != NULL && ::IsWindowVisible(m_pEqualizerDlg->m_hWnd) )
		{ nCount++; }
		if( m_pLyricDlg != NULL && ::IsWindowVisible(m_pLyricDlg->m_hWnd) )
		{ nCount++; }

		HDWP hdwp = BeginDeferWindowPos(nCount);
		
		hdwp = DeferWindowPos(hdwp, m_hWnd, NULL, ptNew.x, ptNew.y, 0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		if(::IsWindowVisible(m_hWndPlayerList) )
		{
			hdwp = DeferWindowPos( hdwp, m_hWndPlayerList, NULL, 
				ptNew.x, 
				ptNew.y + GetHeight(), 
				0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		}
		if( NULL != m_pEqualizerDlg && ::IsWindowVisible(m_pEqualizerDlg->m_hWnd) )
		{
			hdwp = DeferWindowPos( hdwp, m_pEqualizerDlg->m_hWnd, NULL, 
				ptNew.x + GetWidth(), 
				ptNew.y + GetHeight(), 
				0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE );
		}
		if( NULL != m_pLyricDlg && ::IsWindowVisible(m_pLyricDlg->m_hWnd) )
		{
			hdwp = DeferWindowPos( hdwp, m_pLyricDlg->m_hWnd, NULL, 
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
	::GetWindowRect(m_hWnd, &rc);
	m_ptWndPosSizeMove.x = rc.left;
	m_ptWndPosSizeMove.y = rc.top;

	::MapWindowPoints(m_hWnd, NULL, &m_ptCursorSizeMove, 1);

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

#define MENU_ID_SKIN_BASE 900
void MainWindow::OnContextMenu( HWND wnd, POINT point )
{
	if( this->GetHoverObject() != NULL )
		return;

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

	POINT pt;
	GetCursorPos(&pt);

	int nTick1 = GetTickCount();
	Menu*  pMenu = UI_LoadMenu(_T("main_menu"));
	if (NULL == pMenu)
		return;

	MenuBase* pSkinMenuItem = pMenu->GetSubMenuByPos(12);
	if (NULL != pSkinMenuItem)
	{
		// 皮肤列表
		int nCount = ::UI_GetSkinCount();
		int nActiveIndex = ::UI_GetActiveSkinIndex();
		for( int i = 0; i < nCount; i++ )
		{
			int nSize = ::UI_GetSkinName( i, NULL, 0 );
			TCHAR* pszName = new TCHAR[nSize];
			nSize = ::UI_GetSkinName( i, pszName, nSize );
			String strSkinName = pszName ;
			delete [] pszName;
			pszName = NULL;

			UINT nFlag = MF_STRING;
			if( nActiveIndex == i )
			{
				nFlag |= MF_CHECKED;
			}
			pSkinMenuItem->AppendMenu(nFlag, i+MENU_ID_SKIN_BASE, strSkinName.c_str() );
		}
	}
	
	UI_LOG_DEBUG(_T("%s load menu time: %d"), FUNC_NAME, GetTickCount()-nTick1);
	pMenu->TrackPopupMenu(0,pt.x,pt.y, static_cast<Message*>(this));

	SAFE_DELETE(pMenu);
#endif
}

void MainWindow::OnMenuClick(UINT nMenuID)
{
	switch( nMenuID )
	{
	case 1001:
		{		
			COptionWindow win;
			win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
		}		
		break;

	case 1014:
		{
			OnSysClose();
		}
		break;
	}

	int nCount = ::UI_GetSkinCount();
	int nID = nMenuID;
	if (nID >= MENU_ID_SKIN_BASE &&  nID < (MENU_ID_SKIN_BASE+nCount))
	{
		int nActiveIndex = ::UI_GetActiveSkinIndex();
		if( nID != nActiveIndex+MENU_ID_SKIN_BASE )
		{
			UI_ChangeSkin(nMenuID-MENU_ID_SKIN_BASE);
		}
	}

}

void MainWindow::OnMusicProgressPosChanged( int nPos, int nScrollType )
{
	if( SB_ENDSCROLL == nScrollType )
		::mp3_set_cur_pos(nPos);

//	UI_ChangeSkinH(nPos);
}
void MainWindow::OnVolumnChanged( int nPos, int nScrollType )
{
	if( nScrollType != SB_ENDSCROLL )
		::mp3_set_volumn(nPos);
}


void MainWindow::OnMp3ProgressInd(LONGLONG llCur, LONGLONG llDuration)
{
	if( 0 == llDuration )
		return;

	if( ::GetCapture() == m_hWnd )  // 正在拖拽过程中
	{
		Object* pObj = this->GetPressObject();
		if( NULL != pObj )
		{
			if( pObj == m_pProgress || pObj->GetParentObject() == m_pProgress )
			{
				return;
			}
		}
	}

	if( NULL != m_pProgress )
	{
		m_pProgress->SetPos((int)(double)(llCur*100/llDuration));
	}
	
	// 转换为时间
	int n = (int)(double)(llCur/10000000);
	int h = n/3600;
	int temp = n%3600;
	int m = temp/60;
	int s = temp%60;

	TCHAR szTime[32] = _T("");
	if( h > 0 )
	{
		_stprintf(szTime, _T(" %02d:%02d:%02d"), h,m,s );
	}
	else
	{
		_stprintf(szTime, _T(" %02d:%02d"), m,s );
	}
	if (NULL != m_pLabelTime)
	{
		m_pLabelTime->SetText(szTime);
	}
	if (NULL != m_pLEDTime)
	{
		m_pLEDTime->SetText(szTime);
	}
}

void MainWindow::OnMp3VolumeInd(long lVolumn)
{
	if( ::GetCapture() == m_hWnd )  // 正在拖拽过程中
	{
		Object* pObj = this->GetPressObject();
		if( NULL != pObj )
		{
			if( pObj == m_pVolume || pObj->GetParentObject() == m_pVolume )
			{
				return;
			}
		}
	}

	int percent = (10000 + lVolumn) / 100;
	if( NULL != m_pVolume )
	{
		m_pVolume->SetPos(percent);
	}
}

void MainWindow::OnPlayerListDlgCreated(HWND hWnd)
{
	this->SetPlayerListDlgHandle(hWnd);
}
void MainWindow::OnPlayerListDlgVisibleChanged(HWND hWnd, BOOL bVisible)
{
	if (NULL != m_pBtnPlaylist)
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
	if (NULL != m_pBtnEqualizer)
	{
		m_pBtnEqualizer->SetCheck(bVisible?BST_CHECKED:BST_UNCHECKED);
		m_pBtnEqualizer->UpdateObject();
	}
}