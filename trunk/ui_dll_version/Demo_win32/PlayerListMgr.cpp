#include "StdAfx.h"
#include "PlayerListMgr.h"
#include "MainMgr.h"
#include <stdlib.h>
#include <time.h>

void PlayerListItemInfo::SetFilePath(const String& str) 
{ 
	m_strFilePath = str; 
	
	// ��ȡ�ļ�����
	TCHAR szFileName[MAX_PATH] = _T("");
	Util::GetPathFileName(str.c_str(), szFileName);
	m_strFileName = szFileName;
}

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
	ToggleShowPlayerListDlg(GetMainMgr()->GetMainWnd());

	srand( (unsigned)time( NULL ) );

	m_data.Load();  // �������ļ��ж�ȡ��ʷ��¼

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

// �л������б��ڵ���ʾ��������������Ĳ����б�ť
HWND CPlayerListMgr::ToggleShowPlayerListDlg(HWND hParent)
{
	if( NULL == m_pPlaylistDlg )
	{
		m_pPlaylistDlg = new CPlayListDlg(this);
		m_pPlaylistDlg->Create(_T("playlistdlg"), hParent);

		RECT  rc;
		::GetWindowRect(hParent, &rc);
		::SetWindowPos( m_pPlaylistDlg->m_hWnd, NULL, rc.left, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );

		this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE, (WPARAM)m_pPlaylistDlg->m_hWnd);

		// ��������
		int nSize = (int)m_vecPlayerList.size();
		for (int i = 0; i < nSize; i++)
		{
			m_pPlaylistDlg->OnAddItem(m_vecPlayerList[i]);
		}
	}

	BOOL bVisible = m_pPlaylistDlg->IsVisible();
	if (bVisible)
		UIAnimateWindow(m_pPlaylistDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	else
		::ShowWindow(m_pPlaylistDlg->m_hWnd,SW_SHOW);

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED, (WPARAM)m_pPlaylistDlg->m_hWnd, (LPARAM)!bVisible);

	return m_pPlaylistDlg->m_hWnd;
}

// �ر��˲����б���
void CPlayerListMgr::OnPlayListDlgHide()
{
	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED, (WPARAM)m_pPlaylistDlg->m_hWnd, (LPARAM)FALSE);
}

//
// ͨ��OnAddItem�ص�֪ͨ���ڽ��
//
void CPlayerListMgr::AddFile(const String& strFile)
{
	if (m_data.Add(strFile) )
	{
		this->OnLoadItem(strFile);
	}
}

bool CPlayerListMgr::RemoveAllFile()
{
	GetMainMgr()->Stop();
	if (m_data.RemoveAll())
	{
		int nSize = (int)m_vecPlayerList.size();
		for (int i = 0; i < nSize; i++)
		{
			PlayerListItemInfo* pInfo = m_vecPlayerList[i];
			SAFE_DELETE(pInfo);
		}
		m_vecPlayerList.clear();
	}
	return true;
}

//
// ����������ļ�ʱ�����ã�Ҳ����������ʱ��ȡ�����ļ�����
//
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
//	�ڵ�ǰ�����������ʱ���Զ���ȡ��һ��mp3
//
//  Parameter
//		bPlay
//			[out] �����Ƿ���Ҫ������һ��mp3
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
			PlayerListItemInfo* p = GetItem(nResult);
			m_lstPlayHistory.push_back(p);
			return p;
		}
		break;

	default:
		return NULL;
	}

	return NULL;
}

//
// ��ȡ��һ�ײ���MP3
//
PlayerListItemInfo* CPlayerListMgr::GetPrevItem()
{
	switch (m_ePlayMode)
	{
	case ALL_RAND:
		{
			if (NULL != m_pCurPlayingItem && m_lstPlayHistory.size()>0 && m_lstPlayHistory.back() == m_pCurPlayingItem)
				m_lstPlayHistory.pop_back();

			PlayerListItemInfo* p = NULL;
			if (m_lstPlayHistory.size() > 0)
				p = m_lstPlayHistory.back();
			
			if (NULL == p)
			{
				p = GetItem(0);
			}
			else
			{
				m_lstPlayHistory.pop_back();
			}
			return p;
		}
		break;

	default:
		{
			if (NULL == m_pCurPlayingItem)
			{
				return GetItem(0);
			}
			else
			{
				int nIndex = GetItemIndex(m_pCurPlayingItem);
				if (-1 == nIndex)
					return NULL;

				PlayerListItemInfo* p = GetItem(nIndex-1);
				if (NULL == p)
				{
					p = GetItem(GetItemCount()-1);
				}
				return p;
			}
		}
		break;
	}
	return NULL;
}

// ������һ��MP3
PlayerListItemInfo*  CPlayerListMgr::GetNextItem()
{
	switch (m_ePlayMode)
	{
	case ALL_RAND:
		{
			bool bPlay = false;
			return GetNextPlayItem(bPlay);
		}
		break;

	default:
		{
			if (NULL == m_pCurPlayingItem)
			{
				return GetItem(0);
			}
			else
			{
				int nIndex = GetItemIndex(m_pCurPlayingItem);
				if (-1 == nIndex)
					return NULL;

				PlayerListItemInfo* p = GetItem(nIndex+1);
				if (NULL == p)
				{
					p = GetItem(0);
				}
				return p;
			}
		}
		break;
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
				m_pPlaylistDlg->OnMp3Start(m_pCurPlayingItem);
			}
		}
		break;
	}
}