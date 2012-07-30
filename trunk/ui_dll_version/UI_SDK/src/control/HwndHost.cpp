#include "stdafx.h"

HwndHost::HwndHost()
{
	m_hWnd = NULL;
	m_oldWndProc  = NULL;
}
HwndHost::~HwndHost()
{
	m_hWnd = NULL;
	UIASSERT ( NULL == m_oldWndProc );
}

// 
//	子类化窗口，替换它的窗口过程
//
//	Return
//		成功返回false，失败返回false
//
bool HwndHost::SubclassWindow()
{
	UIASSERT( m_hWnd != NULL );

	this->m_thunk.Init( &HwndHost::_WndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();
	this->m_oldWndProc = (WNDPROC)(LONG_PTR) ::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);

	this->OnSubclassWindow();

	return true;
}

// 
//	取消子类化窗口，恢复它的窗口过程
//
//	Return
//		成功返回false，失败返回false
//
bool HwndHost::UnSubclassWindow()
{
	::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
	m_oldWndProc = NULL;

	this->OnUnsubclassWindow();
	return true;
}

void HwndHost::OnSubclassWindow()
{
}
void HwndHost::OnUnsubclassWindow()
{
}

bool HwndHost::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Object::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;
	

	HWND hParentWnd = this->GetHWND();
	if( mapAttrib.count(XML_HWNDHOST_CONTROL_ID) )
	{
		int nValue = _ttoi( mapAttrib[XML_HWNDHOST_CONTROL_ID].c_str() );
		m_hWnd = ::GetDlgItem(hParentWnd, nValue);
		if( NULL == m_hWnd )
		{
			UI_LOG_WARN(_T("HwndHost::SetAttribute Failed. id=%s, GetDlgItem(%d)"), m_strID.c_str(), nValue );
			return false;
		}
		
		this->SubclassWindow();
		m_mapAttribute.erase(XML_HWNDHOST_CONTROL_ID);
	}

	return true;
}

SIZE HwndHost::GetDesiredSize( HRDC hDC )
{
	SIZE s = {0,0};

	if( NULL != m_hWnd )
	{
		CRect rc;
		::GetWindowRect(m_hWnd, &rc);
		s.cx = rc.Width();
		s.cy = rc.Height();
	}

	return s;
}

//
//	[static] LRESULT CALLBACK _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//	被ATL的thunk替换过的窗口过程
//
//	Parameter：
//		hwnd
//			[in]	这里由于被替换过了，这里的hwnd是this指针
//
//		uMsg,wParam,lParam
//			[in]	消息信息
//
LRESULT  HwndHost::_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HwndHost* pThis = (HwndHost*)hwnd;
	return pThis->WndProc( uMsg, wParam, lParam );
}

LRESULT HwndHost::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_oldWndProc )
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}

	return 0;
}

//
//	[private] LRESULT WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
//
//	窗口被子类化过之后的窗口过程
//
LRESULT	HwndHost::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_SETFOCUS:
		{
			this->SetFocus(true);
			WindowBase* pWindow = this->GetWindowObject();
			if (NULL != pWindow)
			{
				pWindow->GetKeyboardMgr().SetFocusObjectDirect(this);
				::UISendMessage(this, WM_SETFOCUS, 
					(WPARAM)pWindow->GetKeyboardMgr().GetOldFocusObject(),
					0 );
			}
		}
		break;

	case WM_KILLFOCUS:
		{
			this->SetFocus(false);
			WindowBase* pWindow = this->GetWindowObject();
			if (NULL != pWindow)
			{
				::UISendMessage(this, WM_SETFOCUS, 
					(WPARAM)pWindow->GetKeyboardMgr().GetFocusObject(),
					0 );
			}
		}
		break;

	case WM_NCDESTROY:
		{
			this->UnSubclassWindow();
		}
		break;
	}
	return DefWindowProc(uMsg,wParam,lParam);
}

