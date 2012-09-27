#include "stdafx.h"

CustomWindow::CustomWindow(void)
{
	m_bNeedToSetWindowRgn = true;   // 首次显示时，需要设置一下窗口形状
	m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;     
	m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_NULL;
	m_pColMask = NULL;
	m_nAlphaMask = 1;                    // 当alpha值为0时，将抠除
	m_TransparentRgn9Region.Set(5);

	m_hRgn_topleft = NULL;
	m_hRgn_topright = NULL;
	m_hRgn_bottomleft = NULL;
	m_hRgn_bottomright = NULL;

	this->m_nResizeBit = WRSB_CAPTION;
	this->m_pLayeredWindowWrap = NULL;		
}
CustomWindow::~CustomWindow( )
{
	SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
	SAFE_RELEASE(m_pColMask);
	SAFE_DELETE(m_pLayeredWindowWrap);
}

BOOL CustomWindow::PreCreateWindow( CREATESTRUCT& cs )
{
	Window::PreCreateWindow( cs );
	cs.style = DS_SETFONT | WS_POPUP | WS_SYSMENU /*| WS_THICKFRAME*/;

	if( NULL != m_pLayeredWindowWrap )
	{
		m_pLayeredWindowWrap->PreCreateWindow( cs );
	}

	return TRUE;
}

void CustomWindow::OnInitWindow( )
{
	Window::OnInitWindow();

	LONG dwStyleEx = GetWindowLong(m_hWnd, GWL_EXSTYLE );
	dwStyleEx &= ~ WS_EX_WINDOWEDGE;
	LONG n = SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyleEx );

	if( NULL != m_pLayeredWindowWrap )
	{
		m_pLayeredWindowWrap->InitLayeredWindow();
	}
}

LRESULT CustomWindow::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if( NULL != m_pLayeredWindowWrap )
	{
		m_pLayeredWindowWrap->ReleaseLayeredWindow();
	}
	return 0;
}


//
// 屏蔽WM_NCACTIVATE消息，否则在win7上被激活或失活会有边框(但是因为现在不使用WS_THICKFRAME来实现拖拽，也没有这个
// 问题了，这里响应该消息仅为了实现窗口刷新）
//
// Remark
//	Return Nonzero if Windows should proceed with default processing; 0 to prevent the caption bar or icon from being deactivated.
//
//	这里不能返回0，否则会导致其它窗口出现很多问题
//	
LRESULT CustomWindow::_OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	this->SetActive(wParam?true:false);
	this->UpdateObject();
	return 1;
}


//
// 对于分层窗口，在显示的时候需要重新更新一次。因为当分层窗口隐藏的时候内存位图没有得到更新，invisible false后，直接return了
//
// 注：在这里没有使用响应WM_SHOWWINDOW来通过分层窗口刷新，因为在响应WM_SHOWWINDOW的时候，IsWindowVisible还是FALSE
//     因此改用OnWindowPosChanged来得到窗口显示的时机，通过分层窗口刷新
//   
LRESULT CustomWindow::_OnWindowPosChanged( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;

	if (NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnWindowPosChanged(lpWndPos);
	}
	return 0;
}
LRESULT CustomWindow::_OnCancelMode( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if (NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnCancelMode();
	}
	return 0;
}


