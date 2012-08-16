#include "StdAfx.h"
#include "PlayerListMgr.h"

CPlayerListMgr::CPlayerListMgr(void)
{
	m_pPlaylistDlg = NULL;
}

CPlayerListMgr::~CPlayerListMgr(void)
{
	SAFE_DELETE(m_pPlaylistDlg);
}

bool CPlayerListMgr::Initialize()
{
	m_data.Load();
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
	}

	if(m_pPlaylistDlg->IsVisible())
		UIAnimateWindow(m_pPlaylistDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	else
		::ShowWindow(m_pPlaylistDlg->m_hWnd,SW_SHOW);

	return m_pPlaylistDlg->m_hWnd;
}

bool CPlayerListMgr::AddFile(const String& strFile)
{
	if (m_data.Add(strFile) && NULL != m_pPlaylistDlg)
	{
		m_pPlaylistDlg->OnAddFile(strFile);
	}
	return true;
}
bool CPlayerListMgr::AddDirectory(const String& strDir)
{
	return true;
}