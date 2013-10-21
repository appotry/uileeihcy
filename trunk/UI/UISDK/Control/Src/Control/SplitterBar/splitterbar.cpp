#include "stdafx.h"
#include "splitterbar.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{

SplitterBar::SplitterBar()
{
    m_pISplitterBar = NULL;
    m_pObjLeft = NULL;
    m_pObjRight = NULL;

    m_ptLButtonDown.x = -1;
    m_ptLButtonDown.y = -1;
    m_ptLastMouseMove.x = -1;
    m_ptLastMouseMove.y = -1;
    m_nDeviation = 0;
    m_bButtonDown = false;

    m_nLeftMin = NDEF;
    m_nLeftMax = NDEF;
    m_nRightMin = NDEF;
    m_nRightMax = NDEF;
    m_rcAvailable.SetRectEmpty();
}
SplitterBar::~SplitterBar()
{

}

void  SplitterBar::ResetAttribute()
{
    DO_PARENT_PROCESS(ISplitterBar, IPanel);

    m_pObjLeft = NULL;
    m_pObjRight = NULL;

    m_eDirection = SPLITTERBAR_DIRECTION_H;
    m_eAlign = SPLITTERBAR_ALIGN_BOTTOM;

    m_nLeftMin = NDEF;
    m_nLeftMax = NDEF;
    m_nRightMin = NDEF;
    m_nRightMax = NDEF;

    m_pISplitterBar->ModifyStyle(0, OBJECT_STYLE_REJECT_MOUSE_MSG_SELF, 0);
}

void  SplitterBar::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(ISplitterBar, IPanel);

    const TCHAR* szText = pMapAttr->GetAttr(XML_SPLITTERBAR_DIRECTION, true);

    SPLITTERBAR_DIRECTION eDir = SPLITTERBAR_DIRECTION_H;
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_SPLITTERBAR_DIRECTION_H))
            eDir = SPLITTERBAR_DIRECTION_H;
        else if (0 == _tcscmp(szText, XML_SPLITTERBAR_DIRECTION_V))
            eDir = SPLITTERBAR_DIRECTION_V;
    }
    SetDirection(eDir);
   
    SPLITTERBAR_ALIGN  eAlign = SPLITTERBAR_ALIGN_BOTTOM;
    szText = pMapAttr->GetAttr(XML_SPLITTERBAR_ALIGN, true);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_SPLITTERBAR_ALIGN_LEFT))
            eAlign = SPLITTERBAR_ALIGN_LEFT;
        else if (0 == _tcscmp(szText, XML_SPLITTERBAR_ALIGN_RIGHT))
            eAlign = SPLITTERBAR_ALIGN_RIGHT;
        else if (0 == _tcscmp(szText, XML_SPLITTERBAR_ALIGN_TOP))
            eAlign = SPLITTERBAR_ALIGN_TOP;
        else if (0 == _tcscmp(szText, XML_SPLITTERBAR_ALIGN_BOTTOM))
            eAlign = SPLITTERBAR_ALIGN_BOTTOM;
    }
    SetAlign(eAlign);

    if (IsHorz())
    {
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nLeftMin);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nRightMin);

        pMapAttr->GetAttr_int(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nLeftMax);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nRightMax);
    }
    else
    {
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nLeftMin);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nRightMin);

        pMapAttr->GetAttr_int(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nLeftMax);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nRightMax);
    }
}
void  SplitterBar::OnObjectLoaded()
{
    IObject* pParent = m_pISplitterBar->GetParentObject();
    if (!pParent)
        return;

    if (IsHorz())
    {
        IMapAttribute*  pMapAttrib = NULL;
        m_pISplitterBar->GetMapAttribute(&pMapAttrib);

        const TCHAR* szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjTop = pParent->FindChildObject(szText);
        }

        szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjBottom = pParent->FindChildObject(szText);
        }

        SAFE_RELEASE(pMapAttrib);
    }
    else
    {
        IMapAttribute*  pMapAttrib = NULL;
        m_pISplitterBar->GetMapAttribute(&pMapAttrib);

        const TCHAR* szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjLeft = pParent->FindChildObject(szText);
        }

        szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjRight = pParent->FindChildObject(szText);
        }

        SAFE_RELEASE(pMapAttrib);
    }
}

void  SplitterBar::SetDirection(SPLITTERBAR_DIRECTION eDir)
{
    m_eDirection = eDir;

    if (m_eDirection == SPLITTERBAR_DIRECTION_H)
    {
        m_pISplitterBar->SetCursor(XML_CURSOR_IDC_SPLITBAR_H);
    }
    else
    {
        m_pISplitterBar->SetCursor(XML_CURSOR_IDC_SPLITBAR_V);
    }
}

