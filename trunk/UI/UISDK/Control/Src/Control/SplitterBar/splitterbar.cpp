#include "stdafx.h"
#include "splitterbar.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{

SplitterBar::SplitterBar()
{
    m_pISplitterBar = NULL;
    m_pObjLeftUp = NULL;
    m_pObjRightBottom = NULL;

    m_ptLButtonDown.x = -1;
    m_ptLButtonDown.y = -1;
    m_ptLastMouseMove.x = -1;
    m_ptLastMouseMove.y = -1;
    m_nDeviation = 0;
    m_bButtonDown = false;

    m_nLeftUpMin = NDEF;
    m_nLeftUpMax = NDEF;
    m_nRightBottomMin = NDEF;
    m_nRightBottomMax = NDEF;
}
SplitterBar::~SplitterBar()
{

}

void  SplitterBar::ResetAttribute()
{
    DO_PARENT_PROCESS(ISplitterBar, IPanel);

    m_pObjLeftUp = NULL;
    m_pObjRightBottom = NULL;

    m_eDirection = SPLITTERBAR_DIRECTION_H;
    m_eAlign = SPLITTERBAR_ALIGN_BOTTOM;

    m_nLeftUpMin = NDEF;
    m_nLeftUpMax = NDEF;
    m_nRightBottomMin = NDEF;
    m_nRightBottomMax = NDEF;

    m_pISplitterBar->ModifyStyle(0, OBJECT_STYLE_REJEST_MOUSE_MSG_SELF, 0);
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
   
    SPLITTERBAR_ALIGN  eAlign = SPLITTERBAR_ALIGN_LEFT;
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
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nLeftUpMin);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nRightBottomMin);

        pMapAttr->GetAttr_int(XML_SPLITTERBAR_TOP_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nLeftUpMax);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nRightBottomMax);
    }
    else
    {
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nLeftUpMin);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MINSIZE, true, &m_nRightBottomMin);

        pMapAttr->GetAttr_int(XML_SPLITTERBAR_LEFT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nLeftUpMax);
        pMapAttr->GetAttr_int(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_MAXSIZE, true, &m_nRightBottomMax);
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
            m_pObjLeftUp = pParent->FindChildObject(szText);
        }

        szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_BOTTOM_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjRightBottom = pParent->FindChildObject(szText);
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
            m_pObjLeftUp = pParent->FindChildObject(szText);
        }

        szText = pMapAttrib->GetAttr(XML_SPLITTERBAR_RIGHT_PREFIX XML_SPLITTERBAR_OBJ_ID, true);
        if (szText)
        {
            m_pObjRightBottom = pParent->FindChildObject(szText);
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

bool  SplitterBar::IsVert()
{
    return m_eDirection == SPLITTERBAR_DIRECTION_V ? true:false;
}
bool  SplitterBar::IsHorz()
{
    return m_eDirection == SPLITTERBAR_DIRECTION_H ? true:false;
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


// 根据鼠标拖拽位置(窗口坐标)，计算最终拖拽条的位置
bool  SplitterBar::TestAvailableDragPos(POINT point, POINT* ptAvailable)  
{
    // 获取需要布局的子对象指针（只布局两个子对象）
    if (NULL == m_pObjLeftUp || NULL == m_pObjRightBottom)
        return false;

    CRect  rcDragBarRect;
    CRect  rcLeftRect;
    CRect  rcRightRect;

    m_pISplitterBar->GetParentRect(&rcDragBarRect);
    m_pObjLeftUp->GetParentRect(&rcLeftRect);
    m_pObjRightBottom->GetParentRect(&rcRightRect);

    bool  bRet = false;
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
            if (m_nLeftUpMin > 0)
            {
                rcAvai1.left = m_nLeftUpMin;
            }
            if (m_nLeftUpMax > 0)
            {
                rcAvai1.right = m_nLeftUpMax;
            }

            if (m_nRightBottomMin > 0)
            {
                rcAvai2.right = nWidth - m_nRightBottomMin - nDragbarSize;
            }
            if (m_nRightBottomMax > 0)
            {
                rcAvai2.left = nWidth - m_nRightBottomMax - nDragbarSize;
            }

            RECT rcIntersect;
            if (::IntersectRect(&rcIntersect, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&rcIntersect, nFullLeft, nFullTop); // 转成窗口坐标

                *ptAvailable = point;
                if (point.x > rcIntersect.right)
                {
                    ptAvailable->x = rcIntersect.right;
                }
                else if (point.x < rcIntersect.left)
                {
                    ptAvailable->x = rcIntersect.left;
                }
            }
            else
            {
                // 配置与当前窗口大小不匹配
                bRet = false;
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
            if (m_nLeftUpMin > 0)
            {
                rcAvai1.top = m_nLeftUpMin;
            }
            if (m_nLeftUpMax > 0)
            {
                rcAvai1.bottom = m_nLeftUpMax;
            }

            if (m_nRightBottomMin > 0)
            {
                rcAvai2.bottom = nHeight - m_nRightBottomMin - nDragbarSize;
            }
            if (m_nRightBottomMax > 0)
            {
                rcAvai2.top = nHeight - m_nRightBottomMax - nDragbarSize;
            }

            RECT rcIntersect;
            if (::IntersectRect(&rcIntersect, &rcAvai1, &rcAvai2))
            {
                ::OffsetRect(&rcIntersect, nFullLeft, nFullTop); // 转成窗口坐标

                *ptAvailable = point;
                if (point.y > rcIntersect.bottom)
                {
                    ptAvailable->y = rcIntersect.bottom;
                }
                else if (point.y < rcIntersect.top)
                {
                    ptAvailable->y = rcIntersect.top;
                }
            }
            else
            {
                // 配置与当前窗口大小不匹配
                bRet = false;
                break;
            }
        }

        bRet = true;
    }while (0);


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
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW);

    if (m_pObjLeftUp)
    {
        CRect rcLeftCtrl;
        m_pObjLeftUp->GetParentRect(&rcLeftCtrl);
        rcLeftCtrl.right = left;
        m_pObjLeftUp->SetObjectPos(&rcLeftCtrl, SWP_NOREDRAW);
    }
    if (m_pObjRightBottom)
    {
        CRect  rcRightCtrl;
        m_pObjRightBottom->GetParentRect(&rcRightCtrl);
        rcRightCtrl.left = right;
        m_pObjRightBottom->SetObjectPos(&rcRightCtrl, SWP_NOREDRAW);
    }

//     if (m_pObjLeftUp)
//         m_pObjLeftUp->DirectDraw(VARIANT_FALSE);
//     this->DirectDraw(VARIANT_FALSE);
//     if (m_pObjRightBottom)
//         m_pObjRightBottom->DirectDraw(VARIANT_FALSE);
}

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
    m_pISplitterBar->SetObjectPos(&rc, SWP_NOREDRAW);
    m_pISplitterBar->UpdateObject(false);

    if (m_pObjLeftUp)
    {
        CRect  rcUpCtrl;
        m_pObjLeftUp->GetParentRect(&rcUpCtrl);
        rcUpCtrl.bottom = top;
        m_pObjLeftUp->SetObjectPos(&rcUpCtrl, SWP_NOREDRAW);
        m_pObjLeftUp->UpdateObject(false);
    }
    if (m_pObjRightBottom)
    {
        CRect  rcBottomCtrl;
        m_pObjRightBottom->GetParentRect(&rcBottomCtrl);
        rcBottomCtrl.top = bottom;
        m_pObjRightBottom->SetObjectPos(&rcBottomCtrl);
        m_pObjRightBottom->UpdateObject(false);
    }

    m_pISplitterBar->GetParentObject()->UpdateObject(true);
   // m_pISplitterBar->GetWindowObject()->CommitDoubleBuffet2Window(NULL, NULL, 0);
}

}