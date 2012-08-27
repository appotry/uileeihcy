#pragma once
#include "Resource.h"
#include "playlistdlg.h"
#include "LyricDlg.h"
#include "EqualizerDlg.h"
#include "AnchorWindowHelper.h"

class MainWindow : 
			public UI::CustomWindow, 
			public IMp3EventCallback,
			public AnchorWindowHelper<MainWindow>
{
public:
	MainWindow(void);
	~MainWindow(void);

	UI_BEGIN_MSG_MAP
		UIMSG_BN_CLICKED( _T("btn_playlist"),  OnBnClickPlaylist )
		UIMSG_BN_CLICKED( _T("btn_lyric"),     OnBnClickLyric )
		UIMSG_BN_CLICKED( _T("btn_equalizer"), OnBnClickEqualizer )
		UIMSG_BN_CLICKED( _T("btn_play"),  OnBnClickPlay )
		UIMSG_BN_CLICKED( _T("btn_open"),  OnBnClickOpen )
		UIMSG_BN_CLICKED( _T("btn_pause"), OnBnClickPause )
		UIMSG_BN_CLICKED( _T("btn_stop"),  OnBnClickStop )
		UIMSG_BN_CLICKED( _T("btn_mute"),  OnBnClickMute )
		UIMSG_TRBN_POSCHANGED( _T("progress_music"), OnMusicProgressPosChanged )
		UIMSG_TRBN_POSCHANGED( _T("progress_voice"), OnVolumnChanged )

// 		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
// 		UIMSG_WM_LBUTTONUP( OnLButtonUp )
// 		UIMSG_WM_MOUSEMOVE( OnMouseMove )
// 		UIMSG_WM_CANCELMODE( OnCancelMode )

#if 1
		UIMSG_WM_WINDOWPOSCHANGING(AnchorWindowHelper<MainWindow>::__OnWindowPosChanging)
#else
		UIMSG_WM_LBUTTONDOWN( AnchorWindowHelper<MainWindow>::OnLButtonDown )
		UIMSG_WM_LBUTTONUP( AnchorWindowHelper<MainWindow>::OnLButtonUp )
		UIMSG_WM_MOUSEMOVE( AnchorWindowHelper<MainWindow>::OnMouseMove )
		UIMSG_WM_CANCELMODE( AnchorWindowHelper<MainWindow>::OnCancelMode )
#endif

		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_WM_TIMER(OnTimer)
		UIMSG_WM_CONTEXTMENU(OnContextMenu);
		UICHAIN_MSG_MAP(CustomWindow)
	UI_END_MSG_MAP


public:
	virtual void OnInitWindow();
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
	void    OnDestroy();
	void    OnBnClickPlay();
	void    OnBnClickOpen();
	void    OnBnClickPlaylist();
	void    OnBnClickLyric();
	void    OnBnClickEqualizer();
	void    OnBnClickPause();
	void    OnBnClickStop();
	void    OnBnClickMute();
	void    OnMusicProgressPosChanged(int nPos, int nScrollType);
	void    OnVolumnChanged(int nPos, int nScrollType);
	void    OnTimer(UINT_PTR nIDEvent);
    void    OnContextMenu(HWND wnd, POINT point);

// 	void    OnLButtonDown(UINT nFlags, POINT point);
// 	void    OnLButtonUp(UINT nFlags, POINT point);
// 	void    OnMouseMove(UINT nFlags, POINT point);
// 	void    OnCancelMode();
// 
// 	void    OnEnterSizeMove();
// 	void    OnExitSizeMove();

	void SetPlayerListDlgHandle(HWND hWnd);

protected:	
	virtual void   on_mp3_stop();
	virtual void   on_mp3_progress_ind(LONGLONG dCur, LONGLONG dLen);
	virtual void   on_mp3_volume_ind(long lVolumn);

protected:
	HWND            m_hWndPlayerList;
	CLyricDlg*      m_pLyricDlg;
	CEqualizerDlg*  m_pEqualizerDlg;

// 	long            m_lSizeMove;    // 0 没开始，1鼠标左键已按下，2鼠标开始拖拽
// 	POINT           m_ptCursorSizeMove;
// 	POINT           m_ptWndPosSizeMove;

	Button*         m_pbtnStart;
	Button*         m_pbtnPause;
	Button*         m_pbtnStop;
	Button*         m_pbtnOpen;
	CheckButton*    m_pbtnMute;
	Label*          m_pLabelPlaystatus;
	Label*          m_pLabelTime;
	LEDCtrl*        m_pLEDTime;
	SliderCtrl*     m_pProgress;
	SliderCtrl*     m_pVolume;

	Menu*           m_pMenu;
};
