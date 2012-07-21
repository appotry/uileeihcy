#include "stdafx.h"


//////////////////////////////////////////////////////////////////////////

//
//	TODO: GroupBox更新Text内容的时候重新计算RECT
//

GroupBox::GroupBox()
{
	this->m_eTextAlign = LABEL_ALIGN_LEFT|LABEL_ALIGN_TOP;
	this->m_hBorderBitmap = NULL;
	this->m_9RegionBorderBitmap.Set(2,2,2,3);
	::SetRectEmpty(&m_rcBorder);
	::SetRectEmpty(&m_rcText);
	::SetRectEmpty(&m_rcClip);
	m_bNeedCalcRect = true;

}
GroupBox::~GroupBox()
{
	if( NULL != m_hBorderBitmap )
	{
		::UI_ReleaseBitmap(m_hBorderBitmap);
		m_hBorderBitmap = NULL;
	}

}

void GroupBox::ResetAttribute()
{
	Panel::ResetAttribute();

	this->m_eTextAlign = LABEL_ALIGN_LEFT|LABEL_ALIGN_TOP;

	if( NULL != m_hBorderBitmap )
	{
		::UI_ReleaseBitmap(m_hBorderBitmap);
		m_hBorderBitmap = NULL;
	}
	this->m_9RegionBorderBitmap.Set(2,2,2,3);

}
bool GroupBox::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Panel::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	// 内容
	if( mapAttrib.count( XML_TEXT ) )
	{
		this->m_strText = mapAttrib[XML_TEXT];
		__super::m_mapAttribute.erase( XML_TEXT );
	}

	// 边框
	if( mapAttrib.count( XML_GROUPBOX_BORDERIMAGE) )
	{
		m_hBorderBitmap = UI_GetBitmap( mapAttrib[XML_GROUPBOX_BORDERIMAGE], GetGraphicsRenderType(GetHWND()) );
		__super::m_mapAttribute.erase( XML_GROUPBOX_BORDERIMAGE );
	}
	if( mapAttrib.count( XML_GOURPBOX_BORDERIMAGE_9REGION) )
	{
		Util::TranslateImage9Region( mapAttrib[XML_GOURPBOX_BORDERIMAGE_9REGION], (void*)&m_9RegionBorderBitmap );
		__super::m_mapAttribute.erase( XML_GOURPBOX_BORDERIMAGE_9REGION );
	}

	if( mapAttrib.count( XML_LABEL_ALIGN_H ))
	{
		m_eTextAlign = m_eTextAlign & 0xF0;

		String str = mapAttrib[XML_LABEL_ALIGN_H];
		if( str == XML_LABEL_ALIGN_LEFT )
		{
			m_eTextAlign |= LABEL_ALIGN_LEFT;
		}
		else if( str == XML_LABEL_ALIGN_RIGHT )
		{
			m_eTextAlign |= LABEL_ALIGN_RIGHT;
		}
		else if( str == XML_LABEL_ALIGN_CENTER )
		{
			m_eTextAlign |= LABEL_ALIGN_CENTER;
		}
		__super::m_mapAttribute.erase( XML_LABEL_ALIGN_H );
	}

	if( mapAttrib.count( XML_LABEL_ALIGN_V ))
	{
		m_eTextAlign = m_eTextAlign & 0x0F;

		String str = mapAttrib[XML_LABEL_ALIGN_V];
		if( str == XML_LABEL_ALIGN_TOP )
		{
			m_eTextAlign |= LABEL_ALIGN_TOP;
		}
		else if( str == XML_LABEL_ALIGN_BOTTOM )
		{
			m_eTextAlign |= LABEL_ALIGN_BOTTOM;
		}
		else if( str == XML_LABEL_ALIGN_CENTER )
		{
			m_eTextAlign |= LABEL_ALIGN_VCENTER;
		}
		__super::m_mapAttribute.erase( XML_LABEL_ALIGN_V );
	}

	if( NULL == m_pBkgndRender )
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
	}

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
	rc.OffsetRect(m_rcParent.left, m_rcParent.top);
	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);
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
	else if( NULL != m_hBorderBitmap )
	{
		DrawBitmap( hRDC, m_hBorderBitmap, m_rcBorder.left, m_rcBorder.top, m_rcBorder.Width(), m_rcBorder.Height(), 
			0,0, ::UI_GetBitmapWidth(m_hBorderBitmap), ::UI_GetBitmapHeight(m_hBorderBitmap), &m_9RegionBorderBitmap );
	}

	//
	//	还原剪裁区域
	//
	::SelectClipRgn(hRDC, hRgnOld);
	::DeleteObject(hRgnOld);
}
void GroupBox::OnPaint( HRDC hRDC )
{
	//
	//	绘制文字 
	//
	//	DrawString(hRDC, m_strText.c_str(), &m_rcText, DT_SINGLELINE|DT_END_ELLIPSIS, GetFont(), m_pColor?m_pColor->GetColor():0 );
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

	::SetRect( &m_rcBorder, 0,0, this->GetWidth(), this->GetHeight() );
	::SetRectEmpty( &m_rcText );

	const UINT SPAN = 10;     //  文字距离边缘的距离
	bool bVDrawText = false;  //  是否需要纵向绘制文字
	switch(m_eTextAlign)
	{
	case LABEL_ALIGN_LEFT|LABEL_ALIGN_TOP:
		{
			SIZE s = MeasureString(GetFont(), m_strText.c_str(), this->GetWidth()-2*SPAN );
			m_rcBorder.top = s.cy/2;
			::SetRect( &m_rcText, SPAN, 0, SPAN+s.cx, s.cy );
		}
		break;
	case LABEL_ALIGN_CENTER|LABEL_ALIGN_TOP:
		break;
	case LABEL_ALIGN_RIGHT|LABEL_ALIGN_TOP:
		break;
	case LABEL_ALIGN_LEFT|LABEL_ALIGN_VCENTER:
		bVDrawText = true;
		break;
	case LABEL_ALIGN_RIGHT|LABEL_ALIGN_VCENTER:
		bVDrawText = true;
		break;
	case LABEL_ALIGN_LEFT|LABEL_ALIGN_BOTTOM:
		break;
	case LABEL_ALIGN_CENTER|LABEL_ALIGN_BOTTOM:
		break;
	case LABEL_ALIGN_RIGHT|LABEL_ALIGN_BOTTOM:
		break;
	}

	//
	// 计算裁剪区域大小	
	//
	::CopyRect(&m_rcClip, &m_rcText);
	if( bVDrawText )
	{
		m_rcClip.top -=2;    // 文字与边框的间距
		m_rcClip.bottom +=2;
	}
	else
	{
		m_rcClip.left -=2;
		m_rcClip.right +=2;
	}
}
void GroupBox::GetBorderRect(RECT* prc)
{
	if( NULL != prc )
	{
		::CopyRect(prc, &m_rcBorder);
	}
}