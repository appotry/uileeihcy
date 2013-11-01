#include "stdafx.h"
#include "trayicon.h"

namespace UI
{

//////////////////////////////////////////////////////////////////////////
UINT TrayIcon::m_uid = 0;
TrayIcon::TrayIcon(IUIApplication* p):m_bFlickering(false)
{
	m_uTaskbarRestartMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));
	m_pUIApplication = p;

    m_lRef = 0;
}

TrayIcon::~TrayIcon()
{
	RemoveTrayIcon();
}

long  TrayIcon::AddRef(void)
{
    return ++m_lRef;
}
long  TrayIcon::Release(void)
{
    --m_lRef; 
    if (0 == m_lRef)
    {
        delete this; 
        return 0;
    } 
    return m_lRef;
}

BOOL TrayIcon::Initialize(HWND  hWndNotify,
						  HICON  hIcon,
						  TCHAR* szInfo,		
						  TCHAR* szTip, 
						  TCHAR* szTitle,		
						  UINT   infoFlag
						 )
{
	if ((NULL != szInfo && wcslen(szInfo) > 255) || (NULL != szTip && wcslen(szTip) > 127)) 
		return FALSE;

	memset(&m_Data, 0, sizeof(m_Data));
	m_Data.cbSize  = sizeof(NOTIFYICONDATA); //NOTIFYICONDATA_V2_SIZE;   //sizeof( m_Data );
	m_Data.hWnd    = hWndNotify;
	m_Data.uID     = m_uid;
	m_Data.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP |  NIF_INFO /*NIF_STATE |*/;
	m_Data.hIcon   = hIcon;
	::DestroyIcon(hIcon);
	m_Data.uCallbackMessage = UI_WM_TRAYICON;

	if(szTitle == NULL)
	{
		wcscpy_s(m_Data.szInfoTitle, _T(""));
	}
	else
	{
		if (wcslen(szTitle) <= 63) 
			wcscpy_s(m_Data.szInfoTitle, szTitle);
		else
			wcscpy_s(m_Data.szInfoTitle, _T(""));
	}

	if(szTip == NULL)
	{
		wcscpy_s(m_Data.szTip, _T(""));
	}
	else
	{
		if (wcslen(szTip) <= 127) 
			wcscpy_s(m_Data.szTip, szTip);
		else
			wcscpy_s(m_Data.szTip, _T(""));
	}

	if(szInfo != NULL)
	{
		if( wcslen(szInfo) <= 255 )
			wcscpy_s(m_Data.szInfo, szInfo);
		else
			wcscpy_s(m_Data.szInfo, _T(""));
	}
	else
	{
		wcscpy_s(m_Data.szInfo, _T(""));
	}
	m_Data.dwInfoFlags = infoFlag;

	//m_Data.uTimeout = 10*1000 | 6;
	m_Data.uVersion = NOTIFYICON_VERSION;   

	//m_Data.dwStateMask = 0;//NIS_HIDDEN | NIS_SHAREDICON;	//NIS_HIDDEN的意思相当于是删除掉了拖盘图标
	//m_Data.dwState = NIS_SHAREDICON;		/*NIS_HIDDEN;*/

	return TRUE;
}

//
// Add之前先调用Initialize
HRESULT TrayIcon::AddTrayIcon(HWND hWndNotify, HICON hIcon, BSTR bstrToolTip)
{
	if (m_uid!=0)
		return E_FAIL;

	this->Initialize(hWndNotify, hIcon, NULL, bstrToolTip);

	m_uid ++;
	Shell_NotifyIcon(NIM_ADD,&m_Data);
	Shell_NotifyIcon(NIM_SETVERSION,&m_Data);	// 开启NOTIFYICON的新版本特性
												// 该函数只能在Add了之后调用。
	return S_OK;
}

////////////////////////////////////////////////////////
//
//	隐藏气泡
//	[注]
//		该函数不会修改提示条的数据
//
void TrayIcon::HideBalloon()
{
	TCHAR   szInfo[256];
	int len = wcslen(m_Data.szInfo);
	len += 1;		// 加上一个 '\0';
	wcscpy_s(szInfo,len, m_Data.szInfo);
	wcscpy_s(m_Data.szInfo, _T(""));
	Shell_NotifyIcon(NIM_MODIFY,&m_Data);
	wcscpy_s(m_Data.szInfo,len, szInfo);
}
void TrayIcon::ShowBalloon()
{
	Shell_NotifyIcon(NIM_MODIFY,&m_Data);
}


void TrayIcon::RemoveTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE,&m_Data);
}

