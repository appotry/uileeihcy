#include "stdafx.h"
#include "MainMgr.h"
#include "MainWindow.h"

CMainMgr::CMainMgr()
{
	m_pMainWindow = NULL;
	m_pCurPlayingItem = NULL;
}

CMainMgr::~CMainMgr()
{
}

bool CMainMgr::Initialize()
{
	// 加载配置文件
	m_configFile.Load(&m_config);

	// 初始化
	if (NULL == m_pMainWindow)
	{
		m_pMainWindow = new MainWindow;
		m_pMainWindow->Create( _T("mainwindow") );
	}

	bool bRet = ::mp3_init(m_pMainWindow->m_hWnd);
	::mp3_add_event_callback(this);

	::GetPlayerListMgr(); // 提前初始化获取播放列表
	::GetEqualizerMgr();  // 初始化均衡器Mgr

	::mp3_mute(m_config.player.m_bMute);

	m_pMainWindow->ShowWindow();
	::UpdateWindow(m_pMainWindow->m_hWnd);

	VisualizationInfo info;
	info.nMask = VI_MASK_HWND|VI_MASK_RECT|VI_MASK_SPECTRUM_BAND_COUNT|VI_MASK_TYPE|VI_MASK_BKGND_BMP|VI_MASK_SPECTRUM_BAND_WIDTH|VI_MASK_FPS;
	info.hWnd = m_pMainWindow->m_hWnd;
	info.hBkgndBmp = m_pMainWindow->GetVisualizationInfo(&info.rcRender);
	info.nSpectrumBandWidth = 3;
	info.nSpectrumBandCount = (info.rcRender.right-info.rcRender.left)/4;
	info.eType = (E_VISUALIZATION_TYPE)m_config.visual.m_nType;
	info.nFps = m_config.visual.m_nFps;

	// 获取透明背景图
	HDC hDC = GetDC(info.hWnd);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, info.rcRender.right-info.rcRender.left, info.rcRender.bottom-info.rcRender.top);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC, 0,0, info.rcRender.right-info.rcRender.left, info.rcRender.bottom-info.rcRender.top,
		hDC, info.rcRender.left, info.rcRender.top, SRCCOPY);
	::SelectObject(hMemDC, hOldBmp);
	::DeleteDC(hMemDC);
	::ReleaseDC(info.hWnd, hDC);
	
	::mp3_set_visualization(&info);
	return bRet;
}

bool CMainMgr::Release()
{
	SAFE_DELETE(m_pMainWindow);

	bool bRet = ::mp3_release();
	if (false == bRet)
		return false;

	return this->m_configFile.Save(&m_config);
}

HWND CMainMgr::GetMainWnd()
{
	if (NULL == m_pMainWindow)
	{
		return NULL;
	}

	return m_pMainWindow->m_hWnd;
}


void CMainMgr::HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
	switch (nEventType)
	{
	case EVENT_TYPE_UI:
		{
			if (NULL == m_pMainWindow)
				break;

			if (UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE == nEventId)
			{
				m_pMainWindow->OnPlayerListDlgCreated((HWND)wParam);
			}
			else if (UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED == nEventId)
			{
				m_pMainWindow->OnPlayerListDlgVisibleChanged((HWND)wParam, (BOOL)lParam);
			}
			else if (UI_EVENT_ID_ON_EQUALIZERDLG_CREATE == nEventId)
			{
				m_pMainWindow->OnEqualizerDlgCreated((HWND)wParam);
			}
			else if(UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED == nEventId)
			{
				m_pMainWindow->OnEqualizerDlgVisibleChanged((HWND)wParam, (BOOL)lParam);
			}
		}
		break;
	case EVENT_TYPE_PLAY:
		{
			if (NULL == m_pMainWindow)
				break;

			if (PLAY_EVENT_ID_ON_START == nEventId )
			{
				m_pMainWindow->OnMp3Start((PlayerListItemInfo*)wParam);
			}
			else if (PLAY_EVENT_ID_ON_PAUSE == nEventId)
			{
				m_pMainWindow->OnMp3Pause();
			}
			else if (PLAY_EVENT_ID_ON_STOP == nEventId)
			{
				m_pMainWindow->OnMp3Stop();
			}
			else if (PLAY_EVENT_ID_ON_CONTINUE == nEventId)
			{
				m_pMainWindow->OnMp3Continue();
			}
			else if (PLAY_EVENT_ID_REMOVEALL == nEventId)
			{
				m_pCurPlayingItem = NULL;
			}
			else if (PLAY_EVENT_ID_REMOVEITEM == nEventId)
			{
				if (m_pCurPlayingItem == (PlayerListItemInfo*)wParam)
				{
					m_pCurPlayingItem = NULL;
				}
			}
		}
		break;
	}
}

