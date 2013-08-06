#include "StdAfx.h"
#include "PlayerListMgr.h"
#include "MainMgr.h"

void PlayerListItemInfo::SetFilePath(const String& str) 
{ 
	m_strFilePath = str; 
	
	// 读取文件属性
	TCHAR szFileName[MAX_PATH] = _T("");
    UI::Util::GetPathFileName(str.c_str(), szFileName);
	m_strFileName = szFileName;

	// TODO: 暂时先把标签解析放在这里进行
	//     : 优化为当其在列表中可见（并且没有在滚动）时进行解析

	bool bRet = mp3_get_tag(str.c_str(), &m_tagAttr);
}

// 获取播放列表控件中显示的文件
const TCHAR* PlayerListItemInfo::GetDisplayText()
{
	if (m_strDisplayText.empty())
	{
		const TCHAR* pTitle = m_tagAttr.GetTitle();
		if (NULL == pTitle || 0 == _tcslen(pTitle))
			return m_strFileName.c_str();

		m_strDisplayText.assign(pTitle);
		m_strDisplayText.append(_T(" - "));

		const TCHAR* pArtist = m_tagAttr.GetArtist();
		if (pArtist && 0 != _tcslen(pArtist))
		{
			m_strDisplayText.append(pArtist);	
		}
	}

	return m_strDisplayText.c_str();
}

//////////////////////////////////////////////////////////////////////////

