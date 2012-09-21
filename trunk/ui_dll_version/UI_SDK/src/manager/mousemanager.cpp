#include "stdafx.h"
#define TOOL_TIMER_ID      ((UINT_PTR)this)
#define TOOL_TIMER_TIME    800
#define TOOLTIP_MAX_WIDTH  256

MouseManager::MouseManager()
{
	m_bMouseTrack      = TRUE;      // 默认需要进行鼠标监视

	m_pWindow          = NULL;
	m_pKeyboardManager = NULL;
	m_pObjPress        = NULL;
	m_pObjHover        = NULL;
	m_hToolTip         = NULL;
}

MouseManager::~MouseManager(void)
{
	m_bMouseTrack      = TRUE;

	m_pKeyboardManager = NULL;
	m_pWindow          = NULL;
	m_pObjPress        = NULL;
	m_pObjHover        = NULL;

	this->DestroyToolTip();
}


void MouseManager::SetWindow( WindowBase* pWnd )
{
	this->m_pWindow = pWnd;
}
void MouseManager::SetKeyboardManager( KeyboardManager* pKeyboardManager )
{
	this->m_pKeyboardManager = pKeyboardManager;
}

Object*  MouseManager::GetHoverObject()
{
	return m_pObjHover;
}

Object*  MouseManager::GetPressObject()
{
	return m_pObjPress;
}



//////////////////////////////////////////////////////////////////////////
//
//	提示条相关

VOID CALLBACK TooltipTimerProc( HWND hwnd,
						UINT uMsg,
						UINT_PTR idEvent,
						DWORD dwTime
						)
{
	MouseManager* pMgr = (MouseManager*)idEvent;
	KillTimer(hwnd, idEvent);

	Object* pObj = pMgr->GetHoverObject();
	if( NULL == pObj )
		return;

	if (0 == UISendMessage(pObj, UI_WM_SHOW_TOOLTIP, 0,0, 0, pMgr->GetWindowObject()))
	{
		String strAttrib;
		if( pObj->GetAttribute( XML_TOOLTIP, strAttrib ) )
		{
			String strTooltipText;
			pMgr->FixStringWordBreakUnderXP(strAttrib, strTooltipText);

			HWND hToolTip = pMgr->GetTooltipHWND();
			TOOLINFO* pToolInfo = pMgr->GetToolInfoPtr();

			pMgr->GetToolInfoPtr()->lpszText = (TCHAR*)strTooltipText.c_str();
			::SendMessage(hToolTip, TTM_UPDATETIPTEXTW, 0, (LPARAM)pToolInfo );

			POINT pt;
			::GetCursorPos(&pt);
			::SendMessage(hToolTip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y+22));
			::SendMessage(hToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)pToolInfo );
		}
	}
}

//
// 解决 tooltip一个长单词在xp下面不会按照max width自动分行的bug
//
void MouseManager::FixStringWordBreakUnderXP(const String& src, String& strOut)
{
	// 只在XP下处理该字符串
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO*) &osvi);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
	{
		if (osvi.dwMajorVersion >= 6)
		{
			strOut = src;
			return ;
		}
	}
	else
	{
		strOut = src;
		return ;
	}

	HFONT hFont = (HFONT)SendMessage(m_hToolTip, WM_GETFONT, 0,0);
	HDC   hDC   = UI_GetCacheDC();
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	const TCHAR* szText = src.c_str();

	int   nStart  = 0;
	int   nLength = src.length(); 
	RECT  rcLimit = {0,0, TOOLTIP_MAX_WIDTH, 1};  // 将高度设置为1，保证DrawTextEx只计算第一行文本的字符数
	UINT  nDrawTextFlag = DT_EDITCONTROL|DT_WORDBREAK/*|DT_NOFULLWIDTHCHARBREAK*/;

	DRAWTEXTPARAMS  param;
	ZeroMemory(&param, sizeof(DRAWTEXTPARAMS));
	param.cbSize = sizeof(DRAWTEXTPARAMS);

	while(nStart < nLength)
	{
		// 计算一行文本中的字符数
		param.uiLengthDrawn = 0;
		DrawTextEx(hDC, (TCHAR*)(szText+nStart), nLength-nStart, &rcLimit, nDrawTextFlag, &param);

		strOut.append(szText+nStart, param.uiLengthDrawn);
		nStart += param.uiLengthDrawn;

		// 手动添加换行符
		TCHAR cLast = szText[nStart-1];
		if (cLast != _T('\r') && cLast != _T('\n') && nStart < nLength)
		{
			strOut.append(_T("\n"));
		}
	}

	SelectObject(hDC, hOldFont);
	UI_ReleaseCacheDC(hDC);
}

