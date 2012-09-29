#include "stdafx.h"

Label::Label()
{
	this->SetTabstop( false );   // Label 不能有焦点
}

Label::~Label()
{
}


String  Label::GetText()
{
	return this->m_strText;
}
void    Label::SetText( const String& strText, bool bRedraw )
{
	this->m_strText = strText;

	if( bRedraw )
		this->UpdateObject();
}


void Label::OnPaint( HRDC hRDC )
{
	CRect  rcText;
	this->GetClientRect(&rcText);

// 	DrawString(hRDC, m_strText.c_str(), &rcText, nDrawFlag, 
// 		this->GetFont(), 
// 		m_pColor?m_pColor->GetColor():0 );

	m_pTextRender->DrawState(hRDC, &rcText, 0, m_strText, -1);
}



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 虚函数                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void Label::ResetAttribute()
{
	Control::ResetAttribute();

	this->ModifyStyle( OBJECT_STYLE_TRANSPARENT );
}
bool Label::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Control::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	// 内容
	ATTRMAP::iterator iter = mapAttrib.find(XML_TEXT);
	if (false==bReload && mapAttrib.end() != iter)
	{
		this->m_strText = iter->second;
		__super::m_mapAttribute.erase( XML_TEXT );
	}

	return bRet;
}

SIZE Label::GetAutoSize( HRDC hRDC )
{
//	SIZE size = MeasureString(GetFont(), this->m_strText.c_str() );
	SIZE size = {0,0};
	if( NULL != m_pTextRender )
	{
		size = m_pTextRender->GetDesiredSize(m_strText);
	}
	size.cx += m_rcPadding.left + m_rcPadding.right;
	size.cy += m_rcPadding.top + m_rcPadding.bottom;

	return size;
}

 

Picture::Picture()
{
//	this->m_hBitmap = NULL;
}
Picture::~Picture()
{
// 	if( NULL != m_hBitmap )
// 	{
// 		::UI_ReleaseBitmap(m_hBitmap);
// 		m_hBitmap = NULL;
// 	}
//	SAFE_DELETE(m_pRender);
}

SIZE Picture::GetAutoSize( HRDC hDC )
{
	SIZE sz = {0,0};
	if( NULL != m_pForegndRender )
	{
		return m_pForegndRender->GetDesiredSize();
	}

	return sz;
}

void Picture::OnPaint( HRDC hRDC )
{
	if( NULL != m_pForegndRender )
	{
		CRect rc;
		this->GetClientRect(&rc);

		m_pForegndRender->DrawState( hRDC, &rc, 0 );   // TODO: 这里可以扩展为支持hover/press状态
	}
}

bool Picture::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( false == Control::SetAttribute(mapAttrib,bReload) )
		return false;

// 	if( 0 != mapAttrib.count(XML_PICTURE_IMAGE) )
// 	{
// 		m_hBitmap = ::UI_GetBitmap( mapAttrib[XML_PICTURE_IMAGE], GetRenderType(GetHWND()) );
// 		this->m_mapAttribute.erase(XML_PICTURE_IMAGE);
// 	}
// 
// 	if( NULL == m_hBitmap )
// 	{
// 		UI_LOG_WARN(_T("Picture::SetAttribute getimage failed."));
// 		return false;
// 	}

// 	if( this->m_mapAttribute.count( XML_RENDER_TYPE ) )
// 	{
// 		SAFE_DELETE(m_pBkgndRender);
// 
// 		const String& strRenderType = mapAttrib[XML_RENDER_TYPE];
// 		this->m_pRender = RenderFactory::GetRender( strRenderType, this );
// 		if( NULL != this->m_pRender )
// 		{
// 			this->m_pRender->SetAttribute( _T(""), mapAttrib,bReload );
// 		}
// 		this->m_mapAttribute.erase(XML_RENDER_TYPE);
// 	}
// 	if( NULL == m_pRender )
// 	{
// 		UI_LOG_WARN( _T("%s, Set Render Attribute faild"), _T(__FUNCTION__));
// 	}


	return true;
}

 