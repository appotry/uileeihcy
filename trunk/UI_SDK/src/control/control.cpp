#include "StdAfx.h"


Control::Control(void)
{
	this->ModifyStyle(CONTROL_STYLE_TABSTOP);
}

Control::~Control(void)
{

}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 虚函数                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
**	获取对象自己期望的大小
**
**	Control::GetDesiredSize实现了如果已指定了width、height直接该大小，否则调用子类的GetAutoSize
**	Control的子类应该继承GetAutoSize虚函数进行实现
*/
SIZE Control::GetDesiredSize( HRDC hRDC )
{
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
		size = this->GetAutoSize( hRDC );

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

bool Control::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute(mapAttrib,bReload);
	if( false == bRet )
		return false;

	return true;
}


