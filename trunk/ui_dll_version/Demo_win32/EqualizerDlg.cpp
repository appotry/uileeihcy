#include "stdafx.h"
#include "player.h"
#include "EqualizerDlg.h"
#include "EqualizerMgr.h"
#include "MainMgr.h"

CEqualizerDlg::CEqualizerDlg()
{
	m_pBalance = NULL;
	m_p3DSound = NULL;

	m_pEq_16k = NULL;
	m_pEq_8k = NULL;
	m_pEq_4k = NULL;
	m_pEq_2k = NULL;
	m_pEq_1k = NULL;
	m_pEq_500 = NULL;
	m_pEq_250 = NULL;
	m_pEq_125 = NULL;
	m_pEq_63 = NULL;
	m_pEq_31 = NULL;
	m_pEq_preamp = NULL;

}
void CEqualizerDlg::OnInitWindow( )
{
	CustomWindow::OnInitWindow();

	m_pBalance = (SliderCtrl*)this->FindChildObject(_T("progress_pan"));
	m_p3DSound = (SliderCtrl*)this->FindChildObject(_T("progress_3d"));

	if (NULL != m_pBalance)
	{
		m_pBalance->SetRange(-10,10, false);
		int nPercent = GetMainMgr()->GetConfigData()->player.m_byteBalance;
		m_pBalance->SetPos(nPercent/10, false);
	}
	if (NULL != m_p3DSound)
	{
		m_p3DSound->SetEnable(false);
	}

	m_pEq_16k = (SliderCtrl*)this->FindChildObject(_T("progress_eq_16k"));
	m_pEq_8k =  (SliderCtrl*)this->FindChildObject(_T("progress_eq_8k"));
	m_pEq_4k =  (SliderCtrl*)this->FindChildObject(_T("progress_eq_4k"));
	m_pEq_2k =  (SliderCtrl*)this->FindChildObject(_T("progress_eq_2k"));
	m_pEq_1k =  (SliderCtrl*)this->FindChildObject(_T("progress_eq_1k"));
	m_pEq_500 = (SliderCtrl*)this->FindChildObject(_T("progress_eq_500"));
	m_pEq_250 = (SliderCtrl*)this->FindChildObject(_T("progress_eq_250"));
	m_pEq_125 = (SliderCtrl*)this->FindChildObject(_T("progress_eq_125"));
	m_pEq_63 = (SliderCtrl*)this->FindChildObject(_T("progress_eq_63"));
	m_pEq_31 = (SliderCtrl*)this->FindChildObject(_T("progress_eq_31"));
	m_pEq_preamp = (SliderCtrl*)this->FindChildObject(_T("progress_eq_preamp"));

	if (NULL != m_pEq_16k)
	{
		m_pEq_16k->SetRange(-12,12,false);
		m_pEq_16k->SetPos(0, false);
	}
	if (NULL != m_pEq_8k)
	{
		m_pEq_8k->SetRange(-12,12,false);
		m_pEq_8k->SetPos(0, false);
	}
	if (NULL != m_pEq_4k)
	{
		m_pEq_4k->SetRange(-12,12,false);
		m_pEq_4k->SetPos(0, false);
	}
	if (NULL != m_pEq_2k)
	{
		m_pEq_2k->SetRange(-12,12,false);
		m_pEq_2k->SetPos(0, false);
	}
	if (NULL != m_pEq_16k)
	{
		m_pEq_1k->SetRange(-12,12,false);
		m_pEq_1k->SetPos(0, false);
	}
	if (NULL != m_pEq_500)
	{
		m_pEq_500->SetRange(-12,12,false);
		m_pEq_500->SetPos(0, false);
	}
	if (NULL != m_pEq_250)
	{
		m_pEq_250->SetRange(-12,12,false);
		m_pEq_250->SetPos(0, false);
	}
	if (NULL != m_pEq_125)
	{
		m_pEq_125->SetRange(-12,12,false);
		m_pEq_125->SetPos(0, false);
	}
	if (NULL != m_pEq_63)
	{
		m_pEq_63->SetRange(-12,12,false);
		m_pEq_63->SetPos(0, false);
	}
	if (NULL != m_pEq_31)
	{
		m_pEq_31->SetRange(-12,12,false);
		m_pEq_31->SetPos(0, false);
	}
	if (NULL != m_pEq_preamp)
	{
		m_pEq_preamp->SetRange(-12,12,false);
		m_pEq_preamp->SetPos(0, false);
	}

	m_pBtnEnable = (CheckButton*)this->FindChildObject(_T("btn_enable"));
	if (NULL != m_pBtnEnable)
	{
//		m_pBtnEnable->SetChecked();
	}

	m_pBtnProfile = (Button*)this->FindChildObject(_T("btn_profile"));
	
	// 由于采用的IIR均衡算法存在问题，基于频域的算法很多细节又不知道如何实现，
	// 因此默认先禁用均衡器
	this->OnBtnEnable();
}

