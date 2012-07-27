#include "StdAfx.h"
#include "MainWindow.h"

MainWindow::MainWindow(void)
{
	m_pPlaylistDlg = NULL;
	m_pEqualizerDlg = NULL;
	m_pLyricDlg = NULL;

	m_pbtnStart = NULL;
	m_pbtnPause = NULL;
	m_pbtnStop = NULL;
	m_pbtnOpen = NULL;
	m_pbtnMute = NULL;
	m_pLabelPlaystatus = NULL;
	m_pLabelTime = NULL;
	m_pProgress = NULL;
	m_pVolume = NULL;

	m_lSizeMove = 0;
	m_ptCursorSizeMove.x = m_ptCursorSizeMove.y = 0;
	m_ptWndPosSizeMove.x = m_ptWndPosSizeMove.y = 0;
}

MainWindow::~MainWindow(void)
{
	if( NULL != m_pPlaylistDlg )
	{
		::DestroyWindow(m_pPlaylistDlg->m_hWnd);
		delete m_pPlaylistDlg;
		m_pPlaylistDlg = NULL;
	}
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
}

BOOL MainWindow::PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx )
{
	cs.style |= WS_MINIMIZE;
	return CustomWindow::PreCreateWindow(cs,dwStyleEx);;

}
void MainWindow::OnDestroy()
{
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


	COptionWindow win;
	win.DoModal(g_hInstance, COptionWindow::IDD, _T("OptionWindow"),m_hWnd);
}

void MainWindow::OnBnClickPlay()
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

	::mp3_play();
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
	CFileDialog dlg(TRUE, _T("*.mp3"), 0,4|2, _T("*.mp3\0*.mp3\0\0"));
	if(IDCANCEL == dlg.DoModal())
		return;

	::mp3_stop();
	::mp3_set_file(dlg.m_szFileName);
	::mp3_play();
}

void MainWindow::OnBnClickPlaylist()
{
	if( NULL == m_pPlaylistDlg )
	{
		m_pPlaylistDlg = new CPlayListDlg;
		m_pPlaylistDlg->Create(_T("playlistdlg"), m_hWnd);

		RECT  rc;
		::GetWindowRect(m_hWnd, &rc);
		::SetWindowPos( m_pPlaylistDlg->m_hWnd, NULL, rc.left, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );
	}

	if(m_pPlaylistDlg->IsVisible())
		UIAnimateWindow(m_pPlaylistDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	else
		::ShowWindow(m_pPlaylistDlg->m_hWnd,SW_SHOW);

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

void MainWindow::OnLButtonDown(UINT nFlags, POINT point)
{
	m_lSizeMove = 1;
	m_ptCursorSizeMove = point;
	::SetCapture(m_hWnd);
}
void MainWindow::OnLButtonUp(UINT nFlags, POINT point)
{
	if ( 2 == m_lSizeMove )
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
		if( m_pPlaylistDlg != NULL && ::IsWindowVisible(m_pPlaylistDlg->m_hWnd) )
		{ nCount++; }
		if( m_pEqualizerDlg != NULL && ::IsWindowVisible(m_pEqualizerDlg->m_hWnd) )
		{ nCount++; }
		if( m_pLyricDlg != NULL && ::IsWindowVisible(m_pLyricDlg->m_hWnd) )
		{ nCount++; }

		HDWP hdwp = BeginDeferWindowPos(nCount);
		
		hdwp = DeferWindowPos(hdwp, m_hWnd, NULL, ptNew.x, ptNew.y, 0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
		if( NULL != m_pPlaylistDlg && ::IsWindowVisible(m_pPlaylistDlg->m_hWnd) )
		{
			hdwp = DeferWindowPos( hdwp, m_pPlaylistDlg->m_hWnd, NULL, 
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

void MainWindow::OnTimer(UINT_PTR nIDEvent)
{

}

void MainWindow::OnContextMenu( HWND wnd, POINT point )
{
	if( this->GetHoverObject() != NULL )
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
		_stprintf(szTime, _T("%02d:%02d:%02d"), h,m,s );
	}
	else
	{
		_stprintf(szTime, _T("%02d:%02d"), m,s );
	}
	if( NULL != m_pLabelTime )
	{
		m_pLabelTime->SetText(szTime);
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


//////////////////////////////////////////////////////////////////////////

COptionWindow::COptionWindow()
{
	m_pListOption = NULL;
}

BOOL COptionWindow::OnInitDialog( HWND, LPARAM )
{
	m_pListOption = (ListBox*)this->FindChildObject(_T("optionlist") );
	m_pListOption->ModifyStyle( 0, LISTCTRLBASE_SORT_ASCEND|LISTCTRLBASE_SORT_DESCEND);
	m_pListOption->SetFixedItemHeight(24, false);

	String strArray[] = { _T("关于"),_T("常规"),_T("播放"),_T("快捷键"),_T("视觉效果"),
		_T("播放列表"),_T("媒体库"),_T("歌词秀"),_T("歌词搜索"),_T("网络连接"),_T("音效插件"),
		_T("音频设备"),_T("皮肤"),_T("全屏显示"),_T("系统关联")};
	int nCount = sizeof(strArray)/sizeof(String);
	for (int i = 0; i < nCount; i++)
	{
		m_pListOption->AddString(strArray[i]);
	}

	return FALSE;
}
void COptionWindow::OnClose()
{
	this->EndDialog(IDCANCEL);
}
void COptionWindow::OnDestroy()
{
}

void COptionWindow::OnBtnSaveAll()
{
}
void COptionWindow::OnBtnResetAll()
{
}
void COptionWindow::OnBtnCancel()
{
	this->OnClose();	
}

void COptionWindow::OnBtnClick1(UINT uNotifyCode, int nID, HWND wndCtl)
{
	Object* pObj = this->FindChildObject(_T("test_check"));
	if( NULL != pObj )
	{
		pObj->SetVisible(!pObj->IsVisible());
	}
}

void COptionWindow::OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl)
{
	Object* pObj = this->FindChildObject(_T("groupbox1"));
	if( NULL != pObj )
	{
		pObj->SetVisible(!pObj->IsVisible());
	}
}