void CustomWindow::ResetAttribute()
{
	__super::ResetAttribute();

	m_bNeedToSetWindowRgn = true;  // 换肤时，重新更新窗口形状
	m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_NULL;
	m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;
	m_TransparentRgn9Region.Set(5);

	SAFE_RELEASE(m_pColMask);
	m_nAlphaMask = 255;

	SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
}
bool CustomWindow::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Window::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;

	ATTRMAP::iterator iter = mapAttrib.find(XML_WINDOW_TRANSPARENT_PART);
	if (mapAttrib.end() != iter)
	{
		String& strType = iter->second;
		if (strType == XML_WINDOW_TRANSPARENT_PART_4_CORNER)
		{
			m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_4_CORNER;
		}
		else if (strType == XML_WINDOW_TRANSPARENT_PART_8_BORDER)
		{
			m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_8_BORDER;
		}
		else if (strType == XML_WINDOW_TRANSPARENT_PART_FULL)
		{
			m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;
		}
		else if (strType == XML_WINDOW_TRANSPARENT_PART_CUSTOM_CORNER)
		{
			m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_CUSTOM_CORNER;
		}
		else
		{
			m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_NULL;
		}
		this->m_mapAttribute.erase(XML_WINDOW_TRANSPARENT_PART);

		// 获取设置参数
		if( m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_4_CORNER ||
			m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_8_BORDER ||
			m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_CUSTOM_CORNER )
		{
			iter = mapAttrib.find(XML_WINDOW_TRANSPARENT_PART_9REGION);
			if (mapAttrib.end() != iter)
			{
				String& str9Region = iter->second;
				Util::TranslateImage9Region(str9Region, &m_TransparentRgn9Region );
				this->m_mapAttribute.erase(XML_WINDOW_TRANSPARENT_PART_9REGION);
			}
			else
			{
				m_TransparentRgn9Region.Set(5);
			}
		}
	}

	iter = mapAttrib.find(XML_WINDOW_TRANSPARENT_TYPE);
	if (mapAttrib.end() != iter)
	{
		String& strType = iter->second;
		m_mapAttribute.erase(XML_WINDOW_TRANSPARENT_TYPE);

		if( XML_WINDOW_TRANSPARENT_TYPE_LAYERED == strType )
		{
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_LAYERED;
			this->SetWindowLayered(true);
		}
		else if( XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR == strType )
		{	
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
		}	
		else if( XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA == strType )
		{
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_MASKALPHA;
		}

		// 透明色
		iter = mapAttrib.find(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE);
		if (mapAttrib.end() != iter)
		{
			const String& strColorID =  iter->second;
			::UI_GetColor( strColorID, &m_pColMask );
			this->m_mapAttribute.erase(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE);

			m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
		}

		// 透明度
		iter = mapAttrib.find(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE);
		if (mapAttrib.end() != iter)
		{
			m_nAlphaMask = _ttoi(iter->second.c_str() );
			this->m_mapAttribute.erase(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE);

			m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_MASKALPHA;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// 屏蔽WM_PAINT消息，不需要绘制Nc (否则在拉伸CustomExWindow的时候会出现thickframe )
LRESULT CustomWindow::_OnNcPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

void CustomWindow::OnEraseBkgnd(HRDC hRDC)
{
	if( NULL != m_pBkgndRender )
	{
		CRect rc(0,0, GetWidth(), GetHeight());
		if( this->IsActive() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, WINDOW_BKGND_RENDER_STATE_ACTIVE );
		}
		else
		{
			m_pBkgndRender->DrawState(hRDC, &rc, WINDOW_BKGND_RENDER_STATE_INACTIVE );
		}
	}
	else
	{
		__super::OnEraseBkgnd(hRDC);
	}

	// 重新设置窗口透明形状
	if( m_bNeedToSetWindowRgn )
	{
		this->UpdateWindowRgn(hRDC);
	}
}

void CustomWindow::UpdateWindowRgn( HRDC hRDC )
{
	this->m_bNeedToSetWindowRgn = false;

	BYTE* pBits = LockBits(hRDC);
	if( NULL == pBits )
		return;

	this->UpdateWindowRgn(pBits);

	UnlockBits(hRDC);
}
void CustomWindow::UpdateWindowRgn( BYTE* pBits )
{
	this->m_bNeedToSetWindowRgn = false;
	if( NULL == pBits )
	{
		return;
	}
	if( WINDOW_TRANSPARENT_TYPE_NULL == m_nWindowTransparentMaskType )
	{
		return;
	}
	if( WINDOW_TRANSPARENT_TYPE_LAYERED & m_nWindowTransparentMaskType ||
		WINDOW_TRANSPARENT_TYPE_NULL == m_nWindowTransparentMaskType )
	{
		return;   // 分层窗口不需要手动设置窗口形状
	}

	switch (m_eTransparentRgnType)
	{
	case WINDOW_TRANSPARENT_PART_NULL:
		return;

	case WINDOW_TRANSPARENT_PART_FULL:
		{
			RECT rc = { 0, 0, this->GetWidth(), this->GetHeight() };
			HRGN hRgn = this->GetExcludeRgn(pBits, rc, false);
			if( NULL != hRgn )
			{
				HRGN hRgnFull = CreateRectRgnIndirect(&rc);
				::CombineRgn(hRgnFull,hRgnFull,hRgn, RGN_DIFF);
				::SetWindowRgn(m_hWnd,hRgnFull,::IsWindowVisible(m_hWnd));
				::DeleteObject(hRgn);
				::DeleteObject(hRgnFull);
			}
		}
		return;

	case WINDOW_TRANSPARENT_PART_4_CORNER:
		{
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if( NULL == m_hRgn_topleft )  // 第一次初始化四个角落和形状参数
			{
				SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);

				RECT topleft = {0,0,m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top};
				m_hRgn_topleft = GetExcludeRgn(pBits, topleft, true);

				RECT topright = { nWindowW-m_TransparentRgn9Region.topright, 0, nWindowW, m_TransparentRgn9Region.top };
				m_hRgn_topright = GetExcludeRgn(pBits, topright, true);

				RECT bottomleft = { 0, nWindowH-m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomleft, nWindowH };
				m_hRgn_bottomleft = GetExcludeRgn(pBits, bottomleft, true);

				RECT bottomright = { nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom, nWindowW, nWindowH };
				m_hRgn_bottomright = GetExcludeRgn(pBits, bottomright, true);
			}
 
			HRGN hRgnTopRight = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomLeft = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomRight = ::CreateRectRgn(0,0,0,0);

			// m_hRgn_xxx都是基于(0,0)坐标的，需要根据当前窗口大小进行offset操作, m_hRgn_topleft不需要offset
			::CombineRgn(hRgnTopRight,m_hRgn_topright, hRgnTopRight, RGN_COPY);
			::OffsetRgn(hRgnTopRight,nWindowW-m_TransparentRgn9Region.topright, 0);

			::CombineRgn(hRgnBottomLeft,m_hRgn_bottomleft, hRgnBottomLeft, RGN_COPY);
			::OffsetRgn(hRgnBottomLeft, 0, nWindowH-m_TransparentRgn9Region.bottom);

			::CombineRgn(hRgnBottomRight,m_hRgn_bottomright, hRgnBottomRight, RGN_COPY);
			::OffsetRgn(hRgnBottomRight, nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom);

			HRGN hRgn = ::CreateRectRgn(0,0,nWindowW,nWindowH);
			::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnTopRight,    RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomLeft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomRight, RGN_DIFF );

			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));
			::DeleteObject(hRgn);

			::DeleteObject(hRgnTopRight);
			::DeleteObject(hRgnBottomLeft);
			::DeleteObject(hRgnBottomRight);
		}
		return;

	case WINDOW_TRANSPARENT_PART_8_BORDER:
		{
			// TODO: 仍有必要进行进一步的优化
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if( NULL == m_hRgn_topleft )  // 第一次初始化四个角落和形状参数
			{
				SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);

				RECT topleft = {0,0,m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top};
				m_hRgn_topleft = GetExcludeRgn(pBits, topleft, true);

				RECT topright = { nWindowW-m_TransparentRgn9Region.topright, 0, nWindowW, m_TransparentRgn9Region.top };
				m_hRgn_topright = GetExcludeRgn(pBits, topright, true);

				RECT bottomleft = { 0, nWindowH-m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomleft, nWindowH };
				m_hRgn_bottomleft = GetExcludeRgn(pBits, bottomleft, true);

				RECT bottomright = { nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom, nWindowW, nWindowH };
				m_hRgn_bottomright = GetExcludeRgn(pBits, bottomright, true);
			}

			RECT top = { m_TransparentRgn9Region.topleft, 0, nWindowW-m_TransparentRgn9Region.topright, m_TransparentRgn9Region.top };
			HRGN hRgn_top = GetExcludeRgn(pBits, top, false);
			RECT left = {0, m_TransparentRgn9Region.top, m_TransparentRgn9Region.left, nWindowH-m_TransparentRgn9Region.bottom};
			HRGN hRgn_left = GetExcludeRgn(pBits, left, false);
			RECT right = { nWindowW-m_TransparentRgn9Region.right, m_TransparentRgn9Region.top, nWindowW, nWindowH-m_TransparentRgn9Region.bottom };
			HRGN hRgn_right = GetExcludeRgn(pBits, right, false);
			RECT bottom = { m_TransparentRgn9Region.bottomleft, nWindowH-m_TransparentRgn9Region.bottomleft, nWindowW-m_TransparentRgn9Region.bottomright, nWindowH };
			HRGN hRgn_bottom = GetExcludeRgn(pBits, bottom, false);

			HRGN hRgnTopRight = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomLeft = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomRight = ::CreateRectRgn(0,0,0,0);

			// m_hRgn_xxx都是基于(0,0)坐标的，需要根据当前窗口大小进行offset操作, m_hRgn_topleft不需要offset
			::CombineRgn(hRgnTopRight,m_hRgn_topright, hRgnTopRight, RGN_COPY);
			::OffsetRgn(hRgnTopRight,nWindowW-m_TransparentRgn9Region.topright, 0);
			::CombineRgn(hRgnBottomLeft,m_hRgn_bottomleft, hRgnBottomLeft, RGN_COPY);
			::OffsetRgn(hRgnBottomLeft, 0, nWindowH-m_TransparentRgn9Region.bottom);
			::CombineRgn(hRgnBottomRight,m_hRgn_bottomright, hRgnBottomRight, RGN_COPY);
			::OffsetRgn(hRgnBottomRight, nWindowW-m_TransparentRgn9Region.bottomright, 0);

			HRGN hRgn = ::CreateRectRgn(0,0,nWindowW,nWindowH);
			::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnTopRight,    RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomLeft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomRight, RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_top,        RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_left,       RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_right,      RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_bottom,     RGN_DIFF );

			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));
			::DeleteObject(hRgn);

			::DeleteObject(hRgnTopRight);
			::DeleteObject(hRgnBottomLeft);
			::DeleteObject(hRgnBottomRight);
			::DeleteObject(hRgn_top);
			::DeleteObject(hRgn_left);
			::DeleteObject(hRgn_right);
			::DeleteObject(hRgn_bottom);
			
		}
		return;

	case WINDOW_TRANSPARENT_PART_CUSTOM_CORNER:
		{
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if (NULL == m_hRgn_topleft && m_TransparentRgn9Region.topleft != 0 && m_TransparentRgn9Region.top != 0)
			{
				m_hRgn_topleft = ::CreateEllipticRgn( 0,0, m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top );
				HRGN hRgnRect = ::CreateRectRgn( 0,0, m_TransparentRgn9Region.topleft/2, m_TransparentRgn9Region.top/2 );
				::CombineRgn( m_hRgn_topleft, hRgnRect, m_hRgn_topleft, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_topright && m_TransparentRgn9Region.topright != 0 && m_TransparentRgn9Region.left != 0)
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.topright, m_TransparentRgn9Region.left};
				::OffsetRect(&rcE, nWindowW-m_TransparentRgn9Region.topright, 0);
				m_hRgn_topright = ::CreateEllipticRgnIndirect( &rcE );

				RECT rcR = {0,0, m_TransparentRgn9Region.topright/2, m_TransparentRgn9Region.left/2};
				::OffsetRect(&rcR, nWindowW-m_TransparentRgn9Region.topright/2, 0 );
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );

				::CombineRgn( m_hRgn_topright, hRgnRect, m_hRgn_topright, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_bottomleft && m_TransparentRgn9Region.right != 0 && m_TransparentRgn9Region.bottomleft != 0)
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.right, m_TransparentRgn9Region.bottomleft};
				::OffsetRect(&rcE, 0, nWindowH-m_TransparentRgn9Region.bottomleft);
				m_hRgn_bottomleft = ::CreateEllipticRgnIndirect(&rcE);

				RECT rcR = {0,0, m_TransparentRgn9Region.right/2, m_TransparentRgn9Region.bottomleft/2};
				::OffsetRect(&rcR, 0, nWindowH-m_TransparentRgn9Region.bottomleft/2);
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );
				::CombineRgn( m_hRgn_bottomleft, hRgnRect, m_hRgn_bottomleft, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_bottomright && m_TransparentRgn9Region.bottom != 0 && m_TransparentRgn9Region.bottomright != 0 )
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomright};
				::OffsetRect(&rcE, nWindowW-m_TransparentRgn9Region.bottom, nWindowH-m_TransparentRgn9Region.bottomright );
				m_hRgn_bottomright = ::CreateEllipticRgnIndirect( &rcE );

				RECT rcR = {0,0, m_TransparentRgn9Region.bottom/2, m_TransparentRgn9Region.bottomright/2};
				::OffsetRect(&rcR,nWindowW-m_TransparentRgn9Region.bottom/2, nWindowH-m_TransparentRgn9Region.bottomright/2 );
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );

				::CombineRgn( m_hRgn_bottomright, hRgnRect, m_hRgn_bottomright, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}

			HRGN hRgn = ::CreateRectRgn(0,0,this->GetWidth(), this->GetHeight());
			if (NULL != m_hRgn_topleft)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			}
			if (NULL != m_hRgn_topright)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_topright,  RGN_DIFF );
			}
			if (NULL != m_hRgn_bottomleft)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_bottomleft,  RGN_DIFF );
			}
			if (NULL != m_hRgn_bottomright)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_bottomright,  RGN_DIFF );
			}
			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));
			SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
			SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
			SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
		}
		return;

	}
}


