#include "stdafx.h"
#include "mainwnd.h"
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"

#pragma comment(lib, "Winmm.lib")
#include <Mmsystem.h>

CMainWnd::CMainWnd()
{
    m_pLEDCtrl = NULL;
    m_nTimerId = 0;
    m_nStartTickCount = 0;
    m_nTotalMS = 0;
}

CMainWnd::~CMainWnd()
{
}

void  CMainWnd::OnInitialize()
{
    __super::xProcessMessage(GetCurMsg(), 0, 0);

    m_pLEDCtrl = (UI::ILEDExCtrl*)FindChildObject(_T("led"));
//    SetWindowPos(GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

#ifdef _DEBUG
    SetTotalTime(0,30,00); 
#else
    SetTotalTime(GetConfig()->GetHour(), GetConfig()->GetMinute(), GetConfig()->GetSecond());
#endif

	GetUIApplication()->GetActiveSkinRes()->ChangeSkinHLS(GetConfig()->GetHls_h(), 0, GetConfig()->GetHls_s(), CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_S);
	GetMainWnd()->SetWindowTransparentAlphaMask(GetConfig()->GetTransparent());

    CRect rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, (RECT*)&rc, 0);
    CRect rcWnd;
    GetWindowRect(&rcWnd);
    ::SetWindowPos(GetHWND(), 0, rc.right-rcWnd.Width(), rc.bottom-rcWnd .Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
}
void  CMainWnd::OnDestroy()
{
    if (m_nTimerId)
    {
        KillTimer(GetHWND(), m_nTimerId);
        m_nTimerId = 0;
    }
    __super::xProcessMessage(GetCurMsg(), 0, 0);
}
BOOL  CMainWnd::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::xProcessMessage(GetCurMsg(), 0, 0);

    pcs->dwExStyle |= WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
    return TRUE;
}

void  CMainWnd::OnRing()
{
    KillTimer(GetHWND(), m_nTimerId);
    m_nTimerId = 0;

    if (GetConfig()->GetPlayMusic())
    {
        TCHAR szPath[MAX_PATH] = _T("");
        UI::Util::GetAppPath_(szPath);
        String str = szPath;
        str.append(_T("Music\\ring.wav"));
        sndPlaySound(str.c_str(), SND_ASYNC);
    }

    CMaskWnd* pMaskWnd = NULL;
    CMaskWnd::CreateInstance(g_pUIApp, &pMaskWnd);
    pMaskWnd->DoModal(g_pUIApp, _T("mask"), NULL);
    SAFE_DELETE_Ixxx(pMaskWnd);

	if (!::IsWindow(GetHWND()))
		return;

    Reset();
}

void  CMainWnd::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    int nTickCount = GetTickCount();
    int nElapse = nTickCount - m_nStartTickCount;
    int nRemain = (m_nTotalMS - nElapse)/1000;
    if (nRemain < 0)
    {
        nRemain = 0;
        OnRing();
    }

	if (!::IsWindow(GetHWND()))
		return;

    int h = nRemain/3600;
    int temp = nRemain%3600;
    int m = temp/60;
    int s = temp%60; 

    TCHAR szText[16] = _T("");
    _stprintf(szText, _T("%02d:%02d:%02d"), h, m, s);
    m_pLEDCtrl->SetText(szText);
}

void  CMainWnd::SetTotalTime(int nHour, int nMinute, int nSecond)
{
    if (!m_pLEDCtrl)
        return;
 
    if (m_nTimerId)
    {
        KillTimer(GetHWND(), m_nTimerId);;
        m_nTimerId = 0;
    }
    m_nTimerId = SetTimer(GetHWND(), 1, 400, NULL);

    m_nStartTickCount = GetTickCount();
    m_nTotalMS = 1000*(nHour*3600 + nMinute*60 + nSecond);
    OnTimer(m_nTimerId, 0);
}

void  CMainWnd::OnContextMenu(HWND hWnd, POINT point)
{
#define MENU_CMD_RESET  1001
#define MENU_CMD_SET    1002
#define MENU_CMD_QUIT   1009

    UI::IMenu*  pMainMenu = NULL;
    UILoadMenu(GetUIApplication(), _T("mainmenu"), &pMainMenu);
    UINT nRet = pMainMenu->TrackPopupMenu(TPM_RETURNCMD, point.x, point.y, this);
    SAFE_DELETE_Ixxx(pMainMenu);

    if (nRet == MENU_CMD_RESET)
    {
        Reset();
    }
    else if (nRet == MENU_CMD_SET)
    {
        CSettingWnd*  pSettingWnd = NULL;
        CSettingWnd::CreateInstance(g_pUIApp, &pSettingWnd);
        if (IDOK == pSettingWnd->DoModal(g_pUIApp, _T("Setting"), GetHWND()))
        {
            SetTotalTime(GetConfig()->GetHour(), GetConfig()->GetMinute(), GetConfig()->GetSecond());
        }
        SAFE_DELETE_Ixxx(pSettingWnd);
    }
    else if (nRet == MENU_CMD_QUIT)
    {
        DestroyWindow(GetHWND());
        ::PostQuitMessage(0);
    }
}

void  CMainWnd::Reset()
{
    m_nStartTickCount = GetTickCount()+500;
    OnTimer(m_nTimerId, 0);
    if (0 == m_nTimerId)
    {
        m_nTimerId = SetTimer(GetHWND(), 1, 400, NULL);
    }
}
