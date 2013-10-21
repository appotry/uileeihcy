#include "stdafx.h"
#include "settingwnd.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "mainwnd.h"

CSettingWnd::CSettingWnd()
{
	m_pBtnOk = NULL;
	m_pBtnCancel = NULL;
	m_pMainWnd = NULL;
	m_pEditHour = NULL;
	m_pEditMinute = NULL;
	m_pEditSecond = NULL;
	m_pCheckBtnMusic = NULL;
	m_pEditBreakTime = NULL;
    m_pBtnBrownerPicFolder = NULL;
    m_pEditPicFolder = NULL;
	m_pCheckBtnAutorun = NULL;
	m_pSliderTransparent = NULL;
	m_pSliderH = NULL;
	m_pSliderS = NULL;
}

CSettingWnd::~CSettingWnd()
{

}

void  CSettingWnd::OnInitialize()
{
	m_pBtnOk = (UI::IButton*)FindChildObject(_T("idok"));
	m_pBtnCancel = (UI::IButton*)FindChildObject(_T("idcancel"));
	m_pEditHour = (UI::IEdit*)FindChildObject(_T("edit_hour"));
	m_pEditMinute = (UI::IEdit*)FindChildObject(_T("edit_minute"));
	m_pEditSecond = (UI::IEdit*)FindChildObject(_T("edit_second"));
	m_pCheckBtnMusic = (UI::ICheckButton*)FindChildObject(_T("checkbtn_music"));
	m_pEditBreakTime = (UI::IEdit*)FindChildObject(_T("edit_breaktime"));
    m_pBtnBrownerPicFolder = (UI::IButton*)FindChildObject(_T("btn_browerpicfolder"));
    m_pEditPicFolder = (UI::IEdit*)FindChildObject(_T("edit_picfolder"));
	m_pCheckBtnAutorun = (UI::ICheckButton*)FindChildObject(_T("checkbtn_autorun"));
	m_pSliderTransparent = (UI::ISliderCtrl*)FindChildObject(_T("slider_transparent"));
	m_pSliderH = (UI::ISliderCtrl*)FindChildObject(_T("slider_h"));
	m_pSliderS = (UI::ISliderCtrl*)FindChildObject(_T("slider_s"));

	if (m_pCheckBtnMusic && GetConfig()->GetPlayMusic())
	{
		m_pCheckBtnMusic->SetChecked();
	}
	if (m_pSliderTransparent)
	{
		m_pSliderTransparent->SetRange(30, 255);
		m_pSliderTransparent->SetPos(GetConfig()->GetTransparent());
	}
	if (m_pSliderH)
	{
		m_pSliderH->SetRange(0, 240);
		m_pSliderH->SetPos(GetConfig()->GetHls_h());
	}
	if (m_pSliderS)
	{
		m_pSliderS->SetRange(0, 99);
		m_pSliderS->SetPos(GetConfig()->GetHls_s());
	}

	TCHAR  szText[128] = _T("");
	if (m_pEditHour)
	{
		_stprintf(szText, _T("%d"), GetConfig()->GetHour());
		m_pEditHour->SetText(szText);
	}
	if (m_pEditMinute)
	{
		_stprintf(szText, _T("%d"), GetConfig()->GetMinute());
		m_pEditMinute->SetText(szText);
	}
	if (m_pEditSecond)
	{
		_stprintf(szText, _T("%d"), GetConfig()->GetSecond());
		m_pEditSecond->SetText(szText);
	}
	if (m_pEditBreakTime)
	{
		_stprintf(szText, _T("%d"), GetConfig()->GetBreakTime());
		m_pEditBreakTime->SetText(szText);
	}
    if (m_pEditPicFolder)
    {
        m_pEditPicFolder->SetText(GetConfig()->GetPicFolder());
    }
	if (m_pCheckBtnAutorun)
	{
		TCHAR szPath[MAX_PATH] = {0};
		if (UI::Util::QueryAutoRun(L"HourGlass", szPath))
		{
			TCHAR szPathMe[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPathMe, MAX_PATH);
			if (0 != _tcscmp(szPathMe, szPath))
			{
				UI::Util::InstallAutoRun(TRUE, L"HourGlass", szPathMe);
			}
			m_pCheckBtnAutorun->SetChecked();
		}
	}
}

