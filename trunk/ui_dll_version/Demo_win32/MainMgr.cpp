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
			if (PLAY_EVENT_ID_ON_START == nEventId && NULL != m_pMainWindow)
			{
				m_pMainWindow->OnMP3Start((PlayerListItemInfo*)wParam);
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
	if (false == ::mp3_set_file(pItem->m_strFilePath))
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