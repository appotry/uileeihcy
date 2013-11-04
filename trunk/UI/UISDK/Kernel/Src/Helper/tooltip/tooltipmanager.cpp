#include "stdafx.h"
#include "tooltipmanager.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"

#pragma region //CSystemTooltip
class CSystemTooltip : public IToolTipUI
{
public:
	CSystemTooltip() 
	{
		m_hToolTip = NULL;
		memset(&m_toolinfo, 0, sizeof(TOOLINFO));

		m_pUIApp = NULL;
	}
	~CSystemTooltip()
	{
		SAFE_RELEASE(m_pUIApp);
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

		// Deleted. �����ý���Ĭ������µ�����������Ч����������û�����á�
		// ��˾����Լ�����ʵ��һ����ʾ��������ϵͳ�����ƣ�

		// �����6.0�ؼ�ʱ����ʾ�����ݲ����е����⡣�����Է�����6.0�ؼ�������margin��ֵ��������6.0���Ǿ��еģ����Բ��ܣ�
		// Ӣ�Ĳ���ϵͳ������OK�ģ������޸ġ�
		// win7��xp������Ҫ�����Ĳ�����һ�¡�
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
		const TCHAR* szFontFace =  /*_T("����");*/_T("΢���ź�");
		// ��������Ϊ ΢���ź�
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

			if (m_hFont)
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
		delete this;
		return true;
	}
	virtual bool  SetText(const TCHAR* szText)
	{
		if (NULL == m_hToolTip)
		{
			Create();
		}

		String strTooltipText;
		FixStringWordBreakUnderXP(szText, strTooltipText);

		m_toolinfo.lpszText = (TCHAR*)strTooltipText.c_str();
		::SendMessage(m_hToolTip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&m_toolinfo );
		return true;
	}
	virtual bool  SetTitle(const TCHAR* szText) 
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
	virtual void  SetAttribute(const ATTRMAP& mapAttrib)
	{
	}
	
	virtual bool  SetUIApplication(IUIApplication* p)
	{
		SAFE_RELEASE(m_pUIApp);
		m_pUIApp = p;
		
		if (m_pUIApp)
			m_pUIApp->AddRef();

		return true;

	}
protected:
	//
	// ��� tooltipһ����������xp���治�ᰴ��max width�Զ����е�bug
	//
	void    FixStringWordBreakUnderXP(const String& src, String& strOut)
	{
		// ֻ��XP�´�����ַ���
		if (m_pUIApp && false == m_pUIApp->IsUnderXpOS())
		{
			strOut = src;
			return ;
		}

		HFONT hFont = (HFONT)SendMessage(m_hToolTip, UI_WM_GETRENDERFONT, 0,0);
		HDC   hDC   = Image::GetCacheDC();
		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
		const TCHAR* szText = src.c_str();

		int   nStart  = 0;
		int   nLength = src.length(); 
		RECT  rcLimit = {0,0, TOOLTIP_MAX_WIDTH, 1};  // ���߶�����Ϊ1����֤DrawTextExֻ�����һ���ı����ַ���
		UINT  nDrawTextFlag = DT_EDITCONTROL|DT_WORDBREAK/*|DT_NOFULLWIDTHCHARBREAK*/;

		DRAWTEXTPARAMS  param;
		ZeroMemory(&param, sizeof(DRAWTEXTPARAMS));
		param.cbSize = sizeof(DRAWTEXTPARAMS);

		while (nStart < nLength)
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
		Image::ReleaseCacheDC(hDC);
	}

protected:
	HWND       m_hToolTip;
	TOOLINFO   m_toolinfo;
	bool       m_bUnderXpOs;  // �Ƿ���XPϵͳ��������WIN7�����߶���tooltip����ʾ��Щ����
	IUIApplication*  m_pUIApp;
};
#pragma endregion
//////////////////////////////////////////////////////////////////////////

ToolTipManager::ToolTipManager(UIApplication* p)
{
	m_pToolTipUI = NULL;
	m_nTimerWait2Show = 0;
    m_pUIApplication = p;

	Init();
}
ToolTipManager::~ToolTipManager()
{
	this->Release();
}

void ToolTipManager::Init(IToolTipUI* pTooltipUI)
{
	if (m_pToolTipUI)
	{
		m_pToolTipUI->Destroy();
	}

	if (NULL == pTooltipUI)
		m_pToolTipUI = new CSystemTooltip;	
	else
		m_pToolTipUI = pTooltipUI;

	m_thunk.Init(ToolTipManager::ToolTipManagerTimerProc, this);
}

void ToolTipManager::Release()
{
	if (m_pToolTipUI)
	{
		m_pToolTipUI->Destroy();
		m_pToolTipUI = NULL;
	}
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
			m_nTimerWait2Show = ::SetTimer(NULL, 0, TOOL_TIMER_TIME, /*ToolTipManager::ToolTipManagerTimerProc*/m_thunk.GetTIMERPROC() );
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
    m_nTimerWait2Show = 0;

	if (m_pToolTipUI)
	{
		m_pToolTipUI->Hide();
	}
	memset(&m_tooltipItem, 0, sizeof(TOOLTIPITEM));
	return true;
}

VOID CALLBACK ToolTipManager::ToolTipManagerTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
//	ToolTipManager* pThis = UI_GetToolTipMgr();
	ToolTipManager* pThis = (ToolTipManager*)hwnd;  // ��thunk�޸Ĺ���

	if (pThis)
	{
		pThis->OnTimer(/*hwnd*/NULL, uMsg, idEvent, dwTime);
	}
}

void ToolTipManager::OnTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);

	if (NULL == m_pToolTipUI || NULL == m_tooltipItem.pNotifyObj)
		return;

    // �п��ܶ���ɾ����
    if (!m_pUIApplication->IsUIObjectAvailable(m_tooltipItem.pNotifyObj))
        return;
    
	if (0 == UISendMessage(m_tooltipItem.pNotifyObj, UI_WM_SHOW_TOOLTIP))
	{
		if (NULL == m_tooltipItem.pItemData)
		{
			const TCHAR* szAttrib = m_tooltipItem.pNotifyObj->GetAttribute(XML_TOOLTIP, false);
			if (false == szAttrib)
				return;

			if (0 == _tcslen(szAttrib))
				return;

			m_pToolTipUI->SetText(szAttrib);

		}
		else  // ������listctrl��item��Ҫ��ʾtooltip���ɿؼ��Լ�ȥ������ʾ����Ϣ
		{
			if (0 == UISendMessage(m_tooltipItem.pNotifyObj, UI_WM_GET_TOOLTIPINFO, (WPARAM)&m_tooltipItem, (LPARAM)m_pToolTipUI))
				return;  // ʧ��
		}

		m_pToolTipUI->Show();
	}
}