#include "stdafx.h"
#include "stacklayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

StackLayout::StackLayout()
{
    m_nGap = 0;
	this->direction = LAYOUT_STACK_LEFTTORIGHT;   // 默认堆栈是从左到右
}
StackLayout::~StackLayout()
{
}
void  StackLayout::SetAttribute(IMapAttribute* pMapAttrib)
{
    if (NULL == pMapAttrib)
        return;

	const TCHAR* szDirection = pMapAttrib->GetAttr(XML_LAYOUT_STACK_DIRECTION, true);
	if (szDirection)
    {
        if (0 == _tcscmp(XML_LAYOUT_STACK_LEFTTORIGHT, szDirection))
        {
            this->direction = LAYOUT_STACK_LEFTTORIGHT;
        }
        else if (0 == _tcscmp(XML_LAYOUT_STACK_RIGHTTOLEFT, szDirection))
        {
            this->direction = LAYOUT_STACK_RIGHTTOLEFT;
        }
        else if (0 == _tcscmp(XML_LAYOUT_STACK_LEFTANDRIGHT, szDirection))
        {
            this->direction = LAYOUT_STACK_LEFTANDRIGHT;
        }
        else if (0 == _tcscmp(XML_LAYOUT_STACK_TOPTOBOTTOM, szDirection))
        {
            this->direction = LAYOUT_STACK_TOPTOBOTTOM;
        }
        else if (0 == _tcscmp(XML_LAYOUT_STACK_BOTTOMTOTOP, szDirection))
        {
            this->direction = LAYOUT_STACK_BOTTOMTOTOP;
        }
        else if (0 == _tcscmp(XML_LAYOUT_STACK_TOPANDBOTTOM, szDirection))
        {
            this->direction = LAYOUT_STACK_TOPANDBOTTOM;
        }
        else
        {
            UI_LOG_WARN( _T("StackLayout::SetAttribute，属性%s=\"%s\"的值不正确"), XML_LAYOUT_STACK_DIRECTION, szDirection);
        }
    }

    pMapAttrib->GetAttr_int(XML_LAYOUT_STACK_GAP, true, &m_nGap);
}
void  StackLayout::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_STACK_DIRECTION, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_STACK_GAP, szPrefix);
}

SIZE  StackLayout::Measure()
{
	SIZE  size = {0,0};

	// 通过子控件来获取自己所需要的大小
	Object*  pChild = NULL;
	while (pChild = this->m_pPanel->EnumChildObject(pChild))
	{
        if (!pChild->IsVisible())
        {
            continue;
        }

        StackLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE  s = pChild->GetDesiredSize();

		if (IsHorz())
        {
			size.cx += s.cx + m_nGap;
			if (size.cy < s.cy)
				size.cy = s.cy;
        }
        else
        {
			size.cy += s.cy + m_nGap;
			if (size.cx < s.cx)
				size.cx = s.cx;
		}
	}

	return size;
}

bool  StackLayout::IsHorz()
{
    if (direction == LAYOUT_STACK_LEFTTORIGHT || 
        direction == LAYOUT_STACK_RIGHTTOLEFT ||
        direction == LAYOUT_STACK_LEFTANDRIGHT)
        return true;

    return false;
}
bool  StackLayout::IsVert()
{
    return !IsHorz();
}

