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
//	���໯���ڣ��滻���Ĵ��ڹ���
//
//	Return
//		�ɹ�����false��ʧ�ܷ���false
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
//	ȡ�����໯���ڣ��ָ����Ĵ��ڹ���
//
//	Return
//		�ɹ�����false��ʧ�ܷ���false
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

bool HwndHost::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;
	

	HWND hParentWnd = this->GetHWND();
	ATTRMAP::iterator iter = m_mapAttribute.find(XML_HWNDHOST_CONTROL_ID);
	if (m_mapAttribute.end() != iter)
	{
		int nValue = _ttoi( iter->second.c_str() );
		m_hWnd = ::GetDlgItem(hParentWnd, nValue);
		if( NULL == m_hWnd )
		{
			UI_LOG_WARN(_T("HwndHost::SetAttribute Failed. id=%s, GetDlgItem(%d)"), m_strID.c_str(), nValue );
			return false;
		}
		
		this->SubclassWindow();
		m_mapAttribute.erase(iter);
	}

	return true;
}

SIZE HwndHost::GetDesiredSize()
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
//	��ATL��thunk�滻���Ĵ��ڹ���
//
//	Parameter��
//		hwnd
//			[in]	�������ڱ��滻���ˣ������hwnd��thisָ��
//
//		uMsg,wParam,lParam
//			[in]	��Ϣ��Ϣ
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
//	���ڱ����໯��֮��Ĵ��ڹ���
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
				::UISendMessage(this, WM_KILLFOCUS, 
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

UINT HwndHost::OnHitTest( POINT* pt )
{
	POINT point = *pt;
	MapWindowPoints(GetHWND(), NULL, &point, 1);
	UINT nRet = ::SendMessage(m_hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
	return nRet;
}

void HwndHost::OnParentVisibleChanged(Object* pParent, bool bVisible)
{
	if (false == this->IsMySelfVisible())
		return;

	if (bVisible)
	{
		// ����ֱ�ӵ���this->IsVisible���ж�IsWindowVisible(m_hWnd)�����µõ���ֵ����ȷ
		// �������������жϸ����ڵĿɼ�
		if (NULL != m_pParent && m_pParent->IsVisible())
		{
			ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		}
	}
	else
	{
		//if (::IsWindowVisible(m_hWnd)) <-- ע�����ﲻ�ܼ�������жϣ���Ϊ����OnInitDialog������parentʱ��HwndHost
		//                                       ��IsWindowVisible�᷵��FALSE�������жϴ���
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
	}
}