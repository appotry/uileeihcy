#include "stdafx.h"
#include "layout.h"

#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"

#if 0

SIZE  DockLayout::MeasureChildObject()
{
	SIZE size = {0,0};

	// 1. �Ȼ�ȡcenter�����SIZE
	Object*  pCenterObj = NULL;
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
		if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
			pCenterObj = pChild;
		else if (0 == (nFlag&LAYOUT_ITEM_ALIGN_LTRB))
			pCenterObj = pChild;
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_CENTER == strDock ||
			strDock.empty() )
		{
			pCenterObj = pChild;
			break;
		}
#endif
	}

	if (pCenterObj)
		size = pCenterObj->GetDesiredSize();

	// 2. �Ӻ�ʼ����ͣ�������ܵ��Ӷ���Ĵ�С
	pChild = NULL;
	while (pChild = this->m_pPanel->REnumChildObject(pChild))
	{
		if (!pChild->IsVisible())
			continue;

		int nFlag = pChild->GetConfigLayoutFlags();

#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT || LAYOUT_ITEM_ALIGN_RIGHT)
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);

		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock ||
			XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			if (s.cy > size.cy)
				size.cy = s.cy;
			size.cx += s.cx;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP || LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock ||
			 XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			if (s.cx > size.cx)
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
void  DockLayout::ArrangeChildObject(Object* pObjToArrage)
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
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
		if( !pChild->IsVisible() )
			continue;

#if 1
		int nFlag = pChild->GetConfigLayoutFlags();
#else
		String  strDock;
		pChild->GetAttribute(XML_LAYOUT_DOCK_DOCK, strDock);
#endif

		// ����� pChild �� rectP
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
#else
		if (XML_LAYOUT_DOCK_DOCK_LEFT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			
			CRect rcObj( 
				nComsumeLeft             +  pChild->GetMarginL(), 
				nComsumeTop              +  pChild->GetMarginT(), 
				nComsumeLeft + s.cx      -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeLeft += s.cx;
		}
		else
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
#else
		if (XML_LAYOUT_DOCK_DOCK_RIGHT == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();
			
			CRect rcObj( 
				nWidth - nComsumeRight - s.cx  +  pChild->GetMarginL(), 
				nComsumeTop                    +  pChild->GetMarginT(), 
				nWidth - nComsumeRight         -  pChild->GetMarginR(), 
				nHeight - nComsumeBottom       -  pChild->GetMarginB());
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeRight += s.cx;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
#else
		if (XML_LAYOUT_DOCK_DOCK_TOP == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();

			CRect rcObj( 
				nComsumeLeft            +  pChild->GetMarginL() , 
				nComsumeTop             +  pChild->GetMarginT() , 
				nWidth - nComsumeRight  -  pChild->GetMarginR() , 
				nComsumeTop + s.cy      -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

			nComsumeTop += s.cy;
		}
		else 
#if 1
		if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
#else
		if (XML_LAYOUT_DOCK_DOCK_BOTTOM == strDock)
#endif
		{
			SIZE s = pChild->GetDesiredSize();

			CRect rcObj(
				nComsumeLeft                    +  pChild->GetMarginL() , 
				nHeight - nComsumeBottom - s.cy +  pChild->GetMarginT() , 
				nWidth - nComsumeRight          -  pChild->GetMarginR() , 
				nHeight - nComsumeBottom        -  pChild->GetMarginB() );
			pChild->SetObjectPos(&rcObj, SWP_NOREDRAW);

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
			SWP_NOREDRAW);
	}
}

#endif
void  DesktopLayout::Arrange(WindowBase*  pWindow)
{
	if (NULL == pWindow)
		return;

    if (pWindow->IsChildWindow())
    {
        HWND hWndParent = GetParent(pWindow->m_hWnd);
        CRect rc;
        ::GetClientRect(hWndParent, &rc);

        int nCX = rc.Width();
        int nCY = rc.Height();

        CanvasLayout::ArrangeObject(pWindow, nCX, nCY);
        return;
    }

	// ��ȡ��������ֵ������rectW
	int  x = 0, y = 0;                                 // ��������Ļ�е�����
	int  nCXScreen = 800, nCYScreen = 600;             // ��Ļ��С
	int  left = NDEF, top = NDEF, right = NDEF, bottom = NDEF;

    CanvasLayoutParam* pParam = CanvasLayout::s_GetObjectLayoutParam(pWindow);
    if (!pParam)
    {
        return;
    }
    left = pParam->GetConfigLeft();
    top = pParam->GetConfigTop();
    right = pParam->GetConfigRight();
    bottom = pParam->GetConfigBottom();

    SIZE s = pWindow->GetDesiredSize();  // ��õ�SIZE������MARGIN�Ĵ�С s.cx=margin.left+width+margin.right

    // �������Ļ��������Ĵ�С(��ʹ��������Ļ�Ĵ�С)
    //nCXScreen = ::GetSystemMetrics( SM_CXSCREEN );
    //nCYScreen = ::GetSystemMetrics( SM_CYSCREEN );

    CRect rcWorkSize;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkSize, 0);
    nCXScreen = rcWorkSize.Width();
    nCYScreen = rcWorkSize.Height();
	
    // ���ͬʱָ����left/right,�����width����
    if (left != NDEF && right != NDEF)
    {
        s.cx = nCXScreen - left - right;
    }
    // ���ͬʱָ����top/bottom�������height����
    if (top != NDEF && bottom != NDEF)
    {
        s.cy = nCYScreen - top - bottom;
    }

	// ���������
	if (left != NDEF)
	{
		x = left;
		x += pWindow->GetMarginL();
	}
	else
	{
		if (right != NDEF)
		{
			x = nCXScreen - right - s.cx;  // right��ָ�����Ҳ������Ļ�Ҳ�ľ���
			x -= pWindow->GetMarginR();
		}
		else
		{
			// ����
			x = ( nCXScreen - s.cx) / 2;
		}
	}
	if (top != NDEF)
	{
		y = top;
		y += pWindow->GetMarginT();
	}
	else
	{
		if (bottom != NDEF)
		{
			y = nCYScreen - bottom - s.cy; // ͬright
			y -= pWindow->GetMarginB();
		}
		else
		{
			// ����
			y = (nCYScreen - s.cy) / 2;
		}
	}

	pWindow->SetObjectPos(x, y, s.cx, s.cy, SWP_NOREDRAW);

	// �ݹ�
	//pWindow->GetLayout()->Arrange(NULL); // <-- SetObjectPos�ᴥ��OnSize����Window��OnSize��Ϣ�л����layout�������������Բ�����
}
