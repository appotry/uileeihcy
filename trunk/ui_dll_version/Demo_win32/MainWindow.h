#pragma once
#include "Resource.h"
#include "playlistdlg.h"
#include "LyricDlg.h"
#include "EqualizerDlg.h"
class COptionWindow;

class MainWindow : public UI::CustomWindow
{
public:
	MainWindow(void);
	~MainWindow(void);

	UI_BEGIN_MSG_MAP
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
		UIMSG_TRBN_POSCHANGED_ID( _T("progress_music"), OnMusicProgressPosChanged )
		UIMSG_TRBN_POSCHANGED( m_pVolume, OnVolumnChanged )

		UIMSG_WM_SKINHLSCHANGED(OnSkinHLSChanged)
		UIMSG_WM_SKINCHANGED(OnSkinChanged)
		UIMSG_WM_TIMER(OnTimer)
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
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)


public:
	virtual void OnInitWindow();
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );

	void    OnPaint(HRDC hRDC);
	void    OnDestroy();
	void    OnBnClickPlay();
	void    OnBnClickOpen();
	void    OnBnClickPlaylist();
	void    OnBnClickLyric();
	void    OnBnClickEqualizer();
	void    OnBnClickPause();
	void    OnBnClickStop();
	void    OnBnClickMute();
	void    OnBnClickPrev();
	void    OnBnClickNext();
	void    OnMusicProgressPosChanged(int nPos, int nScrollType);
	void    OnVolumnChanged(int nPos, int nScrollType);
    void    OnContextMenu(HWND wnd, POINT point);
	void    OnMenuClick(UINT nMenuID);
	void    OnSysCommand(UINT nID, CPoint lParam);
	void    OnSkinHLSChanged();
	void    OnSkinChanged();
	BOOL    OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
	
	void    OnMp3Start(PlayerListItemInfo* pItemInfo);
	void    OnMp3Pause();
	void    OnMp3Stop();
	void    OnMp3Continue();
	void    OnMp3ProgressInd(double dSeconds, double dPercent);
//	void    OnMp3VolumeInd(long lVolumn);

	void    OnPlayerListDlgCreated(HWND hWnd);
	void    OnPlayerListDlgVisibleChanged(HWND wParam, BOOL lParam);
	void    OnEqualizerDlgCreated(HWND hWnd);
	void    OnEqualizerDlgVisibleChanged(HWND wParam, BOOL lParam);
	void    OnBalanceChanged(long lPercent, bool bFinish);
	void    OnTimer(UINT_PTR nIDEvent, LPARAM lParam);

// 	void    OnLButtonDown(UINT nFlags, POINT point);
// 	void    OnLButtonUp(UINT nFlags, POINT point);
// 	void    OnMouseMove(UINT nFlags, POINT point);
// 	void    OnCancelMode();
// 
// 	void    OnEnterSizeMove();
// 	void    OnExitSizeMove();

	void    SetPlayerListDlgHandle(HWND hWnd);
	void    SetEqualizerDlgHandle(HWND hWnd);

	HBITMAP GetVisualizationInfo(RECT* prc);
	void    OnVisualiazationTypeChanged(int nType);

protected:
	HWND            m_hWndPlayerList;
	CLyricDlg*      m_pLyricDlg;
//	CEqualizerDlg*  m_pEqualizerDlg;
	HWND            m_hWndEqualizer;
	COptionWindow*  m_pOptionWindow;

// 	long            m_lSizeMove;    // 0 没开始，1鼠标左键已按下，2鼠标开始拖拽
// 	POINT           m_ptCursorSizeMove;
// 	POINT           m_ptWndPosSizeMove;

	Button*         m_pbtnStart;
	Button*         m_pbtnPause;
	Button*         m_pbtnStop;
	Button*         m_pbtnOpen;
	CheckButton*    m_pBtnPlaylist;
	CheckButton*    m_pBtnLyric;
	CheckButton*    m_pBtnEqualizer;
	CheckButton*    m_pbtnMute;
	Label*          m_pLabelPlaystatus;
	LEDCtrl*        m_pLEDTime;
	SliderCtrl*     m_pProgress;
	SliderCtrl*     m_pVolume;
	Picture*        m_pVisuallzationPic;

	String     m_strStatusText;
};
