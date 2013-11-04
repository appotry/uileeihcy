#include "stdafx.h"
#include "scrollbarmanager.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"

namespace UI
{

ScrollBarManager::ScrollBarManager(IScrollBarManager* p)
{
    m_pIScrollBarManager = p;
    m_pBindObject = NULL;
    m_pVScrollBar = NULL;
    m_pHScrollBar = NULL;
  
    m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
    m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;

    m_inertiaScroll.SetCallback(static_cast<IInertiaScrollCallback*>(this));
}

ScrollBarManager::~ScrollBarManager()
{
}

IScrollBarManager*  ScrollBarManager::GetIScrollBarMgr()
{
    return m_pIScrollBarManager;
}
void  ScrollBarManager::SetBindObject(IObject* pBindObj) 
{ 
    m_pBindObject = pBindObj; 
    if (m_pBindObject)
    {
        m_inertiaScroll.SetUIApplication(m_pBindObject->GetUIApplication());
    }
}
IObject*  ScrollBarManager::GetBindObject()
{ 
    return m_pBindObject; 
}

void  ScrollBarManager::ResetAttribute()
{
    m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
    m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;

    if (m_pHScrollBar)
    {
        UISendMessage(m_pHScrollBar, UI_WM_RESETATTRIBUTE);
    }
    if (m_pVScrollBar)
    {
        UISendMessage(m_pVScrollBar, UI_WM_RESETATTRIBUTE);
    }
}
void  ScrollBarManager::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    const TCHAR* szText = NULL;
    
    szText = pMapAttrib->GetAttr(XML_HSCROLLBAR, true);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_SCROLLBAR_NONE)) 
        {
            m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_NONE;
        }
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_AUTO)) 
        {
            m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
        }	
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_ALWAYSSHOW)) 
        {
            m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_SHOW_ALWAYS;
        }
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_ALWAYSHIDE)) 
        {
            m_ehScrollbarVisibleType = SCROLLBAR_VISIBLE_HIDE_ALWAYS;
        }
    }

    szText = pMapAttrib->GetAttr(XML_VSCROLLBAR, true);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_SCROLLBAR_NONE)) 
        {
            m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_NONE;
        }
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_AUTO)) 
        {
            m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_AUTO;
        }	
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_ALWAYSSHOW)) 
        {
            m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_SHOW_ALWAYS;
        }
        else if (0 == _tcscmp(szText, XML_SCROLLBAR_ALWAYSHIDE)) 
        {
            m_evScrollbarVisibleType = SCROLLBAR_VISIBLE_HIDE_ALWAYS;
        }
    }

    if (m_pBindObject)
        m_pBindObject->ModifyStyle(0, OBJECT_STYLE_HSCROLL|OBJECT_STYLE_HSCROLL, false);

    // 根据类型创建实例
    if (SCROLLBAR_VISIBLE_NONE == m_ehScrollbarVisibleType)
    {
        SAFE_DELETE_Ixxx(m_pHScrollBar);
    }
    else if (SCROLLBAR_VISIBLE_HIDE_ALWAYS == m_evScrollbarVisibleType)
    {
        SAFE_DELETE_Ixxx(m_pHScrollBar);
        if (m_pBindObject)
            m_pBindObject->ModifyStyle(OBJECT_STYLE_HSCROLL, 0, 0);
    }
    else
    {
        // 创建滚动条
        szText = pMapAttrib->GetAttr(XML_HSCROLLBAR_PRIFIX XML_SCROLLBAR_RENDER_TYPE, true);
        if (szText)
        {
            IObject*  pObj = NULL;
            m_pBindObject->GetUIApplication()->CreateInstanceByName(szText, &pObj);
            if (pObj)
            {
                m_pHScrollBar = static_cast<IMessage*>(pObj);
                m_pBindObject->AddNcChild(pObj);
                pObj->SetID(XML_HSCROLLBAR_PRIFIX);
                UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_SETISCROLLBARMGR, (WPARAM)m_pIScrollBarManager);
                pObj->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            }
        }

        if (m_pBindObject)
            m_pBindObject->ModifyStyle(OBJECT_STYLE_HSCROLL, 0, 0);
    }

    if (SCROLLBAR_VISIBLE_NONE == m_evScrollbarVisibleType)
    {
        SAFE_DELETE_Ixxx(m_pVScrollBar);
    }
    else if (SCROLLBAR_VISIBLE_HIDE_ALWAYS == m_evScrollbarVisibleType)
    {
        SAFE_DELETE_Ixxx(m_pVScrollBar);
        if (m_pBindObject)
            m_pBindObject->ModifyStyle(OBJECT_STYLE_VSCROLL, 0, 0);
    }
    else
    {
        // 创建滚动条
        szText = pMapAttrib->GetAttr(XML_VSCROLLBAR_PRIFIX XML_SCROLLBAR_RENDER_TYPE, true);
        if (szText)
        {
            IObject*  pObj = NULL;
            m_pBindObject->GetUIApplication()->CreateInstanceByName(szText, &pObj);
            if (pObj)
            {
                m_pVScrollBar = static_cast<IMessage*>(pObj);
                m_pBindObject->AddNcChild(pObj);
                pObj->SetID(XML_VSCROLLBAR_PRIFIX);
                UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_SETISCROLLBARMGR, (WPARAM)m_pIScrollBarManager);
                pObj->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            }
        }

        if (m_pBindObject)
            m_pBindObject->ModifyStyle(OBJECT_STYLE_VSCROLL, 0, 0);
    }
}


