#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"


class CEqualizerDlg : public UI::ICustomWindow
{
public:
	CEqualizerDlg();

	UI_BEGIN_MSG_MAP_Ixxx(CEqualizerDlg)
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED2( m_pBtnEnable, OnBtnEnable)
		UIMSG_BN_CLICKED2( m_pBtnProfile, OnBtnProfile)
		UIMSG_BN_CLICKED( _T("btn_reset"), OnBtnReset)
		UIMSG_TRBN_POSCHANGED( m_pBalance, OnBalanceChanged )
		UIMSG_TRBN_POSCHANGED( m_pEq_16k, OnEqChanged16k )
		UIMSG_TRBN_POSCHANGED( m_pEq_8k,  OnEqChanged8k )
		UIMSG_TRBN_POSCHANGED( m_pEq_4k,  OnEqChanged4k )
		UIMSG_TRBN_POSCHANGED( m_pEq_2k,  OnEqChanged2k )
		UIMSG_TRBN_POSCHANGED( m_pEq_1k,  OnEqChanged1k )
		UIMSG_TRBN_POSCHANGED( m_pEq_500, OnEqChanged500 )
		UIMSG_TRBN_POSCHANGED( m_pEq_250, OnEqChanged250 )
		UIMSG_TRBN_POSCHANGED( m_pEq_125, OnEqChanged125 )
		UIMSG_TRBN_POSCHANGED( m_pEq_63,  OnEqChanged63 )
		UIMSG_TRBN_POSCHANGED( m_pEq_31,  OnEqChanged31 )
		UIMSG_TRBN_POSCHANGED( m_pEq_preamp, OnEqChangedPreamp )
        UIMSG_WM_INITIALIZE(OnInitWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

	void  OnInitWindow( );

	void    OnClose();
	void    OnBalanceChanged( int nPos, int nScrollType );
	void    OnEqChanged16k( int nPos, int nScrollType );
	void    OnEqChanged8k( int nPos, int nScrollType );
	void    OnEqChanged4k( int nPos, int nScrollType );
	void    OnEqChanged2k( int nPos, int nScrollType );
	void    OnEqChanged1k( int nPos, int nScrollType );
	void    OnEqChanged500( int nPos, int nScrollType );
	void    OnEqChanged250( int nPos, int nScrollType );
	void    OnEqChanged125( int nPos, int nScrollType );
	void    OnEqChanged63( int nPos, int nScrollType );
	void    OnEqChanged31( int nPos, int nScrollType );
	void    OnEqChangedPreamp( int nPos, int nScrollType );

	void    OnBtnEnable();
	void    OnBtnReset();
	void    OnBtnProfile();
	
    UI::ICheckButton* m_pBtnEnable;
	UI::IButton*     m_pBtnProfile;

	UI::ISliderCtrl* m_pBalance;
	UI::ISliderCtrl* m_p3DSound;  // 暂不支持杜比环绕立体声，没有设备

	UI::ISliderCtrl* m_pEq_16k;
	UI::ISliderCtrl* m_pEq_8k;
	UI::ISliderCtrl* m_pEq_4k;
	UI::ISliderCtrl* m_pEq_2k;
	UI::ISliderCtrl* m_pEq_1k;
	UI::ISliderCtrl* m_pEq_500;
	UI::ISliderCtrl* m_pEq_250;
	UI::ISliderCtrl* m_pEq_125;
	UI::ISliderCtrl* m_pEq_63;
	UI::ISliderCtrl* m_pEq_31;
	UI::ISliderCtrl* m_pEq_preamp;
};
