#include "stdafx.h"
#include "canvaslayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

SIZE  CanvasLayout::Measure()
{
    SIZE size = {0,0};

    // 通过子控件来获取自己所需要的大小
    Object*  pChild = NULL;
    while (pChild = this->m_pPanel->EnumChildObject(pChild))
    {
        CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
        if (!pParam)
        {
            continue;
        }
        SIZE  s = pChild->GetDesiredSize();
        

        if (NDEF != pParam->GetConfigLeft())
        {
            s.cx += pParam->GetConfigLeft();
        }
        else if (NDEF != pParam->GetConfigRight())
        {
            s.cx += pParam->GetConfigRight();
        }
        if (NDEF != pParam->GetConfigTop())
        {
            s.cy += pParam->GetConfigTop();
        }
        else if (NDEF != pParam->GetConfigBottom())
        {
            s.cy += pParam->GetConfigBottom();
        }

        if (size.cx < s.cx)
        {
            size.cx = s.cx;
        }
        if (size.cy < s.cy)
        {
            size.cy = s.cy;
        }
    }
    return size;
}
void  CanvasLayout::Arrange(IObject* pIObjToArrage, bool bUpdate)
{
    Object* pObjToArrage = NULL;
    if (pIObjToArrage)
        pObjToArrage = pIObjToArrage->GetImpl();

    // 调用该函数时，自己的大小已经被求出来了
    CRect rcClient;
    m_pPanel->GetClientRect(&rcClient);
    int  nWidth  = rcClient.Width();  //m_pPanel->GetWidth();
    int  nHeight = rcClient.Height(); //m_pPanel->GetHeight();

    Object* pChild = NULL;
    while (pChild = m_pPanel->EnumChildObject(pChild))
    {
        if (pObjToArrage && pObjToArrage != pChild)
            continue;

        if (!pChild->IsMySelfVisible())
        {
            if (pObjToArrage)
                break;
            else
                continue;
        }

        this->ArrangeObject(pChild, nWidth, nHeight);

        if (pObjToArrage && pObjToArrage == pChild)
            break;
    }
    if (bUpdate)
    {
        m_pPanel->UpdateObject(true);
    }
}

void  CanvasLayout::ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight)
{
    int  x = 0, y = 0;                                  // pChild最终在parent中的坐标

    CanvasLayoutParam* pParam = s_GetObjectLayoutParam(pChild);
    if (!pParam)
    {
        return;
    }

    int left = pParam->GetConfigLeft();
    int top  = pParam->GetConfigTop();
    int right  = pParam->GetConfigRight();
    int bottom = pParam->GetConfigBottom();
    int nConfigWidth = pParam->GetConfigWidth();
    int nConfigHeight = pParam->GetConfigHeight();
    int nConfigFlag = pParam->GetConfigLayoutFlags();

    //////////////////////////////////////////////////////////////////////////
    // 计算出 pChild 的 rectP的宽和高
    SIZE s = pParam->CalcDesiredSize();

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
    if (left != NDEF)
    {
        x = left;
        x += pChild->GetMarginL();
    }
    else
    {
        if (right != NDEF)
        {
            x = nWidth - right - nChildW;
            x -= pChild->GetMarginR();
        }
        else
        {
            if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)
            {
                x = ( nWidth - s.cx ) / 2;  // 居中
            }
            else
            {
                x = 0;
                x += pChild->GetMarginL();
            }
        }
    }
    if (top != NDEF)
    {
        y = top;
        y += pChild->GetMarginT();
    }
    else
    {
        if (bottom != NDEF)
        {
            y = nHeight - bottom - nChildH;
            y -= pChild->GetMarginB();
        }
        else
        {
            if (nConfigFlag & LAYOUT_ITEM_ALIGN_CENTER)
            {
                y = ( nHeight - s.cy ) / 2;  // 居中
            }
            else
            {
                y = 0;
                y += pChild->GetMarginT();
            }
        }
    }

    rcChildObj.OffsetRect(x, y);
    pChild->SetObjectPos(&rcChildObj, SWP_NOREDRAW|SWP_NOUPDATELAYOUTPOS);
}
//////////////////////////////////////////////////////////////////////////