////////////////////////////////////////////////////////////
//
//	设置气泡提示条内容
//
bool TrayIcon::SetBalloonInfo(TCHAR* szInfo, bool bShow )
{
	if( wcslen(szInfo) > 255 )
		return false;

	wcscpy_s(m_Data.szInfo,szInfo);

	if (bShow)
		Shell_NotifyIcon(NIM_MODIFY,&m_Data);

	return true;
}

////////////////////////////////////////////////////////////
//
//	设置气泡提示条标题
//
bool TrayIcon::SetBalloonTitle(TCHAR* szTitle, bool bShow)
{
	if( wcslen(szTitle) > 63 )
		return false;

	wcscpy_s(m_Data.szInfoTitle,szTitle);

	if (bShow)
		Shell_NotifyIcon(NIM_MODIFY,&m_Data);

	return true;
}
/*=================================================================================================================================
	NIIF_ERROR
		An error icon.
	NIIF_INFO
		An information icon.
	NIIF_NONE
		No icon.
	NIIF_USER
		Windows XP Service Pack 2 (SP2) and later. Use the icon identified in hIcon as the notification balloon's title icon.
	NIIF_WARNING
		A warning icon.
	NIIF_ICON_MASK
		Version 6.0. Reserved.
	NIIF_NOSOUND
		Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips.
=================================================================================================================================*/
bool TrayIcon::SetBalloonIconFlag(UINT nFlag, bool bShow)
{
	m_Data.dwInfoFlags = nFlag;

	if (bShow)
		Shell_NotifyIcon(NIM_MODIFY,&m_Data);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//	设置拖盘的图标
//	[argument]
//		bUpdate:	true 表示立即显示新图标, false 暂时不显示新图标
//
bool TrayIcon::SetTrayIcon(UINT uID, bool bUpdate)
{
	m_Data.hIcon = ::LoadIcon(::GetModuleHandle(NULL),MAKEINTRESOURCE(uID));
	::DestroyIcon(m_Data.hIcon);

	if (bUpdate)
	{
		Shell_NotifyIcon(NIM_MODIFY, &m_Data);
	}
	
	return true;
}

bool TrayIcon::SetTrayIconTip(TCHAR* szTip, bool bUpdate)
{	
	if (wcslen(szTip) > 127)
		return false;
	
	wcscpy_s(m_Data.szTip, szTip);

	if (bUpdate)
	{
		Shell_NotifyIcon(NIM_MODIFY,&m_Data);
	}
	return true;
}


/*---------------------------------------------------------------------
	[当任务栏崩溃的时候调用该函数重新恢复拖盘图标]

该函数使用方法：
	1.声明：
		UINT			m_uTaskbarRestartMsg;	// 当任务栏崩溃后触发的消息
	2.初始化：
		// 任务栏重启后会向窗口发送消息TaskbarCreated
		m_uTaskbarRestartMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));
	3.响应消息
		MESSAGE_HANDLER(m_uTaskbarRestartMsg,OnTaskbarRestart)
		LRESULT OnTaskbarRestart(UINT, WPARAM, LPARAM, BOOL&);
	
		LRESULT CMainDlg::OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			this->m_trayIcon.OnTaskbarRestart();
			return S_OK;
		}
	4.在父窗口中添加:
		CHAIN_MSG_MAP_ALT_MEMBER(m_trayIcon,0)
----------------------------------------------------------------------*/
LRESULT TrayIcon::OnTaskbarRestart(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	this->RemoveTrayIcon();
	wcscpy_s(m_Data.szInfo,_T(""));

	Shell_NotifyIcon(NIM_ADD,&m_Data);
	return S_OK;
}

//////////////////////////////////////////////////////
//
//	开始闪烁拖盘图标 
//	[注]
//		该函数会将气泡提示内容改为空
//
void TrayIcon::StartFlicker(int nTimer)
{
	UIASSERT(0 && _T("将TrayIcon继承Message，调用内置的计时器"));
#if 0
	if (m_bFlickering)
		return;

	// 闪烁的时候不再显示气泡了
	wcscpy_s(m_Data.szInfo, _T(""));
	
	m_bFlickering = true;

	// 立即响应一次
	OnTimer(FLICKER_TIMER_ID);
	SetTimer(m_Data.hWnd,FLICKER_TIMER_ID,nTimer,NULL);
#endif
}

//////////////////////////////////////////////////////
//
//	停止闪烁拖盘图标 
//
void TrayIcon::EndFlicker()
{
#if 0
	if (m_bFlickering)
	{
		KillTimer(m_Data.hWnd,FLICKER_TIMER_ID);

		// 如果该时候图标为空的话，再运行一次，将图标显示回来
		if(m_Data.hIcon == NULL)
			OnTimer(FLICKER_TIMER_ID);
		
		m_bFlickering = false;
	}
#endif
}
void TrayIcon::OnTimer(UINT_PTR nIDEvent)
{
#if 0
	if (nIDEvent == FLICKER_TIMER_ID)
	{
		if (m_Data.hIcon == NULL)
		{
			m_Data.hIcon = ::LoadIcon(::GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MAINFRAME));
			::DestroyIcon(m_Data.hIcon);
		}
		else
		{
			m_Data.hIcon = NULL;
		}
		Shell_NotifyIcon(NIM_MODIFY,&m_Data);
	}

	this->SetMsgHandled(FALSE);
#endif
}


