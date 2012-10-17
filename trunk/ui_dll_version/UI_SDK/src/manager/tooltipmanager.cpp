#include "stdafx.h"

class ThemeTooltip : public CustomWindow, public IToolTipUI
{
public:
	ThemeTooltip()
	{
		m_nLimitWidth = -1;
	}
	virtual ~ThemeTooltip()
	{
		this->Destroy();
	}

	UI_DECLARE_OBJECT(ThemeTooltip, OBJ_WINDOW)
 	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

	virtual BOOL PreCreateWindow( CREATESTRUCT& cs )
	{
		CustomWindow::PreCreateWindow(cs);
		cs.lpszClass = WND_POPUP_CONTROL_SHADOW_NAME;  // 带阴暗
		cs.dwExStyle |= WS_EX_TOPMOST|WS_EX_NOACTIVATE;
		return TRUE;
	}

	virtual   bool SetAttribute( map<String,String>& mapAttrib, bool bReload)
	{
		bool bRet = __super::SetAttribute(mapAttrib, bReload);
		if (false == bRet)
			return false;

		if (NULL == m_pBkgndRender)
		{
			m_pBkgndRender = RenderFactory::GetRender(RENDER_TYPE_THEME, this);
		}

		return true;
	}

	void  OnPaint(HRDC hRDC)
	{
		CRect rc;
		this->GetClientRectAsWin32(&rc);

	//	rc.right +=2;
		rc.bottom +=2;
		m_pTextRender->DrawState(hRDC, &rc, 0, m_strText);
	}
	virtual bool  Create()
	{
		if (NULL != m_hWnd)
			return false;

		CustomWindow::Create(_T(""),NULL);

		this->SetWindowLayered(true);

		ATTRMAP map;
		this->SetAttribute(map, false);  // 创建一些默认属性

		CRegion4 b(1,1,1,1);
		this->SetBorderRegion(&b);
		CRegion4 r(4,4,4,4);
		this->SetPaddingRegion(&r);      // 文字与边缘的间距

		this->SetWindowResizeType(WRSB_NONE);  // 禁止拖拽
		
		
		return true;
	}
	virtual bool  Destroy() 
	{
		if (NULL == m_hWnd)
			return false;

		::DestroyWindow(m_hWnd);
		return true;
	}
	virtual bool  SetText(const String& strText)
	{
		if (NULL == m_hWnd)
		{
			Create();
		}
		m_strText = strText;
		
		// 计算窗口的新大小
		SIZE s = ::MeasureString(this->GetFont(), strText.c_str(), m_nLimitWidth);
		s.cx += this->GetNonClientW();
		s.cy += this->GetNonClientH();

		this->SetObjectPos(0,0, s.cx,s.cy, SWP_NOMOVE);
		return true;
	}
	virtual bool  SetTitle(const String& strText)
	{
		if (NULL == m_hWnd)
		{
			Create();
		}
		return false;
	}
	virtual bool  Show()
	{
		POINT pt;
		::GetCursorPos(&pt);
		this->SetObjectPos(pt.x, pt.y+22, 0,0, SWP_NOSIZE);

		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		return true;
	}
	virtual bool  Hide() 
	{
		this->HideWindow();
		return true;
	}
	virtual bool  SetAttribute(const ATTRMAP& mapAttrib) 
	{
		return 0;
	}

private:
	String   m_strText;
	int      m_nLimitWidth;
};

class CSystemTooltip : public IToolTipUI
{
public:
	CSystemTooltip() 
	{
		m_hToolTip = NULL;
		memset(&m_toolinfo, 0, sizeof(TOOLINFO));

		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((OSVERSIONINFO*) &osvi);

		m_bUnderXpOs = true;;
		if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
		{
			if (osvi.dwMajorVersion >= 6)
			{
				m_bUnderXpOs = false;
			}
		}
		else
		{
			m_bUnderXpOs = false;
		}
	}
	~CSystemTooltip()
	{
		this->Destroy();
	}

	virtual bool  Create()
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
						NULL,
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
		m_toolinfo.uFlags = 0/* | TTF_TRACK | TTF_ABSOLUTE*/; // 注：加上TTF_TRACK|TTF_ABSOLUTE之后将导致提示条失去显示在屏幕范围之内的功能
		m_toolinfo.hwnd   = NULL;
		m_toolinfo.uId    = (UINT)0;
		m_toolinfo.hinst  = NULL;
		m_toolinfo.lpszText  = _T("leeihcy")/*LPSTR_TEXTCALLBACK*/;
		m_toolinfo.rect.left = m_toolinfo.rect.top = m_toolinfo.rect.bottom = m_toolinfo.rect.right = 0; 

		::SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)&m_toolinfo);
		::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, TOOLTIP_MAX_WIDTH);   // 备注：该属性如果不和6.0控件一起使用的话，在碰到一个很长的单词时，将无视max width，仅显示一行(仅win7下有效)。

		// Deleted. 该设置仅对默认情况下的宋体字体有效，其它字体没有作用。
		// 因此决定自己重新实现一个提示条。摆脱系统的限制！

		// 解决非6.0控件时，提示条内容不居中的问题。（测试发现在6.0控件下无视margin的值，但正好6.0就是居中的，可以不管）
		// 英文操作系统下面是OK的，不用修改。
		// win7和xp下面需要调整的参数不一致。
// 		if (936 == GetACP())
// 		{
// 			if (m_bUnderXpOs)
// 			{
// 				RECT rc = {1,3,0,0};
// 				::SendMessage(this->m_hToolTip, TTM_SETMARGIN, 0, (LPARAM)&rc);
// 			}
// 			else
// 			{
// 				RECT rc = {2,3,0,0};
// 				::SendMessage(this->m_hToolTip, TTM_SETMARGIN, 0, (LPARAM)&rc);
// 			}
// 		}

