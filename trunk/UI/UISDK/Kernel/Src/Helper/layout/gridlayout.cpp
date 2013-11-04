#include "stdafx.h"
#include "gridlayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

GridWH::GridWH()
{
	this->last = 0;
	this->xml = 0;
	this->type = GWHT_AUTO;
}

GridLayout::GridLayout()
{
}
GridLayout::~GridLayout()
{
	this->widths.clear();
	this->heights.clear();
}

void  GridLayout::SetAttribute(IMapAttribute* pMapAttrib)
{
    if (NULL == pMapAttrib)
        return;

    const TCHAR* szText = NULL;

	/*
		1. 解析Grid的每一列的宽度
	*/
	const TCHAR* szWidth = pMapAttrib->GetAttr(XML_LAYOUT_GRID_WIDTH, true);
    if (NULL == szWidth)
	{
		// 如果对于gridlayout没有定义layout.width，则默认layout.width="*"
		szWidth = XML_ASTERISK;
	}

    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(szWidth, XML_SEPARATOR, &pEnum);
	for (int i = 0; i < nCount; i++)
	{
		String  str = pEnum->GetText(i);

		// 判断是否为 AUTO 类型
		if (XML_AUTO == str)
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->widths.push_back( wh );
			continue;
		}

		// 判读是否为 * 类型
		int nIndex = str.find(XML_ASTERISK);
		if (String::npos != nIndex)
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi(str.c_str());
			if (nXml == 0)  // 如果只指定 "*"，那么表示"1*"
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
    SAFE_RELEASE(pEnum);

	/*
		2. 解析Grid的每一列的高度
	*/
	const TCHAR* szHeight = pMapAttrib->GetAttr(XML_LAYOUT_GRID_HEIGHT, true);
    if (NULL == szHeight)
	{
		// 如果对于gridlayout没有定义layout.height，则默认layout.height="*"
		szHeight = XML_ASTERISK;
	}

    nCount = Util::SplitString(szHeight, XML_SEPARATOR, &pEnum);
	for (int i = 0; i < nCount; i++)
	{
		String str = pEnum->GetText(i);

		// 判断是否为 AUTO 类型
		if (XML_AUTO == str)
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->heights.push_back( wh );
			continue;
		}

		// 判读是否为 * 类型
		int nIndex = str.find(XML_ASTERISK);
		if (String::npos != nIndex)
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi(str.c_str());
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
    SAFE_RELEASE(pEnum);
}

void  GridLayout::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_WIDTH, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_HEIGHT, szPrefix);
}