//
//	从一个pBits取该内容上面RC区域内需要被抠掉的范围
//
//	bOffsetToOrigin
//		[in]	要否需要将返回的HRGN的原点设置为(0,0)开始 --- 获取四个角落的HRGN时有用
//
HRGN CustomWindow::GetExcludeRgn( BYTE* pBits, const RECT& rc, bool bOffsetToOrigin )
{
	if(NULL == pBits)
		return NULL;

	int nWindowW = this->GetWidth();
	int nWindowH = this->GetHeight();

	BYTE* p = pBits;
	vector<RECT>  vRectRgnData;
	for (LONG i = rc.top; i < rc.bottom; i++ )   
	{
		p = pBits + nWindowW*4*i + rc.left*4;
		for (LONG j = rc.left; j < rc.right; j++)      
		{
			POINT pt = { j, i };

			BYTE r = *p++;
			BYTE g = *p++;
			BYTE b = *p++;
			BYTE a = *p++;

			bool bExclude = false;
			if( ((int)m_nWindowTransparentMaskType)& WINDOW_TRANSPARENT_TYPE_MASKCOLOR ) 
			{
				if( NULL != m_pColMask && m_pColMask->GetColor() == RGB(r,g,b) )
				{
					bExclude = true;
				}
			}
			if( ((int)m_nWindowTransparentMaskType) & WINDOW_TRANSPARENT_TYPE_MASKALPHA )
			{
				if( a < m_nAlphaMask)
				{
					bExclude = true;
				}
			}
			if( bExclude )
			{
				RECT rcPixel = { pt.x, pt.y, pt.x+1, pt.y+1 };  // 统一设置为从0,0位置开始，便于后面的OffsetRgn
				if( bOffsetToOrigin )
				{
					::OffsetRect(&rcPixel, -rc.left, -rc.top);
				}

				vRectRgnData.push_back(rcPixel);
			}
		}
	}

	int nCount = (int)vRectRgnData.size();
	RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nCount*sizeof(RECT) ];
	pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType      = RDH_RECTANGLES;
	pRgnData->rdh.nCount     = nCount;
	pRgnData->rdh.nRgnSize   = nCount*sizeof(RECT);
	for( int k = 0; k < nCount; k++ )
	{
		RECT* prc = (RECT*)pRgnData->Buffer;
		prc[k] = vRectRgnData[k];
	}

	HRGN hRgn = ::ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + nCount*sizeof(RECT), pRgnData );
	SAFE_ARRAY_DELETE(pRgnData);

	return hRgn;
}