// Note that the WM_HSCROLL message carries only 16 bits of scroll box position data. 
// Thus, applications that rely solely on WM_HSCROLL (and WM_VSCROLL) for scroll position 
// data have a practical maximum position value of 65,535. 
void  ScrollBarManager::FireScrollMessage(SCROLLBAR_DIRECTION_TYPE eType, int nSBCode, int nTrackPos)
{
    // 转化为16位的无符号数值，用于WM_SCRLL的hiword
    if (nTrackPos < 0)
    {
        nTrackPos = 0;
    }

    int nCompatibleTrackPos = nTrackPos;
    if (nCompatibleTrackPos > 65535)
    {
        //UI_LOG_WARN(_T("%s nTrackPos > 65535: %d"), FUNC_NAME, nTrackPos);
        nCompatibleTrackPos = 65535;
    }

    UIMSG msg;
    msg.message = eType==HSCROLLBAR ? WM_HSCROLL:WM_VSCROLL;
    msg.nCode = 0;
    msg.wParam = MAKEWPARAM(nSBCode, nCompatibleTrackPos);  // 传递给类似于windowless richedit时，得保持兼容
    msg.lParam = nTrackPos;
    msg.pMsgFrom = eType==HSCROLLBAR ? m_pHScrollBar:m_pVScrollBar;
    msg.pMsgTo = m_pBindObject;
    m_pIScrollBarManager->DoNotify(&msg);
}


void  ScrollBarManager::OnGetScrollOffset(int* pxOffset, int* pyOffset)
{
    GetScrollPos(pxOffset, pyOffset);
}
void  ScrollBarManager::OnGetScrollRange(int* pxRange, int* pyRange)
{
    GetScrollRange(pxRange, pyRange);
}

void  ScrollBarManager::OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    if (NULL == m_pVScrollBar || m_pVScrollBar != pMsgFrom)
        return;

    nSBCode = LOWORD(nSBCode);
    int nOldPos = m_vScrollInfo.nPos;
    switch (nSBCode)
    {
    case SB_LINEUP:
        SetVScrollPos(m_vScrollInfo.nPos - m_vScrollInfo.nButtonLine);
        break;

    case SB_LINEDOWN:
        SetVScrollPos(m_vScrollInfo.nPos + m_vScrollInfo.nButtonLine);
        break;

    case SB_PAGEUP:
        SetVScrollPos(m_vScrollInfo.nPos - m_vScrollInfo.nPage);
        break;

    case SB_PAGEDOWN:
        SetVScrollPos(m_vScrollInfo.nPos + m_vScrollInfo.nPage);
        break;

    case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
    case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
        SetVScrollPos(nPos);
        break;
    }

    if (nOldPos != m_vScrollInfo.nPos)
    {
        m_pBindObject->UpdateObject();
    }
}