void  StackLayout::Arrange(IObject* pIObjToArrage, bool bUpdate)
{
    Object* pObjToArrage = NULL;
    if (pIObjToArrage)
    {
        pObjToArrage = pIObjToArrage->GetImpl();
    }

	int  nConsume1 = 0; // 当前已消耗的高度或宽度（从left/top开始计数）
	int  nConsume2 = 0; // 当前已消耗的高度或宽度（从right/bottom开始计数）
	
	bool bStartToArrange = false;
	if (NULL == pObjToArrage)
    {
		bStartToArrange = true;
    }

	Object* pChild = NULL;
	while (pChild = m_pPanel->EnumChildObject(pChild))
	{
		if (pObjToArrage && pChild == pObjToArrage)
        {
			bStartToArrange = true;
        }

		if (!pChild->IsMySelfVisible())
        {
			continue;
        }

        StackLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }

		SIZE s;
		if (bStartToArrange)
		{
            s = pChild->GetDesiredSize();
            if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_FILL)
            {
                // 计算出剩下的对象的所需大小
                Object* pChildTemp = pChild;
                SIZE sNeedNext = {0, 0}; 
                while (pChildTemp = m_pPanel->EnumChildObject(pChildTemp))
                {
                    if (!pChildTemp->IsMySelfVisible())
                    {
                        continue;
                    }

                    SIZE sThis = pChildTemp->GetDesiredSize();
                    if (IsHorz())
                    {
                        sNeedNext.cx += sThis.cx + m_nGap;
                    }
                    else
                    {
                        sNeedNext.cy += sThis.cy + m_nGap;
                    }
                }

                CRect rcClient;
                m_pPanel->GetClientRect(&rcClient);
                int  nPanelWidth  = rcClient.Width();
                int  nPanelHeight = rcClient.Height();

                if (IsHorz())
                {
                    s.cx = nPanelWidth - sNeedNext.cx - nConsume2 - nConsume1;
                }
                else
                {
                    s.cy = nPanelHeight - sNeedNext.cy - nConsume2 - nConsume1;
                }
            }
		}
		else
		{
			s.cx = pChild->GetWidth() + pChild->GetMarginW();
			s.cy = pChild->GetHeight() + pChild->GetMarginH();
		}


		switch (this->direction)
		{    
		case LAYOUT_STACK_LEFTTORIGHT:
            ArrangeObject_Left(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_RIGHTTOLEFT:
            ArrangeObject_Right(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_LEFTANDRIGHT:
			{
				bool bDockLeft = true;   // 默认停靠在左侧

				if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_LEFT)
					bDockLeft = true;
				else if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_RIGHT)
					bDockLeft = false;
				else
					bDockLeft = true;

                // 计算位置
				if (bDockLeft)
                    ArrangeObject_Left(pChild, nConsume1, nConsume2, s);
				else
                    ArrangeObject_Right(pChild, nConsume1, nConsume2, s);

			}
			break;

			// 从上到下
		case LAYOUT_STACK_TOPTOBOTTOM:
			ArrangeObject_Top(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_BOTTOMTOTOP:
			ArrangeObject_Bottom(pChild, nConsume1, nConsume2, s);
			break;

		case LAYOUT_STACK_TOPANDBOTTOM:
			{
				bool bDockTop = true;   // 默认停靠在上侧

				if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_TOP)
					bDockTop = true;
				else if (pParam->GetConfigLayoutFlags() & LAYOUT_ITEM_ALIGN_BOTTOM)
					bDockTop = false;
				else
					bDockTop = true;

                // 计算位置
				if (bDockTop)
					ArrangeObject_Top(pChild, nConsume1, nConsume2, s);
				else
					ArrangeObject_Bottom(pChild, nConsume1, nConsume2, s);
			}
			break;
		}
	}

    if (bUpdate)
    {
        m_pPanel->UpdateObject(true);
    }
}


