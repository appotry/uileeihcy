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
		UI_LOG_WARN( _T("LayoutManagerFactory::GetLayout��failed. styType=%s"), strType.c_str() );
	}
	else
	{
		pLayout->m_pPanel = pPanel;
	}

	return pLayout;
}


/*
**	Ŀ�ģ����m_pPanel��widht��height���ѱ�ָ������ôֱ�ӷ��ظô�С������
**		  ������LayoutManager�����MeasureChildObject����ȡ�Լ�˵��Ҫ�Ĵ�С
*/
SIZE Layout::Measure( HRDC hRDC )
{
	SIZE size = {0,0};
	// ��ָ���˴�С�Ĵ���
	if( this->m_pPanel->GetConfigWidth() != AUTO && this->m_pPanel->GetConfigHeight() != AUTO )
	{
		size.cx = this->m_pPanel->GetConfigWidth();
		size.cy = this->m_pPanel->GetConfigHeight();
	}
	else
	{
		// ��ȡ�Ӷ�������Ҫ�Ŀռ�
		size = this->MeasureChildObject( hRDC );

		// ����padding �Ĵ�С
		size.cx += m_pPanel->GetPaddingW();
		size.cy += m_pPanel->GetPaddingH();

		// �����ָ��width��height������һ������ô��������һ���еõ���ֵ
		if( this->m_pPanel->GetConfigWidth() != AUTO )
			size.cx = this->m_pPanel->GetConfigWidth();
		if( this->m_pPanel->GetConfigHeight() != AUTO )
			size.cy = this->m_pPanel->GetConfigHeight();

		if( m_pPanel->GetObjectType() == OBJ_WINDOW )
		{
			// �����ȡ������client��С����Ҫת����window��С
			((WindowBase*)m_pPanel)->CalcWindowSizeByClientSize( size, &size );
		}
	}

	// ���� margin �Ĵ�С
	size.cx += m_pPanel->GetMarginW();
	size.cy += m_pPanel->GetMarginH(); 

	return size;
}
void Layout::Arrange( HRDC hRDC )
{
	this->ArrangeChildObject( hRDC );
	
	// �ݹ�
	Object*  pChild = NULL;
	while( pChild = m_pPanel->EnumChildObject(pChild) )
	{
		// ֻ��Panel����Ҫȥ����
		if( pChild->GetObjectType() == OBJ_PANEL ||
			pChild->GetObjectType() == OBJ_WINDOW )
		{
			((Panel*)pChild)->GetLayout()->Arrange( hRDC );
		}
	}
}