void  ScrollBarManager::OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
{
    if (NULL == m_pHScrollBar || m_pHScrollBar != pMsgFrom)
        return;

    nSBCode = LOWORD(nSBCode);
    int nOldPos = m_hScrollInfo.nPos;
    switch (nSBCode)
    {
    case SB_LINEUP:
        SetHScrollPos(m_hScrollInfo.nPos - m_hScrollInfo.nButtonLine);
        break;

    case SB_LINEDOWN:
        SetHScrollPos(m_hScrollInfo.nPos + m_hScrollInfo.nButtonLine);
        break;

    case SB_PAGEUP:
        SetHScrollPos(m_hScrollInfo.nPos - m_hScrollInfo.nPage);
        break;

    case SB_PAGEDOWN:
        SetHScrollPos(m_hScrollInfo.nPos + m_hScrollInfo.nPage);
        break;

    case SB_THUMBTRACK:    // Drag scroll box to specified position. The current position is provided in nPos.
    case SB_THUMBPOSITION: // Scroll to the absolute position. The current position is provided in nPos
        SetHScrollPos(nPos);
        break;
    }

    if (nOldPos != m_hScrollInfo.nPos)
    {
        m_pBindObject->UpdateObject();
    }
}

// OnMouseWheel内部会判断滚动条显隐情况分别调用OnHMouseWheel或OnVMouseWheel
BOOL  ScrollBarManager::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
    // 判断本次要滚动的滚动条
    if (m_evScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE && 
        m_ehScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
        return TRUE;

    enum ScrollAction { None, HScroll, VScroll };
    UINT nAction = None;

    if (m_evScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
    {
        nAction = HScroll;
    }
    else if (m_ehScrollbarVisibleType == SCROLLBAR_VISIBLE_NONE)
    {
        nAction = VScroll;
    }
    else
    {
        bool bVScrollBarAvailable = m_vScrollInfo.nRange > m_vScrollInfo.nPage;
        bool bHScrollBarAvailable = m_hScrollInfo.nRange > m_hScrollInfo.nPage;

        if (bVScrollBarAvailable && bHScrollBarAvailable && Util::IsKeyDown(VK_SHIFT))
        {
            nAction = HScroll;
        }
        else if (bVScrollBarAvailable)
        {
            nAction = VScroll;
        }
        else if (bHScrollBarAvailable)
        {
            nAction = HScroll;
        }
    }

    if (nAction == VScroll)
    {
        OnVMouseWheel(zDelta, pt);
    }
    else if (nAction == HScroll)
    {
        OnHMouseWheel(zDelta, pt);
    }
    return 0;
}

// H暂时不支持平滑滚动
void  ScrollBarManager::OnHMouseWheel(short zDelta, POINT pt)
{
    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);  // 按下CTRL后，以3倍速进行滚动
    int nWheel = m_hScrollInfo.nWheelLine;
    if (bCtrlDown)
        nWheel *= 3;

    if (zDelta < 0)
        SetHScrollPos(m_hScrollInfo.nPos + nWheel);
    else
        SetHScrollPos(m_hScrollInfo.nPos - nWheel);

    // 重新发送一个MOUSEMOVE消息给obj，用于定位滚动后的hover对象
    MapWindowPoints(NULL, m_pBindObject->GetHWND(), &pt, 1);
    UISendMessage(m_pBindObject, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

    return;
}


void  ScrollBarManager::OnVMouseWheel(short zDelta, POINT pt)
{
    bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);  // 按下CTRL后，以3倍速进行滚动
    if (false == m_inertiaScroll.IsEnable() || bCtrlDown)
    {
        int nWheel = m_vScrollInfo.nWheelLine;
        if (bCtrlDown)
            nWheel *= 3;

        if (zDelta < 0)
            SetVScrollPos(m_vScrollInfo.nPos + nWheel);
        else
            SetVScrollPos(m_vScrollInfo.nPos - nWheel);

        // 重新发送一个MOUSEMOVE消息给obj，用于定位滚动后的hover对象
        MapWindowPoints(NULL, m_pBindObject->GetHWND(), &pt, 1);
        UISendMessage(m_pBindObject, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));

        return;
    }


    if (NULL == m_pBindObject)
        return;

    // 平滑滚动
    m_inertiaScroll.AddPower(zDelta);
}

// 在平滑滚动过程中点击了列表控件，立即停止滚动
LRESULT  ScrollBarManager::OnCaptureStopInertiaScroll(UINT uMsg, WPARAM, LPARAM)
{
    SetMsgHandled(FALSE);

    // 获取当前hover控件
    if (NULL == m_pBindObject)
        return 0;

    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (NULL == pWindow)
        return 0;

    Object* pObjHover = pWindow->GetHoverObject();
    if (NULL == pObjHover)
        return 0;

    if (m_pBindObject->GetImpl() == pObjHover ||
        m_pBindObject->GetImpl()->IsMyChild(pObjHover, true))
    {
        m_inertiaScroll.StopInertia(); 
    }
    return 0;
}

