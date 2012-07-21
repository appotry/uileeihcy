#include "stdafx.h"

LayoutManager* LayoutManagerFactory::GetLayout( String strType, Panel* pPanel )
{
	LayoutManager*  pLayout = NULL;

	/*if( XML_LAYOUT_NULL == strType )
	{
		pLayout = new NullLayout();
	}
	else */
	if( XML_LAYOUT_STACK == strType )
	{
		pLayout = new StackLayout();
	}
	else if( XML_LAYOUT_GRID == strType )
	{
		pLayout = new GridLayout();
	}
	else if( XML_LAYOUT_DOCK == strType )
	{
		pLayout = new DockLayout();
	}
	else if( XML_LAYOUT_CANVAS == strType )
	{
		pLayout = new CanvasLayout();
	}
	else if( XML_LAYOUT_CARD == strType )
	{
		pLayout = new CardLayout();
	}

	if( NULL == pLayout )
	{
		assert( false );
		UI_LOG_WARN( _T("LayoutManagerFactory::GetLayout，failed. styType=%s"), strType.c_str() );
	}
	else
	{
		pLayout->m_pPanel = pPanel;
	}

	return pLayout;
}


/*
**	目的：如果m_pPanel的widht、height都已被指定，那么直接返回该大小，否则
**		  将调用LayoutManager子类的MeasureChildObject来获取自己说需要的大小
*/
SIZE LayoutManager::Measure( HDC hDC )
{
	this->m_pPanel->PrepareDCForMeasure( hDC );

	SIZE size = {0,0};
	// 已指定了大小的窗体
	if( this->m_pPanel->width != AUTO && this->m_pPanel->height != AUTO )
	{
		size.cx = this->m_pPanel->width;
		size.cy = this->m_pPanel->height;
	}
	else
	{
		// 获取子对象所需要的空间
		size = this->MeasureChildObject( hDC );

		// 计算padding 的大小
		size.cx += m_pPanel->rcPadding.left + m_pPanel->rcPadding.right;
		size.cy += m_pPanel->rcPadding.top  + m_pPanel->rcPadding.bottom;

		// 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
		if( this->m_pPanel->width != AUTO )
			size.cx = this->m_pPanel->width;
		if( this->m_pPanel->height!= AUTO )
			size.cy = this->m_pPanel->height;
	}

	// 计算 margin 的大小
	size.cx += m_pPanel->rcMargin.left + m_pPanel->rcMargin.right;
	size.cy += m_pPanel->rcMargin.top  + m_pPanel->rcMargin.bottom; 

	return size;
}
void LayoutManager::Arrange( HDC hDC )
{
	this->ArrangeChildObject( hDC );
	

}

