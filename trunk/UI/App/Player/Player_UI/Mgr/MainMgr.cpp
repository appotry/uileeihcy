#include "stdafx.h"
#include "App\Player\Player_UI\Mgr\MainMgr.h"
#include "App\Player\Player_UI\UI\MainWindow.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "EqualizerMgr.h"

void  UITimerProc_Progress(UINT_PTR nTimerID, UI::TimerItem* pTimerItem);

CMainMgr::CMainMgr()
{
	m_pMainWindow = NULL;
	m_pCurPlayingItem = NULL;
    m_nProgressTimerId = 0;
}

CMainMgr::~CMainMgr()
{
    if (m_nProgressTimerId)
    {
        g_pUIApp->KillTimer(m_nProgressTimerId);
    }
}

bool CMainMgr::Initialize()
{
	// 初始化
	if (NULL == m_pMainWindow)
	{
        MainWindow::CreateInstance(g_pUIApp, &m_pMainWindow);
		m_pMainWindow->Create(g_pUIApp, _T("mainwindow") );
	}

    bool bRet = ::mp3_init(m_pMainWindow->GetHWND());
    ::mp3_add_event_callback(this);

    ::mp3_mute(GetConfigData()->player.m_bMute);
    ::mp3_set_volumn(GetConfigData()->player.m_byteVolumn);
    
	return true;
}

// 二次初始化（界面已加载完成）
void  CMainMgr::Initialize2()
{
    UI::TimerItem ti;
    ti.lParam = (LPARAM)this;
    ti.pProc = UITimerProc_Progress;
    m_nProgressTimerId = g_pUIApp->SetTimer(330, &ti);

    this->InitVisualization();
    this->HandlePlayingFile();
}

bool  CMainMgr::Release()
{
	SAFE_DELETE_Ixxx(m_pMainWindow);

	bool bRet = ::mp3_release();
	if (false == bRet)
		return false;

	return true;
}

HWND CMainMgr::GetMainWnd()
{
	if (NULL == m_pMainWindow)
	{
		return NULL;
	}

	return m_pMainWindow->GetHWND();
}


void CMainMgr::HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
    if (NULL == m_pMainWindow)
        return;

	if (EVENT_TYPE_UI == nEventType)
	{
        switch (nEventId)
        {
        case UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE:
            m_pMainWindow->OnPlayerListDlgCreated((HWND)wParam);
            break;

        case UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED:
            m_pMainWindow->OnPlayerListDlgVisibleChanged((HWND)wParam, (BOOL)lParam);
            break;

        case UI_EVENT_ID_ON_EQUALIZERDLG_CREATE:
            m_pMainWindow->OnEqualizerDlgCreated((HWND)wParam);
            break;

        case UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED:
            m_pMainWindow->OnEqualizerDlgVisibleChanged((HWND)wParam, (BOOL)lParam);
            break;

        case UI_EVENT_ID_ON_LYRICDLG_CREATE:
            m_pMainWindow->OnLyricDlgCreated((HWND)wParam);
            break;

        case UI_EVENT_ID_ON_LYRICDLG_VISIBLE_CHANGED:
            m_pMainWindow->OnLyricDlgVisibleChanged((HWND)wParam, (BOOL)lParam);
            break;

        case UI_EVENT_ID_ON_WNDTOPMOST_CHANGED:
            m_pMainWindow->OnWndTopMostChanged((BOOL)lParam);
            break;
        }
    }
    else if (EVENT_TYPE_PLAY == nEventType)
    {
        switch (nEventId)
        {
        case PLAY_EVENT_ID_ON_START:
            m_pMainWindow->OnMp3Start((IPlayerListItemInfo*)wParam);
            break;

        case PLAY_EVENT_ID_ON_PAUSE:
            m_pMainWindow->OnMp3Pause();
            break;

        case PLAY_EVENT_ID_ON_STOP:
            m_pMainWindow->OnMp3Stop();
            break;

        case PLAY_EVENT_ID_ON_FINISH:
            m_pMainWindow->OnMp3Finish(wParam?true:false);
            break;

        case PLAY_EVENT_ID_ON_CONTINUE:
            m_pMainWindow->OnMp3Continue();
            break;

        case PLAY_EVENT_ID_REMOVEALL:
            m_pCurPlayingItem = NULL;
            break;

        case PLAY_EVENT_ID_REMOVEITEM:
            {
                if (m_pCurPlayingItem == (PlayerListItemInfo*)wParam)
                {
                    m_pCurPlayingItem = NULL;
                }
            }
            break;

        case PLAY_EVENT_ID_BALANCE_CHANGED:
            m_pMainWindow->OnBalanceChanged((long)wParam, lParam?false:true);
            break;

        case PLAY_EVENT_ID_PROGRESS_IND:
            {
                double* pdSeconds = ((double*)wParam);
                double* pdPecent = ((double*)lParam);

                if (pdSeconds || pdPecent)
                    m_pMainWindow->OnMp3ProgressInd(*pdSeconds, *pdPecent);
            }
            break;
        }
    }
}

//
//	开始播放指定文件
//
void CMainMgr::Play(IPlayerListItemInfo* pItem)
{
	if (NULL == pItem)
		return;

	if (m_pCurPlayingItem)
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
		this->m_pMainWindow->OnVisualiazationTypeChanged(nType);

		GetConfigData()->visual.m_bDirty = true;
		GetConfigData()->visual.m_nType = nType;
		GetFrameWork()->SaveConfig();
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
		GetConfigData()->visual.m_bDirty = true;
		GetConfigData()->visual.m_nFps = nFps;
		GetFrameWork()->SaveConfig();
	}

	return false;
}

