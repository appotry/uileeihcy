#include "stdafx.h"

Layout* LayoutManagerFactory::GetLayout( String strType, Panel* pPanel )
{
	Layout*  pLayout = NULL;

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
SIZE Layout::Measure( HRDC hRDC )
{
	SIZE size = {0,0};
	// 已指定了大小的窗体
	if( this->m_pPanel->GetConfigWidth() != AUTO && this->m_pPanel->GetConfigHeight() != AUTO )
	{
		size.cx = this->m_pPanel->GetConfigWidth();
		size.cy = this->m_pPanel->GetConfigHeight();
	}
	else
	{
		// 获取子对象所需要的空间
		size = this->MeasureChildObject( hRDC );

		// 计算padding 的大小
		size.cx += m_pPanel->GetPaddingW();
		size.cy += m_pPanel->GetPaddingH();

		// 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
		if( this->m_pPanel->GetConfigWidth() != AUTO )
			size.cx = this->m_pPanel->GetConfigWidth();
		if( this->m_pPanel->GetConfigHeight() != AUTO )
			size.cy = this->m_pPanel->GetConfigHeight();

		if( m_pPanel->GetObjectType() == OBJ_WINDOW )
		{
			// 这里获取到的是client大小，需要转换成window大小
			((WindowBase*)m_pPanel)->CalcWindowSizeByClientSize( size, &size );
		}
	}

	// 计算 margin 的大小
	size.cx += m_pPanel->GetMarginW();
	size.cy += m_pPanel->GetMarginH(); 

	return size;
}
void Layout::Arrange( HRDC hRDC )
{
	this->ArrangeChildObject( hRDC );
	
	// 递归
	Object*  pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		// 只有Panel才需要去布局
		if( pChild->GetObjectType() == OBJ_PANEL ||
			pChild->GetObjectType() == OBJ_WINDOW )
		{
			((Panel*)pChild)->GetLayout()->Arrange( hRDC );
		}
	}
}

StackLayout::StackLayout()
{
	this->direction = LAYOUT_STACK_LEFTTORIGHT;   // 默认堆栈是从左到右
}
bool StackLayout::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	String strDirection;
	if( this->m_pPanel->GetAttribute( XML_LAYOUT_STACK_DIRECTION, strDirection ) )
	{
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

		this->m_pPanel->EraseAttribute( XML_LAYOUT_STACK_DIRECTION );
	}
	return true;
}
SIZE  StackLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE  size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		SIZE  s = pChild->GetDesiredSize( hRDC );
		if( !pChild->IsVisible() )
			continue;

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

	return size;
}