SIZE  GridLayout::Measure()
{
	SIZE size = {0,0};

	int nGridRows = (int)this->heights.size();
	int nGridCols = (int)this->widths.size();

	Object*   pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
		if (!pChild->IsVisible())
        {
			continue;
        }

        GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;;
        }

		int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        int nnGridRowspan = pParam->GetRowSpan();
        int nnGridColspan = pParam->GetColSpan();

		// 超出了范围
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN( _T("GridLayout::MeasureChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )"),
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}
 
		// 对于宽和高都属于合并单元格的对象，放弃对它的计算
		// 同时放弃对合并列或合并行的计算，但是对于一个合并行，如果它只占一列的话
		// 必须计算该列的大小
		if (nnGridRowspan != 1 && nnGridColspan != 1)
		{
				continue;
		}
	
		// 如果对象所在的行和列都有固定的值，那么不需要计算
		if (widths[nCol].type == GWHT_VALUE &&
			heights[nRow].type == GWHT_VALUE)
		{
			continue;
		}

		SIZE s = pChild->GetDesiredSize();
		
		// 设置对象所在列的宽度
		if (nnGridColspan == 1)
		{
			switch (widths[nCol].type)
			{
			case GWHT_VALUE:     // 该列的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该列是平分大小，在这里先取这一列的最大值，在Arrange里面会再具体赋值
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			case GWHT_AUTO:      // 该列是自动大小，那么取这一列的最大值
				if (s.cx > widths[nCol].last)
					widths[nCol].last = s.cx;
				break;

			default:
				assert(false);
				break;
			}
		}

		//设置对象所在行的高度
		if (nnGridRowspan == 1)
		{
			switch (heights[nRow].type)
			{
			case GWHT_VALUE:     // 该行的值是固定值，不需要修改
				break;

			case GWHT_ASTERISK:  // 该行是平分大小，在这里先取这一行的最大值，在Arrange里面会再具体赋值
				if (s.cy > heights[nRow].last)
					heights[nRow].last = s.cy;
				break;

			case GWHT_AUTO:      // 该行是自动大小，那么去这一行的最大值
				if (s.cy > heights[nRow].last)
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
	for (int i = 0; i < nGridCols; i++)
	{
		if (this->widths[i].type == GWHT_ASTERISK)
		{
			if (widths[i].xml == 0)
				widths[i].xml = 1;

			double rate = (double)widths[i].last / (double)widths[i].xml;
			if (rate > maxRate)
				maxRate = rate;
		}
	}
	// 用求出的基数重新计算
	for (int i = 0; i < nGridCols; i++)
	{
		if (this->widths[i].type == GWHT_ASTERISK)
		{
			widths[i].last = (int)(widths[i].xml * maxRate);
		}
	}

	// 同理求出各行的高度
	maxRate = 0;
	for (int i = 0; i < nGridRows; i++)
	{
		if (this->heights[i].type == GWHT_ASTERISK)
		{
			if (heights[i].xml == 0)
				heights[i].xml = 1;

			double rate = (double)heights[i].last / (double)heights[i].xml;
			if (rate > maxRate)
				maxRate = rate;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{ 
		if (this->heights[i].type == GWHT_ASTERISK)
		{ 
			heights[i].last =(int)(heights[i].xml * maxRate);
		}
	}

	// 返回panel需要的大小
	for (int i = 0; i < nGridCols; i++)
	{
		size.cx += widths[i].last;
	}
	for (int i = 0; i < nGridRows; i++)
	{
		size.cy += heights[i].last;
	}

    return size;
}
void  GridLayout::Arrange(IObject* pObjToArrage, bool bUpdate)
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
	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type == GWHT_AUTO)
		{
			widths[i].last = 0;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type == GWHT_AUTO)
		{
			heights[i].last = 0;
		}
	}

    // 第一步. 必须先安排auto类型
	Object*   pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
		if (!pChild->IsVisible())
        {
			continue;
        }

        GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;;
        }

        int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        int nnGridRowspan = pParam->GetRowSpan();
        int nnGridColspan = pParam->GetColSpan();

		// 超出了范围
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN( _T("GridLayout::ArrangeChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] 超出grid范围 [col=%d,row=%d] )"),
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}

		// 只求auto类型
		if (widths[nCol].type != GWHT_AUTO && heights[nRow].type != GWHT_AUTO)
		{
			continue ;
		}

		SIZE s = pChild->GetDesiredSize();

		if (widths[nCol].type == GWHT_AUTO)
		{
			if( widths[nCol].last < s.cx )
				widths[nCol].last = s.cx;
		}
		if (heights[nRow].type == GWHT_AUTO)
		{
			if (heights[nRow].last < s.cy)
				heights[nRow].last = s.cy;
		}
	}
	
	// 第二步. 计算*类型
	int nWidthRemain  = nWidth;   // 除去auto、value类型后，剩余的宽度
	int nHeightRemain = nHeight;  // 除去auto、value类型后，剩余的宽度
	int nASTERISKWidth  = 0;      // 宽度中的*的数量
	int nASTERISKHeight = 0;      // 高度中的*的数量

	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type != GWHT_ASTERISK)
		{
			nWidthRemain -= widths[i].last;
		}
		else
		{
			nASTERISKWidth += widths[i].xml;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type != GWHT_ASTERISK)
		{
			nHeightRemain -= heights[i].last;
		}
		else
		{
			nASTERISKHeight += heights[i].xml;
		}
	}

	if (nASTERISKWidth == 0)
		nASTERISKWidth = 1;
	if (nASTERISKHeight == 0)
		nASTERISKHeight = 1;

	// 开始平分(TODO. 由于整数的除法，这里可能造成几个像素的误差）
	for (int i = 0; i < nGridCols; i++)
	{
		if (widths[i].type == GWHT_ASTERISK)
		{
			widths[i].last = nWidthRemain / nASTERISKWidth * widths[i].xml;
		}
	}
	for (int i = 0; i < nGridRows; i++)
	{
		if (heights[i].type == GWHT_ASTERISK)
		{
			heights[i].last = nHeightRemain / nASTERISKHeight * heights[i].xml;
		}
	}


	// 第三步. 安排各个控件的位置
	pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
        GridLayoutParam*  pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;;
        }

        int nRow = pParam->GetRow();
        int nCol = pParam->GetCol();
        int nnGridRowspan = pParam->GetRowSpan();
        int nnGridColspan = pParam->GetColSpan();


		// (row,col) -> ( rect.left, rect.top, rect.right, rect.bottom )
		// (0,0)   -> (0,0,widths[0],heights[0])
		// (1,1)   -> (widths[0],heights[0],widths[0]+widths[1],heights[0]+heights[1] )

		RECT  rcObjectInGrid;
		rcObjectInGrid.left   = this->getColPos( nCol );
		rcObjectInGrid.right  = this->getColPos( nCol + nnGridColspan );
		rcObjectInGrid.top    = this->getRowPos( nRow );
		rcObjectInGrid.bottom = this->getRowPos( nRow + nnGridRowspan );

		CRect rc;
		rc.CopyRect(&rcObjectInGrid);
		rc.OffsetRect(this->m_pPanel->GetPaddingL(), this->m_pPanel->GetPaddingT());

 		CRegion4 rcMargin;
 		pChild->GetMarginRegion(&rcMargin);
 		Util::DeflatRect(&rc, &rcMargin);

        int nConfigW = pParam->GetConfigWidth();
        int nConfigH = pParam->GetConfigHeight();
        if (nConfigW >= 0)
            rc.right = rc.left + nConfigW;
        if (nConfigH >= 0)
            rc.bottom = rc.top + nConfigH;
	
		pChild->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
	}

    if (bUpdate)
        m_pPanel->UpdateObject();
}