//
//	开始播放指定文件
//
void CMainMgr::Play(PlayerListItemInfo* pItem)
{
	if (NULL == pItem)
		return;

	if (NULL != m_pCurPlayingItem)
	{
		Stop();
	}
	if (false == ::mp3_set_file(pItem->GetFilePath()))
	{
		UIASSERT(0);
		return;
	}

	if (false == ::mp3_play())
	{
		UIASSERT(0);
		return;
	}

	m_pCurPlayingItem = pItem;
	FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_START, (WPARAM)m_pCurPlayingItem);
}

//
// 开始播放当前文件，或者继续播放当前文件
//
void CMainMgr::Play()
{
	if (NULL == m_pCurPlayingItem)
	{
		bool bPlay = false;
		m_pCurPlayingItem = GetPlayerListMgr()->GetNextPlayItem(bPlay);
		if (bPlay)
		{
			Play(m_pCurPlayingItem);
		}
		return;
	}

	bool bRet = ::mp3_play();
	if (bRet)
	{
		FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_CONTINUE);
	}
}
void CMainMgr::Stop()
{
	if (NULL == m_pCurPlayingItem)
		return;

	if (::mp3_stop())
	{
		FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_STOP);
	}
}
void CMainMgr::Pause()
{
	if (NULL == m_pCurPlayingItem)
		return;

	if (::mp3_pause())
	{
		FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_PAUSE);
	}
}

void CMainMgr::PlayNext()
{
	Play(GetPlayerListMgr()->GetNextItem());
}
void CMainMgr::PlayPrev()
{
	Play(GetPlayerListMgr()->GetPrevItem());
}

bool CMainMgr::SetVisualizationType(int nType)
{
	VisualizationInfo info;
	info.nMask = VI_MASK_TYPE;
	info.eType = (E_VISUALIZATION_TYPE)nType;
	if (::mp3_set_visualization(&info))
	{
		m_config.visual.m_bDirty = true;
		m_config.visual.m_nType = nType;
		m_configFile.Save(&m_config);
	}

	return false;
}
bool CMainMgr::SetVisualizationFps(int nFps)
{
	VisualizationInfo info;
	info.nMask = VI_MASK_FPS;
	info.nFps = nFps;
	if (::mp3_set_visualization(&info))
	{
		m_config.visual.m_bDirty = true;
		m_config.visual.m_nFps = nFps;
		m_configFile.Save(&m_config);
	}

	return false;
}

bool CMainMgr::SetVisualizationBkgndBmp(HBITMAP hBitmap)
{
	VisualizationInfo info;
	info.nMask = VI_MASK_BKGND_BMP;
	info.hBkgndBmp = hBitmap;
	return mp3_set_visualization(&info);
}

bool CMainMgr::SetVisualizationBkgndBmpAndRect(HBITMAP hBitmap, RECT* prc)
{
	VisualizationInfo info;
	info.nMask = VI_MASK_BKGND_BMP|VI_MASK_RECT;
	info.hBkgndBmp = hBitmap;
	CopyRect(&info.rcRender, prc);
	return mp3_set_visualization(&info);
}
void CMainMgr::on_mp3_stop()
{
	FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_STOP);

	bool bPlay = false;
	PlayerListItemInfo* p = GetPlayerListMgr()->GetNextPlayItem(bPlay);
	if (bPlay)
	{
		this->Play(p);
	}
}
void CMainMgr::on_mp3_progress_ind(double dSeconds, double dPercent)
{
	if (NULL != m_pMainWindow)
	{
		m_pMainWindow->OnMp3ProgressInd(dSeconds, dPercent);
	}
}
void CMainMgr::on_mp3_volume_ind(long lVolumn)
{
	if (NULL != m_pMainWindow)
	{
		m_pMainWindow->OnMp3VolumeInd(lVolumn);
	}
}