void  SplitterBar::SetAlign(SPLITTERBAR_ALIGN eAlign)
{
    m_eAlign = eAlign;
}

// 表示的是分隔条控件的方向
bool  SplitterBar::IsVert()
{
    return m_eDirection == SPLITTERBAR_DIRECTION_V ? true:false;
}
bool  SplitterBar::IsHorz()
{
    return m_eDirection == SPLITTERBAR_DIRECTION_H ? true:false;
}

void  SplitterBar::OnSize(UINT nType, int cx, int cy)
{
    OnParentSizeChanged();
    SetMsgHandled(FALSE);
    return;
}

void  SplitterBar::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLButtonDown = point;
    m_bButtonDown = true;

    // 计算误差
    POINT pt;
    m_pISplitterBar->WindowPoint2ObjectPoint(&point, &pt);
    if (IsVert())
    {
        m_nDeviation = m_pISplitterBar->GetMarginL() + pt.x;
    }
    else
    {
        m_nDeviation = m_pISplitterBar->GetMarginT() + pt.y;
    }

//     IWindowBase* pWindow = m_pISplitterBar->GetWindowObject();
//     if (pWindow)
//     {
//         pWindow->GetKeyboardMgr()->SetCapture(this, 0);
//     }
    m_ptLastMouseMove = point;  // 防止第一次响应MOUSEMOVE时就去调用ondraging
    CalcAvailableRegion();
}
void  SplitterBar::OnLButtonUp(UINT nFlags, POINT point)
{
    this->OnCancelMode();
}
void  SplitterBar::OnCancelMode()
{
    m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
    m_ptLastMouseMove.x = m_ptLastMouseMove.y = -1;
    m_bButtonDown = false;
    m_nDeviation = 0;
    m_rcAvailable.SetRectEmpty();
}
void  SplitterBar::OnMouseMove(UINT nFlags, POINT point)
{
    if (!m_bButtonDown)
        return;

    // 防止外部其它因素产生的mousemove消息持续发送，导致CPU占用率高的问题
    // 也用于防止无意义的鼠标移动
    if (IsVert() && point.x == m_ptLastMouseMove.x)
        return;
    if (IsHorz() && point.y == m_ptLastMouseMove.y)
        return;

    m_ptLastMouseMove = point;

    // 在这里传递给layout的坐标参数全部都转换为拖拽条的最左侧/最上侧位置，避免鼠标点击位置不同而产生的误差
    if (IsVert())
        point.x -= m_nDeviation;
    else
        point.y -= m_nDeviation;

    // 将point转换成相对于自己的parent，因为自己的位置也是相对于parent的
    POINT ptParent;
    m_pISplitterBar->GetParentObject()->WindowPoint2ObjectPoint(&point, &ptParent);
    this->OnDraging(ptParent);   
}

void  SplitterBar::OnDraging(POINT ptInParent)  // 相对于父对象的坐标
{
    POINT ptAvailable = {0,0};
    if (false == this->TestAvailableDragPos(ptInParent, &ptAvailable))
        return;

    if (IsVert())
    {
        UpdateLeftRightCtrlPos(ptAvailable.x);
    }
    else
    {
        UpdateUpBottomCtrlPos(ptAvailable.y);
    }
}