void ScrollBarManager::InertiaScroll_Start()
{
    if (NULL == m_pBindObject)
        return;

    IUIApplication* pUIApp = m_pBindObject->GetUIApplication();
    if (NULL == pUIApp)
        return;

    // 监听鼠标事件，用于鼠标点击了列表控件或者滚动条时立即停止平滑滚动
    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (pWindow)
    {
        pWindow->GetMouseMgr()->SetMouseCapture(m_pIScrollBarManager, UI_MSGMAP_ID_INERTIA_MOUSE_CAPTURE);
        pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIScrollBarManager, UI_MSGMAP_ID_INERTIA_KEYBOARD_CAPTURE);
    }
}
void ScrollBarManager::InertiaScroll_Stop()
{
    if (NULL == m_pBindObject)
        return;

    IUIApplication* pUIApp = m_pBindObject->GetUIApplication();
    if (NULL == pUIApp)
        return;

    WindowBase* pWindow = m_pBindObject->GetImpl()->GetWindowObject();
    if (NULL == pWindow)
        return;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIScrollBarManager);
    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIScrollBarManager);

    // 给窗口重新发送一个MOUSEMOVE消息，刷新当前hover对象(TODO: 不应该发送了，会导致滚动不连贯，但如何更新hoveritem)
    POINT pt = {0, 0};
    ::GetCursorPos(&pt);
    ::MapWindowPoints(NULL, pWindow->m_hWnd, &pt, 1);
    ::SendMessage(pWindow->m_hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
}

BOOL ScrollBarManager::InertiaScroll_Scroll(int nDeltaPos)
{
    BOOL bResult = TRUE;

    int nOldPos = m_vScrollInfo.nPos;
    SetVScrollPos(m_vScrollInfo.nPos + nDeltaPos);

    int nNewPos = m_vScrollInfo.nPos;
    if (nNewPos == nOldPos ||
        nNewPos == 0       ||
        nNewPos == (m_vScrollInfo.nRange-m_vScrollInfo.nPage))
    {
        bResult = FALSE;
    }

    UISendMessage(m_pBindObject, UI_WM_INERTIAVSCROLL, nOldPos, nNewPos);
    return bResult;
}

void  ScrollBarManager::SetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eDirType, SCROLLBAR_VISIBLE_TYPE eVisType)
{
    if(HSCROLLBAR==eDirType)
        m_ehScrollbarVisibleType = eVisType;
    else 
        m_evScrollbarVisibleType = eVisType; 
}
SCROLLBAR_VISIBLE_TYPE  ScrollBarManager::GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType)
{
    if(HSCROLLBAR==eType)
        return m_ehScrollbarVisibleType;
    else 
        return m_evScrollbarVisibleType; 
}

//////////////////////////////////////////////////////////////////////////
void  ScrollBarManager::SetVScrollLine(int nLine)
{
    m_vScrollInfo.nButtonLine = nLine;
}
void  ScrollBarManager::SetHScrollLine(int nLine)
{
    m_hScrollInfo.nButtonLine = nLine;
}
void  ScrollBarManager::SetVScrollWheel(int nWheel)
{
    m_vScrollInfo.nWheelLine = nWheel;
}
void  ScrollBarManager::SetHScrollWheel(int nWheel)
{
    m_hScrollInfo.nWheelLine = nWheel;
}

//////////////////////////////////////////////////////////////////////////
long  ScrollBarManager::NotifyHScrollInfoChanged()
{
    long lRet = 0;
    if (m_pHScrollBar)
    {
        lRet = UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)&m_hScrollInfo);
    }
    return lRet;
}

long  ScrollBarManager::NotifyVScrollInfoChanged()
{
    long lRet = 0;
    if (m_pVScrollBar)
    {
        lRet = UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_INFO_CHANGED, (WPARAM)&m_vScrollInfo);
    }
    return lRet;
}