// 获取第nCol列离GRID最左侧的距离
// 注意：这里的参数可以是对象的col+colspan，因此该值可能会超出grid的最大列
int GridLayout::getColPos(int nCol)
{
	int nGridCols = widths.size();
	if (nCol >= nGridCols)
	{
	//	UI_LOG_WARN( _T("GridLayout::getColPos, nCol[%d] > widths.size[%d]"), nCol, nGridCols );
		nCol = nGridCols;
	}

	int nRet = 0;
	for (int i = 0; i < nCol; i++)
	{
		nRet += widths[i].last;
	}
	return nRet;
}
// 获取第nRow行离GRID最上面的距离
int GridLayout::getRowPos( int nRow )
{
	int nGridRows = heights.size();
	if (nRow >= nGridRows)
	{
	//	UI_LOG_WARN( _T("GridLayout::getRowPos, nCol[%d] > heights.size[%d]"), nRow, nGridRows );
		nRow = nGridRows;
	}

	int nRet = 0;
	for (int i = 0; i < nRow; i++)
	{
		nRet += heights[i].last;
	}
	return nRet;
}



//////////////////////////////////////////////////////////////////////////

GridLayoutParam::GridLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;

    m_nCol = m_nRow = 0;
    m_nColSpan = m_nRowSpan = 1;
}
GridLayoutParam::~GridLayoutParam()
{

}

void  GridLayoutParam::UpdateByRect()
{
    CRect  rcParent;
    m_pObj->GetParentRect(&rcParent);

    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  GridLayoutParam::Serialize(SERIALIZEDATA* pData)
{
    if (pData->nFlag & SERIALIZEFLAG_LOAD)
    {
        IMapAttribute* pMapAttrib = pData->pMapAttrib;
        if (NULL == pMapAttrib)
            return;

        //  设置布局相关值
        const TCHAR* szText = pMapAttrib->GetAttr(XML_WIDTH, false);
        if (szText)
        {
            if (0 == _tcscmp(XML_AUTO, szText))
                this->m_nConfigWidth = AUTO;
            else
                this->m_nConfigWidth = _ttoi(szText);
        }

        szText = pMapAttrib->GetAttr(XML_HEIGHT, false);
        if (szText)
        {
            if (0 == _tcscmp(XML_AUTO, szText))
                this->m_nConfigHeight = AUTO;
            else
                this->m_nConfigHeight = _ttoi(szText);
        }

        pMapAttrib->GetAttr_int(XML_LAYOUT_GRID_COL, false, &m_nCol);
        pMapAttrib->GetAttr_int(XML_LAYOUT_GRID_ROW, false, &m_nRow);
        pMapAttrib->GetAttr_int(XML_LAYOUT_GRID_COLSPAN, false, &m_nColSpan);
        pMapAttrib->GetAttr_int(XML_LAYOUT_GRID_ROWSPAN, false, &m_nRowSpan);

        szText = pMapAttrib->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
        if (szText)
        {
            Util::ISplitStringEnum*  pEnum = NULL;
            int nCount = Util::SplitString(szText, XML_FLAG_SEPARATOR, &pEnum);
            m_nConfigLayoutFlags = 0;

            for (int i = 0; i < nCount; i++)
            {
                const TCHAR*  szFlag = pEnum->GetText(i);
                if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_LEFT))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_LEFT;
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_RIGHT))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_RIGHT;
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_TOP))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_TOP;
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_BOTTOM))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_BOTTOM;
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_CENTER))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_CENTER;
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_VCENTER))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_VCENTER;
            }
            SAFE_RELEASE(pEnum);
        }
    }
}

void  GridLayoutParam::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_WIDTH, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_HEIGHT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_COL, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_ROW, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_COLSPAN, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_GRID_ROWSPAN, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_ALIGN, szPrefix);
}
SIZE  GridLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

    bool bWidthNotConfiged = m_nConfigWidth == AUTO ? true:false;
    bool bHeightNotConfiged = m_nConfigHeight == AUTO ? true:false;;

    if (bWidthNotConfiged || bHeightNotConfiged)
    {
        // 获取子对象所需要的空间
        UISendMessage(m_pObj, UI_WM_GETDESIREDSIZE, (WPARAM)&size);

        // 如果有指定width、height的其中一个，那么忽略在上一步中得到的值
        if (this->m_nConfigWidth != AUTO)
            size.cx = this->m_nConfigWidth;
        if (this->m_nConfigHeight!= AUTO)
            size.cy = this->m_nConfigHeight;
    }
    else
    {
        size.cx = this->m_nConfigWidth;
        size.cy = this->m_nConfigHeight;
    }

    // 计算 margin 的大小
    size.cx += m_pObj->GetMarginW();
    size.cy += m_pObj->GetMarginH();

    return size;
}


}