MusicFileTagAttribute::MusicFileTagAttribute()
{
	m_nGenre = m_nTrack = 0;
    m_nBitrate = 0;
    m_nSamplingRate = 0;
    m_nDuration = 0;
}
HRESULT  MusicFileTagAttribute::OnAddAttribute(WCHAR* pName, byte* pValue, int nValueSize, int nDataType)
{
	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetTitle(const WCHAR* szText)
{
	if (NULL == szText)
	{
		m_strTitle.clear();
	}
	else
	{
		m_strTitle = szText;
	}

	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetArtist(const WCHAR* szText)
{
	if (NULL == szText)
	{
		m_strArtist.clear();
	}
	else
	{
		m_strArtist = szText;
	}
	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetAlbum(const WCHAR* szText)
{
	if (NULL == szText)
	{
		m_strAlbum.clear();
	}
	else
	{
		m_strAlbum = szText;
	}
	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetYear(const WCHAR* szText)
{	
	if (NULL == szText)
	{
		m_strYear.clear();
	}
	else
	{
		m_strYear = szText;
	}
	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetComment(const WCHAR* szText)
{
	if (NULL == szText)
	{
		m_strComment.clear();
	}
	else
	{
		m_strComment = szText;
	}
	return S_OK;
}

HRESULT  MusicFileTagAttribute::SetGenreString(const WCHAR* szText)
{
	m_nGenre = -1;
	
	if (NULL == szText)
		m_strGenre.clear();
	else
		m_strGenre.assign(szText);

	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetGenre(int b)
{
	m_nGenre = b;

	const wchar_t* p = mp3_parse_genre(b);
	if (NULL == p)
		m_strGenre.clear();
	else
		m_strGenre.assign(p);

	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetTrack(int b)
{
	m_nTrack = b;
	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetDuration(int nDuration)
{
    m_nDuration = nDuration;
    return S_OK;
}
HRESULT  MusicFileTagAttribute::SetFormat(int nMpegVersion, int nLayerType, int nChannelMode)
{
	m_eMpegVersion = (mp3_mpeg_audio_version)nMpegVersion;
	m_eLayerType = (mp3_layer_type)nLayerType;
	m_eChannelMode = (mp3_channel_mode)nChannelMode;

	return S_OK;
}
HRESULT  MusicFileTagAttribute::SetBitrate(int nBitrate)
{
    m_nBitrate = nBitrate;
    return S_OK;
}
HRESULT  MusicFileTagAttribute::SetSapmleRate(int nSample)
{
    m_nSamplingRate = nSample;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////

CPlayerListMgr::CPlayerListMgr(void)
{
	m_pPlaylistDlg = NULL;
	m_pCurPlayingItem = NULL;
	m_ePlayMode = ALL_RAND;
}

CPlayerListMgr::~CPlayerListMgr(void)
{
    if (m_pPlaylistDlg)
    {
	    SAFE_DELETE_Ixxx(m_pPlaylistDlg);
    }

    m_PlayerList.DELETE_ALL();
}

bool CPlayerListMgr::Initialize()
{
	srand( (unsigned)time( NULL ) );

    if (NULL == m_pPlaylistDlg)
    {
        CPlayListDlg::CreateInstance(g_pUIApp, &m_pPlaylistDlg);
        m_pPlaylistDlg->SetPlayListMgr(this);
        m_pPlaylistDlg->Create(g_pUIApp, _T("playlistdlg"), GetMainMgr()->GetMainWnd());

        RECT  rc;
        ::GetWindowRect(GetMainMgr()->GetMainWnd(), &rc);
        ::SetWindowPos( m_pPlaylistDlg->GetHWND(), NULL, rc.left, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );

        this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_CREATE, (WPARAM)m_pPlaylistDlg->GetHWND());
    }

    m_data.Load();  // 从配置文件中读取历史记录
    m_ePlayMode = (PLAY_MODE) GetConfigData()->player.m_bytePlayMode;

	return true;
}

bool CPlayerListMgr::Release()
{
	if (m_pPlaylistDlg && ::IsWindow(m_pPlaylistDlg->GetHWND()))
	{
		::DestroyWindow(m_pPlaylistDlg->GetHWND());
	}
	return true;
}

// 切换播放列表窗口的显示：如点击了主界面的播放列表按钮
HWND CPlayerListMgr::ToggleShowPlayerListDlg()
{
	BOOL bVisible = m_pPlaylistDlg->IsVisible();
	if (bVisible)
	{
		::ShowWindow(m_pPlaylistDlg->GetHWND(), SW_HIDE);
		//UIAnimateWindow(m_pPlaylistDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	}
	else
    {
		::ShowWindow(m_pPlaylistDlg->GetHWND(),SW_SHOWNOACTIVATE);
    }

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED, (WPARAM)m_pPlaylistDlg->GetHWND(), (LPARAM)!bVisible);

	return m_pPlaylistDlg->GetHWND();
}

// 关闭了播放列表窗口
void CPlayerListMgr::OnPlayListDlgHide()
{
	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_PLAYERLISTDLG_VISIBLE_CHANGED, (WPARAM)m_pPlaylistDlg->GetHWND(), (LPARAM)FALSE);
}


//
// 通过OnAddItem回调通知窗口结果
//
PlayerListItemInfo* CPlayerListMgr::AddFile(const String& strFile)
{
	if (m_data.Add(strFile) )
	{
		return this->OnLoadItem(strFile, NULL);
	}
	return NULL;
}

bool CPlayerListMgr::RemoveAllItem()
{
	GetMainMgr()->Stop();

	this->FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_REMOVEALL);

	if (m_data.RemoveAll())
	{
		m_PlayerList.DELETE_ALL();
		return true;
	}
	else
	{
		UIASSERT(0);
		return false;
	}
	return false;
}

bool CPlayerListMgr::RemovePlayListItem(IPlayerListItemInfo* pInfo)
{
	if (m_pCurPlayingItem == pInfo)
	{
		GetMainMgr()->Stop();
		m_pCurPlayingItem = NULL;
	}

	this->FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_REMOVEITEM, (WPARAM)pInfo);

	if (m_data.Remove(pInfo->GetFilePath()) && NULL != m_pPlaylistDlg)
	{
		m_pPlaylistDlg->OnRemoveItem(pInfo);

        m_PlayerList.Remove((PlayerListItemInfo*)pInfo);
        SAFE_DELETE(pInfo);
		return true;
	}
	return false;
}

int  CPlayerListMgr::RemoveNotExistFile()
{
    int nCount = 0;
    _MyPlayListItem*  pItem = m_PlayerList.m_pFirst;
    _MyPlayListItem*  pNext = NULL;
    while (pItem)
    {
        pNext = pItem->m_pNext;
        PlayerListItemInfo*  p = pItem->m_pData;
        if (PathFileExists(p->GetFilePath()))
        {
            pItem = pNext;
            continue;
        }

        RemovePlayListItem(p);
        nCount ++;
        pItem = pNext;
    }

    return nCount;
}

//
// 可能是添加文件时被调用，也可能是启动时读取配置文件调用
//
PlayerListItemInfo* CPlayerListMgr::OnLoadItem(const String& strFile, UI::IMapAttribute* pMapAttrib)
{
    // 注意：pMapAttrib可能为NULL
	PlayerListItemInfo* pInfo = new PlayerListItemInfo;
	pInfo->SetFilePath(strFile);
    m_PlayerList.Add(pInfo);

	if (m_pPlaylistDlg)
	{
		m_pPlaylistDlg->OnAddItem(pInfo, true);
	}

	return pInfo;
}

bool CALLBACK MyEnumFileInDirProc(const TCHAR* szDir, const TCHAR* szFileName, WPARAM wParam)
{
	CPlayerListMgr* pThis = (CPlayerListMgr*)wParam;
	if (NULL == pThis)
		return false;

	String str = szDir;
	str += szFileName;

	String strExt = str.substr(str.length()-4,4);
	if ( 0 == _tcsicmp(strExt.c_str(), _T(".mp3")) || 
		 0 == _tcsicmp(strExt.c_str(), _T(".wma")) || 
		 0 == _tcsicmp(strExt.c_str(), _T(".wav")) )
	{
		pThis->AddFile(str);
	}
	return true;
}

bool CPlayerListMgr::AddDirectory(const String& strDir)
{
    UI::Util::EnumFileInDirectory(strDir.c_str(), MyEnumFileInDirProc, (WPARAM)this);
	return true;
}


IPlayerListItemInfo* CPlayerListMgr::GetItemByPath(const String& strFilePath)
{
    _MyPlayListItem*  pItem = m_PlayerList.m_pFirst;
    _MyPlayListItem*  pNext = NULL;
    while (pItem)
    {
        pNext = pItem->m_pNext;
        if (pItem->m_pData->GetFilePath() == strFilePath)
            return pItem->m_pData;

        pItem = pNext;
    }
    return NULL;
}
#if 0
IPlayerListItemInfo* CPlayerListMgr::GetItem(int i)
{
    UIASSERT(0);
    return NULL;
// 	if (i < 0 || i >= (int)m_vecPlayerList.size())
// 	{
// 		return NULL;
// 	}
// 
// 	return m_vecPlayerList[i];
}


int CPlayerListMgr::GetItemIndex(IPlayerListItemInfo* pItem)
{
    UIASSERT(0);
// 	if (NULL == pItem)
// 		return -1;
// 
// 	int nSize = (int)m_vecPlayerList.size();
// 	for (int i = 0; i < nSize; i++)
// 	{
// 		if (pItem == m_vecPlayerList[i])
// 			return i;
// 	}
// 	return -1;
}

#endif

//
//	在当前歌曲插入完毕时，自动获取下一首mp3
//
//  Parameter
//		bPlay
//			[out] 返回是否需要播放下一首mp3
//	
IPlayerListItemInfo* CPlayerListMgr::GetNextPlayItem(bool& bPlay)
{
	switch (m_ePlayMode)
	{
	case SINGLE_ONCE:
	case SINGLE_LOOP:
		{
			if (NULL == m_pCurPlayingItem)
			{
				IPlayerListItemInfo* p = GetFirstItem();
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
				IPlayerListItemInfo* p = GetFirstItem();
				if (NULL == p)
					bPlay = false;
				else
					bPlay = true;
				return p;
			}
			else
			{
                _MyPlayListItem* pListItem = m_PlayerList.Find((PlayerListItemInfo*)m_pCurPlayingItem);
				if (NULL == pListItem)
				{
					bPlay = false;
					return NULL;
				}

				// TODO : LOOP
                IPlayerListItemInfo* p = NULL;
                if (pListItem->m_pNext)
                    p = pListItem->m_pNext->m_pData;

				if (ALL_LOOP == m_ePlayMode)
				{
					if (NULL == p)
					{
						p = this->GetFirstItem();
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
			int nCount = (int)m_PlayerList.GetCount();
			if (0 == nCount)
			{
				bPlay = false;
				return NULL;
			}

			int nResult = rand()%nCount;
			bPlay = true;
			IPlayerListItemInfo* p = m_PlayerList.At(nResult);
			if (p == m_pCurPlayingItem && nCount>1)
			{
				return this->GetNextPlayItem(bPlay);  // 又随机到了自己，重来。
			}
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
// 获取上一首播放MP3
//
IPlayerListItemInfo* CPlayerListMgr::GetPrevItem()
{
	switch (m_ePlayMode)
	{
	case ALL_RAND:
		{
			if (m_pCurPlayingItem && m_lstPlayHistory.size()>0 && m_lstPlayHistory.back() == m_pCurPlayingItem)
				m_lstPlayHistory.pop_back();

			IPlayerListItemInfo* p = NULL;
			if (m_lstPlayHistory.size() > 0)
				p = m_lstPlayHistory.back();
			
			if (NULL == p)
			{
				/*p = GetItem(0);*/
				bool bPlay = false;
				p = this->GetNextPlayItem(bPlay);  // 利用GetNextPlayItem再随机获取
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
				return GetFirstItem();
			}
			else
			{
                _MyPlayListItem*  pItem = m_PlayerList.Find((PlayerListItemInfo*)m_pCurPlayingItem);

                IPlayerListItemInfo* p = NULL;
                if (pItem && pItem->m_pPrev)
                    p = pItem->m_pPrev->m_pData;

				if (NULL == p)
				{
                    if (m_PlayerList.m_pLast)
					    p = m_PlayerList.m_pLast->m_pData;
				}
				return p;
			}
		}
		break;
	}
	return NULL;
}

// 播放下一首MP3
IPlayerListItemInfo*  CPlayerListMgr::GetNextItem()
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
				return GetFirstItem();
			}
			else
			{
                _MyPlayListItem* pItem = m_PlayerList.Find((PlayerListItemInfo*)m_pCurPlayingItem);

				IPlayerListItemInfo* p = NULL;
                if (pItem && pItem->m_pNext)
                    p = pItem->m_pNext->m_pData;

				if (NULL == p)
					p = GetFirstItem();

                return p;
			}
		}
		break;
	}
	return NULL;
}

IPlayerListItemInfo*  CPlayerListMgr::GetFirstItem()
{
    if (NULL == m_PlayerList.m_pFirst)
        return NULL;

    return m_PlayerList.m_pFirst->m_pData;
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
				m_pCurPlayingItem = (IPlayerListItemInfo*)wParam;
				m_pPlaylistDlg->OnMp3Start(m_pCurPlayingItem);
			}
		}
		break;
	}
}

void CPlayerListMgr::SetPlayMode(PLAY_MODE e) 
{ 
	m_ePlayMode = e; 

	GetConfigData()->player.m_bytePlayMode = m_ePlayMode;
	GetConfigData()->player.m_bDirty = true;
}