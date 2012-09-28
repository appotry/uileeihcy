#include "stdafx.h"


class CSystemTooltip : public IToolTipUI
{
public:
	CSystemTooltip() 
	{
		m_hToolTip = NULL;
		memset(&m_toolinfo, 0, sizeof(TOOLINFO));
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

protected:
	HWND       m_hToolTip;
	TOOLINFO   m_toolinfo;
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
	m_pToolTipUI = new CSystemTooltip;
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