void  StackLayout::ArrangeChildObject( HRDC hRDC, Object* pObjToArrage, bool bReDraw )
{
	// 调用该函数时，自己的大小已经被求出来了
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int  nConsume1 = 0; // 当前已消耗的高度或宽度（从left/top开始计数）
	int  nConsume2 = 0; // 当前已消耗的高度或宽度（从right/bottom开始计数）

	// 初始化 nComsume1 , nComsume2
	switch( this->direction )
	{
	case LAYOUT_STACK_LEFTTORIGHT:
	case LAYOUT_STACK_LEFTANDRIGHT:
	case LAYOUT_STACK_RIGHTTOLEFT:
		nConsume1 += m_pPanel->GetPaddingL();
		nConsume2 += m_pPanel->GetPaddingR();
		break;
	case LAYOUT_STACK_TOPTOBOTTOM:
	case LAYOUT_STACK_TOPANDBOTTOM:
	case LAYOUT_STACK_BOTTOMTOTOP:
		nConsume1 += m_pPanel->GetPaddingT();
		nConsume2 += m_pPanel->GetPaddingB();
		break;
	}
	
	bool bStartToArrange = false;
	if( NULL == pObjToArrage )
		bStartToArrange = true;

	Object* pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		if( pObjToArrage != NULL && pChild == pObjToArrage )
			bStartToArrange = true;

		if( !pChild->IsVisible() )
			continue;

		SIZE s;
		if( bStartToArrange )
		{
			s = pChild->GetDesiredSize( hRDC );
		}
		else
		{
			s.cx = pChild->GetWidth()+pChild->GetMarginW();
			s.cy = pChild->GetHeight()+pChild->GetMarginH();
		}
			

		switch( this->direction )
		{    
		case LAYOUT_STACK_LEFTTORIGHT:
			pChild->SetObjectPos( 
				nConsume1 + pChild->GetMarginL(), 
				0 + pChild->GetMarginT() + m_pPanel->GetPaddingT(), 
				s.cx - pChild->GetMarginW(), 
				nHeight - pChild->GetMarginH() - m_pPanel->GetPaddingH(),
				bReDraw?0:SWP_NOREDRAW);

			nConsume1 += s.cx;
			break;

		case LAYOUT_STACK_RIGHTTOLEFT:
			pChild->SetObjectPos(
				nWidth - nConsume2 - s.cx + pChild->GetMarginL(), 
				0 + pChild->GetMarginT() + m_pPanel->GetPaddingT(), 
				s.cx - pChild->GetMarginW(), 
				nHeight - pChild->GetMarginH() - m_pPanel->GetPaddingH(),
				bReDraw?0:SWP_NOREDRAW);
			nConsume2 += s.cx;
			break;

		case LAYOUT_STACK_LEFTANDRIGHT:
			{
				bool bDockLeft = true;   // 默认停靠在左侧

				// 判断按钮是停在哪一侧
				String strDockType;
				if( pChild->GetAttribute( XML_LAYOUT_STACK_DOCK, strDockType ) )
				{
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
					pChild->SetObjectPos( 
						nConsume1 + pChild->GetMarginL(), 
						0 + pChild->GetMarginT() + m_pPanel->GetPaddingT(), 
						s.cx - pChild->GetMarginW(), 
						nHeight - pChild->GetMarginH() - m_pPanel->GetPaddingH(),
						bReDraw?0:SWP_NOREDRAW);
					nConsume1 += s.cx;
				}
				else
				{
					pChild->SetObjectPos( 
						nWidth - nConsume2 - s.cx + pChild->GetMarginL(), 
						0 + pChild->GetMarginT() + m_pPanel->GetPaddingT(), 
						s.cx - pChild->GetMarginW(), 
						nHeight - pChild->GetMarginH() - m_pPanel->GetPaddingH(),
						bReDraw?0:SWP_NOREDRAW);
					nConsume2 += s.cx;
				}

			}
			break;


			// 从上到下
		case LAYOUT_STACK_TOPTOBOTTOM:
			pChild->SetObjectPos(
				0 + pChild->GetMarginL() + m_pPanel->GetPaddingL(), 
				nConsume1 + pChild->GetMarginT(), 
				nWidth            - pChild->GetMarginW()  - m_pPanel->GetPaddingW(), 
				s.cy - pChild->GetMarginH(),
				bReDraw?0:SWP_NOREDRAW);
			nConsume1 += s.cy;
			break;
		case LAYOUT_STACK_BOTTOMTOTOP:
			pChild->SetObjectPos(
				0 + pChild->GetMarginL() + m_pPanel->GetPaddingL(), 
				nHeight - nConsume2 - s.cy + pChild->GetMarginT(), 
				nWidth - pChild->GetMarginW() - m_pPanel->GetPaddingW(), 
				s.cy - pChild->GetMarginH(),
				bReDraw?0:SWP_NOREDRAW );
			nConsume2 += s.cy;
			break;
		case LAYOUT_STACK_TOPANDBOTTOM:
			{
				bool bDockTop = true;   // 默认停靠在上侧

				// 判断按钮是停在哪一侧
				String  strDockType;
				if( pChild->GetAttribute( XML_LAYOUT_STACK_DOCK, strDockType ) )
				{
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
					pChild->SetObjectPos( 
						0 + pChild->GetMarginL() + m_pPanel->GetPaddingL(), 
						nConsume1 + pChild->GetMarginT(), 
						nWidth - pChild->GetMarginW() - m_pPanel->GetPaddingW(), 
						s.cy - pChild->GetMarginH(),
						bReDraw?0:SWP_NOREDRAW);
					nConsume1 += s.cy;
				}
				else
				{
					pChild->SetObjectPos( 
						0 + pChild->GetMarginL()  + m_pPanel->GetPaddingL(), 
						nHeight - nConsume2 - s.cy + pChild->GetMarginT(), 
						nWidth - pChild->GetMarginW()  - m_pPanel->GetPaddingW(), 
						s.cy - pChild->GetMarginH(),
						bReDraw?0:SWP_NOREDRAW);
					nConsume2 += s.cy;
				}

			}
			break;
		}
 
		//::RestoreDC( hDC, -1 );
	}

}
void  StackLayout::OnChildObjectWindowPosChaned( Object* pObj )
{

}
CanvasLayout::CanvasLayout()
{
}

