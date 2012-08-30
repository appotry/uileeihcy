#include "StdAfx.h"
#include "PlayerListMgr.h"
#include "MainMgr.h"
#include <stdlib.h>
#include <time.h>

CPlayerListMgr::CPlayerListMgr(void)
{
	m_pPlaylistDlg = NULL;
	m_pCurPlayingItem = NULL;
	m_ePlayMode = ALL_RAND;
}

CPlayerListMgr::~CPlayerListMgr(void)
{
	SAFE_DELETE(m_pPlaylistDlg);

	int nSize = (int)m_vecPlayerList.size();
	for (int i = 0; i < nSize; i++)
	{
		PlayerListItemInfo* pInfo = m_vecPlayerList[i];
		SAFE_DELETE(pInfo);
	}
	m_vecPlayerList.clear();
}

bool CPlayerListMgr::Initialize()
{
	ShowPlayerListDlg(GetMainMgr()->GetMainWnd());

	srand( (unsigned)time( NULL ) );

	m_data.Load();  // 从配置文件中读取历史记录

	return true;
}
bool CPlayerListMgr::Release()
{
	if (NULL != m_pPlaylistDlg && ::IsWindow(m_pPlaylistDlg->m_hWnd))
	{
		::DestroyWindow(m_pPlaylistDlg->m_hWnd);
	}
	return true;
}


HWND CPlayerListMgr::ShowPlayerListDlg(HWND hParent)
{
	if( NULL == m_pPlaylistDlg )
	{
		m_pPlaylistDlg = new CPlayListDlg(this);
		m_pPlaylistDlg->Create(_T("playlistdlg"), hParent);

		RECT  rc;
		::GetWindowRect(hParent, &rc);
		::SetWindowPos( m_pPlaylistDlg->m_hWnd, NULL, rc.left, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );

		this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE, (WPARAM)m_pPlaylistDlg->m_hWnd);

		// 加载数据
		int nSize = (int)m_vecPlayerList.size();
		for (int i = 0; i < nSize; i++)
		{
			m_pPlaylistDlg->OnAddItem(m_vecPlayerList[i]);
		}
	}

	if(m_pPlaylistDlg->IsVisible())
		UIAnimateWindow(m_pPlaylistDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	else
		::ShowWindow(m_pPlaylistDlg->m_hWnd,SW_SHOW);

	return m_pPlaylistDlg->m_hWnd;
}

bool CPlayerListMgr::AddFile(const String& strFile)
{
	if (m_data.Add(strFile) )
	{
		this->OnLoadItem(strFile);
	}
	return true;
}


void CPlayerListMgr::OnLoadItem(const String& strFile)
{

	PlayerListItemInfo* pInfo = new PlayerListItemInfo;
	pInfo->SetFilePath(strFile);
	m_vecPlayerList.push_back(pInfo);

	if ( NULL != m_pPlaylistDlg)
	{
		m_pPlaylistDlg->OnAddItem(pInfo);
	}
}

bool CALLBACK MyEnumFileInDirProc(const TCHAR* szDir, const TCHAR* szFileName, WPARAM wParam)
{
	CPlayerListMgr* pThis = (CPlayerListMgr*)wParam;
	if (NULL == pThis)
		return false;

	String str = szDir;
	str += szFileName;
	if( str.substr(str.length()-4,4) == _T(".mp3") )
	{
		pThis->AddFile(str);
	}
	return true;
}

bool CPlayerListMgr::AddDirectory(const String& strDir)
{
	Util::EnumFileInDirectory(strDir.c_str(), MyEnumFileInDirProc, (WPARAM)this);
	return true;
}

PlayerListItemInfo* CPlayerListMgr::GetItem(int i)
{
	if (i < 0 || i >= (int)m_vecPlayerList.size())
	{
		return NULL;
	}

	return m_vecPlayerList[i];
}
int CPlayerListMgr::GetItemIndex(PlayerListItemInfo* pItem)
{
	if (NULL == pItem)
		return -1;

	int nSize = (int)m_vecPlayerList.size();
	for (int i = 0; i < nSize; i++)
	{
		if (pItem == m_vecPlayerList[i])
			return i;
	}
	return -1;
}

//
//	在当前歌曲插入完毕时，自动获取下一首mp3
//
//  Parameter
//		bPlay
//			[out] 返回是否需要播放下一首mp3
//	
PlayerListItemInfo* CPlayerListMgr::GetNextPlayItem(bool& bPlay)
{
	switch (m_ePlayMode)
	{
	case SINGLE_ONCE:
	case SINGLE_LOOP:
		{
			if (NULL == m_pCurPlayingItem)
			{
				PlayerListItemInfo* p = GetItem(0);
				if (NULL == p)
					bPlay = false;
				else
					bPlay = true;
				return p;
			}
			else
			{
				bPlay = m_ePlayMode == SINGLE_LOOP;
				return m_pCurPlayingItem;
			}
		}
		break;

	case ALL_ONCE:
	case ALL_LOOP:
		{
			if (NULL == m_pCurPlayingItem)
			{
				PlayerListItemInfo* p = GetItem(0);
				if (NULL == p)
					bPlay = false;
				else
					bPlay = true;
				return p;
			}
			else
			{
				int nIndex = GetItemIndex(m_pCurPlayingItem);
				if (-1 == nIndex)
				{
					bPlay = false;
					return NULL;
				}

				// TODO : LOOP
				PlayerListItemInfo* p = this->GetItem(nIndex+1);
				if (ALL_LOOP == m_ePlayMode)
				{
					if (NULL == p)
					{
						p = this->GetItem(0);
						if (NULL == p)
							bPlay = false;
						else
							bPlay = true;
					}
					else
					{
						bPlay = true;
					}
				}
				else
				{
					if (NULL == p)
						bPlay = false;
					else
						bPlay = true;
				}
				return p;
			}
		}
		break;


	case ALL_RAND:
		{
			int nCount = (int)m_vecPlayerList.size();
			if (0 == nCount)
			{
				bPlay = false;
				return NULL;
			}

			int nResult = rand()%nCount;
			bPlay = true;
			return GetItem(nResult);
		}
		break;

	default:
		return NULL;
	}

	return NULL;
}


void CPlayerListMgr::HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
	switch (nEventType)
	{
	case EVENT_TYPE_UI:
		{
		}
		break;
	case EVENT_TYPE_PLAY:
		{
			if (PLAY_EVENT_ID_ON_START == nEventId )
			{
				m_pCurPlayingItem = (PlayerListItemInfo*)wParam;
			}
		}
		break;
	}
}