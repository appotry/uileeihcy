#include "StdAfx.h"
#include "EqualizerMgr.h"
#include "MainMgr.h"


CEqualizerMgr::CEqualizerMgr(void)
{
	m_pEqualizerDlg = NULL;
}

CEqualizerMgr::~CEqualizerMgr(void)
{
	SAFE_DELETE(m_pEqualizerDlg);
}

bool CEqualizerMgr::Initialize()
{
	HWND hWndParent = GetMainMgr()->GetMainWnd();
	m_pEqualizerDlg = new CEqualizerDlg();
	m_pEqualizerDlg->Create(_T("equalizerdlg"), hWndParent);

	RECT  rc;
	::GetWindowRect(hWndParent, &rc);
	::SetWindowPos( m_pEqualizerDlg->m_hWnd, NULL, rc.right, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_CREATE, (WPARAM)m_pEqualizerDlg->m_hWnd);

	::mp3_set_pan(GetMainMgr()->GetConfigData()->player.m_byteBalance);

	return true;
}
bool CEqualizerMgr::Release()
{
	if (NULL != m_pEqualizerDlg && ::IsWindow(m_pEqualizerDlg->m_hWnd))
	{
		::DestroyWindow(m_pEqualizerDlg->m_hWnd);
	}
	return true;
}

void CEqualizerMgr::ToggleShowEqualizerDlg()
{
	if (NULL == m_pEqualizerDlg)
		return;

	BOOL bVisible = m_pEqualizerDlg->IsVisible();
	if (bVisible)
		UIAnimateWindow(m_pEqualizerDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	else
		::ShowWindow(m_pEqualizerDlg->m_hWnd,SW_SHOW);

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED, (WPARAM)m_pEqualizerDlg->m_hWnd, (LPARAM)!bVisible);

}
void CEqualizerMgr::OnEqualizerDlgHide()
{
	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED, (WPARAM)m_pEqualizerDlg->m_hWnd, (LPARAM)FALSE);
}

void CEqualizerMgr::HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam)
{
	switch (nEventType)
	{
	case EVENT_TYPE_UI:
		{
		}
		break;
	case EVENT_TYPE_PLAY:
		{
		}
		break;
	}
}

bool CEqualizerMgr::SetBalance(long lPercent, bool bFinish)
{
	bool bRet = ::mp3_set_pan(lPercent);
	
	if (bRet)
	{
		GetMainMgr()->GetConfigData()->player.m_byteBalance = (char)lPercent;
		GetMainMgr()->GetConfigData()->player.m_bDirty = true;

		this->FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_BALANCE_CHANGED, (WPARAM)lPercent, (LPARAM)bFinish);
	}
	return bRet;
}