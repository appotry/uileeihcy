#include "StdAfx.h"
#include "MainWindow.h"
#include "player.h"
#include "PlayerListMgr.h"
#include "OptionWindow.h"
#include "MainMgr.h"

MainWindow::MainWindow(void)
{
	m_hWndPlayerList = NULL;
	m_pEqualizerDlg = NULL;
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

// 	m_lSizeMove = 0;
// 	m_ptCursorSizeMove.x = m_ptCursorSizeMove.y = 0;
// 	m_ptWndPosSizeMove.x = m_ptWndPosSizeMove.y = 0;
	m_pMenu = NULL;
}

MainWindow::~MainWindow(void)
{
	if( NULL != m_pEqualizerDlg )
	{
		::DestroyWindow(m_pEqualizerDlg->m_hWnd);
		delete m_pEqualizerDlg;
		m_pEqualizerDlg = NULL;
	}
	if( NULL != m_pLyricDlg )
	{
		::DestroyWindow(m_pLyricDlg->m_hWnd);
		delete m_pLyricDlg;
		m_pLyricDlg = NULL;
	}
	SAFE_DELETE(m_pMenu);
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

	m_pbtnStart = (Button*)this->FindChildObject(_T("btn_play") );
	m_pbtnPause = (Button*)this->FindChildObject(_T("btn_pause") );
	m_pbtnStop = (Button*)this->FindChildObject(_T("btn_stop") );
	m_pbtnOpen = (Button*)this->FindChildObject(_T("btn_open") );
	m_pbtnMute = (CheckButton*)this->FindChildObject(_T("btn_mute") );
	m_pLabelPlaystatus = (Label*)this->FindChildObject(_T("label_playstatus"));
	m_pProgress = (SliderCtrl*)this->FindChildObject(_T("progress_music"));
	m_pVolume = (SliderCtrl*)this->FindChildObject(_T("progress_voice"));
	m_pLabelTime = (Label*)this->FindChildObject(_T("label_time"));
	m_pLEDTime = (LEDCtrl*)this->FindChildObject(_T("led_time"));

	if( NULL != m_pbtnStop )
	{
		m_pbtnStop->SetEnable(false);
	}

	if( NULL != m_pbtnPause )
	{
		m_pbtnPause->SetVisible(false);
	}

	::mp3_add_event_callback(this);

	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	String str =  szPath;
	str += _T("wine-of-bincos-lunba.mp3");
	::mp3_set_file(str);

	::SetWindowText(m_hWnd, _T("wine-of-bincos-lunba.mp3"));

// 	COptionWindow win;
// 	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
}

void MainWindow::OnBnClickPlay()
{
	GetMainMgr()->Play();
}
void MainWindow::OnMP3Start(PlayerListItemInfo* pItemInfo)
{
	if( NULL != m_pbtnStart && NULL != m_pbtnPause )
	{
		m_pbtnPause->SetVisible(true, false);
		m_pbtnStart->SetVisible(false, false);
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
	if( NULL != m_pbtnStart && NULL != m_pbtnPause )
	{
		m_pbtnStart->SetVisible(true, false);
		m_pbtnPause->SetVisible(false, false);
	}

	if( NULL != m_pLabelPlaystatus )
	{
		m_pLabelPlaystatus->SetText(_T("状态:暂停"));
	}

	::mp3_pause();
}

void MainWindow::OnBnClickStop()
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

	::mp3_stop();
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
	HWND hWnd = ::GetPlayerListMgr()->ShowPlayerListDlg(m_hWnd);
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
		AnchorWindowData data;
		data.m_hWnd = hWnd;
		data.m_nAnchorType = ANCHOR_OUT_BOTTOM;
		data.m_rcAnchorData.Width = -1;
		data.m_rcAnchorData.Height = -1;
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

		AnchorWindowData data;
		data.m_hWnd = m_pLyricDlg->m_hWnd;
		data.m_nAnchorType = ANCHOR_OUT_RIGHT;
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
	if( NULL == m_pEqualizerDlg )
	{
		m_pEqualizerDlg = new CEqualizerDlg;
		m_pEqualizerDlg->Create(_T("equalizerdlg"), m_hWnd);

		RECT  rc;
		::GetWindowRect(m_hWnd, &rc);
		::SetWindowPos( m_pEqualizerDlg->m_hWnd, NULL, rc.right, rc.bottom, 
			rc.right-rc.left, rc.bottom-rc.top
			, SWP_NOZORDER|SWP_NOSIZE );

		AnchorWindowData data;
		data.m_hWnd = m_pEqualizerDlg->m_hWnd;
		data.m_nAnchorType = ANCHOR_OUT_RIGHT|ANCHOR_OUT_BOTTOM;
		data.m_rcAnchorData.Width = data.m_rcAnchorData.Height = -1;
		this->AddAnchorItem(data);
	}

	if(m_pEqualizerDlg->IsVisible())
	{
		::ShowWindow(m_pEqualizerDlg->m_hWnd, SW_HIDE);
	}
	else
	{
		::ShowWindow(m_pEqualizerDlg->m_hWnd,SW_SHOW);
	}
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
void MainWindow::OnTimer(UINT_PTR nIDEvent)
{

}

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
	if( this->GetHoverObject() != NULL )
		return;

	if (NULL == m_pMenu)
		UICreateInstance(&m_pMenu);

	m_pMenu->AppendMenu(MF_STRING, 101, _T("Test"));
 
 	POINT pt;
 	GetCursorPos(&pt);
 	m_pMenu->TrackPopupMenu(0,pt.x,pt.y,0);

	return;

#define MENU_ID_OPTION    1
#define MENU_ID_SKIN_BASE 1000

	HMENU hMenu = ::CreatePopupMenu();
	HMENU hSkinListMenu = ::CreatePopupMenu();
	::AppendMenu(hMenu, MF_STRING, (UINT_PTR)MENU_ID_OPTION, _T("选项") );
	::AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSkinListMenu, _T("皮肤") );

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
		::AppendMenu(hSkinListMenu, nFlag, i+MENU_ID_SKIN_BASE, strSkinName.c_str() );
	}

	
	UINT nRet = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, point.x, point.y, 0, m_hWnd, NULL );
	::DestroyMenu(hSkinListMenu);
	::DestroyMenu(hMenu);
	hMenu = NULL;
	hSkinListMenu = NULL;


	if( nRet >= MENU_ID_SKIN_BASE )
	{
		if( nRet != nActiveIndex+MENU_ID_SKIN_BASE )
		{
			UI_ChangeSkin(nRet-MENU_ID_SKIN_BASE);
		}
	}
	else
	{
		switch(nRet)
		{
		case MENU_ID_OPTION:
			{
				COptionWindow win;
				win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
				//win.DoModal(_T("OptionWindow"),m_hWnd);
				//win.DoModeless(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
				//win.DoModeless(_T("OptionWindow"),m_hWnd);
				//win.ShowWindow();
			}
			break;
		default:
			
			break;
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

void MainWindow::on_mp3_stop()
{
	this->OnBnClickStop();	
}

void MainWindow::on_mp3_progress_ind(LONGLONG llCur, LONGLONG llDuration)
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

void MainWindow::on_mp3_volume_ind(long lVolumn)
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