void CustomWindow::InvalidateObject( Object* pInvalidateObj, RECT* prc, bool bUpdateNow )
{
	if( NULL == m_pLayeredWindowWrap )
	{
		__super::InvalidateObject(pInvalidateObj, prc, bUpdateNow);
		return;
	}
	else
	{
		m_pLayeredWindowWrap->InvalidateObject(pInvalidateObj, bUpdateNow);
	}
}

HRDC CustomWindow::BeginDrawObject( Object* pInvalidateObj )
{
	if( NULL == m_pLayeredWindowWrap )
	{
		return __super::BeginDrawObject(pInvalidateObj);
	}
	else
	{
		return m_pLayeredWindowWrap->BeginDrawObject(pInvalidateObj);
	}
}
void CustomWindow::EndDrawObject( CRect* prcWindow, bool bFinish)
{
	__super::EndDrawObject(prcWindow, bFinish);

	if (bFinish && NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->Commit2LayeredWindow();
	}
}

//
//	获取一个POINT在CustomWindow上面的位置
//
UINT CustomWindow::OnHitTest( POINT* pt )
{
	if (m_nResizeBit == WRSB_NONE)
	{
		return HTCLIENT;
	}
	else if (m_nResizeBit == WRSB_CAPTION)
	{
		if( NULL == GetHoverObject() && NULL == GetPressObject())
			return HTCAPTION;
	}

	if( NULL == pt )
		return HTERROR;

	UINT nHitTest = HTCLIENT;
	CONST int BORDER = GetSystemMetrics(SM_CYDLGFRAME)*2;

	RECT rc;
	::GetClientRect( m_hWnd, &rc );
	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if( pt->x < BORDER )
	{
		if( pt->y < BORDER && this->TestResizeBit(WRSB_TOPLEFT) )
			nHitTest = HTTOPLEFT;
		else if( pt->y > nHeight - BORDER  && this->TestResizeBit(WRSB_BOTTOMLEFT) )
			nHitTest = HTBOTTOMLEFT;
		else if( this->TestResizeBit(WRSB_LEFT) )
			nHitTest = HTLEFT;
	}
	else if( pt->x > nWidth-BORDER )
	{
		if( pt->y < BORDER && this->TestResizeBit(WRSB_TOPRIGHT)  )
			nHitTest = HTTOPRIGHT;
		else if( pt->y > nHeight - BORDER && this->TestResizeBit(WRSB_BOTTOMRIGHT) )
			nHitTest = HTBOTTOMRIGHT;
		else if( this->TestResizeBit(WRSB_RIGHT) )
			nHitTest = HTRIGHT;
	}
	else if( pt->y < BORDER && this->TestResizeBit(WRSB_TOP) )
	{
		nHitTest = HTTOP;
	}
	else if( pt->y > nHeight - BORDER && this->TestResizeBit(WRSB_BOTTOM))
	{
		nHitTest = HTBOTTOM;
	}
	else
	{
		if (m_nResizeBit & WRSB_CAPTION && NULL == GetHoverObject() && NULL == GetPressObject())
		{
			nHitTest = HTCAPTION;
		}
	}

	return nHitTest;

}