StackLayout::StackLayout()
{
	this->direction = LAYOUT_STACK_LEFTTORIGHT;   // Ĭ�϶�ջ�Ǵ�����
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
			UI_LOG_WARN( _T("StackLayout::SetAttribute������%s=\"%s\"��ֵ����ȷ"), XML_LAYOUT_STACK_DIRECTION, strDirection.c_str());
		}

		this->m_pPanel->EraseAttribute( XML_LAYOUT_STACK_DIRECTION );
	}
	return true;
}
SIZE  StackLayout::MeasureChildObject( HRDC hRDC )
{
	SIZE  size = {0,0};

	// ͨ���ӿؼ�����ȡ�Լ�����Ҫ�Ĵ�С
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int  nConsume1 = 0; // ��ǰ�����ĵĸ߶Ȼ��ȣ���left/top��ʼ������
	int  nConsume2 = 0; // ��ǰ�����ĵĸ߶Ȼ��ȣ���right/bottom��ʼ������

	// ��ʼ�� nComsume1 , nComsume2
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
				bool bDockLeft = true;   // Ĭ��ͣ�������

				// �жϰ�ť��ͣ����һ��
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
						UI_LOG_WARN( _T("StackLayout::Arrange�������ֵ��%s=\"%s\"����Ĭ��Ϊ%s"), 
							XML_LAYOUT_STACK_DOCK, strDockType.c_str(), XML_LAYOUT_STACK_DOCK_LEFT );
					}
				}else
				{
					bDockLeft = true;
				}

				// ����λ��
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


			// ���ϵ���
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
				bool bDockTop = true;   // Ĭ��ͣ�����ϲ�

				// �жϰ�ť��ͣ����һ��
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
						UI_LOG_WARN( _T("StackLayout::Arrange�������ֵ��%s=\"%s\"����Ĭ��Ϊ%s"), 
							XML_LAYOUT_STACK_DOCK, strDockType.c_str(), XML_LAYOUT_STACK_DOCK_TOP );
					}
				}else
				{
					bDockTop = true;
				}

				// ����λ��
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

	// ͨ���ӿؼ�����ȡ�Լ�����Ҫ�Ĵ�С
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
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
	

		int  x = 0, y = 0;                                  // pChild������parent�е�����
		int  left=NDEF, top=NDEF, right=NDEF, bottom=NDEF;  // pChild��xml����

		// ��XML�ж�ȡ����ֵ������ pChild��rectP��left��top
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
		// ����� pChild �� rectP�Ŀ�͸�

		SIZE s = {0,0};
		if( left == NDEF || right == NDEF || top == NDEF || bottom == NDEF )
		{
			s = pChild->GetDesiredSize( hRDC );
		}

		// ���ͬʱָ����left/right,�����width����
		if (left != NDEF && right != NDEF)
		{
			s.cx = nWidth - left - right;
		}
		// ���ͬʱָ����top/bottom�������height����
		if (top != NDEF && bottom != NDEF)
		{
			s.cy = nHeight - top - bottom;
		}

		CRect rcChildObj ;
		int nChildW = s.cx - pChild->GetMarginW();
		int nChildH = s.cy - pChild->GetMarginH();
		rcChildObj.SetRect(0, 0, nChildW, nChildH );


		// ��������꣬��left/right,top/bottom����һ��δָ���ģ���ôȡ0������DesktopLayout����ȡ���У�
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
				//x = ( nWidth - s.cx ) / 2;  // ����
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
				
				//y = ( nHeight - s.cy ) / 2;  // ����
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
		1. ����Grid��ÿһ�еĿ��
	*/
	String strWidth;
	if( this->m_pPanel->GetAttribute( XML_LAYOUT_GRID_WIDTH,strWidth ) )
	{
		this->m_pPanel->EraseAttribute( XML_LAYOUT_GRID_WIDTH );
	}
	else
	{
		// �������gridlayoutû�ж���layout.width����Ĭ��layout.width="*"
		strWidth = XML_ASTERISK;
	}

	vector<String>  strVec;
	UI_Split( strWidth, XML_SEPARATOR, strVec );
	int nCount = (int)strVec.size();
	for( int i = 0; i < nCount; i++ )
	{
		String  str = strVec[i];
		// �ж��Ƿ�Ϊ AUTO ����
		if( XML_AUTO == str )
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->widths.push_back( wh );
			continue;
		}

		// �ж��Ƿ�Ϊ * ����
		int nIndex = str.find( XML_ASTERISK );
		if( String::npos != nIndex )
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi( str.c_str() );
			if( nXml == 0 )  // ���ָֻ�� "*"����ô��ʾ"1*"
				nXml = 1;

			wh.xml = nXml;
			this->widths.push_back( wh );
			continue;
		}

		// �ж��������
		GridWH  wh;
		wh.type = GWHT_VALUE;
		wh.last = _ttoi( str.c_str() );
		this->widths.push_back( wh );

		continue;
	}

	 

	/*
		2. ����Grid��ÿһ�еĸ߶�
	*/
	String strHeight;
	if( this->m_pPanel->GetAttribute( XML_LAYOUT_GRID_HEIGHT, strHeight ) )
	{
		this->m_pPanel->EraseAttribute( XML_LAYOUT_GRID_HEIGHT );
	}
	else
	{
		// �������gridlayoutû�ж���layout.width����Ĭ��layout.width="*"
		strHeight = XML_ASTERISK;
	}
	strVec.clear();
	UI_Split( strHeight, XML_SEPARATOR, strVec );
	nCount = (int)strVec.size();
	for( int i = 0; i < nCount; i++ )
	{
		String str = strVec[i];
		// �ж��Ƿ�Ϊ AUTO ����
		if( XML_AUTO == str )
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->heights.push_back( wh );
			continue;
		}

		// �ж��Ƿ�Ϊ * ����
		int nIndex = str.find( XML_ASTERISK );
		if( String::npos != nIndex )
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi( str.c_str() );
			if( nXml == 0 )  // ���ָֻ�� "*"����ô��ʾ"1*"
				nXml = 1;

			wh.xml = nXml;

			this->heights.push_back( wh );
			continue;
		}

		// �ж��������
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

		// �����˷�Χ
		if( nCol >= nGridCols || nRow >= nGridRows )
		{
			UI_LOG_WARN( _T("GridLayout::MeasureChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] ����grid��Χ [col=%d,row=%d] )"),
				pChild->m_strID.c_str(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}
 
		// ���ڿ�͸߶����ںϲ���Ԫ��Ķ��󣬷��������ļ���
		// ͬʱ�����Ժϲ��л�ϲ��еļ��㣬���Ƕ���һ���ϲ��У������ֻռһ�еĻ�
		// ���������еĴ�С
		if( nnGridRowspan != 1 && nnGridColspan != 1 )
		{
				continue;
		}
	
		// ����������ڵ��к��ж��й̶���ֵ����ô����Ҫ����
		if( widths[nCol].type == GWHT_VALUE &&
			widths[nRow].type == GWHT_VALUE )
		{
			continue;
		}

		SIZE s = pChild->GetDesiredSize( hRDC );
		
		// ���ö��������еĿ��
		if( nnGridColspan == 1)
		{
			switch( widths[nCol].type )
			{
			case GWHT_VALUE:     // ���е�ֵ�ǹ̶�ֵ������Ҫ�޸�
				break;

			case GWHT_ASTERISK:  // ������ƽ�ִ�С����������ȡ��һ�е����ֵ����Arrange������پ��帳ֵ
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			case GWHT_AUTO:      // �������Զ���С����ôȡ��һ�е����ֵ
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			default:
				assert(false);
				break;
			}
		}

		//���ö��������еĸ߶�
		if( nnGridRowspan == 1 )
		{
			switch( heights[nRow].type )
			{
			case GWHT_VALUE:     // ���е�ֵ�ǹ̶�ֵ������Ҫ�޸�
				break;

			case GWHT_ASTERISK:  // ������ƽ�ִ�С����������ȡ��һ�е����ֵ����Arrange������پ��帳ֵ
				if( s.cy > heights[nRow].last )
					heights[nRow].last = s.cy;
				break;

			case GWHT_AUTO:      // �������Զ���С����ôȥ��һ�е����ֵ
				if( s.cy > heights[nRow].last )
					heights[nRow].last = s.cy;
				break;

			default:
				assert(false);
				break;
			}
		}
	}// end of while( pChild != this->m_pPanel->EnumChildObject( pChild ) )

	// ���뽫���� * ���͵ı�����ϵ�����ã��� 1* : 2* : 3* �����ʱ������ֱ��Ӧ 5, 5, 10��
	// ��ô���밲�ųɡ�5,10,15��������С�Ǹ���
	// ������Ϊ���ȷֱ����������ϵ��5/1 : 5/2 : 10/3 = 5 : 2.5 : 3.3
	// ȡ�����Ǹ���ֵ���Ը���ֵ��Ϊ�µı���������ȥ���¼����� 1*5 : 2*5 : 3*5
	
	double maxRate = 0;
	// �����������
	for( int i = 0; i < nGridCols; i++ )
	{
		if( this->widths[i].type == GWHT_ASTERISK )
		{
			double rate = (double)widths[i].last / (double)widths[i].xml;
			if( rate > maxRate )
				maxRate = rate;
		}
	}
	// ������Ļ������¼���
	for( int i = 0; i < nGridCols; i++ )
	{
		if( this->widths[i].type == GWHT_ASTERISK )
		{
			widths[i].last = (int)( widths[i].xml * maxRate );
		}
	}

	// ͬ��������еĸ߶�
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

	// ����panel��Ҫ�Ĵ�С
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������

	// �����GRID�Ŀ�Ⱥ͸߶ȣ�����Ը����Ӷ���Ĳ��ֶ����Ȼ���GRID��
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int nGridRows = (int)this->heights.size();  // Grid������
	int nGridCols = (int)this->widths.size();   // Grid������

	// ��ʼ����������һ�εĽ��Ӱ�챾�εĲ���
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
	
	// ��һ��. �����Ȱ���auto����
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

		// �����˷�Χ
		if( nCol >= nGridCols || nRow >= nGridRows )
		{
			UI_LOG_WARN( _T("GridLayout::ArrangeChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] ����grid��Χ [col=%d,row=%d] )"),
				pChild->m_strID.c_str(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}

		// ֻ��auto����
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
	
	// �ڶ���. ����*����
	int nWidthRemain  = nWidth;   // ��ȥauto��value���ͺ�ʣ��Ŀ��
	int nHeightRemain = nHeight;  // ��ȥauto��value���ͺ�ʣ��Ŀ��
	int nASTERISKWidth  = 0;      // ����е�*������
	int nASTERISKHeight = 0;      // �߶��е�*������

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

	// ��ʼƽ��(TODO. ���������ĳ��������������ɼ������ص���
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


	// ������. ���Ÿ����ؼ���λ��
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

// ��ȡ��nCol����GRID�����ľ���
// ע�⣺����Ĳ��������Ƕ����col+colspan����˸�ֵ���ܻᳬ��grid�������
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
// ��ȡ��nRow����GRID������ľ���
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

	// 1. �Ȼ�ȡcenter�����SIZE
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

	// 2. �Ӻ�ʼ����ͣ�������ܵ��Ӷ���Ĵ�С
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
	int  nWidth  = m_pPanel->GetWidth();
	int  nHeight = m_pPanel->GetHeight();

	int  nComsumeTop    = this->m_pPanel->GetPaddingT();
	int  nComsumeLeft   = this->m_pPanel->GetPaddingL();
	int  nComsumeBottom = this->m_pPanel->GetPaddingB();
	int  nComsumeRight  = this->m_pPanel->GetPaddingR();

	Object* pCenterObj  = NULL;  // ��¼�Ƿ��Ѿ����ù� dock�����е� center �����ˣ���Ϊֻ������һ��center����
	
	// arrange��ʱ��Ҫ��ǰ����ʼ�Ų�
	Object* pChild = NULL;
	while( pChild = this->m_pPanel->EnumChildObject(pChild) )
	{
		if( !pChild->IsVisible() )
			continue;

		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		// ����� pChild �� rectP
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
		else  // ��������Ϊcenter����
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������
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
	SIZE s = this->m_pWindow->GetDesiredSize( hRDC );  // ��õ�SIZE������MARGIN�Ĵ�С s.cx=margin.left+width+margin.right

	// ��ȡ��������ֵ������rectW
	int  x = 0, y = 0;                                 // ��������Ļ�е�����
	int  nCXScreen = 800, nCYScreen = 600;             // ��Ļ��С

	// ��XML�ж�ȡ����ֵ
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

	
	// �������Ļ��������Ĵ�С(��ʹ��������Ļ�Ĵ�С)
	//nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
	//nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

	CRect rcWorkSize;
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkSize, 0 );
	nCXScreen = rcWorkSize.Width();
	nCYScreen = rcWorkSize.Height();

	// ���������
	if( this->left != NDEF )
	{
		x = this->left;
		x += m_pWindow->GetMarginL();
	}
	else
	{
		if( this->right != NDEF )
		{
			x = nCXScreen - this->right - s.cx;  // right��ָ�����Ҳ������Ļ�Ҳ�ľ���
			x -= m_pWindow->GetMarginR();
		}
		else
		{
			// ����
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
			y = nCYScreen - this->bottom - s.cy; // ͬright
			y -= m_pWindow->GetMarginB();
		}
		else
		{
			// ����
			y = ( nCYScreen - s.cy ) / 2;
		}
	}

	m_pWindow->SetObjectPos(x, y, s.cx, s.cy, SWP_NOREDRAW);

	// �ݹ�
	this->m_pWindow->GetLayout()->Arrange( hRDC );
}
