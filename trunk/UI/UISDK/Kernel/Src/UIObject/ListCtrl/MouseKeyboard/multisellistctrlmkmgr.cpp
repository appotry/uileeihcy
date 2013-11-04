#include "stdafx.h"
#include "multisellistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

MultiSelListCtrlMKMgr::MultiSelListCtrlMKMgr()
{
    m_pSelectRegionRender = NULL;
}
MultiSelListCtrlMKMgr::~MultiSelListCtrlMKMgr()
{
    SAFE_DELETE(m_pSelectRegionRender);
}

BOOL  MultiSelListCtrlMKMgr::DoProcessMessage(UIMSG* pMsg)
{
    bool bInterestMsg = true;
    switch (pMsg->message)
    {
    case WM_LBUTTONDOWN:
        OnLButtonDown(pMsg);
        break;

    case WM_MOUSEMOVE:
        OnMouseMove(pMsg);
        break;

    case WM_KEYDOWN:
        {
            BOOL bHandled = __super::DoProcessMessage(pMsg);
            if (bHandled)
                return TRUE;

            OnKeyDown(pMsg, &bInterestMsg);
        }
        break;

    case WM_MOUSELEAVE:
        OnMouseLeave(pMsg);
        break;;

    case WM_LBUTTONUP:
        OnLButtonUp(pMsg);
        break;

    default:
        return __super::DoProcessMessage(pMsg);
        break;
    }

    if (bInterestMsg)
    {
        m_pListCtrlBase->Refresh();
    }
    return FALSE;
}

void  MultiSelListCtrlMKMgr::OnMouseMove(UIMSG* pMsg)
{
    if (pMsg->wParam & MK_LBUTTON)
    {
        if (NULL == m_pItemPress && NULL == m_pObjPress)
        {
            // 画框选择
            OnMouseMove_DragSelectRegion(pMsg);
            return;
        }
    }

    __super::OnMouseMove(pMsg);
}

void  MultiSelListCtrlMKMgr::OnLButtonUp(UIMSG* pMsg)
{
    if (m_pSelectRegionRender)
    {
        m_pSelectRegionRender->LButtonUp();
        SAFE_DELETE(m_pSelectRegionRender);
    }

    __super::OnLButtonUp(pMsg);
}
void  MultiSelListCtrlMKMgr::OnMouseLeave(UIMSG* pMsg)
{
    if (m_pSelectRegionRender)
    {
        m_pSelectRegionRender->LButtonUp();
        SAFE_DELETE(m_pSelectRegionRender);
    }

    __super::OnMouseLeave(pMsg);
}

// 仅处理选项
void  MultiSelListCtrlMKMgr::OnLButtonDown(UIMSG* pMsg)
{
    __super::OnLButtonDown(pMsg);

    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);
    bool bShiftDown = Util::IsKeyDown(VK_SHIFT);

    if (NULL == m_pItemPress)
    {
        if (!bCtrlDown && !bShiftDown)
        {
            m_pListCtrlBase->ClearSelectItem(true);
        }
        return;
    }

    if (bCtrlDown && bShiftDown)
    {
        m_pListCtrlBase->AddSelectItem(m_pItemPress);
    }
    else if (bCtrlDown)
    {
        if (m_pItemPress->IsSelected())
            m_pListCtrlBase->RemoveSelectItem(m_pItemPress, true);
        else
            m_pListCtrlBase->AddSelectItem(m_pItemPress);
    }
    else if (bShiftDown)
    {

    }
    else
    {
        m_pListCtrlBase->SelectItem(m_pItemPress, false);
    }
}

void  MultiSelListCtrlMKMgr::OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg)
{
    if (pbInterestMsg)
        *pbInterestMsg = true;

    switch (pMsg->wParam)
    {
    case VK_DOWN:
        OnKeyDown_down(pMsg);
        return;

    case VK_UP:
        OnKeyDown_up(pMsg);
        return;

    default:
        if (pbInterestMsg)
            *pbInterestMsg = false;
        break;
    }
}