//
//	注：这个消息其它有可能是在MOUSEMOVE之后手动POST出来的一个消息
//
BOOL CustomWindow::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	POINT pt;
	::GetCursorPos(&pt);
	::MapWindowPoints(NULL,m_hWnd,&pt,1);
	nHitTest = this->OnHitTest(&pt);
	switch(nHitTest)
	{
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENWSE) );
		break;

	case HTTOP:
	case HTBOTTOM:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENS) );
		break;

	case HTLEFT:
	case HTRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZEWE) );
		break;

	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENESW) );
		break;

	case HTCAPTION:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;

	default:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;	
	}
	
	return TRUE;
}

//
//	使用通过在 NCHitTest中返回HTCAPTION也能使用点击拖拽的效果，但缺点是会一直产生一个WM_MOUSELEAVE消息，因为鼠标不在客户区域了
//
//	使用WS_THICKFRAME也可以直接实现边框拖拽效果，但还需要去响应NCCACLSIZE和添加一个属性，可能会导致窗口在WIN7下面显示透明边框
//	
//  另外UpdateLayeredWindow支持更平滑的窗口拉伸效果，因此对于分层窗口的拉伸是另外一套逻辑实现的
//
void CustomWindow::OnLButtonDown(UINT nFlags, POINT pt)
{
	SetMsgHandled(FALSE);

	UINT nHitTest = this->OnHitTest(&pt);

	switch(nHitTest)
	{
	case HTTOPLEFT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOP:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOP, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOPRIGHT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTLEFT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_LEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTRIGHT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_RIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMLEFT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOM:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOM, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMRIGHT:
		if (NULL != m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTCAPTION:
		::PostMessage( m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x,pt.y) );
		break;
	}
}

void CustomWindow::OnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnLButtonUp();
	}
}

void CustomWindow::OnMouseMove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnMouseMove();
	}
}
void CustomWindow::OnSize( UINT nType, int cx, int cy )
{
	SetMsgHandled(FALSE);

	// 最小化和从最小化还原时，不需要重新计算窗口的形状
	if (SIZE_MINIMIZED == nType || (SIZE_RESTORED==nType && ::IsIconic(m_hWnd)))
		return;

	if (NULL != m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnSize(nType, cx, cy);
	}
	else
	{
		this->m_bNeedToSetWindowRgn = true;
	}
}