void  StackLayout::ArrangeObject_Left(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  y = 0;
    int  h = nPanelHeight;

    int nMaxWidthNow = nPanelWidth-nLeftCustom-nRightCustom;
    if (size.cx > nMaxWidthNow)
        size.cx = nMaxWidthNow;

    if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
    {
        y = 0;
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
    {
        y = (nPanelHeight - size.cy);
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_VCENTER)
    {
        y = (nPanelHeight - size.cy)/2;
        h = size.cy;
    }
  
    pChildObj->SetObjectPos(
        nLeftCustom + pChildObj->GetMarginL(), 
        y + pChildObj->GetMarginT(), 
        size.cx - pChildObj->GetMarginW(), 
        h - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);

    nLeftCustom += size.cx + m_nGap;
}

void  StackLayout::ArrangeObject_Right(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  y = 0;
    int  h = nPanelHeight;

    int nMaxWidthNow = nPanelWidth-nLeftCustom-nRightCustom;
    if (size.cx > nMaxWidthNow)
        size.cx = nMaxWidthNow;

    if (nFlag & LAYOUT_ITEM_ALIGN_TOP)
    {
        y = 0;
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_BOTTOM)
    {
        y = (nPanelHeight - size.cy);
        h = size.cy;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_VCENTER)
    {
        y = (nPanelHeight - size.cy)/2;
        h = size.cy;
    }

    pChildObj->SetObjectPos(
        nPanelWidth - nRightCustom - size.cx + pChildObj->GetMarginL(), 
        0 + pChildObj->GetMarginT(), 
        size.cx - pChildObj->GetMarginW(), 
        h - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);

    nRightCustom += size.cx + m_nGap;
}

void  StackLayout::ArrangeObject_Top(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size)
{
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  x = 0;
    int  w = nPanelWidth;

    int nMaxHeigthNow = nPanelHeight-nTopCustom-nBottomCustom;
    if (size.cy > nMaxHeigthNow)
        size.cy = nMaxHeigthNow;

    if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
    {
        x = 0;
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
    {
        x = (nPanelWidth - size.cx);
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
    {
        x = (nPanelWidth - size.cx)/2;
        w = size.cx;
    }

    pChildObj->SetObjectPos(
        x + pChildObj->GetMarginL(), 
        nTopCustom + pChildObj->GetMarginT(), 
        w - pChildObj->GetMarginW(), 
        size.cy - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);

    nTopCustom += size.cy + m_nGap;
}
void  StackLayout::ArrangeObject_Bottom(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size)
{
    // 调用该函数时，自己的大小已经被求出来了
    StackLayoutParam* pParam = s_GetObjectLayoutParam(pChildObj);
    if (!pParam)
        return;

    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nPanelWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nPanelHeight = rcClient.Height(); //m_pPanel->GetHeight();

    int  nFlag = pParam->GetConfigLayoutFlags();
    int  x = 0;
    int  w = nPanelWidth;

    int nMaxHeigthNow = nPanelHeight-nTopCustom-nBottomCustom;
    if (size.cy > nMaxHeigthNow)
        size.cy = nMaxHeigthNow;

    if (nFlag & LAYOUT_ITEM_ALIGN_LEFT)
    {
        x = 0;
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_RIGHT)
    {
        x = (nPanelWidth - size.cx);
        w = size.cx;
    }
    else if (nFlag & LAYOUT_ITEM_ALIGN_CENTER)
    {
        x = (nPanelWidth - size.cx)/2;
        w = size.cx;
    }

    pChildObj->SetObjectPos( 
        x + pChildObj->GetMarginL(), 
        nPanelHeight - nBottomCustom - size.cy + pChildObj->GetMarginT(), 
        w - pChildObj->GetMarginW(), 
        size.cy - pChildObj->GetMarginH(),
        SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);

    nBottomCustom += size.cy + m_nGap;
}

//////////////////////////////////////////////////////////////////////////

StackLayoutParam::StackLayoutParam()
{
    m_nConfigWidth = AUTO;
    m_nConfigHeight = AUTO;
    m_nConfigLayoutFlags = 0;
}
StackLayoutParam::~StackLayoutParam()
{

}

void  StackLayoutParam::UpdateByRect()
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
void  StackLayoutParam::Serialize(SERIALIZEDATA* pData)
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
                else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_FILL))
                    m_nConfigLayoutFlags |= LAYOUT_ITEM_ALIGN_FILL;
            }
            SAFE_RELEASE(pEnum);
        }
    }
}
void  StackLayoutParam::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_WIDTH, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_HEIGHT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_ALIGN, szPrefix);
}

SIZE  StackLayoutParam::CalcDesiredSize()
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