SIZE  CanvasLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		SIZE  s = pChild->GetDesiredSize( hRDC );
		
		String strAttribute;
		if( pChild->GetAttribute(XML_LAYOUT_CANVAS_LEFT, strAttribute) )
		{
			s.cx += _ttoi( strAttribute.c_str() );
		}
		else if( pChild->GetAttribute(XML_LAYOUT_CANVAS_RIGHT, strAttribute) )
		{
			s.cx += _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute(XML_LAYOUT_CANVAS_TOP, strAttribute) )
		{
			s.cy += _ttoi( strAttribute.c_str() );
		}
		else if( pChild->GetAttribute(XML_LAYOUT_CANVAS_BOTTOM, strAttribute) )
		{
			s.cy += _ttoi( strAttribute.c_str() );
		}
		
		if( size.cx < s.cx )
			size.cx = s.cx;
		if( size.cy < s.cy )
			size.cy = s.cy;
	}
	return size;
}
void  CanvasLayout::ArrangeChildObject( HRDC hRDC, Object* pObjToArrage, bool bReDraw )
{
	// 调用该函数时，自己的大小已经被求出来了
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	Object* pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		if( NULL != pObjToArrage && pObjToArrage != pChild )
			continue;

		if( !pChild->IsVisible() )
		{
			if( NULL != pObjToArrage )
				break;
			else
				continue;
		}
	

		int  x = 0, y = 0;                                  // pChild最终在parent中的坐标
		int  left=NDEF, top=NDEF, right=NDEF, bottom=NDEF;  // pChild的xml属性

		// 从XML中读取属性值来决定 pChild的rectP的left、top
		String strAttribute; 
		if( pChild->GetAttribute( XML_LAYOUT_CANVAS_LEFT, strAttribute ) )
		{ 
			left = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_CANVAS_TOP, strAttribute ) )
		{
			top = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_CANVAS_RIGHT, strAttribute ) )
		{
			right = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_CANVAS_BOTTOM, strAttribute ) )
		{
			bottom = _ttoi( strAttribute.c_str() );
		}

		//////////////////////////////////////////////////////////////////////////
		// 计算出 pChild 的 rectP的宽和高

		SIZE s = {0,0};
		if( left == NDEF || right == NDEF || top == NDEF || bottom == NDEF )
		{
			s = pChild->GetDesiredSize( hRDC );
		}

		// 如果同时指定了left/right,则忽略width属性
		if (left != NDEF && right != NDEF)
		{
			s.cx = nWidth - left - right;
		}
		// 如果同时指定了top/bottom，则忽略height属性
		if (top != NDEF && bottom != NDEF)
		{
			s.cy = nHeight - top - bottom;
		}

		CRect rcChildObj ;
		int nChildW = s.cx - pChild->GetMarginW();
		int nChildH = s.cy - pChild->GetMarginH();
		rcChildObj.SetRect(0, 0, nChildW, nChildH );


		// 计算出坐标，若left/right,top/bottom中有一个未指定的，那么取0（但在DesktopLayout中是取居中）
		if( left != NDEF )
		{
			x = left;
			x += pChild->GetMarginL();
		}
		else
		{
			if( right != NDEF )
			{
				x = nWidth - right - nChildW;
				x -= pChild->GetMarginR();
			}
			else
			{
				//x = ( nWidth - s.cx ) / 2;  // 居中
				x = 0;
				x += pChild->GetMarginL();
			}
		}
		if( top != NDEF )
		{
			y = top;
			y += pChild->GetMarginT();
		}
		else
		{
			if( bottom != NDEF )
			{
				y = nHeight - bottom - nChildH;
				y -= pChild->GetMarginB();
			}
			else
			{
				
				//y = ( nHeight - s.cy ) / 2;  // 居中
				y = 0;
				y += pChild->GetMarginT();
			}
		}

		rcChildObj.OffsetRect(x, y);
		pChild->SetObjectPos( &rcChildObj, bReDraw?0:SWP_NOREDRAW );

		if( NULL != pObjToArrage && pObjToArrage == pChild )
			break;
	}

}