enum SETSCROLLINFO_RET
{
    SETSCROLLINFO_RET_ERROR,  // -1, 出错
    SETSCROLLINFO_RET_OK,     // 不需要再处理
    SETSCROLLINFO_RET_NEED_UPDATA_VISIBLE  // 需要通知滚动条数据进行刷新
};
bool  ScrollBarManager::_SetScrollInfo1(SCROLLBAR_DIRECTION_TYPE eType, UISCROLLINFO* lpsi)
{
    long lRetSet = _SetScrollInfo2(eType, lpsi);

    if (SETSCROLLINFO_RET_ERROR == lRetSet)
    {
        return false;
    }
    else if (SETSCROLLINFO_RET_OK == lRetSet)
    {
        return true;
    }
    else if (SETSCROLLINFO_RET_NEED_UPDATA_VISIBLE == lRetSet)
    {
        long lNotifyRet = 0;
        if (eType == HSCROLLBAR && m_pHScrollBar)
        {
            lNotifyRet = NotifyHScrollInfoChanged();
        }
        else if (eType == VSCROLLBAR && m_pVScrollBar)
        {
            lNotifyRet = NotifyVScrollInfoChanged();
        }

        if (1 == lNotifyRet)  // 显隐发生变化，更新客户区域
        {
            UpdateBindObjectNonClientRect();
            UISendMessage(m_pBindObject, WM_SIZE, 0,
                MAKELPARAM(
                    m_pBindObject->GetWidth(), 
                    m_pBindObject->GetHeight())
                );
            return false;
        }
    }
    return true;
}

long  ScrollBarManager::_SetScrollInfo2(SCROLLBAR_DIRECTION_TYPE eType, UISCROLLINFO* lpsi)
{
    if (NULL == lpsi || 0 == lpsi->nMask)
        return SETSCROLLINFO_RET_ERROR;

    UISCROLLINFO* pInfo = NULL;
    if (eType == VSCROLLBAR)
        pInfo = &m_vScrollInfo;
    else
        pInfo = &m_hScrollInfo;

    bool bNeedCalcScrollBarVisible = false;
    if (lpsi->nMask & UISIF_RANGE)
    {
        int nRange = lpsi->nRange;
        if (nRange < 0)
            nRange = 0;

         pInfo->nRange = nRange;
         bNeedCalcScrollBarVisible = true;
    }

    if (lpsi->nMask & UISIF_PAGE)
    {
        int nPage = lpsi->nPage;
        if (nPage < 0)
            nPage = 0;

        // 	if (nPage > m_nRange)  // page就是有可能超出range大小.richedit??
        // 	{
        // 		nPage = m_nRange;
        // 	}

        pInfo->nPage = nPage;
        bNeedCalcScrollBarVisible = true;
    }

    // 更新pos位置，不要超出范围
    int nNewPos = pInfo->nPos;
    if (lpsi->nMask & UISIF_POS)
    {
        nNewPos = lpsi->nPos;
    }

    if (nNewPos > pInfo->nRange-pInfo->nPage)
        nNewPos = pInfo->nRange-pInfo->nPage;
    if (nNewPos < 0)
        nNewPos = 0;

    pInfo->nPos = nNewPos;


    if (lpsi->nMask & UISIF_BUTTONLINE)
        pInfo->nButtonLine = lpsi->nButtonLine;

    if (lpsi->nMask & UISIF_WHEELLINE)
        pInfo->nWheelLine = lpsi->nWheelLine;

    return bNeedCalcScrollBarVisible ? SETSCROLLINFO_RET_NEED_UPDATA_VISIBLE:SETSCROLLINFO_RET_OK;
}

void  ScrollBarManager::SetScrollRange(int nX, int nY)
{
    if (m_hScrollInfo.nRange == nX && m_vScrollInfo.nRange == nY)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nX;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;

    UISCROLLINFO info_v;
    info_v.nMask = UISIF_RANGE;
    info_v.nRange = nY;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_v))
        return;
}

void  ScrollBarManager::SetHScrollRange(int nX)
{
    if (m_hScrollInfo.nRange == nX)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nX;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;
}
void  ScrollBarManager::SetVScrollRange(int nY)
{
    if (m_vScrollInfo.nRange == nY)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE;
    info_h.nRange = nY;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_h))
        return;
}

void  ScrollBarManager::SetScrollPageAndRange(const SIZE* pPage, const SIZE* pRange)
{
    if (NULL == pPage || NULL == pRange)
        return;

    UISCROLLINFO info_h;
    info_h.nMask = UISIF_RANGE|UISIF_PAGE;
    info_h.nRange = pRange->cx;
    info_h.nPage = pPage->cx;
    if (false == _SetScrollInfo1(HSCROLLBAR, &info_h))
        return;

    UISCROLLINFO info_v;
    info_v.nMask = UISIF_RANGE|UISIF_PAGE;
    info_v.nRange = pRange->cy;
    info_v.nPage = pPage->cy;
    if (false == _SetScrollInfo1(VSCROLLBAR, &info_v))
        return;

    if (m_pHScrollBar)
        UISendMessage(m_pHScrollBar, UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED);
    if (m_pVScrollBar)
        UISendMessage(m_pVScrollBar, UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED);
}

