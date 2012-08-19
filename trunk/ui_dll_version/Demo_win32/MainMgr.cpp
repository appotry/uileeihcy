#include "stdafx.h"
#include "MainMgr.h"
#include "MainWindow.h"


CMainMgr::CMainMgr()
{
	m_pMainWindow = NULL;
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


void CMainMgr::DoEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
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
	}
}