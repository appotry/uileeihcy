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
//                                 �麯��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

/*
**	��ȡ�����Լ������Ĵ�С
**
**	Control::GetDesiredSizeʵ���������ָ����width��heightֱ�Ӹô�С��������������GetAutoSize
**	Control������Ӧ�ü̳�GetAutoSize�麯������ʵ��
*/
SIZE Control::GetDesiredSize()
{
	SIZE size = {0,0};
	// ��ָ���˴�С�Ĵ���
	if (this->m_nConfigWidth != AUTO && this->m_nConfigHeight != AUTO)
	{
		size.cx = this->m_nConfigWidth;
		size.cy = this->m_nConfigHeight;
	}
	else
	{
		// ��ȡ�Ӷ�������Ҫ�Ŀռ�
		size = this->GetAutoSize();

		// �����ָ��width��height������һ������ô��������һ���еõ���ֵ
		if (this->m_nConfigWidth != AUTO)
			size.cx = this->m_nConfigWidth;
		if (this->m_nConfigHeight!= AUTO)
			size.cy = this->m_nConfigHeight;
	}

	// ���� margin �Ĵ�С
	size.cx += this->m_rcMargin.left + this->m_rcMargin.right;
	size.cy += this->m_rcMargin.top  + this->m_rcMargin.bottom; 

	return size;
}