void ScrollBarManager::UpdateBindObjectNonClientRect()
{
    m_pBindObject->UpdateObjectNonClientRegion();

    // 直接修改滚动条的page值
    CRect rcClient;
    this->m_pBindObject->GetClientRect(&rcClient);

    if (m_pHScrollBar)
    {
        m_hScrollInfo.nPage = rcClient.Width();
    }
    if (m_pVScrollBar)
    {
        m_vScrollInfo.nPage = rcClient.Height();
    }
}

void  ScrollBarManager::GetScrollInfo(SCROLLBAR_DIRECTION_TYPE eDirType, UISCROLLINFO* pInfo)
{
    if (pInfo && pInfo->nMask)
    {
        if (pInfo->nMask & UISIF_POS)
            pInfo->nPos = eDirType==HSCROLLBAR ? m_hScrollInfo.nPos : m_vScrollInfo.nPos;

        if (pInfo->nMask & UISIF_RANGE)
            pInfo->nRange = eDirType==HSCROLLBAR ? m_hScrollInfo.nRange : m_vScrollInfo.nRange;  

        if (pInfo->nMask & UISIF_PAGE)
            pInfo->nPage = eDirType==HSCROLLBAR ? m_hScrollInfo.nPage : m_vScrollInfo.nPage;  

        if (pInfo->nMask & UISIF_BUTTONLINE)
            pInfo->nButtonLine = eDirType==HSCROLLBAR ? m_hScrollInfo.nButtonLine : m_vScrollInfo.nButtonLine; 

        if (pInfo->nMask & UISIF_WHEELLINE)
            pInfo->nWheelLine = eDirType==HSCROLLBAR ? m_hScrollInfo.nWheelLine : m_vScrollInfo.nWheelLine; 
    }

}

//////////////////////////////////////////////////////////////////////////
void  ScrollBarManager::SetHScrollPos(int nPos)
{
    UISCROLLINFO info;
    info.nMask = UISIF_POS;
    info.nPos = nPos;
    _SetScrollInfo1(HSCROLLBAR, &info);
}
void  ScrollBarManager::SetVScrollPos(int nPos)
{
    UISCROLLINFO info;
    info.nMask = UISIF_POS;
    info.nPos = nPos;
    _SetScrollInfo1(VSCROLLBAR, &info);
}
void  ScrollBarManager::GetScrollPos(int* pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nPos;
    if (pY)
        *pY = m_vScrollInfo.nPos;
}
int   ScrollBarManager::GetScrollPos(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nPos;
    else
        return m_vScrollInfo.nPos;
}
int   ScrollBarManager::GetHScrollPos()
{
    return m_hScrollInfo.nPos;
}
int   ScrollBarManager::GetVScrollPos()
{
    return m_vScrollInfo.nPos;
}

void  ScrollBarManager::GetScrollPage(int* pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nPage;
    if (pY)
        *pY = m_vScrollInfo.nPage;
}
int   ScrollBarManager::GetScrollPage(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nPage;
    else
        return m_vScrollInfo.nPage;
}
int   ScrollBarManager::GetHScrollPage()
{
    return m_hScrollInfo.nPage;
}
int   ScrollBarManager::GetVScrollPage()
{
    return m_vScrollInfo.nPage;
}

void  ScrollBarManager::GetScrollRange(int *pX, int* pY)
{
    if (pX)
        *pX = m_hScrollInfo.nRange;
    if (pY)
        *pY = m_vScrollInfo.nRange;
}
int   ScrollBarManager::GetScrollRange(SCROLLBAR_DIRECTION_TYPE eDirType)
{
    if (HSCROLLBAR == eDirType)
        return m_hScrollInfo.nRange;
    else
        return m_vScrollInfo.nRange;
}
int   ScrollBarManager::GetHScrollRange()
{
    return m_hScrollInfo.nRange;
}
int   ScrollBarManager::GetVScrollRange()
{
    return m_vScrollInfo.nRange;
}
}