void  SplitterBar::CalcAvailableRegion()
{
    m_rcAvailable.SetRectEmpty();

    if (NULL == m_pObjLeft || NULL == m_pObjRight)
        return;

    CRect  rcDragBarRect;
    CRect  rcLeftRect;
    CRect  rcRightRect;

    m_pISplitterBar->GetParentRect(&rcDragBarRect);
    m_pObjLeft->GetParentRect(&rcLeftRect);
    m_pObjRight->GetParentRect(&rcRightRect);

    do 
    {
        if (IsVert())
        {
            // 计算拖拽条的宽度
            int  nDragbarSize = rcDragBarRect.Width();
            int  nFullLeft = rcLeftRect.left;
            int  nFullRight = rcRightRect.right;
            int  nFullTop = rcDragBarRect.top;
            int  nFullBottom = rcDragBarRect.bottom;

            long nWidth = nFullRight - nFullLeft;
            long nHeight = rcDragBarRect.Height();

            RECT rcAvai1 = {0,0,nWidth, nHeight};
            RECT rcAvai2 = {0,0,nWidth-nDragbarSize, nHeight};

            // 计算可拖拽范围
            if (m_nLeftMin > 0)
            {
                rcAvai1.left = m_nLeftMin;
            }
            if (m_nLeftMax > 0)
            {
                rcAvai1.right = m_nLeftMax;
            }

            if (m_nRightMin > 0)
            {
                rcAvai2.right = nWidth - m_nRightMin - nDragbarSize;
            }
            if (m_nRightMax > 0)
            {
                rcAvai2.left = nWidth - m_nRightMax - nDragbarSize;
            }

            if (::IntersectRect(&m_rcAvailable, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&m_rcAvailable, nFullLeft, nFullTop); // 转成窗口坐标
            }
            else
            {
                // 配置与当前窗口大小不匹配
                m_rcAvailable.SetRectEmpty();
                break;
            }
        }
        else
        {
            // 计算拖拽条的高度
            int  nDragbarSize = rcDragBarRect.Height();
            int  nFullLeft = rcDragBarRect.left;
            int  nFullRight = rcDragBarRect.right;
            int  nFullTop = rcLeftRect.top;
            int  nFullBottom = rcRightRect.bottom;

            long nHeight = nFullBottom - nFullTop;
            long nWidth = rcDragBarRect.Width();

            RECT rcAvai1 = {0,0,nWidth, nHeight};
            RECT rcAvai2 = {0,0,nWidth, nHeight-nDragbarSize};

            // 计算可拖拽范围
            if (m_nLeftMin > 0)
            {
                rcAvai1.top = m_nLeftMin;
            }
            if (m_nLeftMax > 0)
            {
                rcAvai1.bottom = m_nLeftMax;
            }

            if (m_nRightMin > 0)
            {
                rcAvai2.bottom = nHeight - m_nRightMin - nDragbarSize;
            }
            if (m_nRightMax > 0)
            {
                rcAvai2.top = nHeight - m_nRightMax - nDragbarSize;
            }

            if (::IntersectRect(&m_rcAvailable, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&m_rcAvailable, nFullLeft, nFullTop); // 转成窗口坐标
            }
            else
            {
                // 配置与当前窗口大小不匹配
                m_rcAvailable.SetRectEmpty();
                break;
            }
        }
    }while (0);
}

// 根据鼠标拖拽位置(窗口坐标)，计算最终拖拽条的位置
bool  SplitterBar::TestAvailableDragPos(POINT point, POINT* ptAvailable)  
{
    // 获取需要布局的子对象指针（只布局两个子对象）
    if (m_rcAvailable.IsRectEmpty())
        return false;

    *ptAvailable = point;
    if (IsVert())
    {
        if (point.x > m_rcAvailable.right)
        {
            ptAvailable->x = m_rcAvailable.right;
        }
        else if (point.x < m_rcAvailable.left)
        {
            ptAvailable->x = m_rcAvailable.left;
        }
    }
    else
    {
        if (point.y > m_rcAvailable.bottom)
        {
            ptAvailable->y = m_rcAvailable.bottom;
        }
        else if (point.y < m_rcAvailable.top)
        {
            ptAvailable->y = m_rcAvailable.top;
        }
    }

    return true;
}



void  SplitterBar::UpdateLeftRightCtrlPos(int nPos)
{
    CRect  rc;
    m_pISplitterBar->GetParentRect(&rc);

    LONG nPrevLeft = rc.left;
    if (nPrevLeft == nPos)
        return;

    LONG  nWidth = rc.Width();

    long  left = nPos;
    long  right = left + nWidth;

    rc.left = left;
    rc.right = right;
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOSENDCHANGING);

    if (m_pObjLeft)
    {
        CRect rcLeftCtrl;
        m_pObjLeft->GetParentRect(&rcLeftCtrl);
        rcLeftCtrl.right = left;
        m_pObjLeft->SetObjectPos(&rcLeftCtrl, SWP_NOREDRAW);
    }
    if (m_pObjRight)
    {
        CRect  rcRightCtrl;
        m_pObjRight->GetParentRect(&rcRightCtrl);
        rcRightCtrl.left = right;
        m_pObjRight->SetObjectPos(&rcRightCtrl, SWP_NOREDRAW);
    }

    m_pISplitterBar->GetParentObject()->UpdateObject(true);
}

// nPos为top或者left
void  SplitterBar::UpdateUpBottomCtrlPos(int nPos)
{
    CRect  rc;
    m_pISplitterBar->GetParentRect(&rc);

    LONG  nPrevTop = rc.top;
    if (nPrevTop == nPos)
        return;

    LONG  nHeight = rc.Height();

    long  top = nPos;
    long  bottom = top + nHeight;
    rc.top = top;
    rc.bottom = bottom;
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW|SWP_NOSENDCHANGING);
    //m_pISplitterBar->UpdateObject(false);

    if (m_pObjTop)
    {
        CRect  rcUpCtrl;
        m_pObjTop->GetParentRect(&rcUpCtrl);
        rcUpCtrl.bottom = top;
        m_pObjTop->SetObjectPos(&rcUpCtrl, SWP_NOREDRAW);
    //    m_pObjLeft->UpdateObject(false);
    }
    if (m_pObjBottom)
    {
        CRect  rcBottomCtrl;
        m_pObjBottom->GetParentRect(&rcBottomCtrl);
        rcBottomCtrl.top = bottom;
        m_pObjBottom->SetObjectPos(&rcBottomCtrl);
    //    m_pObjRight->UpdateObject(false);
    }

    m_pISplitterBar->GetParentObject()->UpdateObject(true);
   // m_pISplitterBar->GetWindowObject()->CommitDoubleBuffet2Window(NULL, NULL, 0);
}