///////////////////////////////////////////////////////////////////////////////////////
//
// 处理图标事件
//
BOOL TrayIcon::nvProcessMessage(UIMSG* pMsg, UINT nMsgID, bool)
{
	if (pMsg->message == m_Data.uCallbackMessage)
	{
		BOOL bHandled = TRUE;
		OnMsgTrayIcon(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
		return TRUE;
	}
	else if (pMsg->message == m_uTaskbarRestartMsg)
	{
		BOOL bHandled = TRUE;
		OnTaskbarRestart(pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);
		return bHandled;
	}
	return FALSE;
}

// The wParam parameter of the message contains the identifier 
// of the taskbar icon in which the event occurred. The lParam 
// parameter holds the mouse or keyboard message associated with 
// the event. For example, when the pointer moves over a taskbar 
// icon, lParam is set to WM_MOUSEMOVE. See the Taskbar guide 
// chapter for further discussion.
//
LRESULT TrayIcon::OnMsgTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_Data.uID != wParam)
	{
		bHandled = FALSE;
		return 0;
	}

	// wParam 为Add 拖盘图标时指定的 uid;
	HWND hWnd = m_Data.hWnd;
#if 0
	if (lParam == WM_CONTEXTMENU && this->m_uMenuID != NULL)
	{
		CMenu menu;
		if( FALSE == menu.LoadMenuW(m_uMenuID) )
			return S_FALSE;

		CPoint pt;
		::GetCursorPos(&pt);
		::SetForegroundWindow(hWnd);	// 如果不加这句话，不选择菜单项的话，菜单不会消失
		::TrackPopupMenu(menu.GetSubMenu(0),TPM_LEFTALIGN,pt.x,pt.y,0,hWnd,NULL);
		menu.DestroyMenu();
	}
	else 
#endif
	if( lParam == WM_LBUTTONDBLCLK  )
	{
		OnClickTrayIcon();
	}
#if 0
	else if( lParam == NIN_BALLOONSHOW )
	{
		//ATLTRACE("TrayIcon::   NIN_BALLOONSHOW\n");
		OnBalloonShow();
	}

	else if( lParam == NIN_BALLOONHIDE )
	{
		// This message is not sent if the balloon is dismissed 
		// because of a timeout or mouse click by the user.
		//ATLTRACE("TrayIcon::   NIN_BALLOONHIDE\n");
		OnBalloonHide();
	}

	else if( lParam == NIN_BALLOONTIMEOUT )
	{
		//ATLTRACE("TrayIcon::   NIN_BALLOONTIMEOUT\n");
		OnBalloonTimerOut();
	}

	else if( lParam == NIN_BALLOONUSERCLICK )
	{
		//ATLTRACE("TrayIcon::   NIN_BALLOONUSERCLICK\n");
		OnBalloonUserClick();
	}

	else if( lParam == NIN_KEYSELECT )
	{
		//ATLTRACE("TrayIcon::   NIN_KEYSELECT\n");
		OnClickTrayIcon();
	}

	else if( lParam == NIN_SELECT )
	{
		//ATLTRACE("TrayIcon::   NIN_SELECT\n");
		OnClickTrayIcon();
	}
#endif
	return S_OK;
}	

// 响应用户点击了拖盘图标事件，包括用键盘点击
void TrayIcon::OnClickTrayIcon()
{
	HWND hWnd = this->m_Data.hWnd;
	ShowWindow(hWnd, SW_SHOW);
	if(::IsIconic(hWnd))
	{
		//The OpenIcon function restores a minimized (iconic) window
		//to its previous size and position; it then activates the window. 
		OpenIcon(hWnd);
	}
	::SetForegroundWindow(hWnd);
}

}