void  MultiSelListCtrlMKMgr::OnKeyDown_up(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstVisbleItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
        if (pFirstVisbleItem)
        {
            this->SetFocusItem(pFirstVisbleItem);
            this->m_pListCtrlBase->SelectItem(pFirstVisbleItem, false);
        }
        return;
    }

    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);
    bool bShiftDown = Util::IsKeyDown(VK_SHIFT);

    if (bCtrlDown && bShiftDown)
    {
    }
    else if (bCtrlDown)
    {
        // 仅改变focusitem，不影响selection
        ListItemBase* pPrevItem = m_pItemFocus->GetPrevVisibleItem();
        if (pPrevItem)
            this->SetFocusItem(pPrevItem);
    }
    else if (bShiftDown)
    {
    }
    else
    {
        ListItemBase* pPrevItem = m_pItemFocus->GetPrevVisibleItem();
        if (pPrevItem)
        {
            SetFocusItem(pPrevItem);
            this->m_pListCtrlBase->SelectItem(pPrevItem, false);
        }
    }
}

void  MultiSelListCtrlMKMgr::OnKeyDown_down(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstVisbleItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
        if (pFirstVisbleItem)
        {
            this->SetFocusItem(pFirstVisbleItem);
            this->m_pListCtrlBase->SelectItem(pFirstVisbleItem, false);
        }
        return;
    }

    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);
    bool bShiftDown = Util::IsKeyDown(VK_SHIFT);

    if (bCtrlDown && bShiftDown)
    {
    }
    else if (bCtrlDown)
    {
        ListItemBase* pNextItem = m_pItemFocus->GetNextVisibleItem();
        if (pNextItem)
            this->SetFocusItem(pNextItem);
    }
    else if (bShiftDown)
    {
    }
    else
    {
        ListItemBase* pNextItem = m_pItemFocus->GetNextVisibleItem();
        if (pNextItem)
        {
            SetFocusItem(pNextItem);
            this->m_pListCtrlBase->SelectItem(pNextItem, false);
        }
    }
}



void  MultiSelListCtrlMKMgr::OnMouseMove_DragSelectRegion(UIMSG* pMsg)
{
    POINT point = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};
    if (NULL == m_pSelectRegionRender)
    {
        m_pSelectRegionRender = new DotLine_SelectRegionRender();
        if (false == m_pSelectRegionRender->Init(m_pListCtrlBase, point))
        {
            SAFE_DELETE(m_pSelectRegionRender);
            return;
        }
    }
    else
    {
        m_pSelectRegionRender->MouseMove(point);
    }

    CRect  rcSelectRegion;
    m_pSelectRegionRender->GetSelectRegion(&rcSelectRegion);
    rcSelectRegion.NormalizeRect();

    CRect  rcTest;
    ListItemBase* pItem = m_pListCtrlBase->GetFirstDrawItem();
    ListItemBase* pLastVisibleItem = m_pListCtrlBase->GetLastDrawItem();
    while (pItem)
    {
        CRect rcItem;
        pItem->GetParentRect(&rcItem);

        bool bOldSelect = m_pListCtrlBase->IsSelected(pItem);
        bool bNowSelect = rcTest.IntersectRect(&rcSelectRegion, &rcItem) ? true:false;

        if (bOldSelect != bNowSelect)
        {
            if (bNowSelect)
            {
                m_pListCtrlBase->AddSelectItem(pItem);
            }
            else
            {
                m_pListCtrlBase->RemoveSelectItem(pItem, true);
            }
            m_pListCtrlBase->InvalidateItem(pItem);
        }

        if (pItem == pLastVisibleItem)
            break;

        pItem = pItem->GetNextVisibleItem();
    }
}

//////////////////////////////////////////////////////////////////////////


DotLine_SelectRegionRender::DotLine_SelectRegionRender()
{
    m_hWndDC  = NULL;
    m_hWnd    = NULL;
    m_hClipRgn = NULL;
    m_rcClip.SetRectEmpty();
    m_rcView.SetRectEmpty();
}
DotLine_SelectRegionRender::~DotLine_SelectRegionRender()
{
    if (m_hWndDC)
    {
        ReleaseDC(m_hWnd, m_hWndDC);
    }
    SAFE_DELETE_GDIOBJECT(m_hClipRgn);
}