CanvasLayoutParam::CanvasLayoutParam()
{
    m_nConfigWidth = m_nConfigHeight = AUTO;
    m_nConfigLeft = m_nConfigRight = m_nConfigTop = m_nConfigBottom = NDEF;
    m_nConfigLayoutFlags = 0;

}
CanvasLayoutParam::~CanvasLayoutParam()
{

}

SIZE  CanvasLayoutParam::CalcDesiredSize()
{
    SIZE size = {0,0};

    bool bWidthNotConfiged = ((m_nConfigLeft == NDEF || m_nConfigRight == NDEF) && m_nConfigWidth==AUTO);
    bool bHeightNotConfiged = ((m_nConfigTop == NDEF || m_nConfigBottom == NDEF) && m_nConfigHeight == AUTO);

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

void  CanvasLayoutParam::UpdateByRect()
{
    Object* pParent = m_pObj->GetParentObject();
    if (!pParent)
        return;

    CRect  rcParent;
    CRect  rcPanel;
    m_pObj->GetParentRect(&rcParent);
    pParent->GetClientRect(&rcPanel);

    if (m_nConfigLeft >= 0)
    {
        m_nConfigLeft = rcParent.left;
    }
    if (m_nConfigTop >= 0)
    {
        m_nConfigTop = rcParent.top;
    }
    if (m_nConfigRight >= 0)
    {
        m_nConfigRight = rcPanel.Width() - rcParent.right;
    }
    if (m_nConfigBottom >= 0)
    {
        m_nConfigBottom = rcPanel.Height() - rcParent.bottom;
    }
    if (m_nConfigWidth >= 0)
    {
        m_nConfigWidth = rcParent.Width();
    }
    if (m_nConfigHeight >= 0)
    {
        m_nConfigHeight = rcParent.Height();
    }
}
void  CanvasLayoutParam::Serialize(SERIALIZEDATA* pData)
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

        pMapAttrib->GetAttr_int(XML_LAYOUT_ITEM_LEFT,   false, &m_nConfigLeft);
        pMapAttrib->GetAttr_int(XML_LAYOUT_ITEM_TOP,    false, &m_nConfigTop);
        pMapAttrib->GetAttr_int(XML_LAYOUT_ITEM_RIGHT,  false, &m_nConfigRight);
        pMapAttrib->GetAttr_int(XML_LAYOUT_ITEM_BOTTOM, false, &m_nConfigBottom);

        szText = pMapAttrib->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
        if (szText)
        {
            m_nConfigLayoutFlags = ParseAlignAttr(szText);
        }
    }
}
void  CanvasLayoutParam::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_WIDTH, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_HEIGHT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_LEFT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_TOP, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_RIGHT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_BOTTOM, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_LAYOUT_ITEM_ALIGN, szPrefix);
}

int  CanvasLayoutParam::ParseAlignAttr(const TCHAR* szAttr)
{
    Util::ISplitStringEnum*  pEnum = NULL;
    int nCount = Util::SplitString(szAttr, XML_FLAG_SEPARATOR, &pEnum);

    int nRet = 0;

    for (int i = 0; i < nCount; i++)
    {
        const TCHAR*  szFlag = pEnum->GetText(i);
        if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_LEFT))
            nRet |= LAYOUT_ITEM_ALIGN_LEFT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_RIGHT))
            nRet |= LAYOUT_ITEM_ALIGN_RIGHT;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_TOP))
            nRet |= LAYOUT_ITEM_ALIGN_TOP;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_BOTTOM))
            nRet |= LAYOUT_ITEM_ALIGN_BOTTOM;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_CENTER))
            nRet |= LAYOUT_ITEM_ALIGN_CENTER;
        else if (0 == _tcscmp(szFlag, XML_LAYOUT_ITEM_ALIGN_VCENTER))
            nRet |= LAYOUT_ITEM_ALIGN_VCENTER;
    }
    SAFE_RELEASE(pEnum);

    return nRet;
}

}