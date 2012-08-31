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
	bool bRet = ::mp3_init();
	::mp3_add_event_callback(this);

	if (NULL == m_pMainWindow)
	{
		m_pMainWindow = new MainWindow;
		m_pMainWindow->Create( _T("mainwindow") );
	}

	::GetPlayerListMgr(); // 提前初始化获取插入列表

	m_pMainWindow->ShowWindow();
	return bRet;
}

bool CMainMgr::Release()
{
	SAFE_DELETE(m_pMainWindow);
	return ::mp3_release();
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
			if (UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE == nEventId)
			{
				if (NULL != m_pMainWindow)
				{
					m_pMainWindow->SetPlayerListDlgHandle((HWND)wParam);
				}
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
		return;

	if (false == ::mp3_play())
		return;

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
	
}
void CMainMgr::PlayPrev()
{

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
void CMainMgr::on_mp3_progress_ind(LONGLONG dCur, LONGLONG dLen)
{
	if (NULL != m_pMainWindow)
	{
		m_pMainWindow->OnMp3ProgressInd(dCur, dLen);
	}
}
void CMainMgr::on_mp3_volume_ind(long lVolumn)
{
	if (NULL != m_pMainWindow)
	{
		m_pMainWindow->OnMp3VolumeInd(lVolumn);
	}
}