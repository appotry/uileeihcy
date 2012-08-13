#include "stdafx.h"


Panel::Panel(void)
{
	this->m_pLayout = NULL;
	
}
Panel::~Panel()
{
}

void Panel::DestroyUI()
{
	Object::DestroyUI();
	SAFE_DELETE(m_pLayout);
}

Layout* Panel::GetLayout()
{
	return this->m_pLayout; 
}

void  Panel::SetLayout(Layout*  pLayout)
{
	assert( pLayout != NULL );

	if( NULL != this->m_pLayout )
	{
		delete this->m_pLayout;
		this->m_pLayout = NULL;
	}

	this->m_pLayout = pLayout;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                              �麯��                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


SIZE Panel::GetDesiredSize( HRDC hRDC )
{
	UIASSERT( this->m_pLayout != NULL );

	SIZE s = {0,0};
	if( this->m_pLayout != NULL )
		s = this->m_pLayout->Measure( hRDC );

	return s;
}

void Panel::ResetAttribute()
{
	Object::ResetAttribute();
	SAFE_DELETE(m_pLayout);
	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);
}
bool Panel::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = Object::SetAttribute( mapAttrib, bReload );
	if( false == bRet )	return false;

	// ��������
	if( this->m_mapAttribute.count( XML_LAYOUT ) )
	{
		String strLayout = this->m_mapAttribute[ XML_LAYOUT ];
		this->m_pLayout = LayoutManagerFactory::GetLayout( strLayout, this );
		if( NULL == this->m_pLayout )
		{
			UI_LOG_ERROR( _T("Panel::SetAttribute����ȡ����\"%s\"ʧ��"), XML_LAYOUT );
			// TODO. �Ƿ���Ҫreturn false;?
		}else
		{
			this->m_pLayout->SetAttribute( mapAttrib,bReload );
		}
		this->m_mapAttribute.erase( XML_LAYOUT );
	}
	else
	{
		// Ĭ��Ϊcanvas����
		this->m_pLayout = LayoutManagerFactory::GetLayout( XML_LAYOUT_CANVAS, this );
		UIASSERT(NULL != m_pLayout);
		this->m_pLayout->SetAttribute( mapAttrib,bReload );
	}
 
	return true;
}