//
//	设置窗口是否可以拖拽
//
void CustomWindow::SetWindowResizeType( UINT nType )
{
	m_nResizeBit = nType;
}

//
// 判断窗口的拉伸状态位
//
bool CustomWindow::TestResizeBit( int nBit )
{
	if (m_nResizeBit & nBit)
		return true;
	else 
		return false;
}


bool CustomWindow::IsWindowLayered()
{
	return (NULL!=m_pLayeredWindowWrap);
}
//
//	设置/取消一个窗口为分层窗口
//
//	Remark
//		如果一个窗口需要在分层与非分层之间进行切换的话，则必须响应 UI_WM_GETRENDERTYPE 消息，
//		并返回RENDER_TYPE_GDIPLUS类型。因为目前不支持切换为分层窗口时，将该窗口中所使用到的
//		图片、字体资源都同时转换成GDIPLUS类型的。
//
void CustomWindow::SetWindowLayered(bool b)
{
	if( b && NULL != m_pLayeredWindowWrap )
		return;
	if( !b && NULL == m_pLayeredWindowWrap )
		return;

	if( b )
	{
		m_pLayeredWindowWrap = new LayeredWindowWrap(this);

		// Remove WS_EX_LAYERED from this window styles
		SetWindowLong(m_hWnd, 
			GWL_EXSTYLE,
			GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		SetWindowRgn(m_hWnd, NULL, FALSE);   // 取消窗口的异形，由分层窗口自己来处理

		this->ReCreateRenderTarget();  //放在InitLayeredWindow的前面，避免在InitLayeredWindow->Invalidate->WM_PAINT中创建的RenderTarget又被销毁了
		m_pLayeredWindowWrap->InitLayeredWindow();
	}
	else
	{
		// 注：直接去除WS_EX_LAYERED属性，会导致窗口先变黑，然后再刷新一次。
		//     因此在这里去除分层属性之后，直接将mem bitmap的内容先画到窗口上来规避这个问题。
		//     同时为了解决画到窗口上的内容有rgn以外的部分，在去除分层属性之前，先计算
		//     窗口的形状
		
		DIBSECTION  dibsection;
		::GetObject( this->m_pLayeredWindowWrap->m_hLayeredBitmap, sizeof(DIBSECTION), &dibsection );
		this->UpdateWindowRgn( (BYTE*)dibsection.dsBm.bmBits );

		// Remove WS_EX_LAYERED from this window styles
		SetWindowLong(m_hWnd, 
			GWL_EXSTYLE,
			GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

		HDC hDC = ::GetDC(m_hWnd);
		::BitBlt(hDC,0,0, GetWidth(), GetHeight(), this->m_pLayeredWindowWrap->m_hLayeredMemDC, 0,0, SRCCOPY);
		ReleaseDC(m_hWnd, hDC);

		m_pLayeredWindowWrap->ReleaseLayeredWindow();
		SAFE_DELETE(m_pLayeredWindowWrap);

		// Ask the window and its children to repaint (Bitblt已完成了刷新功能)
// 		RedrawWindow(m_hWnd, 
// 			NULL, 
// 			NULL, 
// 			RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		
		this->ReCreateRenderTarget();
	}
}

void CustomWindow::SetWindowTransparentMaskType( int type )
{
	m_nWindowTransparentMaskType = type;
}
void CustomWindow::SetWindowTransparentColMask( COLORREF col )
{
	UIColor::CreateInstance(col, &m_pColMask);
	m_pColMask->AddRef();
}
void CustomWindow::SetWindowTransparentColMask( const String& strColdID )
{
	UI_GetColor(strColdID, &m_pColMask);
}
void CustomWindow::SetWindowTransparentAlphaMask( int nAlpha )
{
	m_nAlphaMask = nAlpha;
}



// -> 注：每次强制返回HTCAPTION之后，将导致生成WM_MOUSELEAVE消息（鼠标进入非客户区域 )
// 当鼠标不再窗口控件上的位置拖动时候，默认拖动窗口
//LRESULT  CustomWindow::OnNcHitTest( POINT pt )
//{
// 	if( NULL == MouseManager::GetObjectByPos( this->GetWindowObject(), &pt ) )
// 	{
// 		__super::SetMsgHandled(TRUE);
// 		return HTCAPTION;
// 	}
// 	return HTCLIENT;
//}


/*  注：Removed. 改进：不再通过使用OnNcHitTest返回HTCAPTION来实现拖拽
	目的：在使用CustomWindow/CustomExWindow的时候，如果鼠标拖动的client区域没有其他控件
	      则 OnNcHitTest 会返回 HTCAPTION 。
		  这样就会产生一个问题：返回HTCAPTION之后，就不会产生 WM_MOUSEMOVE消息，而是会
		  产生 WM_NCMOUSEMOVE 消息，这就给 MouseManager 在处理 WM_MOUSEMOVE 消息时带来
		  了错误。因此有必要在 Custom(Ex)Window中将 WM_NCXXX消息再转换成 WM_XXX消息
*/

// void CustomWindow::OnNcMouseMove( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEMOVE, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseHover( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEHOVER, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseLeave()
// {
// 	SetMsgHandled(FALSE);
// 	::SendMessage( m_hWnd, WM_MOUSELEAVE, (WPARAM)0, (LPARAM)0 );
// }
// void CustomWindow::OnNcLButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }


//////////////////////////////////////////////////////////////////////////


LayeredWindowWrap::LayeredWindowWrap(CustomWindow* pWindow)
{
	m_pWindow = pWindow;

	m_hLayeredMemDC = NULL;
	m_hLayeredBitmap = NULL;

	m_nHitTestFlag = 0;
	m_ptStartSizeMove.x = 0;
	m_ptStartSizeMove.y = 0;
	m_ptWindowOld.x = 0;
	m_ptWindowOld.y = 0;
	m_sizeWindowOld.cx = 0;
	m_sizeWindowOld.cy = 0;
	m_ptWindow.x = 0;
	m_ptWindow.y = 0;
	m_sizeWindow.cx = 0;
	m_sizeWindow.cy = 0;
}
LayeredWindowWrap::~LayeredWindowWrap()
{
	m_pWindow = NULL;
}
BOOL LayeredWindowWrap::PreCreateWindow( CREATESTRUCT& cs )
{
	// 添加分层属性 
	cs.dwExStyle |= WS_EX_LAYERED;
	return TRUE;
}


void LayeredWindowWrap::InitLayeredWindow()
{
	RECT rc;
	::GetWindowRect( m_pWindow->m_hWnd, &rc );

	m_hLayeredMemDC = ::CreateCompatibleDC(NULL/*m_hScreenDC*/);

	Image image;
	image.Create( rc.right-rc.left, -(rc.bottom-rc.top), 32, Image::createAlphaChannel );
	m_hLayeredBitmap = image.Detach();
	::SelectObject(m_hLayeredMemDC, m_hLayeredBitmap);

	this->InvalidateObject(m_pWindow,TRUE);
}
void LayeredWindowWrap::ReleaseLayeredWindow()
{
	if( NULL != m_hLayeredMemDC )
	{
		::DeleteDC(m_hLayeredMemDC);
		m_hLayeredMemDC = NULL;
	}

	SAFE_DELETE_GDIOBJECT(m_hLayeredBitmap);
}
void LayeredWindowWrap::OnSize( UINT nType, int cx, int cy )
{
	if( NULL == m_hLayeredMemDC )
		return;

	SAFE_DELETE_GDIOBJECT(m_hLayeredBitmap);

	Image image;
	image.Create(cx, -cy, 32, Image::createAlphaChannel);
	m_hLayeredBitmap = image.Detach();
	::SelectObject(m_hLayeredMemDC, m_hLayeredBitmap);

	this->InvalidateObject(m_pWindow, TRUE);
}

void LayeredWindowWrap::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
	m_ptWindow.x = lpWndPos->x;
	m_ptWindow.y = lpWndPos->y;
	m_sizeWindow.cx = lpWndPos->cx;
	m_sizeWindow.cy = lpWndPos->cy;

	if (lpWndPos->flags & SWP_SHOWWINDOW)  // 窗口显示（窗口隐藏时，DrawObject会失败）
		this->InvalidateObject(m_pWindow, TRUE);
}

void LayeredWindowWrap::InvalidateObject( Object* pInvalidateObj, bool bUpdateNow )
{
	if (NULL == pInvalidateObj)
		return;

	if (OBJ_WINDOW == pInvalidateObj->GetObjectType())
	{
		if (!IsWindowVisible(m_pWindow->m_hWnd))  // 避免第一次绘制时窗口不可见时，仍然Commit2LayeredWindow，导致窗口全黑
			return;

		::SendMessage(m_pWindow->m_hWnd, WM_PAINT, (WPARAM)m_hLayeredMemDC, 0);  // 通过发送原始WM_PAINT消息，使分层窗口也能和普通窗口一样响应WM_PAINT

		// 备注：测试证明，通过RedrawWindow也能够使得分层窗口收到WM_PAINT消息，太神奇了，但BeginPaint得到的HDC
		//       在分层窗口上面也没法用吧。另外发现第一次调用的时候收不到该消息，难道是因为窗口还没有显示？
// 		BOOL bRet = ::RedrawWindow(m_pWindow->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
// 		UIASSERT(bRet)
	}
	else
	{
		m_pWindow->_InvalidateObject(pInvalidateObj, m_hLayeredMemDC);  // <-- 这里的第二个参数与WindowBase::InvalidateObject不一致
	}

	this->Commit2LayeredWindow();
}

HRDC LayeredWindowWrap::BeginDrawObject( Object* pInvalidateObj)
{
	if( NULL == pInvalidateObj )
		return NULL;

	if (false == BeginDraw(m_pWindow->m_hRenderTarget, m_hLayeredMemDC))
		return NULL;

	RenderOffsetClipHelper roc(m_pWindow);
	pInvalidateObj->DrawObjectTransparentBkgnd(m_pWindow->m_hRenderTarget, roc, true);
	roc.Reset(m_pWindow->m_hRenderTarget);

	::UISendMessage(pInvalidateObj, WM_ERASEBKGND, (WPARAM)m_pWindow->m_hRenderTarget, (LPARAM)1 );

	roc.DrawClient(m_pWindow->m_hRenderTarget, pInvalidateObj, false);
	roc.Scroll(m_pWindow->m_hRenderTarget, pInvalidateObj, false);
	roc.Update(m_pWindow->m_hRenderTarget);

	return m_pWindow->m_hRenderTarget;
}

// 模拟拖拽窗口拉伸过程
void LayeredWindowWrap::OnLButtonDown(UINT nHitTest)
{
	OnEnterSizeMove(nHitTest);
}
void LayeredWindowWrap::OnLButtonUp()
{
	OnExitSizeMove();
}
void LayeredWindowWrap::OnCancelMode()
{
	OnExitSizeMove();
}
void LayeredWindowWrap::OnMouseMove()
{
	if (0 == m_nHitTestFlag)
		return;

	POINT ptCursor;
	GetCursorPos(&ptCursor);

	int nxMoveDiff = 0;
	int nyMoveDiff = 0;

	int oldCX = m_sizeWindow.cx;
	int oldCY = m_sizeWindow.cy;

	// 计算窗口的新坐标
	switch(m_nHitTestFlag)
	{
	case HTLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_ptWindow.x = m_ptWindowOld.x - nxMoveDiff;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
		break;

	case HTRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
		break;

	case HTTOP:
		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_ptWindow.y = m_ptWindowOld.y - nyMoveDiff;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTBOTTOM:
		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTTOPLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_ptWindow.x = m_ptWindowOld.x - nxMoveDiff;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_ptWindow.y = m_ptWindowOld.y - nyMoveDiff;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTTOPRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_ptWindow.y = m_ptWindowOld.y - nyMoveDiff;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTBOTTOMLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_ptWindow.x = m_ptWindowOld.x - nxMoveDiff;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTBOTTOMRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;
	}

	if (oldCX == m_sizeWindow.cx && oldCY == m_sizeWindow.cy)
	{
		return;
	}

	ResizeRenderTarget(m_pWindow->m_hRenderTarget, m_sizeWindow.cx,m_sizeWindow.cy);

	// 注意：m_rcParent的更新千万不能使用GetWindowRect。因为窗口的大小现在就没有变
	//       所以这里也就没有采用SendMessage(WM_SIZE)的方法
	SetRect(&m_pWindow->m_rcParent, 0,0, m_sizeWindow.cx, m_sizeWindow.cy);
	::UI_UpdateLayout(m_pWindow, FALSE);

	this->OnSize(0, m_sizeWindow.cx, m_sizeWindow.cy);
}
void LayeredWindowWrap::OnEnterSizeMove(UINT nHitTest)
{
	SetCapture(m_pWindow->m_hWnd);
	m_nHitTestFlag = nHitTest;

	POINT ptWindow;
	GetCursorPos(&ptWindow);

	m_ptStartSizeMove.x = ptWindow.x;
	m_ptStartSizeMove.y = ptWindow.y;

	m_ptWindowOld.x = m_ptWindow.x;
	m_ptWindowOld.y = m_ptWindow.y;
	m_sizeWindowOld.cx = m_sizeWindow.cx;
	m_sizeWindowOld.cy = m_sizeWindow.cy;
}

void LayeredWindowWrap::OnExitSizeMove()
{
	if (0 == m_nHitTestFlag)
		return;

	if (m_pWindow->m_hWnd == GetCapture())
		ReleaseCapture();

	m_nHitTestFlag = 0;
	m_ptStartSizeMove.x = 0;
	m_ptStartSizeMove.y = 0;

	m_ptWindowOld.x = 0;
	m_ptWindowOld.y = 0;
	m_sizeWindowOld.cx = 0;
	m_sizeWindowOld.cy = 0;
}

void LayeredWindowWrap::Commit2LayeredWindow()
{
	POINT ptMemDC  = {0,0};
	int   nFlag = ULW_OPAQUE;
	DWORD dwColorMask = 0;

	BLENDFUNCTION bf;
	bf.BlendOp     = AC_SRC_OVER ;
	bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA 会导致窗口被搂空,AC_SRC_OVER不使用透明
	bf.BlendFlags  = 0;                   // Must be zero. 
	bf.SourceConstantAlpha = 255;         // 0~255 透明度

	// TODO [注：在xp、win7不使用桌面主题的情况下，COLOR将有问题，因此建议全部使用PNG来实现透明
	if( m_pWindow->m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKALPHA )
	{
		nFlag &= ~ULW_OPAQUE;
		nFlag |= ULW_ALPHA;
		bf.SourceConstantAlpha = m_pWindow->m_nAlphaMask;
	}
	else if( m_pWindow->m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKCOLOR ) 
	{
		if( NULL != m_pWindow->m_pColMask )
		{
			dwColorMask = m_pWindow->m_pColMask->GetColor();
			nFlag = ULW_COLORKEY;
		}
	}

	BOOL bRet = ::UpdateLayeredWindow( m_pWindow->m_hWnd, NULL, &m_ptWindow, &m_sizeWindow/*&leftTop, &size*/, m_hLayeredMemDC, &ptMemDC, dwColorMask, &bf, nFlag ); 
	if (FALSE == bRet)
		UI_LOG_ERROR(_T("%s UpdateLayeredWindow Failed."), FUNC_NAME);
	
}