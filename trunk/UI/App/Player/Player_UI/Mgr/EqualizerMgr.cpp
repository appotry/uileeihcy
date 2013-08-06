#include "StdAfx.h"
#include "EqualizerMgr.h"
#include "MainMgr.h"


CEqualizerMgr::CEqualizerMgr(void)
{
	m_pEqualizerDlg = NULL;
}

CEqualizerMgr::~CEqualizerMgr(void)
{
    if (m_pEqualizerDlg)
    {
	    SAFE_DELETE_Ixxx(m_pEqualizerDlg);
    }
}

bool CEqualizerMgr::Initialize()
{
	HWND hWndParent = GetMainMgr()->GetMainWnd();
    CEqualizerDlg::CreateInstance(g_pUIApp, &m_pEqualizerDlg);
	m_pEqualizerDlg->Create(g_pUIApp, _T("equalizerdlg"), hWndParent);

	RECT  rc;
	::GetWindowRect(hWndParent, &rc);
	::SetWindowPos( m_pEqualizerDlg->GetHWND(), NULL, rc.right, rc.bottom, 0,0/*rc.right-rc.left, rc.bottom-rc.top*/, SWP_NOZORDER|SWP_NOSIZE );

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_CREATE, (WPARAM)m_pEqualizerDlg->GetHWND());

	::mp3_set_pan(GetConfigData()->player.m_byteBalance);

	return true;
}
bool CEqualizerMgr::Release()
{
	if (m_pEqualizerDlg && ::IsWindow(m_pEqualizerDlg->GetHWND()))
	{
		::DestroyWindow(m_pEqualizerDlg->GetHWND());
	}
	return true;
}

void CEqualizerMgr::ToggleShowEqualizerDlg()
{
	if (NULL == m_pEqualizerDlg)
		return;

	BOOL bVisible = m_pEqualizerDlg->IsVisible();
	if (bVisible)
	{
		::ShowWindow(m_pEqualizerDlg->GetHWND(), SW_HIDE);
		//UIAnimateWindow(m_pEqualizerDlg->m_hWnd,UIAW_HIDE_KUOSAN,0);
	}
	else
    {
		::ShowWindow(m_pEqualizerDlg->GetHWND(),SW_SHOWNOACTIVATE);
    }

	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED, (WPARAM)m_pEqualizerDlg->GetHWND(), (LPARAM)!bVisible);

}
void CEqualizerMgr::OnEqualizerDlgHide()
{
	this->FireEvent(EVENT_TYPE_UI, UI_EVENT_ID_ON_EQUALIZERDLG_VISIBLE_CHANGED, (WPARAM)m_pEqualizerDlg->GetHWND(), (LPARAM)FALSE);
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
	bool bRet = false;
	if (!bFinish)
	{
		bRet = ::mp3_set_pan(lPercent);
	}
	
	if (bRet)
	{
		GetConfigData()->player.m_byteBalance = (char)lPercent;
		GetConfigData()->player.m_bDirty = true;

		this->FireEvent(EVENT_TYPE_PLAY, PLAY_EVENT_ID_BALANCE_CHANGED, (WPARAM)lPercent, (LPARAM)bFinish);
	}
	return bRet;
}
bool CEqualizerMgr::SetEq(E_EQ_FREQ e, int nValue)
{
	return ::mp3_set_eq(e, nValue);
}
bool CEqualizerMgr::EnableEq(bool bEnable)
{
	return ::mp3_enable_eq(bEnable);
}
bool CEqualizerMgr::ReSetEq()
{
	return ::mp3_reset_eq();
}