void MouseManager::CreateToolTip()
{
	UIASSERT( m_hToolTip == NULL );

	m_hToolTip = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_pWindow->m_hWnd,
		NULL,
		NULL,
		NULL
		);

	::SetWindowPos(m_hToolTip,
		HWND_TOPMOST,
		0,
		0,
		0,
		0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	/*
		如果#define _WIN32_WINNT 0x0500，那么tooltip一切正常
		当有一天#define _WIN32_WINNT 0x0501了，这时设置tooltip.cbSize = sizeof(TOOLINFO)的时候就不显示了。
		编译调试不显示任何错误，但是tooltip就是显示不出来了
		因为系统默认加载comctl 5.82，这个版本里面的tooltip的size根本没有sizeof(TOOLINFO),
		里面没有void *lpReserved;所以cbSize设大了，出错了。
		这时需要写成tooltip.cbSize = TTTOOLINFOA_V2_SIZE;
		或者强制指定comctl 6.0
		#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
		
		指定#define _WIN32_WINNT 0x0501将导致程序只能运行在xp及以上系统。
	*/
	m_toolinfo.cbSize = /*sizeof(TOOLINFO)*/ TTTOOLINFOA_V2_SIZE;
	m_toolinfo.uFlags = TTF_IDISHWND/* | TTF_TRACK | TTF_ABSOLUTE*/; // 注：加上TTF_TRACK|TTF_ABSOLUTE之后将导致提示条失去显示在屏幕范围之内的功能
	m_toolinfo.hwnd   = m_pWindow->m_hWnd;
	m_toolinfo.uId    = (UINT)m_pWindow->m_hWnd;
	m_toolinfo.hinst  = g_pUIApplication->GetModuleInstance();
	m_toolinfo.lpszText  = _T("leeihcy")/*LPSTR_TEXTCALLBACK*/;
	m_toolinfo.rect.left = m_toolinfo.rect.top = m_toolinfo.rect.bottom = m_toolinfo.rect.right = 0; 

	::SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)&m_toolinfo);
	::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, TOOLTIP_MAX_WIDTH);   // 备注：该属性如果不和6.0控件一起使用的话，在碰到一个很长的单词时，将无视max width，仅显示一行(仅win7下有效)。
}

void MouseManager::HideToolTip()
{
	KillTimer(m_pWindow->m_hWnd, TOOL_TIMER_ID);
	::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&this->m_toolinfo );
}
void MouseManager::DestroyToolTip()
{
	if( NULL != m_hToolTip )
	{
		::DestroyWindow(m_hToolTip);
		m_hToolTip = NULL;
	}
}