StackLayout::StackLayout()
{
	this->direction = LAYOUT_STACK_TOPTOBOTTOM;   // 默认堆栈是从上到下排布
}
void StackLayout::SetAttribute( map<String,String>& mapAttrib )
{
	if( this->m_pPanel->attribute.count( XML_LAYOUT_STACK_DIRECTION ) )
	{
		String strDirection = this->m_pPanel->attribute[ XML_LAYOUT_STACK_DIRECTION ];

		if( XML_LAYOUT_STACK_LEFTTORIGHT == strDirection )
		{
			this->direction = LAYOUT_STACK_LEFTTORIGHT;
		}
		else if( XML_LAYOUT_STACK_RIGHTTOLEFT == strDirection )
		{
			this->direction = LAYOUT_STACK_RIGHTTOLEFT;
		}
		else if( XML_LAYOUT_STACK_LEFTANDRIGHT == strDirection )
		{
			this->direction = LAYOUT_STACK_LEFTANDRIGHT;
		}
		else if( XML_LAYOUT_STACK_TOPTOBOTTOM == strDirection )
		{
			this->direction = LAYOUT_STACK_TOPTOBOTTOM;
		}
		else if( XML_LAYOUT_STACK_BOTTOMTOTOP == strDirection )
		{
			this->direction = LAYOUT_STACK_BOTTOMTOTOP;
		}
		else if( XML_LAYOUT_STACK_TOPANDBOTTOM == strDirection )
		{
			this->direction = LAYOUT_STACK_TOPANDBOTTOM;
		}
		else
		{
			UI_LOG_WARN( _T("StackLayout::SetAttribute，属性%s=\"%s\"的值不正确"), XML_LAYOUT_STACK_DIRECTION, strDirection.c_str());
		}

		this->m_pPanel->attribute.erase( XML_LAYOUT_STACK_DIRECTION );
	}
}
SIZE  StackLayout::MeasureChildObject( HDC hDC )
{
	this->m_pPanel->PrepareDCForMeasure( hDC );

	SIZE size = {0,0};

	// 已指定了大小的窗体
	if( this->m_pPanel->width != AUTO && this->m_pPanel->height != AUTO )
	{
		size.cx = this->m_pPanel->width;
		size.cy = this->m_pPanel->height;

		// 计算 margin 的大小
		size.cx += m_pPanel->rcMargin.left + m_pPanel->rcMargin.right;
		size.cy += m_pPanel->rcMargin.top  + m_pPanel->rcMargin.bottom; 
		return size;
	}

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		SIZE  s = pChild->GetDesiredSize( hDC );

		switch( this->direction )
		{
		case LAYOUT_STACK_LEFTTORIGHT:
		case LAYOUT_STACK_RIGHTTOLEFT:
		case LAYOUT_STACK_LEFTANDRIGHT:
			size.cx += s.cx;
			if( size.cy < s.cy )
				size.cy = s.cy;
			break;

		case LAYOUT_STACK_TOPTOBOTTOM:
		case LAYOUT_STACK_BOTTOMTOTOP:
		case LAYOUT_STACK_TOPANDBOTTOM:
			size.cy += s.cy;
			if( size.cx < s.cx )
				size.cx = s.cx;
			break;
		}
	}

	// 计算padding 的大小
	size.cx += m_pPanel->rcPadding.left + m_pPanel->rcPadding.right;
	size.cy += m_pPanel->rcPadding.top  + m_pPanel->rcPadding.bottom;

	// 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
	if( this->m_pPanel->width != AUTO )
		size.cx = this->m_pPanel->width;
	if( this->m_pPanel->height!= AUTO )
		size.cy = this->m_pPanel->height;

	// 计算 margin 的大小
	size.cx += m_pPanel->rcMargin.left + m_pPanel->rcMargin.right;
	size.cy += m_pPanel->rcMargin.top  + m_pPanel->rcMargin.bottom; 

	return size;
}

