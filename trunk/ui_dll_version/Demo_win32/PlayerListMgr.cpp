#include "StdAfx.h"
#include "PlayerListMgr.h"


CPlayerListMgr::CPlayerListMgr(void)
{
	m_pPlaylistDlg = NULL;
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
	pInfo->m_strFilePath = strFile;
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