void  CSettingWnd::OnTransparentPosChanged( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
	{
		GetConfig()->SetTransparent(nPos);
	}
	else
	{
		GetMainWnd()->SetWindowTransparentAlphaMask(nPos);
		GetMainWnd()->UpdateObject();
	}
}
void  CSettingWnd::OnHPosChanged(int nPos, int nScrollType)
{
	if (nScrollType == SB_ENDSCROLL)
	{
		GetConfig()->SetHls_h(nPos);
	}
	else
	{
		GetUIApplication()->GetActiveSkinRes()->ChangeSkinHLS(nPos, 0, m_pSliderS->GetPos(), CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_S);
		GetMainWnd()->UpdateObject();
	}
}
void  CSettingWnd::OnSPosChanged(int nPos, int nScrollType)
{
	if (nScrollType == SB_ENDSCROLL)
	{
		GetConfig()->SetHls_s(nPos);
	}
	else
	{
		GetUIApplication()->GetActiveSkinRes()->ChangeSkinHLS(m_pSliderH->GetPos(), 0, nPos, CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_S);
		GetMainWnd()->UpdateObject();
	}
}
void  CSettingWnd::OnBtnClick(IMessage*  pMsgFrom)
{
	if (pMsgFrom == static_cast<UI::IMessage*>(m_pBtnOk))
	{
		int nHour = 0, nMinute = 0, nSecond = 0;
		int nBreakTime = 0;
		if (m_pEditHour)
		{
			nHour = _ttoi(m_pEditHour->GetText());
		}
		if (m_pEditMinute)
		{
			nMinute = _ttoi(m_pEditMinute->GetText());
		}
		if (m_pEditSecond)
		{
			nSecond = _ttoi(m_pEditSecond->GetText());
		}

		if (nHour < 0 || nMinute < 0 || nSecond < 0)
			return;
		if (nHour == 0 && nMinute == 0 && nSecond == 0)
			return;

		if (m_pEditBreakTime)
		{
			nBreakTime = _ttoi(m_pEditBreakTime->GetText());
		}
		if (nBreakTime < 5)
			return;

		GetConfig()->SetTime(nHour, nMinute, nSecond);
		GetConfig()->SetBreakTime(nBreakTime);

        if (m_pEditPicFolder)
        {
            GetConfig()->SetPicFolder(m_pEditPicFolder->GetText());
        }

		GetConfig()->SetPlayMusic(m_pCheckBtnMusic->IsChecked());
		EndDialog(IDOK);
	}
	else if (pMsgFrom == static_cast<UI::IMessage*>(m_pBtnCancel))
	{
		EndDialog(IDCANCEL);
	}
	else if (pMsgFrom == static_cast<UI::IMessage*>(m_pCheckBtnMusic))
	{

	}
    else if (pMsgFrom == static_cast<UI::IMessage*>(m_pBtnBrownerPicFolder))
    {
        CFolderDialog dlg;
        if (IDOK == dlg.DoModal())
        {
            if (m_pEditPicFolder)
            {
                m_pEditPicFolder->SetText(dlg.m_szFolderPath);
            }
        }
    }
	else if (pMsgFrom == static_cast<UI::IMessage*>(m_pCheckBtnAutorun))
	{
		if (m_pCheckBtnAutorun->IsChecked())
		{
			TCHAR szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL, szPath, MAX_PATH);
			UI::Util::InstallAutoRun(TRUE, L"HourGlass", szPath);
		}
		else
		{
			UI::Util::InstallAutoRun(FALSE, L"HourGlass", NULL);
		}
	}
}
