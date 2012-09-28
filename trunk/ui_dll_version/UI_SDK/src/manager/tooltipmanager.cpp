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
			���#define _WIN32_WINNT 0x0500����ôtooltipһ������
			����һ��#define _WIN32_WINNT 0x0501�ˣ���ʱ����tooltip.cbSize = sizeof(TOOLINFO)��ʱ��Ͳ���ʾ�ˡ�
			������Բ���ʾ�κδ��󣬵���tooltip������ʾ��������
			��ΪϵͳĬ�ϼ���comctl 5.82������汾�����tooltip��size����û��sizeof(TOOLINFO),
			����û��void *lpReserved;����cbSize����ˣ������ˡ�
			��ʱ��Ҫд��tooltip.cbSize = TTTOOLINFOA_V2_SIZE;
			����ǿ��ָ��comctl 6.0
			#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			
			ָ��#define _WIN32_WINNT 0x0501�����³���ֻ��������xp������ϵͳ��
		*/
		m_toolinfo.cbSize = /*sizeof(TOOLINFO)*/ TTTOOLINFOA_V2_SIZE;
		m_toolinfo.uFlags = 0/* | TTF_TRACK | TTF_ABSOLUTE*/; // ע������TTF_TRACK|TTF_ABSOLUTE֮�󽫵�����ʾ��ʧȥ��ʾ����Ļ��Χ֮�ڵĹ���
		m_toolinfo.hwnd   = NULL;
		m_toolinfo.uId    = (UINT)0;
		m_toolinfo.hinst  = NULL;
		m_toolinfo.lpszText  = _T("leeihcy")/*LPSTR_TEXTCALLBACK*/;
		m_toolinfo.rect.left = m_toolinfo.rect.top = m_toolinfo.rect.bottom = m_toolinfo.rect.right = 0; 

		::SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)&m_toolinfo);
		::SendMessage(m_hToolTip, TTM_SETMAXTIPWIDTH, 0, TOOLTIP_MAX_WIDTH);   // ��ע���������������6.0�ؼ�һ��ʹ�õĻ���������һ���ܳ��ĵ���ʱ��������max width������ʾһ��(��win7����Ч)��
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
		// TODO: ����������Ϣ������⡢ͼ��ȣ������´ε�����ʱ������ʾ���ε�����
		return true;
	}
	virtual bool  SetAttribute(const ATTRMAP& mapAttrib)
	{
		return true;
	}
	
protected:
	//
	// ��� tooltipһ����������xp���治�ᰴ��max width�Զ����е�bug
	//
	void    FixStringWordBreakUnderXP(const String& src, String& strOut)
	{
		// ֻ��XP�´�����ַ���
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
		RECT  rcLimit = {0,0, TOOLTIP_MAX_WIDTH, 1};  // ���߶�����Ϊ1����֤DrawTextExֻ�����һ���ı����ַ���
		UINT  nDrawTextFlag = DT_EDITCONTROL|DT_WORDBREAK/*|DT_NOFULLWIDTHCHARBREAK*/;

		DRAWTEXTPARAMS  param;
		ZeroMemory(&param, sizeof(DRAWTEXTPARAMS));
		param.cbSize = sizeof(DRAWTEXTPARAMS);

		while(nStart < nLength)
		{
			// ����һ���ı��е��ַ���
			param.uiLengthDrawn = 0;
			DrawTextEx(hDC, (TCHAR*)(szText+nStart), nLength-nStart, &rcLimit, nDrawTextFlag, &param);

			strOut.append(szText+nStart, param.uiLengthDrawn);
			nStart += param.uiLengthDrawn;

			// �ֶ���ӻ��з�
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
		else  // ������listctrl��item��Ҫ��ʾtooltip���ɿؼ��Լ�ȥ������ʾ����Ϣ
		{
			if (0 == UISendMessage((Message*)m_tooltipItem.pNotifyObj, UI_WM_GET_TOOLTIPINFO, (WPARAM)&m_tooltipItem, (LPARAM)m_pToolTipUI))
				return;  // ʧ��
		}

		m_pToolTipUI->Show();
	}
}