bool CMainMgr::InitVisualization()
{
	VisualizationInfo info;
	info.nMask = VI_MASK_HWND|VI_MASK_RECT|VI_MASK_SPECTRUM_BAND_COUNT|VI_MASK_TYPE|VI_MASK_SPECTRUM_BAND_WIDTH|VI_MASK_FPS|VI_MASK_SPECTRUM_GAP_WIDTH;
	info.hWnd = m_pMainWindow->GetHWND();
	info.nSpectrumBandWidth = 1;
	info.nSpectrumGapWidth = 0;

    // 背景图
    info.hBkgndBmp = m_pMainWindow->GetVisualizationInfo(&info.rcRender);
    info.nMask |= VI_MASK_BKGND_BMP;

    if (0 == info.nSpectrumBandWidth+info.nSpectrumGapWidth)
        info.nSpectrumBandCount = info.rcRender.right-info.rcRender.left;
    else
	    info.nSpectrumBandCount = (info.rcRender.right-info.rcRender.left)/(info.nSpectrumBandWidth+info.nSpectrumGapWidth);

	info.eType = (E_VISUALIZATION_TYPE)GetConfigData()->visual.m_nType;
	info.nFps = GetConfigData()->visual.m_nFps;

	// 设置一个前景图
    UI::IRenderBitmap* pRenderBitmap = NULL;
	UI::IImageRes* pImageRes = g_pUIApp->GetActiveSkinImageRes();
	if (pImageRes)
    {
        pImageRes->GetBitmap(_T("visualization_band.bmp"), UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI,(UI::IRenderBitmap**)&pRenderBitmap);
    }
	
    // 将RenderBitmap转成HBITMAP
	if (pRenderBitmap)
	{
		info.hForegndBmp = pRenderBitmap->Detach();
		info.nMask |= VI_MASK_FOREGND_BMP;
		pRenderBitmap->Release();
	}
	::mp3_set_visualization(&info);

	return true;
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

bool CMainMgr::SetMute(bool bMute)
{
	bool bRet = mp3_mute(bMute);
	if (bRet)
	{
		GetConfigData()->SetPlayerMute(bMute);
	}
	return bRet;
}

bool CMainMgr::SetVolumn(long lPercent)
{
	bool bRet = ::mp3_set_volumn(lPercent);
	if (bRet)
	{
		GetConfigData()->player.m_byteVolumn = (byte)lPercent;
		GetConfigData()->player.m_bDirty = true;
	}
	return bRet;
}

// 设置当前播放进行。如nPercent=50,表示播放50%处
bool CMainMgr::SetPlayProgressPercent(int nPercent)
{
	if (nPercent < 0 || nPercent > 100)
		return false;

	return ::mp3_set_cur_pos(nPercent/100.0);
}

// 由playlistmgr调用，保存当前正在播放的文件信息
bool CMainMgr::SavePlayingFileInfo()
{
	if (m_pCurPlayingItem)
	{
		GetConfigData()->player.m_strPlayingFileName = m_pCurPlayingItem->GetFilePath();

		GetConfigData()->player.m_nPlayingTime = 0;  // TODO: 目前仅取得播放比例，没有去获取当前播放精确时刻
		if (m_pMainWindow)
		{
			GetConfigData()->player.m_nPlayingTime = m_pMainWindow->GetCurrentPlayingPercent();
		}
		GetConfigData()->player.m_bDirty = true;
	}
	return true;
}

// 播放上一次的文件
void CMainMgr::HandlePlayingFile()
{
	if (GetConfigData()->player.m_strPlayingFileName.length() > 0)
	{
		if (!PathFileExists(GetConfigData()->player.m_strPlayingFileName.c_str()))
		{
			GetConfigData()->player.m_strPlayingFileName.clear();
			GetConfigData()->player.m_nPlayingTime = 0;
			GetConfigData()->player.m_bDirty = true;
			return;
		}

		IPlayerListItemInfo* pLastPlaingFileInfo = GetPlayerListMgr()->GetItemByPath(GetConfigData()->player.m_strPlayingFileName);
		if (NULL == pLastPlaingFileInfo)
		{
			GetConfigData()->player.m_strPlayingFileName.clear();
			GetConfigData()->player.m_nPlayingTime = 0;
			GetConfigData()->player.m_bDirty = true;
			return;
		}

		Play(pLastPlaingFileInfo);
		if (0 != GetConfigData()->player.m_nPlayingTime)
		{
			this->SetPlayProgressPercent(GetConfigData()->player.m_nPlayingTime);
		}
	}
}

void  CMainMgr::OnSkinChanged(const TCHAR* szNewSkinName)
{
    GetConfigData()->SetSkinActiveName(szNewSkinName);
}

// 主窗口正在退出
void CMainMgr::OnMainWindowDestroy()
{
    if (m_nProgressTimerId)
    {
        g_pUIApp->KillTimer(m_nProgressTimerId);
    }
	this->SavePlayingFileInfo();
}

void CMainMgr::on_mp3_finish()
{
	bool bPlay = false;
	IPlayerListItemInfo* p = GetPlayerListMgr()->GetNextPlayItem(bPlay);

    FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_ON_FINISH, bPlay?1:0);
    m_pCurPlayingItem = NULL;

	if (bPlay)
	{
		this->Play(p);
	} 
}

// 定时获取当前mp3进度
void  UITimerProc_Progress(UINT_PTR nTimerID, UI::TimerItem* pTimerItem)
{
    CMainMgr* pThis = (CMainMgr*)pTimerItem->lParam;
    
    double dSeconds = 0;
    double dPercent = 0;
    if (mp3_get_cur_pos(&dSeconds, &dPercent))
    {
        pThis->FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_PROGRESS_IND, (WPARAM)&dSeconds, (LPARAM)&dPercent);
    }
}
