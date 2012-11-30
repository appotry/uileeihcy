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

	this->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT);
}
bool Label::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	ATTRMAP::iterator iter = m_mapAttribute.find(XML_TEXTRENDER_TYPE);
	if (m_mapAttribute.end() != iter)
	{
		SAFE_DELETE(m_pTextRender);
		const String& strTextRenderType = iter->second;
		m_pTextRender = TextRenderFactory::GetTextRender(strTextRenderType, this);
		m_pTextRender->SetAttribute(_T(""),m_mapAttribute);

		this->m_mapAttribute.erase(iter);
	}
	else if (NULL == m_pTextRender)
	{
		m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
		m_pTextRender->SetAttribute(_T(""), m_mapAttribute);
	}

	// 内容
	iter = m_mapAttribute.find(XML_TEXT);
	if (false==bReload && m_mapAttribute.end() != iter)
	{
		this->m_strText = iter->second;
		m_mapAttribute.erase(iter);
	}

	return bRet;
}

SIZE Label::GetAutoSize()
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
	this->SetTabstop( false );   // Picture 不能有焦点
}
Picture::~Picture()
{
}

SIZE Picture::GetAutoSize()
{
	SIZE sz = {0,0};
	if (NULL != m_pBkgndRender)
	{
		return m_pBkgndRender->GetDesiredSize();
	}
	if (NULL != m_pForegndRender)
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


//////////////////////////////////////////////////////////////////////////

GifPicture::GifPicture()
{
	m_pGifRender = NULL;
}
GifPicture::~GifPicture()
{
	SAFE_RELEASE(m_pGifRender);
}

SIZE GifPicture::GetAutoSize()
{
	SIZE s = {0, 0};
	if (NULL != m_pGifRender)
	{
		s.cx = m_pGifRender->GetWidth();
		s.cy = m_pGifRender->GetHeight();
	}
	return s;
}

void GifPicture::ResetAttribute()
{
	__super::ResetAttribute();
	this->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT);  // 默认不透明
	return;
}
bool GifPicture::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	ATTRMAP::iterator iter = m_mapAttribute.find(XML_GIFPICTURE_PATH);
	if (iter != m_mapAttribute.end())
	{
		String& strGifID = iter->second;

		GifImageBase* pGifImage = UI_GetGifImage(strGifID);
		if (NULL != pGifImage)
		{
			SAFE_RELEASE(m_pGifRender);

//			POINT pt = this->GetRealPosInWindow();
//			Gif_Timer_Notify notify(GetHWND(), pt.x + m_rcNonClient.left, pt.y + m_rcNonClient.top);
			Gif_Timer_Notify notify(static_cast<Message*>(this), 1);
			m_pGifRender = pGifImage->AddRender(&notify);
			m_pGifRender->Start();
		}
		else
		{
			UI_LOG_WARN(_T("%s load gif image failed. id=%s"), FUNC_NAME, strGifID.c_str());
		}

		m_mapAttribute.erase(iter);
	}

	return true;
}

void GifPicture::OnPaint(IRenderTarget* pRenderTarget)
{
	if (NULL != m_pGifRender)
	{
		// GIF都是直接带alpha通道的，可以用原始HDC绘制。否则使用Gdipulus::Graphics.GetHDC
		// 会大大降低绘制效率
		HDC hDC = pRenderTarget->GetOriginHDC(); 
		                                                      
		m_pGifRender->OnPaint(hDC, 0,0);  // 因为HDC是已经带偏移量的，因此直接绘制在0,0即可
	}
}

void GifPicture::OnMove(CPoint ptPos)
{
	if (NULL != m_pGifRender)
	{
// 		POINT pt = this->GetRealPosInWindow();
// 		Gif_Timer_Notify notify(GetHWND(),pt.x + m_rcNonClient.left, pt.y + m_rcNonClient.top);
// 		m_pGifRender->ModifyRenderParam(&notify);
	}
}

void GifPicture::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	if (NULL != m_pGifRender && m_pGifRender->GetStatus() == GIF_DRAW_STATUS_START)
	{
		this->UpdateObject();
	}
}