void MouseManager::SetHoverObject( Object* pNewHoverObj )
{
	// 提示条相关逻辑
	if (NULL == m_hToolTip)
	{
		this->CreateToolTip();
	}
	else
	{
		HideToolTip();
	}

	if( pNewHoverObj != m_pObjHover && NULL != pNewHoverObj )
	{
		::SetTimer(m_pWindow->m_hWnd, TOOL_TIMER_ID, TOOL_TIMER_TIME, TooltipTimerProc );
	}

	// 状态变化逻辑
	if( NULL != m_pObjHover && m_pObjHover != pNewHoverObj )
	{
		int nOldStateBits = m_pObjHover->GetStateBit();
		m_pObjHover->SetHover(false);
		::UISendMessage(m_pObjHover, UI_WM_STATECHANGED, nOldStateBits, m_pObjHover->GetStateBit() );
	}
	m_pObjHover = pNewHoverObj;
	if( NULL != m_pObjHover )
	{
		int nOldStateBits = m_pObjHover->GetStateBit();
		m_pObjHover->SetHover(true);
		::UISendMessage(m_pObjHover, UI_WM_STATECHANGED, nOldStateBits, m_pObjHover->GetStateBit() );
	}
}
void MouseManager::SetPressObject( Object* pNewPressObj )
{
	// 状态变化逻辑
	if( NULL != m_pObjPress && m_pObjPress != pNewPressObj )
	{
		int nOldStateBits = m_pObjPress->GetStateBit();
		m_pObjPress->SetPress(false);
		::UISendMessage(m_pObjPress, UI_WM_STATECHANGED, nOldStateBits, m_pObjPress->GetStateBit()  );
	}
	m_pObjPress = pNewPressObj;
	if( NULL != m_pObjPress )
	{
		int nOldStateBits = m_pObjPress->GetStateBit();
		m_pObjPress->SetPress(true);
		::UISendMessage(m_pObjPress, UI_WM_STATECHANGED, nOldStateBits, m_pObjPress->GetStateBit()  );
	}

	// 提示条相关逻辑
	HideToolTip();
}


//
//The low-order word specifies the x-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//The high-order word specifies the y-coordinate of the cursor. The coordinate is relative to the upper-left corner of the client area. 
//
LRESULT MouseManager::HandleMessage( UINT msg, WPARAM w, LPARAM l )
{
	int vkFlag = (int)w;
	int xPos = GET_X_LPARAM ( l );
	int yPos = GET_Y_LPARAM ( l );

	switch( msg )
	{
	case WM_MOUSEMOVE:
		{
			if (m_bMouseTrack)    //若允许追踪，则。
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof( TRACKMOUSEEVENT );
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = HOVER_DEFAULT;
				tme.hwndTrack = this->m_pWindow->m_hWnd;
				::TrackMouseEvent( &tme );
				m_bMouseTrack = FALSE;
			}

			Object* pOldHover = m_pObjHover;
			Object* pOldPress = m_pObjPress;
			LRESULT lRet = this->MouseMove( vkFlag, xPos, yPos );

			if (pOldPress==m_pObjPress && pOldHover!=m_pObjHover)  // 鼠标下的对象发生变化，需要重置鼠标样式
			{
				::PostMessage(m_pWindow->m_hWnd, WM_SETCURSOR, NULL, MAKELPARAM(0,1));  // hiword 0表示弹出菜单
			}
			return lRet;
		}
		break;

	case WM_MOUSELEAVE:
		return this->MouseLeave( vkFlag, xPos, yPos );
		break;

	case WM_LBUTTONDOWN:
		return this->LButtonDown( vkFlag, xPos, yPos );
		break;

	case WM_LBUTTONUP:
		return this->LButtonUp( vkFlag, xPos, yPos );
		break;

	case WM_RBUTTONDOWN:
		return this->RButtonDown( w,l );
		break;

	case WM_RBUTTONUP:
		return this->RButtonUp( w,l );
		break;

	case WM_LBUTTONDBLCLK:
		this->LButtonDown( vkFlag, xPos, yPos );
		return this->LButtonDBClick( w,l );
		break;

	case WM_KILLFOCUS:
		this->KillFocus();
		break;

	case WM_SETFOCUS:
		this->SetFocus();
		break;
		 
	case WM_SETCURSOR:
		return this->SetCursor( w, l );
		break;

	case WM_NCDESTROY:
		this->NcDestroy();
		break;
	}
	return 0L;
}