bool  DotLine_SelectRegionRender::Init(Object* pObj, POINT pt)
{
    __super::Init(pObj, pt);
    m_hWnd = m_pObject->GetHWND();
#ifdef _DEBUG
    long lStyleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
    if (lStyleEx & WS_EX_LAYERED)
    {
        return false;
    }
#endif

    // 计算裁剪区域
    CRect rcVisible;
    CRect rcClient;
    if (false == m_pObject->GetObjectVisibleRect(&rcVisible, true))   // 该对象在窗口上不可见，不绘制
        return false;

    m_hWndDC = GetDC(m_hWnd);

    m_pObject->GetClientRectInWindow(&rcClient);
    m_rcClip.IntersectRect(&rcClient, &rcVisible);
    m_hClipRgn = CreateRectRgnIndirect(&m_rcClip);
    ::SelectClipRgn(m_hWndDC, m_hClipRgn);

    // 设置DC偏移
    int xOffset = 0, yOffset = 0;
    m_pObject->GetScrollOffset(&xOffset, &yOffset);
    ::OffsetViewportOrgEx(m_hWndDC, rcClient.left-xOffset, rcClient.top-yOffset, NULL);

    // 绘制范围
    int xRange = 0, yRange = 0;
    m_pObject->GetScrollRange(&xRange, &yRange);
    m_rcView.right = max(rcClient.Width(), xRange);
    m_rcView.bottom = max(rcClient.Height(), yRange);

    // 初始位置
    POINT ptClient = {0,0};
    m_pObject->WindowPoint2ObjectClientPoint_CalcScroll(&pt, &ptClient);
    m_rcSelectRegion.SetRect(ptClient.x, ptClient.y, ptClient.x, ptClient.y);

    CRect rcTempNew = NormalizeRect(&m_rcSelectRegion);
    Draw(&rcTempNew);
    return true;
}

void  DotLine_SelectRegionRender::MouseMove(POINT point)
{
    POINT ptClient = {0,0};
    m_pObject->WindowPoint2ObjectClientPoint_CalcScroll(&point, &ptClient);

    if (ptClient.x < m_rcView.left)
        ptClient.x = m_rcView.left;
    else if (ptClient.x > m_rcView.right)
        ptClient.x = m_rcView.right;

    if (ptClient.y < m_rcView.top)
        ptClient.y = m_rcView.top;
    else if (ptClient.y > m_rcView.bottom)
        ptClient.y = m_rcView.bottom;

    CRect rcPrevMouseMove(0,0,0,0);
    ::CopyRect(&rcPrevMouseMove, &m_rcSelectRegion);

    m_rcSelectRegion.right = ptClient.x;
    m_rcSelectRegion.bottom = ptClient.y;

    CRect rcTempOld = NormalizeRect(&rcPrevMouseMove);
    CRect rcTempNew = NormalizeRect(&m_rcSelectRegion);

    Draw(&rcTempOld);
    Draw(&rcTempNew);
}
void  DotLine_SelectRegionRender::LButtonUp()
{
    RECT rcTemp = NormalizeRect(&m_rcSelectRegion);
    Draw(&rcTemp);

    ::SelectClipRgn(m_hWndDC, NULL);
}

void  DotLine_SelectRegionRender::Draw(RECT* prc)
{
    WindowBase* pWindow = m_pObject->GetWindowObject();
    HDC hDC = pWindow->GetRenderChainMemDC();
    ::DrawFocusRect(hDC, prc);
    pWindow->CommitDoubleBuffet2Window(NULL, prc);
}
// 
// void  DotLine_SelectRegionRender::PrePaint()
// {
//     WindowBase* pWindow = m_pObject->GetWindowObject();
//     if (pWindow)
//     {
//         pWindow->AddCommitWindowBufferListener(static_cast<ICommitWindowBufferListener*>(this));
//     }
// 
//     RECT rcTemp = NormalizeRect(&m_rcSelectRegion);
//     ::DrawFocusRect(m_hWndDC, &rcTemp);
// }
// 
// void  DotLine_SelectRegionRender::PreCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount)
// {
// 
// }
// void  DotLine_SelectRegionRender::PostCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount)
// {
//     RECT rcTemp = NormalizeRect(&m_rcSelectRegion);
//     ::DrawFocusRect(m_hWndDC, &rcTemp);
// }

}