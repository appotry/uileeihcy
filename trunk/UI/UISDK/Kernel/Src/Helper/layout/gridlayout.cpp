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
		1. ����Grid��ÿһ�еĿ��
	*/
	const TCHAR* szWidth = pMapAttrib->GetAttr(XML_LAYOUT_GRID_WIDTH, true);
    if (NULL == szWidth)
	{
		// �������gridlayoutû�ж���layout.width����Ĭ��layout.width="*"
		szWidth = XML_ASTERISK;
	}

    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(szWidth, XML_SEPARATOR, &pEnum);
	for (int i = 0; i < nCount; i++)
	{
		String  str = pEnum->GetText(i);

		// �ж��Ƿ�Ϊ AUTO ����
		if (XML_AUTO == str)
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->widths.push_back( wh );
			continue;
		}

		// �ж��Ƿ�Ϊ * ����
		int nIndex = str.find(XML_ASTERISK);
		if (String::npos != nIndex)
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi(str.c_str());
			if (nXml == 0)  // ���ָֻ�� "*"����ô��ʾ"1*"
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
    SAFE_RELEASE(pEnum);

	/*
		2. ����Grid��ÿһ�еĸ߶�
	*/
	const TCHAR* szHeight = pMapAttrib->GetAttr(XML_LAYOUT_GRID_HEIGHT, true);
    if (NULL == szHeight)
	{
		// �������gridlayoutû�ж���layout.height����Ĭ��layout.height="*"
		szHeight = XML_ASTERISK;
	}

    nCount = Util::SplitString(szHeight, XML_SEPARATOR, &pEnum);
	for (int i = 0; i < nCount; i++)
	{
		String str = pEnum->GetText(i);

		// �ж��Ƿ�Ϊ AUTO ����
		if (XML_AUTO == str)
		{
			GridWH  wh;
			wh.type = GWHT_AUTO;

			this->heights.push_back( wh );
			continue;
		}

		// �ж��Ƿ�Ϊ * ����
		int nIndex = str.find(XML_ASTERISK);
		if (String::npos != nIndex)
		{
			GridWH  wh;
			wh.type = GWHT_ASTERISK;

			int nXml = _ttoi(str.c_str());
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

		// �����˷�Χ
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN( _T("GridLayout::MeasureChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] ����grid��Χ [col=%d,row=%d] )"),
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}
 
		// ���ڿ�͸߶����ںϲ���Ԫ��Ķ��󣬷��������ļ���
		// ͬʱ�����Ժϲ��л�ϲ��еļ��㣬���Ƕ���һ���ϲ��У������ֻռһ�еĻ�
		// ���������еĴ�С
		if (nnGridRowspan != 1 && nnGridColspan != 1)
		{
				continue;
		}
	
		// ����������ڵ��к��ж��й̶���ֵ����ô����Ҫ����
		if (widths[nCol].type == GWHT_VALUE &&
			heights[nRow].type == GWHT_VALUE)
		{
			continue;
		}

		SIZE s = pChild->GetDesiredSize();
		
		// ���ö��������еĿ��
		if (nnGridColspan == 1)
		{
			switch (widths[nCol].type)
			{
			case GWHT_VALUE:     // ���е�ֵ�ǹ̶�ֵ������Ҫ�޸�
				break;

			case GWHT_ASTERISK:  // ������ƽ�ִ�С����������ȡ��һ�е����ֵ����Arrange������پ��帳ֵ
				if( s.cx > widths[nCol].last )
					widths[nCol].last = s.cx;
				break;

			case GWHT_AUTO:      // �������Զ���С����ôȡ��һ�е����ֵ
				if (s.cx > widths[nCol].last)
					widths[nCol].last = s.cx;
				break;

			default:
				assert(false);
				break;
			}
		}

		//���ö��������еĸ߶�
		if (nnGridRowspan == 1)
		{
			switch (heights[nRow].type)
			{
			case GWHT_VALUE:     // ���е�ֵ�ǹ̶�ֵ������Ҫ�޸�
				break;

			case GWHT_ASTERISK:  // ������ƽ�ִ�С����������ȡ��һ�е����ֵ����Arrange������پ��帳ֵ
				if (s.cy > heights[nRow].last)
					heights[nRow].last = s.cy;
				break;

			case GWHT_AUTO:      // �������Զ���С����ôȥ��һ�е����ֵ
				if (s.cy > heights[nRow].last)
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
	// ������Ļ������¼���
	for (int i = 0; i < nGridCols; i++)
	{
		if (this->widths[i].type == GWHT_ASTERISK)
		{
			widths[i].last = (int)(widths[i].xml * maxRate);
		}
	}

	// ͬ��������еĸ߶�
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

	// ����panel��Ҫ�Ĵ�С
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
	// ���øú���ʱ���Լ��Ĵ�С�Ѿ����������

	// �����GRID�Ŀ�Ⱥ͸߶ȣ�����Ը����Ӷ���Ĳ��ֶ����Ȼ���GRID��
	CRect rcClient;
	m_pPanel->GetClientRect(&rcClient);
	int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
	int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

	int nGridRows = (int)this->heights.size();  // Grid������
	int nGridCols = (int)this->widths.size();   // Grid������

	// ��ʼ����������һ�εĽ��Ӱ�챾�εĲ���
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

    // ��һ��. �����Ȱ���auto����
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

		// �����˷�Χ
		if (nCol >= nGridCols || nRow >= nGridRows)
		{
			UI_LOG_WARN( _T("GridLayout::ArrangeChildObject, Object[ m_strID=\"%s\", col=%d, row=%d ] ����grid��Χ [col=%d,row=%d] )"),
				pChild->GetId(), nCol, nRow, nGridCols, nGridRows );
			continue;
		}

		// ֻ��auto����
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
	
	// �ڶ���. ����*����
	int nWidthRemain  = nWidth;   // ��ȥauto��value���ͺ�ʣ��Ŀ��
	int nHeightRemain = nHeight;  // ��ȥauto��value���ͺ�ʣ��Ŀ��
	int nASTERISKWidth  = 0;      // ����е�*������
	int nASTERISKHeight = 0;      // �߶��е�*������

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

	// ��ʼƽ��(TODO. ���������ĳ��������������ɼ������ص���
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


	// ������. ���Ÿ����ؼ���λ��
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

// ��ȡ��nCol����GRID�����ľ���
// ע�⣺����Ĳ��������Ƕ����col+colspan����˸�ֵ���ܻᳬ��grid�������
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
// ��ȡ��nRow����GRID������ľ���
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

        //  ���ò������ֵ
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
        // ��ȡ�Ӷ�������Ҫ�Ŀռ�
        UISendMessage(m_pObj, UI_WM_GETDESIREDSIZE, (WPARAM)&size);

        // �����ָ��width��height������һ������ô��������һ���еõ���ֵ
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

    // ���� margin �Ĵ�С
    size.cx += m_pObj->GetMarginW();
    size.cy += m_pObj->GetMarginH();

    return size;
}


}