void  StackLayout::ArrangeChildObject( HDC hDC )
{
	// 调用该函数时，自己的大小已经被求出来了
	int  nWidth  = U::RectW( &this->m_pPanel->rectW );
	int  nHeight = U::RectH( &this->m_pPanel->rectW );

	int  nConsume1 = 0; // 当前已消耗的高度或宽度（从left/top开始计数）
	int  nConsume2 = 0; // 当前已消耗的高度或宽度（从right/bottom开始计数）

	// 初始化 nComsume1 , nComsume2
	switch( this->direction )
	{
	case LAYOUT_STACK_LEFTTORIGHT:
	case LAYOUT_STACK_LEFTANDRIGHT:
	case LAYOUT_STACK_RIGHTTOLEFT:
		nConsume1 += m_pPanel->rcPadding.left;
		nConsume2 += m_pPanel->rcPadding.right;
		break;
	case LAYOUT_STACK_TOPTOBOTTOM:
	case LAYOUT_STACK_TOPANDBOTTOM:
	case LAYOUT_STACK_BOTTOMTOTOP:
		nConsume1 += m_pPanel->rcPadding.top;
		nConsume2 += m_pPanel->rcPadding.bottom;
		break;
	}
	

	Object* pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		::SaveDC( hDC );
		
		SIZE s = pChild->GetDesiredSize( hDC );

		switch( this->direction )
		{    
		case LAYOUT_STACK_LEFTTORIGHT:
			::SetRect( &pChild->rectP, 
				nConsume1         + pChild->rcMargin.left , 
				0                 + pChild->rcMargin.top     + m_pPanel->rcPadding.top, 
				nConsume1 + s.cx  - pChild->rcMargin.right, 
				nHeight           - pChild->rcMargin.bottom  - m_pPanel->rcPadding.bottom);
			nConsume1 += s.cx;
			break;

		case LAYOUT_STACK_RIGHTTOLEFT:
			::SetRect( &pChild->rectP, 
				nWidth - nConsume2 - s.cx         + pChild->rcMargin.left, 
				0                                 + pChild->rcMargin.top       + m_pPanel->rcPadding.top, 
				nWidth - nConsume2                - pChild->rcMargin.right, 
				nHeight                           - pChild->rcMargin.bottom    - m_pPanel->rcPadding.bottom);
			nConsume2 += s.cx;
			break;

		case LAYOUT_STACK_LEFTANDRIGHT:
			{
				bool bDockLeft = true;   // 默认停靠在左侧

				// 判断按钮是停在哪一侧
				if( pChild->attribute.count( XML_LAYOUT_STACK_DOCK ) )
				{
					String  strDockType = pChild->attribute[XML_LAYOUT_STACK_DOCK];
					if( XML_LAYOUT_STACK_DOCK_RIGHT == strDockType )
					{
						bDockLeft = false;
					}
					else if( XML_LAYOUT_STACK_DOCK_LEFT == strDockType )
					{
						bDockLeft = true;
					}
					else
					{
						bDockLeft = true;
						UI_LOG_WARN( _T("StackLayout::Arrange，错误的值：%s=\"%s\"，将默认为%s"), 
							XML_LAYOUT_STACK_DOCK, strDockType.c_str(), XML_LAYOUT_STACK_DOCK_LEFT );
					}
				}else
				{
					bDockLeft = true;
				}

				// 计算位置
				if( bDockLeft )
				{
					::SetRect( &pChild->rectP, 
						nConsume1         + pChild->rcMargin.left, 
						0                 + pChild->rcMargin.top         + m_pPanel->rcPadding.top, 
						nConsume1 + s.cx  - pChild->rcMargin.right, 
						nHeight           - pChild->rcMargin.bottom      - m_pPanel->rcPadding.bottom);
					nConsume1 += s.cx;
				}
				else
				{
					::SetRect( &pChild->rectP, 
						nWidth - nConsume2 - s.cx         + pChild->rcMargin.left, 
						0                                 + pChild->rcMargin.top       + m_pPanel->rcPadding.top, 
						nWidth - nConsume2                - pChild->rcMargin.right, 
						nHeight                           - pChild->rcMargin.bottom    - m_pPanel->rcPadding.bottom );
					nConsume2 += s.cx;
				}

			}
			break;


			// 从上到下
		case LAYOUT_STACK_TOPTOBOTTOM:
			::SetRect( &pChild->rectP, 
				0                 + pChild->rcMargin.left   + m_pPanel->rcPadding.left, 
				nConsume1         + pChild->rcMargin.top, 
				nWidth            - pChild->rcMargin.right  - m_pPanel->rcPadding.right, 
				nConsume1 + s.cy  - pChild->rcMargin.bottom );
			nConsume1 += s.cy;
			break;
		case LAYOUT_STACK_BOTTOMTOTOP:
			::SetRect( &pChild->rectP, 
				0                                  + pChild->rcMargin.left   + m_pPanel->rcPadding.left, 
				nHeight - nConsume2 - s.cy         + pChild->rcMargin.top, 
				nWidth                             - pChild->rcMargin.right  - m_pPanel->rcPadding.right, 
				nHeight - nConsume2                - pChild->rcMargin.bottom );
			nConsume2 += s.cy;
			break;
		case LAYOUT_STACK_TOPANDBOTTOM:
			{
				bool bDockTop = true;   // 默认停靠在上侧

				// 判断按钮是停在哪一侧
				if( pChild->attribute.count( XML_LAYOUT_STACK_DOCK ) )
				{
					String  strDockType = pChild->attribute[XML_LAYOUT_STACK_DOCK];
					if( XML_LAYOUT_STACK_DOCK_BOTTOM == strDockType )
					{
						bDockTop = false;
					}
					else if( XML_LAYOUT_STACK_DOCK_TOP == strDockType )
					{
						bDockTop = true;
					}
					else
					{
						bDockTop = true;
						UI_LOG_WARN( _T("StackLayout::Arrange，错误的值：%s=\"%s\"，将默认为%s"), 
							XML_LAYOUT_STACK_DOCK, strDockType.c_str(), XML_LAYOUT_STACK_DOCK_TOP );
					}
				}else
				{
					bDockTop = true;
				}

				// 计算位置
				if( bDockTop )
				{
					::SetRect( &pChild->rectP, 
						0                 + pChild->rcMargin.left     + m_pPanel->rcPadding.left, 
						nConsume1         + pChild->rcMargin.top, 
						nWidth            - pChild->rcMargin.right    - m_pPanel->rcPadding.right, 
						nConsume1 + s.cy  - pChild->rcMargin.bottom );
					nConsume1 += s.cy;
				}
				else
				{
					::SetRect( &pChild->rectP, 
						0                                  + pChild->rcMargin.left   + m_pPanel->rcPadding.left, 
						nHeight - nConsume2 - s.cy         + pChild->rcMargin.top, 
						nWidth                             - pChild->rcMargin.right  - m_pPanel->rcPadding.right, 
						nHeight - nConsume2                - pChild->rcMargin.bottom );
					nConsume2 += s.cy;
				}

			}
			break;
		}
 
		pChild->UpdateRect();

		::RestoreDC( hDC, -1 );
	}

	// 递归
	pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		// 只有Panel才需要去布局
		if( pChild->GetObjectType() != OBJ_CONTROL )
			((Panel*)pChild)->GetLayout()->Arrange( hDC );
	}

}
//
//SIZE  NullLayout::Measure( HDC hDC )
//{
//	SIZE size = {0,0};
//	return size;
//}
//void  NullLayout::Arrange( HDC hDC )
//{
//
//}

