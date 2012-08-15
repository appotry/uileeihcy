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

	m_MgrPlayerList.Initialize();

	m_pMainWindow->ShowWindow();
	return bRet;
}

bool CMainMgr::Release()
{
	m_MgrPlayerList.Release();

	SAFE_DELETE(m_pMainWindow);
	return ::mp3_release();
}