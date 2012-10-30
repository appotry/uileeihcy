#include "stdafx.h"


//////////////////////////////////////////////////////////////////////////

//
//	TODO: GroupBox更新Text内容的时候重新计算RECT，
//        增加文字位于左侧、中间、右侧、下侧的情况
//

GroupBox::GroupBox()
{
	this->m_pBorderBitmap = NULL;
	this->m_9RegionBorderBitmap.Set(2,2,2,3);
	::SetRectEmpty(&m_rcBorder);
	::SetRectEmpty(&m_rcText);
	::SetRectEmpty(&m_rcClip);
	m_bNeedCalcRect = true;
	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);

}
GroupBox::~GroupBox()
{
	SAFE_RELEASE(m_pBorderBitmap);
}

void GroupBox::ResetAttribute()
{
	__super::ResetAttribute();

	SAFE_RELEASE(m_pBorderBitmap);
	this->m_9RegionBorderBitmap.Set(2,2,2,3);

}
bool GroupBox::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Panel::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	// 内容
	ATTRMAP::iterator iter = m_mapAttribute.find(XML_TEXT);
	if (m_mapAttribute.end() != iter)
	{
		this->m_strText = iter->second;
		m_mapAttribute.erase(iter);
	}

	// 边框
	iter = m_mapAttribute.find(XML_GROUPBOX_BORDERIMAGE);
	if (m_mapAttribute.end() != iter)
	{
		m_pBorderBitmap = UI_GetBitmap(iter->second, GetGraphicsRenderType(GetHWND()) );
		m_mapAttribute.erase(iter);
	}

	iter = m_mapAttribute.find(XML_GOURPBOX_BORDERIMAGE_9REGION);
	if (m_mapAttribute.end() != iter)
	{
		Util::TranslateImage9Region(iter->second, (void*)&m_9RegionBorderBitmap );
		m_mapAttribute.erase(iter);
	}

	if( NULL == m_pBkgndRender )
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
	}

	this->CalcBorderRect();

	return bRet;
}

void GroupBox::OnEraseBkgnd( HRDC hRDC )
{
	if( m_bNeedCalcRect )
	{
		this->CalcBorderRect();
	}

	//
	// 将文字区域裁剪出来
	//
	HRGN hRgnOld = GetClipRgn(hRDC);
	CRect rc = m_rcClip;
	POINT pt = this->GetRealPosInWindow();
	rc.OffsetRect(pt.x, pt.y);
	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);  // 剪裁区域是基于窗口的，而不是基于偏移
	SelectClipRgn(hRDC, hRgnClip, RGN_DIFF);
	::DeleteObject(hRgnClip);

	//
	//	绘制边框
	//
	if( NULL != m_pBkgndRender )
	{
		if( this->IsEnable() )
			m_pBkgndRender->DrawState(hRDC, &m_rcBorder, GROUPBOX_BKGND_RENDER_STATE_NORMAL );
		else
			m_pBkgndRender->DrawState(hRDC, &m_rcBorder, GROUPBOX_BKGND_RENDER_STATE_DISABLE );
	}
	else if( NULL != m_pBorderBitmap )
	{
		DrawBitmap( hRDC, m_pBorderBitmap, m_rcBorder.left, m_rcBorder.top, m_rcBorder.Width(), m_rcBorder.Height(), 
			0,0, m_pBorderBitmap->GetWidth(), m_pBorderBitmap->GetHeight(), &m_9RegionBorderBitmap );
	}

	//
	//	还原剪裁区域
	//
	::SelectClipRgn(hRDC, hRgnOld);
	::DeleteObject(hRgnOld);

	//
	//	绘制文字，放在onerasebkgnd中可以直接画在ncclient区域，不受限制
	//
	// DrawString(hRDC, m_strText.c_str(), &m_rcText, DT_SINGLELINE|DT_END_ELLIPSIS, GetFont(), 0 );
	if( NULL != m_pTextRender )
	{
		m_pTextRender->DrawState(hRDC, &m_rcText, 0, m_strText, DT_SINGLELINE|DT_END_ELLIPSIS );
	}
}

void GroupBox::OnSize( UINT nType, int cx, int cy )
{
	m_bNeedCalcRect = true;
}

void GroupBox::CalcBorderRect()
{
	m_bNeedCalcRect = false;
	if (NULL == m_pTextRender)
		return;

	::SetRect( &m_rcBorder, 0,0, this->GetWidth(), this->GetHeight() );
	::SetRectEmpty( &m_rcText );

	const UINT SPAN = 10;       //  文字距离边缘的距离
	const UINT TEXT_MARGIN = 2; //  文字距离线条的距离

	bool  bVDrawText = false;  //  是否需要纵向绘制文字
	int   nDrawFlag = m_pTextRender->GetDrawTextFlag();

	SIZE s = MeasureString(GetFont(), m_strText.c_str(), this->GetWidth()-2*SPAN );
	
	m_rcBorder.top = s.cy/2;
	
	CRegion4 rBorder(2,2,2,2);
	rBorder.top = s.cy;
	this->SetBorderRegion(&rBorder);

	::SetRect( &m_rcText, SPAN, 0, SPAN+s.cx, s.cy );

// 	{
// 	case DT_LEFT|DT_TOP:
// 		{
// 			SIZE s = MeasureString(GetFont(), m_strText.c_str(), this->GetWidth()-2*SPAN );
// 			m_rcBorder.top = s.cy/2;
// 			::SetRect( &m_rcText, SPAN, 0, SPAN+s.cx, s.cy );
// 		}
// 		break;
// 	case DT_CENTER|DT_TOP:
// 		break;
// 	case DT_RIGHT|DT_TOP:
// 		break;
// 	case DT_LEFT|DT_VCENTER:
// 		bVDrawText = true;
// 		break;
// 	case DT_RIGHT|DT_VCENTER:
// 		bVDrawText = true;
// 		break;
// 	case DT_LEFT|DT_BOTTOM:
// 		break;
// 	case DT_CENTER|DT_BOTTOM:
// 		break;
// 	case DT_RIGHT|DT_BOTTOM:
// 		break;
// 	}

	//
	// 计算裁剪区域大小	
	//
	::CopyRect(&m_rcClip, &m_rcText);
	if( bVDrawText )
	{
 		m_rcClip.top -= TEXT_MARGIN;    // 文字与边框的间距
 		m_rcClip.bottom += TEXT_MARGIN;
	}
	else
	{
 		m_rcClip.left -= TEXT_MARGIN;
 		m_rcClip.right += TEXT_MARGIN;
	}
}
void GroupBox::GetBorderRect(RECT* prc)
{
	if( NULL != prc )
	{
		::CopyRect(prc, &m_rcBorder);
	}
}