GridWH::GridWH()
{
	this->last = 0;
	this->xml = 0;
	this->type = GWHT_AUTO;
}
GridLayout::~GridLayout()
{
	this->widths.clear();
	this->heights.clear();
}
bool  GridLayout::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	/*
		1. 解析Grid的每一列的宽度
	*/
	String strWidth;
	if( this->m_pPanel->GetAttribute( XML_LAYOUT_GRID_WIDTH,strWidth ) )
	{
		this->m_pPanel->EraseAttribute( XML_LAYOUT_GRID_WIDTH );
	}
	else
	{
		// 如果对于gridlayout没有定义layout.width，则默认layout.width="*"
		strWidth = XML_ASTERISK;
	}

	vector<String>  strVec;
	UI_Split( strWidth, XML_SEPARATOR, strVec );
	int nCount = (int)strVec.size();
	for( int i = 0; i < nCount; i++ )
	{
		String  str = strVec[i];
		// 判断是否为 AUTO 类型
		if( XML_AUTO == str )
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->widths.push_back( wh );
			continue;
		}

		// 判读是否为 * 类型
		int nIndex = str.find( XML_ASTERISK );
		if( String::npos != nIndex )
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi( str.c_str() );
			if( nXml == 0 )  // 如果只指定 "*"，那么表示"1*"
				nXml = 1;

			wh.xml = nXml;
			this->widths.push_back( wh );
			continue;
		}

		// 判断其他情况
		GridWH  wh;
		wh.type = GWHT_VALUE;
		wh.last = _ttoi( str.c_str() );
		this->widths.push_back( wh );

		continue;
	}

	 

	/*
		2. 解析Grid的每一列的高度
	*/
	String strHeight;
	if( this->m_pPanel->GetAttribute( XML_LAYOUT_GRID_HEIGHT, strHeight ) )
	{
		this->m_pPanel->EraseAttribute( XML_LAYOUT_GRID_HEIGHT );
	}
	else
	{
		// 如果对于gridlayout没有定义layout.width，则默认layout.width="*"
		strHeight = XML_ASTERISK;
	}
	strVec.clear();
	UI_Split( strHeight, XML_SEPARATOR, strVec );
	nCount = (int)strVec.size();
	for( int i = 0; i < nCount; i++ )
	{
		String str = strVec[i];
		// 判断是否为 AUTO 类型
		if( XML_AUTO == str )
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->heights.push_back( wh );
			continue;
		}

		// 判读是否为 * 类型
		int nIndex = str.find( XML_ASTERISK );
		if( String::npos != nIndex )
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi( str.c_str() );
			if( nXml == 0 )  // 如果只指定 "*"，那么表示"1*"
				nXml = 1;

			wh.xml = nXml;

			this->heights.push_back( wh );
			continue;
		}

		// 判断其他情况
		GridWH  wh;
		wh.type = GWHT_VALUE;
		wh.last = _ttoi( str.c_str() );
		this->heights.push_back( wh );

		continue;
	}

	return true;
}
SIZE  GridLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE size = {0,0};

	int nGridRows = (int)this->heights.size();
	int nGridCols = (int)this->widths.size();

	Object*   pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		if( !pChild->IsVisible() )
			continue;

		int nRow = 0, nCol = 0, nnGridRowspan = 1, nnGridColspan = 1 ;

		String strAttribute;
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROW, strAttribute ) )
		{
			nRow = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COL, strAttribute ) )
		{
			nCol = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROWSPAN, strAttribute ) )
		{
			nnGridRowspan = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COLSPAN, strAttribute ) )
		{
			nnGridColspan = _ttoi( strAttribute.c_str() );
		}

		// 超出了范围
		if( nCol >= nGridCols || nRow >= nGridRows )
		{
			UI_LOG_WARN( _T("GridLayout::MeasureChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )"),
				pChild->m_strID.c_str(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}
 
		// 对于宽和高都属于合并单元格的对象，放弃对它的计算
		// 同时放弃对合并列或合并行的计算，但是对于一个合并行，如果它只占一列的话
		// 必须计算该列的大小
		if( nnGridRowspan != 1 && nnGridColspan != 1 )
		{
				continue;
		}
	
		// 如果对象所在的行和列都有固定的值，那么不需要计算
		if( widths[nCol].type == GWHT_VALUE &&
			widths[nRow].type == GWHT_VALUE )
		{
			continue;
		}

		SIZE s = pChild->GetDesiredSize( hRDC );
		
		// 设置对象所在列的宽度
		if( nnGridColspan == 1)
		{
			switch( widths[nCol].type )
			{
			case GWHT_VALUE:     // 该列的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该列是平分大小，在这里先取这一列的最大值，在Arrange里面会再具体赋值
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			case GWHT_AUTO:      // 该列是自动大小，那么取这一列的最大值
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			default:
				assert(false);
				break;
			}
		}

		//设置对象所在行的高度
		if( nnGridRowspan == 1 )
		{
			switch( heights[nRow].type )
			{
			case GWHT_VALUE:     // 该行的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该行是平分大小，在这里先取这一行的最大值，在Arrange里面会再具体赋值
				if( s.cy > heights[nRow].last )
					heights[nRow].last = s.cy;
				break;

			case GWHT_AUTO:      // 该行是自动大小，那么去这一行的最大值
				if( s.cy > heights[nRow].last )
					heights[nRow].last = s.cy;
				break;

			default:
				assert(false);
				break;
			}
		}
	}// end of while( pChild != this->m_pPanel->EnumChildObject( pChild ) )

	// 必须将所有 * 类型的比例关系调整好，如 1* : 2* : 3* ，这个时候如果分别对应 5, 5, 10，
	// 那么必须安排成　5,10,15（满足最小那个）
	// 具体求法为：先分别算出比例关系：5/1 : 5/2 : 10/3 = 5 : 2.5 : 3.3
	// 取最大的那个数值，以该数值作为新的比例基数，去重新计算结果 1*5 : 2*5 : 3*5
	
	double maxRate = 0;
	// 求出比例基数
	for( int i = 0; i < nGridCols; i++ )
	{
		if( this->widths[i].type == GWHT_ASTERISK )
		{
			double rate = (double)widths[i].last / (double)widths[i].xml;
			if( rate > maxRate )
				maxRate = rate;
		}
	}
	// 用求出的基数重新计算
	for( int i = 0; i < nGridCols; i++ )
	{
		if( this->widths[i].type == GWHT_ASTERISK )
		{
			widths[i].last = (int)( widths[i].xml * maxRate );
		}
	}

	// 同理求出各行的高度
	maxRate = 0;
	for( int i = 0; i < nGridRows; i++ )
	{
		if( this->heights[i].type == GWHT_ASTERISK )
		{
			double rate = (double)heights[i].last / (double)heights[i].xml;
			if( rate > maxRate )
				maxRate = rate;
		}
	}
	for( int i = 0; i < nGridRows; i++ )
	{
		if( this->heights[i].type == GWHT_ASTERISK )
		{
			heights[i].last =(int)( heights[i].xml * maxRate );
		}
	}

	// 返回panel需要的大小
	for( int i = 0; i < nGridCols; i++ )
	{
		size.cx += widths[i].last;
	}
	for( int i = 0; i < nGridRows; i++ )
	{
		size.cy += heights[i].last;
	}
	
	return size;
}
void  GridLayout::ArrangeChildObject( HRDC hRDC, Object* pObjToArrage, bool bReDraw )
{
	// 调用该函数时，自己的大小已经被求出来了

	// 先求出GRID的宽度和高度，后面对各个子对象的布局都是先基于GRID的
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int nGridRows = (int)this->heights.size();  // Grid的行数
	int nGridCols = (int)this->widths.size();   // Grid的列数

	// 初始化，避免上一次的结果影响本次的布局
	for( int i = 0; i < nGridCols; i++ )
	{
		if( widths[i].type == GWHT_AUTO )
		{
			widths[i].last = 0;
		}
	}
	for( int i = 0; i < nGridRows; i++ )
	{
		if( heights[i].type == GWHT_AUTO )
		{
			heights[i].last = 0;
		}
	}
	
	// 第一步. 必须先安排auto类型
	Object*   pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		if( !pChild->IsVisible() )
			continue;

		int nRow = 0, nCol = 0, nnGridRowspan = 1, nnGridColspan = 1 ;

		String strAttribute;
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROW, strAttribute ) )
		{
			nRow = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COL, strAttribute ) )
		{
			nCol = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROWSPAN, strAttribute ) )
		{
			nnGridRowspan = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COLSPAN, strAttribute ) )
		{
			nnGridColspan = _ttoi( strAttribute.c_str() );
		}

		// 超出了范围
		if( nCol >= nGridCols || nRow >= nGridRows )
		{
			UI_LOG_WARN( _T("GridLayout::ArrangeChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )"),
				pChild->m_strID.c_str(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}

		// 只求auto类型
		if(  widths[nCol].type != GWHT_AUTO && heights[nRow].type != GWHT_AUTO )
		{
			continue ;
		}

		SIZE s = pChild->GetDesiredSize( hRDC );
		//::RestoreDC( hDC, -1 );

		if( widths[nCol].type == GWHT_AUTO )
		{
			if( widths[nCol].last < s.cx )
				widths[nCol].last = s.cx;
		}
		if( heights[nRow].type == GWHT_AUTO )
		{
			if( heights[nRow].last < s.cy )
				heights[nRow].last = s.cy;
		}
	}
	
	// 第二步. 计算*类型
	int nWidthRemain  = nWidth;   // 除去auto、value类型后，剩余的宽度
	int nHeightRemain = nHeight;  // 除去auto、value类型后，剩余的宽度
	int nASTERISKWidth  = 0;      // 宽度中的*的数量
	int nASTERISKHeight = 0;      // 高度中的*的数量

	for( int i = 0; i < nGridCols; i++ )
	{
		if( widths[i].type != GWHT_ASTERISK )
		{
			nWidthRemain -= widths[i].last;
		}
		else
		{
			nASTERISKWidth += widths[i].xml;
		}
	}
	for( int i = 0; i < nGridRows; i++ )
	{
		if( heights[i].type != GWHT_ASTERISK )
		{
			nHeightRemain -= heights[i].last;
		}
		else
		{
			nASTERISKHeight += heights[i].xml;
		}
	}

	// 开始平分(TODO. 由于整数的除法，这里可能造成几个像素的误差）
	for( int i = 0; i < nGridCols; i++ )
	{
		if( widths[i].type == GWHT_ASTERISK )
		{
			widths[i].last = nWidthRemain / nASTERISKWidth * widths[i].xml;
		}
	}
	for( int i = 0; i < nGridRows; i++ )
	{
		if( heights[i].type == GWHT_ASTERISK )
		{
			heights[i].last = nHeightRemain / nASTERISKHeight * heights[i].xml;
		}
	}


	// 第三步. 安排各个控件的位置
	pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		int nRow = 0, nCol = 0, nGridRowspan = 1, nGridColspan = 1 ;

		String strAttribute;
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROW, strAttribute ) )
		{
			nRow = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COL, strAttribute ) )
		{
			nCol = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_ROWSPAN, strAttribute ) )
		{
			nGridRowspan = _ttoi( strAttribute.c_str() );
		}
		if( pChild->GetAttribute( XML_LAYOUT_GRID_COLSPAN, strAttribute ) )
		{
			nGridColspan = _ttoi( strAttribute.c_str() );
		}

		// (row,col) -> ( rect.left, rect.top, rect.right, rect.bottom )
		// (0,0)   -> (0,0,widths[0],heights[0])
		// (1,1)   -> (widths[0],heights[0],widths[0]+widths[1],heights[0]+heights[1] )

		RECT  rcObjectInGrid;
		rcObjectInGrid.left   = this->getColPos( nCol );
		rcObjectInGrid.right  = this->getColPos( nCol + nGridColspan );
		rcObjectInGrid.top    = this->getRowPos( nRow );
		rcObjectInGrid.bottom = this->getRowPos( nRow + nGridRowspan );

		CRect rc;
		rc.CopyRect(&rcObjectInGrid);
		rc.OffsetRect(this->m_pPanel->GetPaddingL(), this->m_pPanel->GetPaddingT());

		CRegion4 rcMargin;
		pChild->GetMarginRegion(&rcMargin);
		Util::DeflatRect(&rc, &rcMargin);
	
		pChild->SetObjectPos(&rc, bReDraw?0:SWP_NOREDRAW);
	}
}