// 在窗口大小改变时，要重新调整两侧控件大小，保证二者都可见
void  SplitterBar::OnParentSizeChanged()
{
    if (NULL == m_pObjLeft || NULL == m_pObjRight)
        return;
    
    CRect rcDragBar;
    m_pISplitterBar->GetParentRect(&rcDragBar);

    POINT ptLast = {-1, -1};

    if (IsHorz())
    {
        CRect rcTop, rcBottom;
        m_pObjTop->GetParentRect(&rcTop);
        m_pObjBottom->GetParentRect(&rcBottom);

        bool bHandled = false;
        if (m_eAlign == SPLITTERBAR_ALIGN_BOTTOM)
        {
            do 
            {
                if (m_nBottomMax != NDEF && rcBottom.Height() > m_nBottomMax)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMax - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nBottomMin != NDEF && rcBottom.Height() < m_nBottomMin)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMin - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nTopMin != NDEF && rcTop.Height() < m_nTopMin)
                {
                    ptLast.y = rcTop.top + m_nTopMin;
                    bHandled = true;
                    break;
                }

                if (m_nTopMax != NDEF && rcTop.Height() > m_nTopMax)
                {
                    ptLast.y = rcTop.top + m_nTopMax;
                    bHandled = true;
                    break;
                }
            }while(0);
        }
        else if (m_eAlign == SPLITTERBAR_ALIGN_TOP)
        {
            do 
            {
                if (m_nTopMin != NDEF && rcTop.Height() < m_nTopMin)
                {
                    ptLast.y = rcTop.top + m_nTopMin;
                    bHandled = true;
                    break;
                }

                if (m_nTopMax != NDEF && rcTop.Height() > m_nTopMax)
                {
                    ptLast.y = rcTop.top + m_nTopMax;
                    bHandled = true;
                    break;
                }

                if (m_nBottomMax != NDEF && rcBottom.Height() > m_nBottomMax)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMax - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

                if (m_nBottomMin != NDEF && rcBottom.Height() < m_nBottomMin)
                {
                    ptLast.y = rcBottom.bottom - m_nBottomMin - rcDragBar.Height();
                    bHandled = true;
                    break;
                }

            }while(0);
        }

        if (bHandled)
        {
            UpdateUpBottomCtrlPos(ptLast.y);
        }
    }
    else
    {
        CRect rcLeft, rcRight;
        m_pObjLeft->GetParentRect(&rcLeft);
        m_pObjRight->GetParentRect(&rcRight);

         bool bHandled = false;
        if (SPLITTERBAR_ALIGN_RIGHT == m_eAlign)
        {
            do
            {
                if (m_nRightMax != NDEF && rcRight.Width() > m_nRightMax)
                {
                    ptLast.x = rcRight.right - m_nRightMax - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nRightMin != NDEF && rcRight.Width() < m_nRightMin)
                {
                    ptLast.x = rcRight.right - m_nRightMin - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nLeftMin != NDEF && rcLeft.Width() < m_nLeftMin)
                {
                    ptLast.x = rcLeft.left + m_nLeftMin;
                    bHandled = true;
                    break;
                }

                if (m_nLeftMax != NDEF && rcLeft.Width() > m_nLeftMax)
                {
                    ptLast.x = rcLeft.left + m_nLeftMax;
                    bHandled = true;
                    break;
                }
            }while(0);
        }
        else
        {
            do
            {
                if (m_nLeftMin != NDEF && rcLeft.Width() < m_nLeftMin)
                {
                    ptLast.x = rcLeft.left + m_nLeftMin;
                    bHandled = true;
                    break;
                }

                if (m_nLeftMax != NDEF && rcLeft.Width() > m_nLeftMax)
                {
                    ptLast.x = rcLeft.left + m_nLeftMax;
                    bHandled = true;
                    break;
                }

                if (m_nRightMax != NDEF && rcRight.Width() > m_nRightMax)
                {
                    ptLast.x = rcRight.right - m_nRightMax - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

                if (m_nRightMin != NDEF && rcRight.Width() < m_nRightMin)
                {
                    ptLast.x = rcRight.right - m_nRightMin - rcDragBar.Width();
                    bHandled = true;
                    break;
                }

            }while(0);
        }
        if (bHandled)
        {
            UpdateLeftRightCtrlPos(ptLast.x);
        }
    }
}

}