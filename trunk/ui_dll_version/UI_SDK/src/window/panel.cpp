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
//                              虚函数                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


SIZE Panel::GetDesiredSize( HRDC hRDC )
{
	UIASSERT( this->m_pLayout != NULL );

	SIZE size = {0,0};
	// 已指定了大小的窗体
	if( this->m_nConfigWidth != AUTO && this->m_nConfigHeight != AUTO )
	{
		size.cx = this->m_nConfigWidth;
		size.cy = this->m_nConfigHeight;
	}
	else
	{
		// 获取子对象所需要的空间
	 	if( this->m_pLayout != NULL )
	 		size = this->m_pLayout->Measure( hRDC );

		// 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
		if( this->m_nConfigWidth != AUTO )
			size.cx = this->m_nConfigWidth;
		if( this->m_nConfigHeight!= AUTO )
			size.cy = this->m_nConfigHeight;
	}

	// 计算 margin 的大小
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

	// 布局类型
	ATTRMAP::iterator iter = mapAttrib.find(XML_LAYOUT);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pLayout);

		String& strLayout = iter->second;
		this->m_pLayout = LayoutManagerFactory::GetLayout( strLayout, this );
		if( NULL == this->m_pLayout )
		{
			UI_LOG_ERROR( _T("Panel::SetAttribute，获取属性\"%s\"失败"), XML_LAYOUT );
			// TODO. 是否需要return false;?
		}else
		{
			this->m_pLayout->SetAttribute( mapAttrib,bReload );
		}
		this->m_mapAttribute.erase( XML_LAYOUT );
	}
	else if(NULL == m_pLayout)
	{
		// 默认为canvas布局
		this->m_pLayout = LayoutManagerFactory::GetLayout( XML_LAYOUT_CANVAS, this );
		UIASSERT(NULL != m_pLayout);
		this->m_pLayout->SetAttribute( mapAttrib,bReload );
	}
 
	return true;
}
