#pragma once
#include "Resource.h"
#include "playlistdlg.h"

class MainWindow : public UI::CustomWindow, public IMp3EventCallback
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
		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_MOUSEMOVE( OnMouseMove )
		UIMSG_WM_CANCELMODE( OnCancelMode )
		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_WM_TIMER(OnTimer)
		UIMSG_WM_CONTEXTMENU(OnContextMenu);
		UICHAIN_MSG_MAP(CustomWindow)
	UI_END_MSG_MAP


public:
	virtual void OnInitWindow();
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs , DWORD& dwStyleEx );
	void OnDestroy();
	void OnBnClickPlay();
	void OnBnClickOpen();
	void OnBnClickPlaylist();
	void OnBnClickLyric();
	void OnBnClickEqualizer();
	void OnBnClickPause();
	void OnBnClickStop();
	void OnBnClickMute();
	void OnMusicProgressPosChanged( int nPos, int nScrollType );
	void OnVolumnChanged( int nPos, int nScrollType );
	void OnTimer(UINT_PTR nIDEvent);
    void OnContextMenu( HWND wnd, POINT point );
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnMouseMove(UINT nFlags, POINT point);
	void OnCancelMode();

	void OnEnterSizeMove();
	void OnExitSizeMove();

protected:	
	virtual void on_mp3_stop();
	virtual void on_mp3_progress_ind(LONGLONG dCur, LONGLONG dLen);
	virtual void on_mp3_volume_ind(long lVolumn);

protected:
	CPlayListDlg*   m_pPlaylistDlg;
	CLyricDlg*      m_pLyricDlg;
	CEqualizerDlg*  m_pEqualizerDlg;

	long     m_lSizeMove;    // 0 û��ʼ��1�������Ѱ��£�2��꿪ʼ��ק
	POINT    m_ptCursorSizeMove;
	POINT    m_ptWndPosSizeMove;

	Button*  m_pbtnStart;
	Button*  m_pbtnPause;
	Button*  m_pbtnStop;
	Button*  m_pbtnOpen;
	CheckButton*  m_pbtnMute;
	Label*        m_pLabelPlaystatus;
	Label*        m_pLabelTime;
	SliderCtrl*   m_pProgress;
	SliderCtrl*   m_pVolume;
};



class COptionWindow : public UI::Window
{
public:
	enum{ IDD=IDD_OPTION };
	COptionWindow();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_INITDIALOG(OnInitDialog)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_WM_DESTROY(OnDestroy)
		UIMSG_BN_CLICKED(_T("btn_cancel"), OnBtnCancel)
		UIMSG_BN_CLICKED(_T("btn_save_all"),OnBtnSaveAll)
		UIMSG_BN_CLICKED(_T("btn_reset_all"), OnBtnResetAll)
		UICOMMAND_HANDLER_EX(IDC_BUTTON1, BN_CLICKED, OnBtnClick1 )
		UICOMMAND_HANDLER_EX(IDC_BUTTON2, BN_CLICKED, OnBtnClick2 )
		UICHAIN_MSG_MAP(WindowBase)
		UI_END_MSG_MAP


public:
	BOOL OnInitDialog( HWND, LPARAM );
	void OnDestroy();
	void OnClose();
	void OnBtnCancel();
	void OnBtnSaveAll();
	void OnBtnResetAll();

	void OnBtnClick1(UINT uNotifyCode, int nID, HWND wndCtl);
	void OnBtnClick2(UINT uNotifyCode, int nID, HWND wndCtl);

private:
	ListBox*   m_pListOption;
};