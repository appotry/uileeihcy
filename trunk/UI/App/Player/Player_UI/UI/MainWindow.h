#pragma once
#include "App\Player\Player_UI\Other\Resource.h"
#include "App\Player\Player_UI\UI\playlistdlg.h"
#include "App\Player\Player_UI\UI\LyricDlg.h"
#include "App\Player\Player_UI\UI\EqualizerDlg.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
class COptionWindow;

namespace UI
{
    interface ILabel;
    interface IButton;
    interface ILEDCtrl;
    interface ISliderCtrl;
    interface IPictureCtrl;
    interface IMenu;
    interface IStoryboard;
}
interface IMultiLabel;

enum LEDTimeShowType
{
    LEDTimeShow_ELAPSE,  // 已播放时间
    LEDTimeShow_REMAIN,  // 剩余时间
};
class MainWindow : public UI::ICustomWindow
{
public:
	MainWindow(void);
	~MainWindow(void);

	UI_BEGIN_MSG_MAP_Ixxx(MainWindow)
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_BN_CLICKED( _T("btn_playlist"),  OnBnClickPlaylist )
		UIMSG_BN_CLICKED( _T("btn_lyric"),     OnBnClickLyric )
		UIMSG_BN_CLICKED( _T("btn_equalizer"), OnBnClickEqualizer )
		UIMSG_BN_CLICKED( _T("btn_play"),  OnBnClickPlay )
		UIMSG_BN_CLICKED( _T("btn_open"),  OnBnClickOpen )
		UIMSG_BN_CLICKED( _T("btn_pause"), OnBnClickPause )
		UIMSG_BN_CLICKED( _T("btn_stop"),  OnBnClickStop )
		UIMSG_BN_CLICKED( _T("btn_mute"),  OnBnClickMute )
		UIMSG_BN_CLICKED( _T("btn_prev"),  OnBnClickPrev )
		UIMSG_BN_CLICKED( _T("btn_next"),  OnBnClickNext )
        UIMSG_BN_CLICKED2(m_pLEDTime,  OnLEDTimeClicked)
		UIMSG_TRBN_POSCHANGED( m_pProgress, OnMusicProgressPosChanged )
		UIMSG_TRBN_POSCHANGED( m_pVolume, OnVolumnChanged )

		UIMSG_WM_SKINHLSCHANGED(OnSkinHLSChanged)
		UIMSG_WM_SKINCHANGED(OnSkinChanged)
		UIMSG_WM_TIMER(OnTimer)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)

		UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
// 		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
// 		UIMSG_WM_LBUTTONUP( OnLButtonUp )
// 		UIMSG_WM_MOUSEMOVE( OnMouseMove )
// 		UIMSG_WM_CANCELMODE( OnCancelMode )
#if 1
//		UIMSG_WM_WINDOWPOSCHANGING(SyncWindowHelper<MainWindow>::__OnWindowPosChanging)
#else
		UIMSG_WM_LBUTTONDOWN( SyncWindowHelper<MainWindow>::OnLButtonDown )
		UIMSG_WM_LBUTTONUP( SyncWindowHelper<MainWindow>::OnLButtonUp )
		UIMSG_WM_MOUSEMOVE( SyncWindowHelper<MainWindow>::OnMouseMove )
		UIMSG_WM_CANCELMODE( SyncWindowHelper<MainWindow>::OnCancelMode )
#endif
		UIMSG_WM_CONTEXTMENU(OnContextMenu);
		UIMSG_MENU_CLICK(OnMenuClick)	

		UIMSG_WM_SYSCOMMAND(OnSysCommand)
		UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)


protected:
    void  OnInitWindow();
    void  LoadCtrls();
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);

    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  OnDestroy();
    void  OnBnClickPlay();
    void  OnBnClickOpen();
    void  OnBnClickPlaylist();
    void  OnBnClickLyric();
    void  OnBnClickEqualizer();
    void  OnBnClickPause();
    void  OnBnClickStop();
    void  OnBnClickMute();
    void  OnBnClickPrev();
    void  OnBnClickNext();
    void  OnLEDTimeClicked();
    void  OnMusicProgressPosChanged(int nPos, int nScrollType);
    void  OnVolumnChanged(int nPos, int nScrollType);
    void  OnContextMenu(HWND wnd, POINT point);
    void  OnMenuClick(UINT nMenuID);
    void  OnSysCommand(UINT nID, CPoint lParam);
    void  OnSkinHLSChanged();
    void  OnSkinChanged();
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);

public:
	void  OnMp3Start(IPlayerListItemInfo* pItemInfo);
	void  OnMp3Pause();
	void  OnMp3Stop();
    void  OnMp3Finish(bool bHaveNext);
	void  OnMp3Continue();
	void  OnMp3ProgressInd(double dSeconds, double dPercent);
//	void  OnMp3VolumeInd(long lVolumn);

	void  OnPlayerListDlgCreated(HWND hWnd);
	void  OnPlayerListDlgVisibleChanged(HWND wParam, BOOL lParam);
	void  OnEqualizerDlgCreated(HWND hWnd);
	void  OnEqualizerDlgVisibleChanged(HWND wParam, BOOL lParam);
    void  OnLyricDlgCreated(HWND hWnd);
    void  OnLyricDlgVisibleChanged(HWND wParam, BOOL lParam);
    
    void  SetPlayerListDlgHandle(HWND hWnd);
    void  SetEqualizerDlgHandle(HWND hWnd);
    void  SetLyricDlgHandle(HWND hWnd);

	void  OnBalanceChanged(long lPercent, bool bFinish);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);

	HBITMAP  GetVisualizationInfo(RECT* prc);
	void  OnVisualiazationTypeChanged(int nType);

	int   GetCurrentPlayingPercent();

protected:
	void  LoadMainMenu();
    void  UpdateLEDTimeValue();

protected:
	HWND  m_hWndPlayerList;
	HWND  m_hWndEqualizer;
    HWND  m_hWndLyric;

	COptionWindow*  m_pOptionWindow;

    UI::IButton*       m_pbtnStart;
	UI::IButton*       m_pbtnPause;
	UI::IButton*       m_pbtnStop;
	UI::IButton*       m_pbtnOpen;
	UI::ICheckButton*  m_pBtnPlaylist;
	UI::ICheckButton*  m_pBtnLyric;
	UI::ICheckButton*  m_pBtnEqualizer;
	UI::ICheckButton*  m_pbtnMute;
	UI::ILabel*        m_pLabelPlaystatus;
	IMultiLabel*       m_pMultiLabelTitle;
	UI::ILEDCtrl*      m_pLEDTime;
    UI::ILabel*        m_pChannelMode;
	UI::ISliderCtrl*   m_pProgress;
	UI::ISliderCtrl*   m_pVolume;
	UI::IPictureCtrl*  m_pVisuallzationPic;

	UI::IMenu*  m_pMainMenu;
	String      m_strStatusText;

    LEDTimeShowType   m_eLEDTimeShowType;
    int  m_nTimeElapse;  // 当前歌曲已经播放了的时间，用于切换时间显示类型(单位ms)
};

