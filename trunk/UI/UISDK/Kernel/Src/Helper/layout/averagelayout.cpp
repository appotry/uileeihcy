#include "stdafx.h"
#include "averagelayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

AverageLayout::AverageLayout()
{
    m_nGap = 0;
	this->direction = LAYOUT_AVERAGE_H;   // Ĭ�϶�ջ�Ǵ�����
}
AverageLayout::~AverageLayout()
{
}

void  AverageLayout::SetAttribute(IMapAttribute* pMapAttrib)
{
    if (NULL == pMapAttrib)
        return;

	const TCHAR* szDirection = pMapAttrib->GetAttr(XML_LAYOUT_AVERAGE_DIRECTION, true);
	if (NULL == szDirection)
        return;

    if (0 == _tcscmp(XML_LAYOUT_AVERAGE_H, szDirection))
    {
        this->direction = LAYOUT_AVERAGE_H;
    }
    else if (0 == _tcscmp(XML_LAYOUT_AVERAGE_V, szDirection))
    {
        this->direction = LAYOUT_AVERAGE_V;
    }
    else
    {
        UI_LOG_WARN( _T("AverageLayout::SetAttribute������%s=\"%s\"��ֵ����ȷ"), XML_LAYOUT_AVERAGE_DIRECTION, szDirection);
    }

    pMapAttrib->GetAttr_int(XML_LAYOUT_AVERAGE_GAP, true, &m_nGap);
}

void  AverageLayout::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_AVERAGE_DIRECTION, szPrefix);
}


SIZE  AverageLayout::Measure()
{
	SIZE  size = {0,0};

	// ͨ���ӿؼ�����ȡ�Լ�����Ҫ�Ĵ�С
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
        if (!pChild->IsVisible())
        {
            continue;
        }

        AverageLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();
		

		switch (this->direction)
		{
		case LAYOUT_AVERAGE_H:
			size.cx += s.cx + m_nGap;
			if (size.cy < s.cy)
				size.cy = s.cy;
			break;

		case LAYOUT_AVERAGE_V:
			size.cy += s.cy + m_nGap;
			if (size.cx < s.cx)
				size.cx = s.cx;
			break;
		}
	}

	return size;
}

void  AverageLayout::Arrange(IObject* pIObjToArrage, bool bUpdate)
{
	int  nConsume1 = 0; // ��ǰ�����ĵĸ߶Ȼ��ȣ���left/top��ʼ������
    int  nChildCount = 0;
    int  nAverageSize = 0;
    int  nDiff = 0;  // ���ڽ������������û��ռ�����С�

    // ����ƽ�����
    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pChild = NULL;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pChild->IsMySelfVisible())
            nChildCount++;
    }
    if (0 == nChildCount)
        return;

    if (direction == LAYOUT_AVERAGE_H)
    {
        nAverageSize = (nPanelWidth - m_nGap*(nChildCount-1)) / nChildCount;
        nDiff = nPanelWidth%nChildCount;
    }
    else
    {
        nAverageSize = (nPanelHeight - m_nGap*(nChildCount-1)) / nChildCount;
        nDiff = nPanelHeight%nChildCount;
    }    

    int nIndex = 0;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		if (!pChild->IsMySelfVisible())
			continue;
        nIndex ++;

		switch (this->direction)
		{    
		case LAYOUT_AVERAGE_H:
            {
                CRect  rcChild;
                rcChild.left = nConsume1;
                rcChild.right = rcChild.left + nAverageSize;
                rcChild.top = 0;
                rcChild.bottom = nPanelHeight;

                if (nIndex == nChildCount)
                {
                    rcChild.right += nDiff;
                }

                ArrangeObject_H(pChild, &rcChild);
                nConsume1 += nAverageSize + m_nGap;
            }
			break;

		case LAYOUT_AVERAGE_V:
            {
                CRect  rcChild;
                rcChild.top = nConsume1;
                rcChild.bottom = rcChild.top + nAverageSize;
                rcChild.left = 0;
                rcChild.right = nPanelWidth;

                if (nIndex == nChildCount)
                {
                    rcChild.bottom += nDiff;
                }

                ArrangeObject_V(pChild, &rcChild);
                nConsume1 += nAverageSize + m_nGap;
            }
			break;
		}
	}

    if (bUpdate)
    {
        m_pPanel->UpdateObject(true);
    }
}


void  AverageLayout::ArrangeObject_H(Object* pChildObj, CRect* prc)
{
    pChildObj->SetObjectPos(
        prc->left + pChildObj->GetMarginL(), 
        prc->top + m_pPanel->GetPaddingT(), 
        prc->Width() - pChildObj->GetMarginW(), 
        prc->Height() - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
}


void  AverageLayout::ArrangeObject_V(Object* pChildObj, CRect* prc)
{
    pChildObj->SetObjectPos(
        prc->left + pChildObj->GetMarginL(), 
        prc->top + m_pPanel->GetPaddingT(), 
        prc->Width() - pChildObj->GetMarginW(), 
        prc->Height() - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
}

}