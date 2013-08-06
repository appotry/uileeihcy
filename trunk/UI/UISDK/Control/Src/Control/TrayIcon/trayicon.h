#ifndef TRAYICON_H_EA332A60_B4FE_4c4f_8D3F_BD4D0BF32002
#define TRAYICON_H_EA332A60_B4FE_4c4f_8D3F_BD4D0BF32002
#include <ShellAPI.h>
#include "UISDK\Control\Inc\Interface\itrayicon.h"

namespace UI
{

#define FLICKER_TIMER_ID	0x0F11
class TrayIcon : public ITrayIcon
{
public:
	TrayIcon(IUIApplication* p);
	~TrayIcon();

	virtual HRESULT  AddTrayIcon(HWND hWndNotify, HICON hIcon, BSTR bstrToolTip);
	virtual BOOL  xProcessMessage(UIMSG* pMsg, UINT nMsgID, bool);

    virtual long  AddRef(void);
    virtual long  Release(void);
public:

	BOOL Initialize(
		HWND   hWndNotify,
		HICON  hIcon,
		TCHAR* szInfo    = NULL, 
		TCHAR* szTip     = NULL, 
		TCHAR* szTitle   = NULL, 
		UINT   infoFlag  = NIIF_NONE
		);

	void RemoveTrayIcon();
	void HideBalloon();
	void ShowBalloon();

	bool SetBalloonInfo(WCHAR* szInfo, bool bShow = false);
	bool SetBalloonTitle(WCHAR* szTitle, bool bShow = false);
	bool SetBalloonIconFlag(UINT nFlag, bool bShow = true);

	bool SetTrayIcon(UINT uID, bool bUpdate = true);
	bool SetTrayIconTip(WCHAR* szTip,  bool bUpdate = true);

public:
	void	StartFlicker(int nTimer = 500);
	void	EndFlicker();

private:
	void	OnTimer(UINT_PTR nIDEvent);
	LRESULT OnTaskbarRestart(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMsgTrayIcon(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void    OnClickTrayIcon();

private:
    // The application-defined identifier of the taskbar icon. 
    // The Shell uses hWnd and uID to identify which icon to 
    // operate on when Shell_NotifyIcon is invoked. You can 
    // have multiple icons associated with a single hWnd by 
    // assigning each a different uID.
	static	UINT    m_uid; // Õœ≈Ã±Í ∂°£
    ULONG   m_lRef;

	NOTIFYICONDATA	m_Data;
	UINT  m_uTaskbarRestartMsg;

	IUIApplication* m_pUIApplication;
	bool  m_bFlickering;
};
}

#endif // TRAYICON_H_EA332A60_B4FE_4c4f_8D3F_BD4D0BF32002