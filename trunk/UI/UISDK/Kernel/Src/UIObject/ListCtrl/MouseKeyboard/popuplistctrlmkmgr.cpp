#include "stdafx.h"
#include "popuplistctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"

namespace UI
{

BOOL  PopupListCtrlMKMgr::DoProcessMessage(UIMSG* pMsg)
{
    bool bInterestMsg = true;
    switch (pMsg->message)
    {
    case WM_MOUSEMOVE:
        OnMouseMove(pMsg);
        break;

    case WM_MOUSELEAVE:
        OnMouseLeave(pMsg);
        break;

    case WM_LBUTTONDOWN:
        OnLButtonDown(pMsg);
        break;

    case WM_LBUTTONUP:
        OnLButtonUp(pMsg);
        break;

    case WM_KEYDOWN:
        OnKeyDown(pMsg, &bInterestMsg);
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

void  PopupListCtrlMKMgr::OnMouseMove(UIMSG* pMsg)
{
    POINT  ptWindow = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
    ListItemBase*  pNewHover = GetItemByPos(ptWindow);

    if (pNewHover != m_pItemHover)
    {
        if (NULL == pNewHover)  // 不取消当前高亮，除非有一个新的hover对象
            return;  

        SetHoverItem(pNewHover);

        ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
        if (pSelItem)
            this->m_pListCtrlBase->InvalidateItem(pSelItem);  // 当有hover项时，不显示selected项
    }
    if (m_pItemHover)
    {
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
    }	
}

void  PopupListCtrlMKMgr::OnMouseLeave(UIMSG* pMsg)
{
    if (m_pItemHover)
    {
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSELEAVE, pMsg->wParam, pMsg->lParam);
    }
}

// 鼠标按下时不选中，弹起时才选中
void  PopupListCtrlMKMgr::OnLButtonUp(UIMSG* pMsg)
{
    if (m_pItemHover)  // 选中最后鼠标下的那一项，而不是鼠标当初按下的那项
    {
        POINT pt = {0};
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pListCtrlBase->GetHWND(), &pt);
        if (m_pItemHover == m_pListCtrlBase->HitTest(pt))
        {
            // 只选这一个
			ListItemBase* pItemSave = m_pItemHover;
            m_pListCtrlBase->SelectItem(m_pItemHover, false);
            pItemSave->Fire_ClickNotify();
        }
    }
    else
    {
        m_pListCtrlBase->ClearSelectItem(true);
    }

    if (m_pItemPress)
    {
        POINT point = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

        ListItemBase* pSave = m_pItemPress;
        UISendMessage(pSave->GetIListItemBase(), WM_LBUTTONUP, pMsg->wParam, pMsg->lParam);
    }
}
void  PopupListCtrlMKMgr::OnLButtonDown(UIMSG* pMsg)
{
    ListCtrlMKMgrBase::OnLButtonDown(pMsg);
}

void  PopupListCtrlMKMgr::OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg)
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

void  PopupListCtrlMKMgr::OnKeyDown_up(UIMSG* pMsg)
{
    if (NULL == m_pItemHover)
    {
        ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
        if (NULL == pSelItem)
        {
            ListItemBase* pItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
            if (pItem)
                m_pListCtrlBase->SelectItem(pItem, false);
        }
        else 
        {
            ListItemBase* pPrevItem = pSelItem->GetPrevVisibleItem();
            if (pPrevItem)
                m_pListCtrlBase->SelectItem(pPrevItem, false);
        }
    }
    else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
    {
        ListItemBase* pPrevItem = m_pItemHover->GetPrevVisibleItem();
        if (pPrevItem)
            m_pListCtrlBase->SelectItem(pPrevItem, false);
        else
            m_pListCtrlBase->SelectItem(m_pItemHover, false);
    }

    if (m_pListCtrlBase->GetFirstSelectItem())  // 清除hover对象，显示selection对象
    {
        SetHoverItem(NULL);
    }
}

void  PopupListCtrlMKMgr::OnKeyDown_down(UIMSG* pMsg)
{
    if (NULL == m_pItemHover)
    {
        ListItemBase* pSelItem = m_pListCtrlBase->GetFirstSelectItem();
        if (NULL == pSelItem)
        {
            ListItemBase* pItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
            if (pItem)
                m_pListCtrlBase->SelectItem(pItem, false);
        }
        else 
        {
            ListItemBase* pNextItem = pSelItem->GetNextVisibleItem();
            if (pNextItem)
                m_pListCtrlBase->SelectItem(pNextItem, false);		
        }
    }
    else   // 在存在HOVER对象的情况下面，选择HOVER的下一个对象
    {
        ListItemBase* pNextItem = m_pItemHover->GetNextVisibleItem();
        if (pNextItem)
            m_pListCtrlBase->SelectItem(pNextItem, false);
        else
            m_pListCtrlBase->SelectItem(m_pItemHover, false);
    }

    if (m_pListCtrlBase->GetFirstSelectItem())  // 清除hover对象，显示selection对象
    {
        SetHoverItem(NULL);
    }
}


}