// 获取第nCol列离GRID最左侧的距离
// 注意：这里的参数可以是对象的col+colspan，因此该值可能会超出grid的最大列
int GridLayout::getColPos( int nCol )
{
	int nGridCols = widths.size();
	if( nCol > nGridCols )
	{
		UI_LOG_WARN( _T("GridLayout::getColPos, nCol[%d] > widths.size[%d]"), nCol, nGridCols );
		nCol = nGridCols;
	}

	int nRet = 0;
	for( int i = 0; i < nCol; i++ )
	{
		nRet += widths[i].last;
	}
	return nRet;
}
// 获取第nRow行离GRID最上面的距离
int GridLayout::getRowPos( int nRow )
{
	int nGridRows = heights.size();
	if( nRow > nGridRows )
	{
		UI_LOG_WARN( _T("GridLayout::getRowPos, nCol[%d] > heights.size[%d]"), nRow, nGridRows );
		nRow = nGridRows;
	}

	int nRet = 0;
	for( int i = 0; i < nRow; i++ )
	{
		nRet += heights[i].last;
	}
	return nRet;
}

SIZE  DockLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE size = {0,0};

	// 1. 先获取center对象的SIZE
	Object*  pCenterObj = NULL;
	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if( XML_LAYOUT_DOCK_DOCK_CENTER == strDock ||
			strDock.empty() )
		{
			pCenterObj = pChild;
			break;
		}
	}

	if( pCenterObj )
		size = pCenterObj->GetDesiredSize( hRDC );

	// 2. 从后开始遍历停靠在四周的子对象的大小
	pChild = NULL;
	while( pChild = this->m_pPanel->REnumChildObject( pChild ) )
	{
		if( !pChild->IsVisible() )
			continue;

		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if( XML_LAYOUT_DOCK_DOCK_LEFT == strDock ||
			XML_LAYOUT_DOCK_DOCK_RIGHT == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );
			if( s.cy > size.cy )
				size.cy = s.cy;
			size.cx += s.cx;
		}
		else 
		if ( XML_LAYOUT_DOCK_DOCK_TOP == strDock ||
			 XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );
			if( s.cx > size.cx )
				size.cx = s.cx;
			size.cy += s.cy;
		}

		else
		{
			continue;
		}

	}

	return size;
}
void  DockLayout::ArrangeChildObject( HRDC hRDC, Object* pObjToArrage, bool bReDraw )
{
	// 调用该函数时，自己的大小已经被求出来了
	int  nWidth  = m_pPanel->GetWidth();
	int  nHeight = m_pPanel->GetHeight();

	int  nComsumeTop    = this->m_pPanel->GetPaddingT();
	int  nComsumeLeft   = this->m_pPanel->GetPaddingL();
	int  nComsumeBottom = this->m_pPanel->GetPaddingB();
	int  nComsumeRight  = this->m_pPanel->GetPaddingR();

	Object* pCenterObj  = NULL;  // 记录是否已经设置过 dock布局中的 center 对象了（因为只允许有一个center对象）
	
	// arrange的时候要从前往后开始排布
	Object* pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject(pChild) )
	{
		if( !pChild->IsVisible() )
			continue;

		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		// 计算出 pChild 的 rectP
		if( XML_LAYOUT_DOCK_DOCK_LEFT == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );
			
			CRect rcObj( 
				nComsumeLeft             +  pChild->GetMarginL(), 
				nComsumeTop              +  pChild->GetMarginT(), 
				nComsumeLeft + s.cx      -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, bReDraw?0:SWP_NOREDRAW);

			nComsumeLeft += s.cx;
		}
		else
		if( XML_LAYOUT_DOCK_DOCK_RIGHT == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );
			
			CRect rcObj( 
				nWidth - nComsumeRight - s.cx  +  pChild->GetMarginL(), 
				nComsumeTop                    +  pChild->GetMarginT(), 
				nWidth - nComsumeRight         -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom       -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, bReDraw?0:SWP_NOREDRAW);

			nComsumeRight += s.cx;
		}
		else 
		if ( XML_LAYOUT_DOCK_DOCK_TOP == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );

			CRect rcObj( 
				nComsumeLeft            +  pChild->GetMarginL() , 
				nComsumeTop             +  pChild->GetMarginT() , 
				nWidth - nComsumeRight  -  pChild->GetMarginR() , 
				nComsumeTop + s.cy      -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, bReDraw?0:SWP_NOREDRAW);

			nComsumeTop += s.cy;
		}
		else 
		if( XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock )
		{
			SIZE s = pChild->GetDesiredSize( hRDC );

			CRect rcObj(
				nComsumeLeft                    +  pChild->GetMarginL() , 
				nHeight - nComsumeBottom - s.cy +  pChild->GetMarginT() , 
				nWidth - nComsumeRight          -  pChild->GetMarginR() , 
				nHeight - nComsumeBottom        -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, bReDraw?0:SWP_NOREDRAW);

			nComsumeBottom += s.cy;	 
		}
		else  // 其他都归为center对象
		{
			if( NULL == pCenterObj )
			{
				pCenterObj = pChild;
				continue;
			}
		}
	}

	if( pCenterObj )
	{
		pCenterObj->SetObjectPos(
			nComsumeLeft                            +  pCenterObj->GetMarginL() , 
			nComsumeTop                             +  pCenterObj->GetMarginT() , 
			nWidth  - nComsumeRight  - nComsumeLeft -  pCenterObj->GetMarginR() , 
			nHeight - nComsumeBottom - nComsumeTop  -  pCenterObj->GetMarginB() ,
			bReDraw?0:SWP_NOREDRAW);
	}
}

