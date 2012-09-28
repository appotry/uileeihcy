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

	SIZE size = {0,0};
	// ��ָ���˴�С�Ĵ���
	if( this->m_nConfigWidth != AUTO && this->m_nConfigHeight != AUTO )
	{
		size.cx = this->m_nConfigWidth;
		size.cy = this->m_nConfigHeight;
	}
	else
	{
		// ��ȡ�Ӷ�������Ҫ�Ŀռ�
	 	if( this->m_pLayout != NULL )
	 		size = this->m_pLayout->Measure( hRDC );

		// �����ָ��width��height������һ������ô��������һ���еõ���ֵ
		if( this->m_nConfigWidth != AUTO )
			size.cx = this->m_nConfigWidth;
		if( this->m_nConfigHeight!= AUTO )
			size.cy = this->m_nConfigHeight;
	}

	// ���� margin �Ĵ�С
	size.cx += this->m_rcMargin.left + this->m_rcMargin.right;
	size.cy += this->m_rcMargin.top  + this->m_rcMargin.bottom; 

	return size;
}

void Panel::ResetAttribute()
{
	__super::ResetAttribute();
	SAFE_DELETE(m_pLayout);
	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);
}
bool Panel::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Object::SetAttribute( mapAttrib, bReload );
	if( false == bRet )	return false;

	// ��������
	ATTRMAP::iterator iter = mapAttrib.find(XML_LAYOUT);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pLayout);

		String& strLayout = iter->second;
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
	else if(NULL == m_pLayout)
	{
		// Ĭ��Ϊcanvas����
		this->m_pLayout = LayoutManagerFactory::GetLayout( XML_LAYOUT_CANVAS, this );
		UIASSERT(NULL != m_pLayout);
		this->m_pLayout->SetAttribute( mapAttrib,bReload );
	}
 
	return true;
}