CanvasLayout::CanvasLayout()
{
}

SIZE  CanvasLayout::MeasureChildObject( HDC hDC )
{
	this->m_pPanel->PrepareDCForMeasure( hDC );

	SIZE size = {0,0};

	// 已指定了大小的窗体
	if( this->m_pPanel->width != AUTO && this->m_pPanel->height != AUTO )
	{
		size.cx = this->m_pPanel->width;
		size.cy = this->m_pPanel->height;

		// 计算 margin 的大小
		size.cx += m_pPanel->rcMargin.left + m_pPanel->rcMargin.right;
		size.cy += m_pPanel->rcMargin.top  + m_pPanel->rcMargin.bottom; 
		return size;
	}

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		SIZE  s = pChild->GetDesiredSize( hDC );
		
		if( pChild->attribute.count(XML_LAYOUT_CANVAS_LEFT) )
		{
			s.cx += _ttoi( pChild->attribute[XML_LAYOUT_CANVAS_LEFT].c_str() );
		}
		else if( pChild->attribute.count(XML_LAYOUT_CANVAS_RIGHT) )
		{
			s.cx += _ttoi( pChild->attribute[XML_LAYOUT_CANVAS_RIGHT].c_str() );
		}
		if( pChild->attribute.count(XML_LAYOUT_CANVAS_TOP) )
		{
			s.cy += _ttoi( pChild->attribute[XML_LAYOUT_CANVAS_TOP].c_str() );
		}
		else if( pChild->attribute.count(XML_LAYOUT_CANVAS_BOTTOM) )
		{
			s.cy += _ttoi( pChild->attribute[XML_LAYOUT_CANVAS_BOTTOM].c_str() );
		}
		
		if( size.cx < s.cx )
			size.cx = s.cx;
		if( size.cy < s.cy )
			size.cy = s.cy;
	}

	// 计算padding 的大小
	size.cx += m_pPanel->rcPadding.left + m_pPanel->rcPadding.right;
	size.cy += m_pPanel->rcPadding.top  + m_pPanel->rcPadding.bottom;

	// 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
	if( this->m_pPanel->width != AUTO )
		size.cx = this->m_pPanel->width;
	if( this->m_pPanel->height!= AUTO )
		size.cy = this->m_pPanel->height;

	// 计算 margin 的大小
	size.cx += m_pPanel->rcMargin.left + m_pPanel->rcMargin.right;
	size.cy += m_pPanel->rcMargin.top  + m_pPanel->rcMargin.bottom; 

	return size;
}
void  CanvasLayout::ArrangeChildObject( HDC hDC )
{
	// 调用该函数时，自己的大小已经被求出来了
	int  nWidth  = U::RectW( &this->m_pPanel->rectW );
	int  nHeight = U::RectH( &this->m_pPanel->rectW );

	Object* pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		::SaveDC( hDC );
		
		// 计算出 pChild 的 rectP
		SIZE s = pChild->GetDesiredSize( hDC );
		::SetRect( &pChild->rectP, 
			0         , 
			0         , 
			s.cx      - pChild->rcMargin.top  - pChild->rcMargin.right, 
			s.cy      - pChild->rcMargin.left - pChild->rcMargin.bottom );


		int  x = 0, y = 0;                                  // pChild最终在parent中的坐标
		int  left=NDEF, top=NDEF, right=NDEF, bottom=NDEF;  // pChild的xml属性

		// 从XML中读取属性值
		if( pChild->attribute.count( XML_LAYOUT_CANVAS_LEFT ) )
		{
			String  strLeft = pChild->attribute[XML_LAYOUT_CANVAS_LEFT];
			left = _ttoi( strLeft.c_str() );
		}
		if( pChild->attribute.count( XML_LAYOUT_CANVAS_TOP ) )
		{
			String  strTop = pChild->attribute[XML_LAYOUT_CANVAS_TOP];
			top = _ttoi( strTop.c_str() );
		}
		if( pChild->attribute.count( XML_LAYOUT_CANVAS_RIGHT ) )
		{
			String  strRight = pChild->attribute[XML_LAYOUT_CANVAS_RIGHT];
			right = _ttoi( strRight.c_str() );
		}
		if( pChild->attribute.count( XML_LAYOUT_CANVAS_BOTTOM ) )
		{
			String  strBottom = pChild->attribute[XML_LAYOUT_CANVAS_BOTTOM];
			bottom = _ttoi( strBottom.c_str() );
		}

		// 计算出坐标，若left/right,top/bottom中有一个未指定的，那么取0（但在DesktopLayout中是取居中）
		if( left != NDEF )
		{
			x = left;
			x += pChild->rcMargin.left + m_pPanel->rcPadding.left;
		}
		else
		{
			if( right != NDEF )
			{
				x = nWidth - right - U::RectW(&pChild->rectP);
				x -= (pChild->rcMargin.right + m_pPanel->rcPadding.right);
			}
			else
			{
				//x = ( nWidth - s.cx ) / 2;  // 居中
				x = 0;
				x += pChild->rcMargin.left + m_pPanel->rcPadding.left;
			}
		}
		if( top != NDEF )
		{
			y = top;
			y += pChild->rcMargin.top + m_pPanel->rcPadding.right;
		}
		else
		{
			if( bottom != NDEF )
			{
				y = nHeight - bottom - U::RectH(&pChild->rectP);
				y -= (pChild->rcMargin.bottom + pChild->GetParentObject()->rcPadding.bottom);
			}
			else
			{
				
				//y = ( nHeight - s.cy ) / 2;  // 居中
				y = 0;
				y += pChild->rcMargin.top + pChild->GetParentObject()->rcPadding.right;
			}
		}

		::OffsetRect( &pChild->rectP, x, y );

 
		// 由 rectP得到 rectW
		pChild->UpdateRect();

		::RestoreDC( hDC, -1 );
	}

	// 递归
	pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		// 只有Panel才需要去布局
		if( pChild->GetObjectType() != OBJ_CONTROL )
			((Panel*)pChild)->GetLayout()->Arrange( hDC );
	}
}