SIZE  CardLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE size = {0,0};

	Object*  pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject( pChild ) )
	{
		SIZE  s = pChild->GetDesiredSize( hRDC );

		if( size.cx < s.cx )
			size.cx = s.cx;
		if( size.cy < s.cy )
			size.cy = s.cy;
	}
	return size;
}
void  CardLayout::ArrangeChildObject( HRDC hRDC, Object* pObjToArrage, bool bReDraw )
{
	// 调用该函数时，自己的大小已经被求出来了
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	Object* pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		if( NULL != pObjToArrage && pObjToArrage != pChild )
			continue;

		if( !pChild->IsVisible() )
		{
			if( NULL != pObjToArrage )
				break;
			else
				continue;
		}

		CRect rcChildObj ;
		int nChildW = nWidth - pChild->GetMarginW();
		int nChildH = nHeight - pChild->GetMarginH();
		rcChildObj.SetRect(0, 0, nChildW, nChildH );

		pChild->SetObjectPos( &rcChildObj, bReDraw?0:SWP_NOREDRAW );

		if( NULL != pObjToArrage && pObjToArrage == pChild )
			break;
	}
}

DesktopLayout::DesktopLayout()
{
	this->left = this->top = this->right = this->bottom = NDEF;
}
 void  DesktopLayout::Arrange( HRDC hRDC )
{
	SIZE s = this->m_pWindow->GetDesiredSize( hRDC );  // 获得的SIZE包括了MARGIN的大小 s.cx=margin.left+width+margin.right

	// 读取其他属性值来设置rectW
	int  x = 0, y = 0;                                 // 最终在屏幕中的坐标
	int  nCXScreen = 800, nCYScreen = 600;             // 屏幕大小

	// 从XML中读取属性值
	String  strLeft;
	if( this->m_pWindow->GetAttribute( XML_LAYOUT_CANVAS_LEFT, strLeft ) )
	{
		this->left = _ttoi( strLeft.c_str() );
	}
	String  strTop;
	if( this->m_pWindow->GetAttribute( XML_LAYOUT_CANVAS_TOP, strTop ) )
	{
		this->top = _ttoi( strTop.c_str() );
	}
	String  strRight;
	if( this->m_pWindow->GetAttribute( XML_LAYOUT_CANVAS_RIGHT, strRight ) )
	{
		this->right = _ttoi( strRight.c_str() );
	}
	String  strBottom;
	if( this->m_pWindow->GetAttribute( XML_LAYOUT_CANVAS_BOTTOM, strBottom ) )
	{
		this->bottom = _ttoi( strBottom.c_str() );
	}

	
	// 计算出屏幕工作区域的大小(不使用整个屏幕的大小)
	//nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
	//nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

	CRect rcWorkSize;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkSize, 0 );
	nCXScreen = rcWorkSize.Width();
	nCYScreen = rcWorkSize.Height();

	// 计算出坐标
	if( this->left != NDEF )
	{
		x = this->left;
		x += m_pWindow->GetMarginL();
	}
	else
	{
		if( this->right != NDEF )
		{
			x = nCXScreen - this->right - s.cx;  // right是指窗口右侧距离屏幕右侧的距离
			x -= m_pWindow->GetMarginR();
		}
		else
		{
			// 居中
			x = ( nCXScreen - s.cx) / 2;
		}
	}
	if( this->top != NDEF )
	{
		y = this->top;
		y += m_pWindow->GetMarginT();
	}
	else
	{
		if( this->bottom != NDEF )
		{
			y = nCYScreen - this->bottom - s.cy; // 同right
			y -= m_pWindow->GetMarginB();
		}
		else
		{
			// 居中
			y = ( nCYScreen - s.cy ) / 2;
		}
	}

	m_pWindow->SetObjectPos(x, y, s.cx, s.cy, SWP_NOREDRAW);

	// 递归
	this->m_pWindow->GetLayout()->Arrange( hRDC );
}