/*
	[WARN] three months ago, there are 2 person can understande this code,
	but now noly the god known what it is!
*/
//
//	Parameter
//		vkFlag
//			[in]
//
//		xPos, yPos
//			[in]	当前鼠标位置，以窗口左上角为(0,0)起点
//
LRESULT MouseManager::MouseMove( int vkFlag, int xPos, int yPos )
{	
	// 1. 判断当前鼠标位置
	POINT pt = { xPos, yPos };

	Object* pObj = this->GetObjectByPos( this->m_pWindow, &pt );
	if (NULL == this->m_pObjPress)
	{
		if (NULL == this->m_pObjHover)
		{
			if (pObj == this->m_pObjHover)   // 鼠标还是在外面移动
			{
			}
			else                              // 鼠标移动到了pObj上
			{
				if( vkFlag & MK_LBUTTON )
				{
					                          // 为了避免出现在自定义窗口拖拽时出错，添加这个条件限制
				}
				else
				{
					this->SetHoverObject( pObj );
					::UISendMessage( m_pObjHover, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
				}
			}
		}
		else
		{
			if (pObj == this->m_pObjHover)   // 鼠标在A上面移动
			{
				::UISendMessage( m_pObjHover, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
			}
			else                              // 鼠标刚才放在A上面，但现在又移出去了，可能移到B上面，也可能没有移动任何对象上
			{
				::UISendMessage( m_pObjHover, WM_MOUSELEAVE, (WPARAM) 0, 0);

				// this->m_pObjHover = pObj;
				this->SetHoverObject(pObj);
				if( NULL != pObj )
				{
					::UISendMessage( pObj, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
				}
			}
		}

	}
	else
	{
		if (NULL == this->m_pObjHover)
		{
			if (pObj == m_pObjPress)  // 鼠标按在了A对象上，但鼠标在A外面。现在又移动回来了
			{
				//m_pObjHover = m_pObjPress;
				this->SetHoverObject(m_pObjPress);

				::UISendMessage( m_pObjPress, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );

			}	
			else                    // 鼠标在A对象上按下了，但鼠标现在不再A上面
			{
				::UISendMessage( m_pObjPress, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
			}
		}
		else
		{
			if (pObj == m_pObjPress)  // 鼠标按在了A对象上，现在在A上移动
			{
				UIASSERT(m_pObjPress == m_pObjHover);
				UIASSERT(m_pObjHover == pObj);

				::UISendMessage( m_pObjPress, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
			}
			else  // 鼠标刚才按在了A对象上，但现在鼠标移出来了。
			{
				UIASSERT(m_pObjPress==m_pObjHover);	

				// m_pObjHover = NULL;
				this->SetHoverObject(NULL);
				::UISendMessage( m_pObjPress, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( xPos, yPos ) );
			}
		}
	}

	return 0L;
}


LRESULT MouseManager::MouseLeave( int vkFlag, int xPos, int yPos )
{
	// 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
	Object*   pSaveObjPress = m_pObjPress;
	Object*   pSaveObjHover = m_pObjHover;

	if( m_pObjHover != NULL )
	{
		UIMSG   msg;
		msg.pObjMsgTo = m_pObjHover;
		msg.message   = WM_MOUSELEAVE;
		msg.wParam    = vkFlag;
		msg.lParam    = MAKELPARAM( xPos, yPos );
		::UISendMessage( &msg );
	}

	if( pSaveObjPress != NULL && pSaveObjPress != pSaveObjHover )
	{
		::UISendMessage( pSaveObjPress, WM_MOUSELEAVE );
	}

	this->m_bMouseTrack = TRUE;// 继续开启TRACKMOUSEEVENT

	HideToolTip();

	this->SetHoverObject(NULL);
	this->SetPressObject(NULL);
	if (GetCapture() == m_pWindow->m_hWnd)  // 有可能是其它对象直接发送过来WM_MOUSELEAVE，例如COMBOBOX.button.onlbuttondown
	{
		::ReleaseCapture();
	}
	return TRUE;
}


LRESULT MouseManager::LButtonDown( int vkFlag, int xPos, int yPos )
{
	if (NULL != this->m_pObjHover)
	{
		::SetCapture( this->m_pWindow->m_hWnd );
		this->SetPressObject(m_pObjHover);

		Object* pObjSave = m_pObjHover;

		UIMSG  msg;
		msg.pObjMsgTo = pObjSave;
		msg.message   = WM_LBUTTONDOWN;
		msg.wParam    = (WPARAM)vkFlag;
		msg.lParam    = MAKELPARAM(xPos,yPos);
		::UISendMessage( &msg );      // 有可能导致m_pObjPress为NULL了

		if(NULL != m_pObjPress && this->m_pObjPress->GetObjectType() == OBJ_CONTROL )  
		{
			if( this->m_pObjPress->IsTabstop() )
			{
				this->m_pKeyboardManager->SetFocusObject( this->m_pObjPress );
			}
		}
	}
	return 0L;
}

LRESULT MouseManager::LButtonUp( int vkFlag, int xPos, int yPos )
{
	if( this->m_pObjPress != NULL )
	{
		// 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
		Object*   pSaveObjPress = m_pObjPress;
		Object*   pSaveObjHover = m_pObjHover;

		UIMSG  msg;
		msg.pObjMsgTo = m_pObjPress;
		msg.message   = WM_LBUTTONUP;
		msg.wParam    = (WPARAM)vkFlag;
		msg.lParam    = MAKELPARAM(xPos,yPos);
		
		::UISendMessage( &msg );

		// 如果这个时候鼠标离开了控件，发送MOUSELEAVE消息. 这个时候鼠标位置可能已经重新变化了，需要重新获取
		POINT pt = {0,0};
		::GetCursorPos(&pt);
		::MapWindowPoints(NULL, this->m_pWindow->m_hWnd, &pt, 1 );
		Object* pNowHover = this->GetObjectByPos( this->m_pWindow, &pt );
		//this->m_pObjPress = NULL;
		this->SetPressObject(NULL);
		//this->m_pObjHover = pNowHover;
		this->SetHoverObject(pNowHover);

		if( pNowHover != pSaveObjPress && NULL != pSaveObjPress )
		{
			::UISendMessage( pSaveObjPress, WM_MOUSELEAVE );
		}
		if( pNowHover != pSaveObjHover && NULL != pNowHover )
		{
			::UISendMessage( pNowHover, WM_MOUSEMOVE, (WPARAM) vkFlag, MAKELPARAM( pt.x, pt.y ) );
		}

		if (GetCapture() == m_pWindow->m_hWnd) 
		{
			::ReleaseCapture();
		}
	}
	 
	return 0L;
}

LRESULT MouseManager::LButtonDBClick( WPARAM w,LPARAM l )
{
	if( NULL != m_pObjPress )
	{
		return ::UISendMessage( m_pObjPress, WM_LBUTTONDBLCLK, w, l );
	}
	else if( NULL != m_pObjHover )
	{
		return ::UISendMessage( m_pObjHover, WM_LBUTTONDBLCLK, w, l );
	}
	return 0;
}

LRESULT MouseManager::RButtonDown( WPARAM w,LPARAM l )
{
	if( NULL != m_pObjPress )
	{
		return ::UISendMessage( m_pObjPress, WM_RBUTTONDOWN, w, l );
	}
	else if( NULL != m_pObjHover )
	{
		return ::UISendMessage( m_pObjHover, WM_RBUTTONDOWN, w, l );
	}
	return 0;
}
LRESULT MouseManager::RButtonUp( WPARAM w,LPARAM l )
{
	if( NULL != m_pObjPress )
	{
		return ::UISendMessage( m_pObjPress, WM_RBUTTONUP, w, l );
	}
	else if( NULL != m_pObjHover )
	{
		return ::UISendMessage( m_pObjHover, WM_RBUTTONUP, w, l );
	}
	return 0;
}

void MouseManager::KillFocus()
{
	if( this->m_pObjHover != NULL )
	{
		::UISendMessage( m_pObjHover, WM_MOUSELEAVE );
	}
	if( this->m_pObjPress != NULL )
	{
		if (GetCapture() == m_pWindow->m_hWnd) 
		{
			::ReleaseCapture();
		}

		if( this->m_pObjPress !=this->m_pObjHover )  // 避免将WM_MOUSELEAVE发送两次
			::UISendMessage( m_pObjPress, WM_MOUSELEAVE );
	}
// 	this->m_pObjHover = NULL;
// 	this->m_pObjPress = NULL;
	this->SetHoverObject(NULL);
	this->SetPressObject(NULL);
}

void MouseManager::SetFocus()
{
// 	POINT   pt;
// 	::GetCursorPos( &pt );
// 	::MapWindowPoints( NULL, m_pWindow->m_hWnd, &pt, 1 );
// 	this->MouseMove( 0, pt.x, pt.y );
}

BOOL MouseManager::SetCursor( WPARAM w,LPARAM l )
{
	if( NULL != m_pObjPress )
	{
		return ::UISendMessage( m_pObjPress, WM_SETCURSOR, w, l );
	}
	else if( NULL != m_pObjHover )
	{
		return ::UISendMessage( m_pObjHover, WM_SETCURSOR, w, l );
	}

	return FALSE;
}

void MouseManager::NcDestroy()
{
	m_pObjHover = m_pObjPress = NULL;  // 不调用SetHoverObject/SetPressObject
}

void MouseManager::OnObjectHideInd(Object* pObj)
{
	this->OnObjectDeleteInd(pObj);
}
void MouseManager::OnObjectDeleteInd(Object* pObj)
{
	if (NULL == pObj)
		return;
	if (m_pObjHover == pObj)
	{
		m_pObjHover = NULL;
	}
	if (m_pObjPress == pObj)
	{
		m_pObjPress = NULL;
	}
}

//
//	[递归+遍历] [public] [static] 
//
//	获取当前鼠标下的最小单位控件，即：控件在该位置上并且其pChild为null
//
//	Parameter
//
//		pObjParent
//			[in]
//				递归遍历时，使用的parent object.调用该函数时赋值为窗口对象(Window/)pObj即可。
//		pt
//			[in]
//				在父对象中的鼠标位置，初始值为在窗口中的位置
//
//	Return
//		该位置下的对象，没有则返回空
//
Object* MouseManager::GetObjectByPos(Object* pObjParent, POINT* pt)
{
	Object* pHitObj = NULL;
	POINT   ptHit = *pt;

	Object* pChild = NULL;
	while (pChild = pObjParent->EnumNcChildObject(pChild))
	{
		if (!pChild->IsVisible())
			continue;
		if (!pChild->IsEnable())
			continue;

		UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
		if( HTNOWHERE != nHitTest )
		{
			pHitObj = pChild;
			break;
		}
	}

	if (NULL == pHitObj)  // 继续搜索客户区的子对象
	{
		ptHit.x -= pObjParent->GetNonClientL();
		ptHit.y -= pObjParent->GetNonClientT();

		POINT ptOffset = {0,0};
		if (pObjParent->GetScrollOffset((int*)&ptOffset.x, (int*)&ptOffset.y))
		{
			ptHit.x += ptOffset.x;
			ptHit.y += ptOffset.y;
		}

		pChild = NULL;
		while (pChild = pObjParent->EnumChildObject(pChild))
		{
			if (!pChild->IsVisible())
				continue;
			if (!pChild->IsEnable())
				continue;

			UINT nHitTest = UISendMessage(pChild, UI_WM_HITTEST, ptHit.x, ptHit.y);
			if( HTNOWHERE != nHitTest )
			{
				pHitObj = pChild;
				break;
			}
		}
	}

	if (NULL == pHitObj)
		return NULL;

	// 将坐标转换为相对于pHitObj的坐标
	ptHit.x -= pHitObj->GetParentRectL();
	ptHit.y -= pHitObj->GetParentRectT();
	Object* pRetObj = GetObjectByPos(pHitObj, &ptHit);

	// 若子控件中没有处于该位置的，直接返回本对象
	if( NULL == pRetObj )
		return pHitObj;
	else
		return pRetObj;
}
