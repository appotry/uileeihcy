#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
namespace UI
{
	interface ILEDCtrl;
	interface IButton;
	interface IEdit;
	interface ICheckButton;
	interface ISliderCtrl;
}
class CMainWnd;
class CSettingWnd : public UI::IWindow
{
public:
	CSettingWnd();
	~CSettingWnd();

	UI_BEGIN_MSG_MAP_Ixxx(CSettingWnd)
		UIMSG_BN_CLICKED3(OnBtnClick)
		UIMSG_TRBN_POSCHANGED(m_pSliderTransparent, OnTransparentPosChanged)
		UIMSG_TRBN_POSCHANGED(m_pSliderH, OnHPosChanged)
		UIMSG_TRBN_POSCHANGED(m_pSliderS, OnSPosChanged)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
	void  OnInitialize();
	void  OnBtnClick(IMessage*  pMsgFrom);
	void  OnTransparentPosChanged( int nPos, int nScrollType );
	void  OnHPosChanged(int nPos, int nScrollType);
	void  OnSPosChanged(int nPos, int nScrollType);
	

	void  SetMainWnd(CMainWnd* p)
	{
		m_pMainWnd = p;
	}
private:
	UI::IButton*  m_pBtnOk;
	UI::IButton*  m_pBtnCancel;
	UI::IEdit*    m_pEditHour;
	UI::IEdit*    m_pEditMinute;
	UI::IEdit*    m_pEditSecond;
	UI::IEdit*    m_pEditBreakTime;
	UI::ICheckButton*  m_pCheckBtnMusic;
    UI::IButton*  m_pBtnBrownerPicFolder;
    UI::IEdit*    m_pEditPicFolder;
	UI::ICheckButton*  m_pCheckBtnAutorun;
	UI::ISliderCtrl*  m_pSliderTransparent;
	UI::ISliderCtrl*  m_pSliderH;
	UI::ISliderCtrl*  m_pSliderS;

	CMainWnd*     m_pMainWnd;

	// public:
	//     int  m_nHour;
	//     int  m_nMinute;
	//     int  m_nSecond;
};