void CEqualizerDlg::OnClose()
{
	this->HideWindow();
	GetEqualizerMgr()->OnEqualizerDlgHide();
}

void CEqualizerDlg::OnBalanceChanged( int nPos, int nScrollType )
{
	GetEqualizerMgr()->SetBalance(nPos*10, SB_ENDSCROLL==nScrollType);  // 百分制
}
void CEqualizerDlg::OnEqChanged16k( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_16K, nPos);
}
void CEqualizerDlg::OnEqChanged8k( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_8K, nPos);
}
void CEqualizerDlg::OnEqChanged4k( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_4K, nPos);
}
void CEqualizerDlg::OnEqChanged2k( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_2K, nPos);
}
void CEqualizerDlg::OnEqChanged1k( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_1K, nPos);
}
void CEqualizerDlg::OnEqChanged500( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_500, nPos);
}
void CEqualizerDlg::OnEqChanged250( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_250, nPos);
}
void CEqualizerDlg::OnEqChanged125( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_125, nPos);
}

void CEqualizerDlg::OnEqChanged63( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_63, nPos);
}
void CEqualizerDlg::OnEqChanged31( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_31, nPos);
}
void CEqualizerDlg::OnEqChangedPreamp( int nPos, int nScrollType )
{
	if (nScrollType == SB_ENDSCROLL)
		GetEqualizerMgr()->SetEq(EQ_FREQ_PREAMP, nPos);
}
void CEqualizerDlg::OnBtnEnable()
{
	bool bRet = m_pBtnEnable->IsChecked();
	GetEqualizerMgr()->EnableEq(bRet);

	if (NULL != m_pEq_16k)
		m_pEq_16k->SetEnable(bRet, false);
	if (NULL != m_pEq_8k)
		m_pEq_8k->SetEnable(bRet, false);
	if (NULL != m_pEq_4k)
		m_pEq_4k->SetEnable(bRet, false);
	if (NULL != m_pEq_2k)
		m_pEq_2k->SetEnable(bRet, false);
	if (NULL != m_pEq_1k)
		m_pEq_1k->SetEnable(bRet, false);
	if (NULL != m_pEq_500)
		m_pEq_500->SetEnable(bRet, false);
	if (NULL != m_pEq_250)
		m_pEq_250->SetEnable(bRet, false);
	if (NULL != m_pEq_125)
		m_pEq_125->SetEnable(bRet, false);
	if (NULL != m_pEq_63)
		m_pEq_63->SetEnable(bRet, false);
	if (NULL != m_pEq_31)
		m_pEq_31->SetEnable(bRet, false);
	if (NULL != m_pEq_preamp)
		m_pEq_preamp->SetEnable(bRet, false);

	this->UpdateObject();
}
void CEqualizerDlg::OnBtnReset()
{
	if (NULL != m_pEq_16k)
	{
		m_pEq_16k->SetPos(0, false);
	}
	if (NULL != m_pEq_8k)
	{
		m_pEq_8k->SetPos(0, false);
	}
	if (NULL != m_pEq_4k)
	{
		m_pEq_4k->SetPos(0, false);
	}
	if (NULL != m_pEq_2k)
	{
		m_pEq_2k->SetPos(0, false);
	}
	if (NULL != m_pEq_1k)
	{
		m_pEq_1k->SetPos(0, false);
	}
	if (NULL != m_pEq_500)
	{
		m_pEq_500->SetPos(0, false);
	}
	if (NULL != m_pEq_250)
	{
		m_pEq_250->SetPos(0, false);
	}
	if (NULL != m_pEq_125)
	{
		m_pEq_125->SetPos(0, false);
	}
	if (NULL != m_pEq_63)
	{
		m_pEq_63->SetPos(0, false);
	}
	if (NULL != m_pEq_31)
	{
		m_pEq_31->SetPos(0, false);
	}
	if (NULL != m_pEq_preamp)
	{
		m_pEq_preamp->SetPos(0, false);
	}
	GetEqualizerMgr()->ReSetEq();
	this->UpdateObject();
}
void CEqualizerDlg::OnBtnProfile()
{
	Menu* pMenu = UI_LoadMenu(_T("menu_eq_profile"));
	if (NULL == pMenu)
		return ;

	CRect rc;
	m_pBtnProfile->GetWindowRect(&rc);
	::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rc, 2);

	int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, NULL);
	switch (nRet)
	{
	case 1301:
		break;
	default:
		break;
	}
	SAFE_DELETE(pMenu);
}