SIZE  GridLayout::MeasureChildObject( HDC hDC )
{
	SIZE size = {0,0};
	return size;
}
void  GridLayout::ArrangeChildObject( HDC hDC )
{

}

SIZE  DockLayout::MeasureChildObject( HDC hDC )
{
	SIZE size = {0,0};
	return size;
}
void  DockLayout::ArrangeChildObject( HDC hDC )
{

}
SIZE  CardLayout::MeasureChildObject( HDC hDC )
{
	SIZE size = {0,0};
	return size;
}
void  CardLayout::ArrangeChildObject( HDC hDC )
{

}

DesktopLayout::DesktopLayout()
{
	this->left = this->top = this->right = this->bottom = NDEF;
}
 


void  DesktopLayout::Arrange( HDC hDC )
{
	SIZE s = this->m_pWindow->GetLayout()->Measure( hDC );  // 获得的SIZE包括了MARGIN的大小 s.cx=margin.left+width+margin.right

	// 窗口的client大小
	::SetRect( &this->m_pWindow->rectW, 
			0         , 
			0         , 
			s.cx      - m_pWindow->rcMargin.top  - m_pWindow->rcMargin.right, 
			s.cy      - m_pWindow->rcMargin.left - m_pWindow->rcMargin.bottom );

	// 窗口的屏幕宽度和高度
	this->CalcWindowRectByClinetRect( &this->m_pWindow->rectW, &this->m_pWindow->rectP  );   // 现在rectP中还只是包含了高度和宽度，并没有位置

	// 读取其他属性值来设置rectW
	int  x = 0, y = 0;                                 // 最终在屏幕中的坐标
	int  nCXScreen = 800, nCYScreen = 600;             // 屏幕大小

	// 从XML中读取属性值
	if( this->m_pWindow->attribute.count( XML_LAYOUT_CANVAS_LEFT ) )
	{
		String  strLeft = this->m_pWindow->attribute[XML_LAYOUT_CANVAS_LEFT];
		this->left = _ttoi( strLeft.c_str() );
	}
	if( this->m_pWindow->attribute.count( XML_LAYOUT_CANVAS_TOP ) )
	{
		String  strTop = this->m_pWindow->attribute[XML_LAYOUT_CANVAS_TOP];
		this->top = _ttoi( strTop.c_str() );
	}
	if( this->m_pWindow->attribute.count( XML_LAYOUT_CANVAS_RIGHT ) )
	{
		String  strRight = this->m_pWindow->attribute[XML_LAYOUT_CANVAS_RIGHT];
		this->right = _ttoi( strRight.c_str() );
	}
	if( this->m_pWindow->attribute.count( XML_LAYOUT_CANVAS_BOTTOM ) )
	{
		String  strBottom = this->m_pWindow->attribute[XML_LAYOUT_CANVAS_BOTTOM];
		this->bottom = _ttoi( strBottom.c_str() );
	}

	// 计算出坐标
	nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
	nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );
	if( this->left != NDEF )
	{
		x = this->left;
		x += m_pWindow->rcMargin.left;
	}
	else
	{
		if( this->right != NDEF )
		{
			x = nCXScreen - this->right - U::RectW(&this->m_pWindow->rectP);  // right是指窗口右侧距离屏幕右侧的距离
			x -= m_pWindow->rcMargin.right;
		}
		else
		{
			// 居中
			x = ( nCXScreen - U::RectW(&this->m_pWindow->rectP) ) / 2;
		}
	}
	if( this->top != NDEF )
	{
		y = this->top;
		y += m_pWindow->rcMargin.top;
	}
	else
	{
		if( this->bottom != NDEF )
		{
			y = nCYScreen - this->bottom - U::RectH(&this->m_pWindow->rectP); // 同right
			y -= m_pWindow->rcMargin.bottom;
		}
		else
		{
			// 居中
			y = ( nCYScreen - U::RectH(&this->m_pWindow->rectP) ) / 2;
		}
	}

	::OffsetRect( &this->m_pWindow->rectP, x, y );

	// 递归
	this->m_pWindow->GetLayout()->Arrange( hDC );
}
 