#if 0
		const TCHAR* szFontFace =  /*_T("宋体");*/_T("微软雅黑");
		// 设置字体为 微软雅黑
		if ( Util::IsFontExist(szFontFace) )
		{
			m_hFont = ::CreateFont(
				14,//18,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				szFontFace);               // lpszFacename

			if (NULL != m_hFont)
			{
				::SendMessage(m_hToolTip, WM_SETFONT, (WPARAM)m_hFont, 0);

				RECT rcMargin;
				::SendMessage(m_hToolTip, TTM_GETMARGIN, 0, (LPARAM)(LPRECT)&rcMargin);
				int a= 0;
			}
		}
#endif
		return true;
	}
	virtual bool  Destroy()
	{
		if( NULL != m_hToolTip )
		{
			::DestroyWindow(m_hToolTip);
			m_hToolTip = NULL;
		}
		return true;
	}
	virtual bool  SetText(const String& strText)
	{
		if (NULL == m_hToolTip)
		{
			Create();
		}

		String strTooltipText;
		FixStringWordBreakUnderXP(strText, strTooltipText);

		m_toolinfo.lpszText = (TCHAR*)strTooltipText.c_str();
		::SendMessage(m_hToolTip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&m_toolinfo );
		return true;
	}
	virtual bool  SetTitle(const String& strText) 
	{
		if (NULL == m_hToolTip)
		{
			Create(); 
		}
		return true;
	}
	virtual bool  Show()
	{
		if (NULL == m_hToolTip)
		{
			Create();
		}

		POINT pt;
		::GetCursorPos(&pt);
		::SendMessage(m_hToolTip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y+22));
		::SendMessage(m_hToolTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_toolinfo );
		return true;
	}
	virtual bool  Hide()
	{
		::SendMessage(this->m_hToolTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&this->m_toolinfo );
		// TODO: 清理其它信息，如标题、图标等，避免下次弹出来时，还显示本次的数据
		return true;
	}
	virtual bool  SetAttribute(const ATTRMAP& mapAttrib)
	{
		return true;
	}
	
protected:
	//
	// 解决 tooltip一个长单词在xp下面不会按照max width自动分行的bug
	//
	void    FixStringWordBreakUnderXP(const String& src, String& strOut)
	{
		// 只在XP下处理该字符串
		if (false == m_bUnderXpOs)
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

protected:
	HWND       m_hToolTip;
	TOOLINFO   m_toolinfo;
	bool       m_bUnderXpOs;  // 是否是XP系统。区别于WIN7，两者对于tooltip的显示有些区别
};

//////////////////////////////////////////////////////////////////////////

ToolTipManager::ToolTipManager()
{
	m_pToolTipUI = NULL;
	m_nTimerWait2Show = 0;

	Init();
}
ToolTipManager::~ToolTipManager()
{
	SAFE_DELETE(m_pToolTipUI);
}

void ToolTipManager::Init()
{
//	m_pToolTipUI = new CSystemTooltip;
	m_pToolTipUI = new ThemeTooltip;
}

bool ToolTipManager::Show(TOOLTIPITEM* pItemInfo)
{
	if (NULL == m_pToolTipUI || NULL == pItemInfo)
		return false;

	Hide();
	m_tooltipItem = *pItemInfo;

	switch (pItemInfo->nToolTipFlag)
	{
	case TOOLTIP_ACTION_FLAG_NORMAL:
		{
			m_nTimerWait2Show = ::SetTimer(NULL, 0, TOOL_TIMER_TIME, ToolTipManager::ToolTipManagerTimerProc );
		}
		break;

	case TOOLTIP_ACTION_FLAG_CAN_MOUSE_HOVER:
		break;

	case TOOLTIP_ACTION_FLAG_INPLACE:
		break;

	case TOOLTIP_ACTION_FLAG_MESSAGEBOX:
		break;
	}
	return true;
}

bool ToolTipManager::Hide()
{
	KillTimer(NULL, m_nTimerWait2Show);
	if (NULL != m_pToolTipUI)
	{
		m_pToolTipUI->Hide();
	}
	memset(&m_tooltipItem, 0, sizeof(TOOLTIPITEM));
	return true;
}

VOID CALLBACK ToolTipManager::ToolTipManagerTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	ToolTipManager* pThis = UI_GetToolTipMgr();
	if (NULL != pThis)
	{
		pThis->OnTimer(hwnd, uMsg, idEvent, dwTime);
	}
}

void ToolTipManager::OnTimer( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	KillTimer(hwnd, idEvent);

	if (NULL == m_pToolTipUI || NULL == m_tooltipItem.pNotifyObj)
		return;

	if (0 == UISendMessage((Message*)m_tooltipItem.pNotifyObj, UI_WM_SHOW_TOOLTIP))
	{
		if (NULL == m_tooltipItem.pItemData)
		{
			String strAttrib;
			if (false == m_tooltipItem.pNotifyObj->GetAttribute( XML_TOOLTIP, strAttrib ))
				return;

			if (0 == strAttrib.length())
				return;

			m_pToolTipUI->SetText(strAttrib);

		}
		else  // 可能是listctrl的item需要显示tooltip，由控件自己去设置提示条信息
		{
			if (0 == UISendMessage((Message*)m_tooltipItem.pNotifyObj, UI_WM_GET_TOOLTIPINFO, (WPARAM)&m_tooltipItem, (LPARAM)m_pToolTipUI))
				return;  // 失败
		}

		m_pToolTipUI->Show();
	}
}