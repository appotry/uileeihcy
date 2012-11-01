#include "stdafx.h"

Label::Label()
{
	this->SetTabstop( false );   // Label �����н���
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
//                                 �麯��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void Label::ResetAttribute()
{
	Control::ResetAttribute();

	this->ModifyStyle( OBJECT_STYLE_TRANSPARENT );
}
bool Label::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute( mapAttrib,bReload );
	if( false == bRet )	return bRet;

	// ����
	ATTRMAP::iterator iter = m_mapAttribute.find(XML_TEXT);
	if (false==bReload && m_mapAttribute.end() != iter)
	{
		this->m_strText = iter->second;
		m_mapAttribute.erase(iter);
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
	this->SetTabstop( false );   // Picture �����н���
}
Picture::~Picture()
{
}

SIZE Picture::GetAutoSize( HRDC hDC )
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

		m_pForegndRender->DrawState( hRDC, &rc, 0 );   // TODO: ���������չΪ֧��hover/press״̬
	}
}


//////////////////////////////////////////////////////////////////////////

GifPicture::GifPicture()
{
	m_pGifImage = NULL;
	b = true;
}
GifPicture::~GifPicture()
{
	SAFE_DELETE(m_pGifImage);
}

SIZE GifPicture::GetAutoSize( HRDC hDC )
{
	SIZE s = {0, 0};
	if (NULL != m_pGifImage)
	{
		s.cx = m_pGifImage->GetWidth();
		s.cy = m_pGifImage->GetHeight();
	}
	return s;
}

bool GifPicture::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	ATTRMAP::iterator iter = m_mapAttribute.find(XML_GIFPICTURE_PATH);
	if (iter != m_mapAttribute.end())
	{
		String& strPath = iter->second;

		if (NULL == m_pGifImage)
		{
			m_pGifImage = new GifImage;
		}
		m_pGifImage->Destroy();
		bool bRet = m_pGifImage->Load(strPath.c_str());
		if (false == bRet)
		{
			UI_LOG_WARN(_T("%s load gif image failed. path=%s"), FUNC_NAME, strPath.c_str());
		}
		m_mapAttribute.erase(iter);
	}

	return true;
}

void GifPicture::OnPaint( HRDC hRDC )
{
	if (NULL != m_pGifImage)
	{
		if (b)
		{
			CRect rc;
			this->GetWindowRect(&rc);
			m_pGifImage->SetDrawParam(GetHWND(),rc.left, rc.top);
			m_pGifImage->Start();

			b = false;
		}
		HDC hDC = GetHDC(hRDC);
		m_pGifImage->OnPaint(hDC, 0,0);  // ��ΪHDC���Ѿ���ƫ�����ģ����ֱ�ӻ�����0,0����
		ReleaseHDC(hRDC, hDC);
	}
}