//
//	如果已知一个窗口的client区域大小，求这个窗口的window区域大小
//
//	参数
//		rcClient
//			[in]	该窗口的客户区大小（默认left = 0, top = 0 )
//		rcWindow
//			[out]	返回窗口的window大小（默认left=0,top=0，即只返回窗口大小）
//
void DesktopLayout::CalcWindowRectByClinetRect( RECT* rcClient, RECT* rcWindow )
{
	RECT  tempRcWindow, tempRcClient;

	NCCALCSIZE_PARAMS     np;
	WINDOWPOS             wp;

	// 通过 WM_NCCALCSIZE 消息，让系统告诉我们一个window rect 对应的client rect是多少
	np.lppos = &wp;
	::GetWindowRect( this->m_pWindow->m_hWnd, &np.rgrc[0] );
	::CopyRect( &tempRcWindow, &np.rgrc[0] );
	::SendMessage( this->m_pWindow->m_hWnd, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&np );
	::CopyRect( &tempRcClient, &np.rgrc[0] );

	rcWindow->left   = rcWindow->top = 0;
	rcWindow->right  = U::RectW( rcClient ) + U::RectW( &tempRcWindow )-U::RectW( &tempRcClient );
	rcWindow->bottom = U::RectH( rcClient ) + U::RectH( &tempRcWindow )-U::RectH( &tempRcClient );
}