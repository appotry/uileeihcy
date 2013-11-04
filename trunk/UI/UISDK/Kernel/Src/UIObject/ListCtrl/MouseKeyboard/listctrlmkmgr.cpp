#include "stdafx.h"
#include "listctrlmkmgr.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemBase\listitembase.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"

#include "UISDK\Kernel\Src\Helper\mousekeyboard\mkmgr.h"
ListCtrlMKMgrBase::ListCtrlMKMgrBase()
{
    m_pObjHover = NULL;
    m_pObjPress = NULL;
    m_pObjFocus = NULL;

    m_pItemHover = NULL;
    m_pItemPress = NULL;
    m_pItemFocus = NULL;
//     m_pItemSelected = NULL;

    m_pUIApplication = NULL;
    m_pListCtrlBase = NULL;
	m_bKeepHoverWhenPress = true;
}

ListCtrlMKMgrBase::~ListCtrlMKMgrBase()
{

}

BOOL  ListCtrlMKMgrBase::DoProcessMessage(UIMSG* pMsg)
{
    bool  bInterestMsg = true;
    switch(pMsg->message)
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

    case WM_LBUTTONDBLCLK:
        OnLButtonDBClick(pMsg);
        break;

    case WM_LBUTTONUP:
        OnLButtonUp(pMsg);
        break;

    case WM_RBUTTONDOWN:
        OnRButtonDown(pMsg);
        break;

    case WM_RBUTTONUP:
        OnRButtonUp(pMsg);
        break;

    case WM_KILLFOCUS:
        OnKillFocus(pMsg);
        break;

    case WM_SETFOCUS:
        OnSetFocus(pMsg);
        break;

    case WM_SETCURSOR:
        {
            if (OnSetCursor(pMsg))
                return true;
            else
                return false;
        }
        break;

    case WM_CHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
	case WM_IME_STARTCOMPOSITION:   // EDIT输入法跟随
        if (OnKeyMsg(pMsg))
            return true;
        else
            return false;
        break;

    default:
        bInterestMsg = false;
        break;
    }


    if (bInterestMsg)
    {
        m_pListCtrlBase->Refresh();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL  ListCtrlMKMgrBase::DoProcessItemMessage(UIMSG* pMsg, ListItemBase* pItem)
{
    if (NULL == pItem->GetRootPanel())
        return FALSE;

    bool bInterestMsg = true;
    switch (pMsg->message)
    {
    case WM_MOUSEMOVE:
        OnItemMouseMove(pMsg, pItem);
        break;

    case WM_MOUSELEAVE:
        OnItemMouseLeave(pMsg, pItem);
        break;

    case WM_LBUTTONDOWN:
        OnItemLButtonDown(pMsg, pItem);
        break;

    case WM_LBUTTONDBLCLK:
        OnItemLButtonDBClick(pMsg, pItem);
        break;

    case WM_LBUTTONUP:
        {
            BOOL b = GetHoverObject() || GetPressObject();
            OnItemLButtonUp(pMsg, pItem);
            if (b)
                return true;
            else
                return false;
        }
        break;

    case WM_KILLFOCUS:
        OnItemKillFocus(pMsg, pItem);
        break;

    case WM_SETCURSOR:
        {
            if (OnItemSetCursor(pMsg, pItem))
                return true;
            else
                return false;
        }
        break;

    case WM_KEYDOWN:
    case WM_CHAR:
    case WM_KEYUP:
	case WM_IME_STARTCOMPOSITION:  // EDIT输入法跟随
        if (OnItemKeyMsg(pMsg, pItem))
            return true;
        else 
            return false;

    default:
        bInterestMsg = false;
    }

    if (!bInterestMsg)
        return false;

//     if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
//     {
//         if (!GetHoverObject() && !GetPressObject())
//             return false;
//     }
    return true;
}

void  ListCtrlMKMgrBase::SetListCtrlBase(UIApplication* pUIApp, ListCtrlBase*  pCtrl)
{
    m_pListCtrlBase = pCtrl;
    m_pUIApplication = pUIApp;
}


void  ListCtrlMKMgrBase::OnMouseMove(UIMSG* pMsg)
{
    POINT  ptWindow = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
    ListItemBase*  pNewHover = GetItemByPos(ptWindow);

    if ( (!m_bKeepHoverWhenPress || NULL == m_pItemPress) && pNewHover != m_pItemHover)  // m_bKeepHoverWhenPress为true时表示当鼠标已经按住了一个控件时，不改变hover item
    {
        ListItemBase* pSave = m_pItemHover;
        SetHoverItem(pNewHover);

        if (pSave != pNewHover)
        {
            if (pSave)
                UISendMessage(pSave->GetIListItemBase(), WM_MOUSELEAVE);
            
            if (pNewHover)
                UISendMessage(pNewHover->GetIListItemBase(), WM_MOUSEHOVER);
        }
    }

    if (m_pItemHover)
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
}

void  ListCtrlMKMgrBase::OnMouseLeave(UIMSG* pMsg)
{
    ListItemBase* pSave = m_pItemHover;
    SetPressItem(NULL);
    SetHoverItem(NULL);

    if (pSave)
    {
        UISendMessage(pSave->GetIListItemBase(), WM_MOUSELEAVE, pMsg->wParam, pMsg->lParam);
        this->m_pListCtrlBase->InvalidateItem(pSave);
    }

}


void  ListCtrlMKMgrBase::OnItemMouseMove(UIMSG* pMsg, ListItemBase* pItem)
{
    IPanel* pIRootPanel = pItem->GetRootPanel();
    if (NULL == pIRootPanel)
        return;

    // 将鼠标转换为相对于ITEM
    POINT pt = { GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam) };
    POINT ptInPanel = {0,0};
    m_pListCtrlBase->WindowPoint2ItemPoint(pItem, &pt, &ptInPanel);

    Object* pObj = pIRootPanel->GetImpl()->GetObjectByPos(&ptInPanel);
    _OnMouseMove(pObj, pMsg->wParam, pMsg->lParam, this);
}

void  ListCtrlMKMgrBase::OnItemMouseLeave(UIMSG* pMsg, ListItemBase* pItem)
{
    IPanel* pIRootPanel = pItem->GetRootPanel();
    if (NULL == pIRootPanel)
        return;

    _OnMouseLeave(this);
}

void  ListCtrlMKMgrBase::OnItemLButtonDown(UIMSG* pMsg, ListItemBase* pItem)
{
    BOOL bHandled = FALSE;
    _OnLButtonDown(pMsg->wParam, pMsg->lParam, &bHandled, this);
}
void  ListCtrlMKMgrBase::OnItemLButtonDBClick(UIMSG* pMsg, ListItemBase* pItem)
{
    BOOL bHandled = FALSE;
    _OnLButtonDBClick(pMsg->wParam, pMsg->lParam, &bHandled, this);
}
void  ListCtrlMKMgrBase::OnItemLButtonUp(UIMSG* pMsg, ListItemBase* pItem)
{
    _OnLButtonUp(pMsg->wParam, pMsg->lParam, this);
}
bool  ListCtrlMKMgrBase::OnItemSetCursor(UIMSG* pMsg, ListItemBase* pItem)
{
    if (m_pObjPress)
    {
        ::UISendMessage(m_pObjPress, WM_SETCURSOR, pMsg->wParam, pMsg->lParam);
        return true;
    }
    else if (m_pObjHover)
    {
        ::UISendMessage(m_pObjHover, WM_SETCURSOR, pMsg->wParam, pMsg->lParam);
        return true;
    }
    return false;
}
void  ListCtrlMKMgrBase::OnItemKillFocus(UIMSG* pMsg, ListItemBase* pItem)
{
    if (m_pObjFocus)
    {
        SetFocusObject(NULL);
    }
}
bool  ListCtrlMKMgrBase::OnItemKeyMsg(UIMSG* pMsg, ListItemBase* pItem)
 {
    if (m_pObjFocus)
    {
        BOOL bHandled = FALSE;
        ::UISendMessage(m_pObjFocus->GetIObject(), pMsg->message, pMsg->wParam, pMsg->lParam,
            0,0,0, &bHandled);

        return bHandled ?  true:false;
    }
    return false;
}

// 处理press item，但不处理selection item
void  ListCtrlMKMgrBase::OnLButtonDown(UIMSG* pMsg)
{
    if (m_pObjPress)
        return;

    if (m_pItemHover)
    {
        this->SetPressItem(m_pItemHover);
        this->SetFocusItem(m_pItemHover);
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_LBUTTONDOWN, pMsg->wParam, pMsg->lParam);
    }
}

void  ListCtrlMKMgrBase::OnLButtonUp(UIMSG* pMsg)
{
    if (m_pItemPress)
    {
        POINT point = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

        ListItemBase* pSave = m_pItemPress;
        this->SetPressItem(NULL);
        UISendMessage(pSave->GetIListItemBase(), WM_LBUTTONUP, pMsg->wParam, pMsg->lParam);
        pSave->Fire_ClickNotify();
    }
}

void  ListCtrlMKMgrBase::OnLButtonDBClick(UIMSG* pMsg)
{
    if (m_pObjPress)
        return;

    if (m_pItemHover)
    {
        this->SetPressItem(m_pItemHover);
        this->SetFocusItem(m_pItemHover);

        UISendMessage(m_pItemHover->GetIListItemBase(), WM_LBUTTONDBLCLK, pMsg->wParam, pMsg->lParam);
    }
}


void  ListCtrlMKMgrBase::OnRButtonDown(UIMSG* pMsg)
{
    if (m_pObjPress)
        return;

    if (!m_pItemHover)
        return;

    if (!m_pItemHover->IsSelected())
    {
        this->SetFocusItem(m_pItemHover);
        m_pListCtrlBase->SelectItem(m_pItemHover, true);

        if (m_pItemHover)
            UISendMessage(m_pItemHover->GetIListItemBase(), WM_RBUTTONDOWN, pMsg->wParam, pMsg->lParam);
    }
    else
    {
        UISendMessage(m_pItemHover->GetIListItemBase(), WM_RBUTTONDOWN, pMsg->wParam, pMsg->lParam);
    }
}
void  ListCtrlMKMgrBase::OnRButtonUp(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        UISendMessage(m_pItemFocus->GetIListItemBase(), WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
    }
}
void  ListCtrlMKMgrBase::OnSetFocus(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        UISendMessage(m_pItemFocus->GetIListItemBase(), WM_SETFOCUS, pMsg->wParam, pMsg->lParam);
    }
}
void  ListCtrlMKMgrBase::OnKillFocus(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        UISendMessage(m_pItemFocus->GetIListItemBase(), WM_KILLFOCUS, pMsg->wParam, pMsg->lParam);
    }
}
bool  ListCtrlMKMgrBase::OnSetCursor(UIMSG* pMsg)
{
    ListItemBase*  pItem = m_pItemPress;
    if (NULL == pItem)
        pItem = m_pItemHover;

    BOOL bHandled = FALSE;
    if (pItem)
    {
        UISendMessage(pItem->GetIListItemBase(), WM_SETCURSOR, pMsg->wParam, pMsg->lParam, 0, 0, 0, &bHandled);
    }
    return bHandled?true:false;
}

bool  ListCtrlMKMgrBase::OnKeyMsg(UIMSG* pMsg)
{
    if (m_pItemFocus)
    {
        BOOL bHandled = FALSE;
        UISendMessage(m_pItemFocus->GetIListItemBase(), pMsg->message, pMsg->wParam, pMsg->lParam,
            0,0,0, &bHandled);

        return bHandled ? true : false;
    }

    return false;
}

void ListCtrlMKMgrBase::OnRemoveAll()
{
    m_pItemHover = m_pItemPress = m_pItemFocus = NULL; 
    m_pObjPress = m_pObjHover = m_pObjFocus = NULL;
}

void  ListCtrlMKMgrBase::OnRemoveItem(ListItemBase* pItem)
{
    if (m_pItemHover == pItem)
    {
        m_pItemHover = NULL;
    }

    if (m_pItemPress == pItem)
    {
        m_pItemPress = NULL;
    }

    if (m_pItemFocus == pItem)
    {
        ListItemBase*  pNextItem = pItem->GetNextVisibleItem();
        if (NULL == pNextItem)
            pNextItem = pItem->GetPrevVisibleItem();
        
        m_pItemFocus = NULL; // 避免SetFocusItem中去刷新pItem，造成崩溃
        SetFocusItem(pNextItem);
    }

    // 从selection列表中删除，放在最后通知sel changed
    if (pItem->IsSelected())
    {
        m_pListCtrlBase->RemoveSelectItem(pItem, false);
//         if (NULL == m_pListCtrlBase->GetFirstSelectItem())  // 保持至少有一个选项--- 错误 -- 应该是保持一个焦点项
//         {
//             ListItemBase*  pNextItem = pItem->GetNextVisibleItem();
//             if (NULL == pNextItem)
//             {
//                 pNextItem = pItem->GetPrevVisibleItem();
//             }
//             if (pNextItem)
//             {
//                 m_pListCtrlBase->SelectItem(pNextItem, false);
//             }
//             else
//             {
//                 m_pListCtrlBase->FireSelectItemChanged(NULL);
//             }
//         }
    }

//     Object* pFocusObj = GetFocusObject();
//     if (NULL == pFocusObj)
//         return;
// 
//     IPanel* pPanel = pItem->GetRootPanel();
//     if (NULL == pPanel)
//         return;
// 
//     if (pPanel->GetImpl()->IsMyChild(pFocusObj, true))
//     {
//         SetFocusObject(NULL);
//     }
}
// void  ListCtrlMKMgrBase::OnRemoveObject(IObject* pObj)
// {
//     if (NULL == pObj)
//         return;
// 
//     Object* p = pObj->GetImpl();
//     Object* pFocusObj = GetFocusObject();
//     if (p == pFocusObj || p->IsMyChild(pFocusObj, true))
//     {
//         SetFocusObject(NULL);
//     }
// }

void  ListCtrlMKMgrBase::OnObjectHideInd(Object* pObj)
{
    if (NULL == pObj)
        return;

    if (pObj == m_pObjFocus || pObj->IsMyChild(m_pObjFocus, true))
    {
        SetFocusObject(NULL);
    }
    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
    {
        SetHoverObject(NULL);
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
    {
        SetPressObject(NULL);
    }

}
void  ListCtrlMKMgrBase::OnObjectDeleteInd(Object* pObj)
{
    if (NULL == pObj)
        return;

    if (m_pObjHover == pObj || pObj->IsMyChild(m_pObjHover, true))
    {
        m_pObjHover = NULL;
    }
    if (m_pObjPress == pObj || pObj->IsMyChild(m_pObjPress, true))
    {
        m_pObjPress = NULL;
    }
    if (pObj == m_pObjFocus || pObj->IsMyChild(m_pObjFocus, true))
    {
        m_pObjFocus = NULL;
    }
}

ListItemBase*  ListCtrlMKMgrBase::GetItemByPos(POINT ptWindow)
{
    CRect rcClient;
    m_pListCtrlBase->GetClientRect(&rcClient);

    // 1. 转换为内部坐标
    POINT pt = {0};
    m_pListCtrlBase->WindowPoint2ObjectPoint(&ptWindow, &pt);
    if (FALSE == rcClient.PtInRect(pt))
        return NULL;

    m_pListCtrlBase->ObjectPoint2ObjectClientPoint(&pt, &pt);

    int nxOffset=0, nyOffset=0;
    m_pListCtrlBase->GetScrollOffset(&nxOffset, &nyOffset);
    pt.x += nxOffset;
    pt.y += nyOffset;

    UIMSG  msg;
    msg.message = UI_LCM_HITTEST;
    msg.wParam = MAKEWPARAM(pt.x, pt.y);
    msg.pMsgTo = m_pListCtrlBase->GetIListCtrlBase();

    IListItemBase*  pHoverItem = (IListItemBase*)UISendMessage(&msg);
    if (NULL == pHoverItem)
        return NULL;

    return pHoverItem->GetImpl();
}



void  ListCtrlMKMgrBase::SetHoverItem(ListItemBase* pItem)
{
    if (m_pItemHover == pItem)
        return;

    ListItemBase* pOldHoverItem = m_pItemHover;

    // 提示条逻辑 

    // 为了解决鼠标移到了一个item上的object时，要弹出一个提示条，结果又
    // 调用SetHoverItem为item弹出一个tooltip，导致object的提示条弹不出来.
    // 因此在这里增加了判断hover/press object
    // if (!m_pObjHover && !m_pObjPress)  -- 2013.6.25 17:11，删除该条件，现在是先判断 item , 再判断obj
    {
        m_pUIApplication->HideToolTip();

        if (pItem && m_pItemHover != pItem)
        {
            TOOLTIPITEM  item;
            item.pNotifyObj = m_pListCtrlBase->GetIListCtrlBase();
            item.pItemData = (void*)pItem->GetIListItemBase();
            m_pUIApplication->ShowToolTip(&item);
        }
    }

    if (m_pItemHover)
    {
        m_pItemHover->SetHover(false);
        this->m_pListCtrlBase->InvalidateItem(m_pItemHover);
    }
    if (pItem)
    {
        pItem->SetHover(true);
        this->m_pListCtrlBase->InvalidateItem(pItem);
    }

    m_pItemHover = pItem;
    FireHoverItemChanged(pOldHoverItem);
}


void  ListCtrlMKMgrBase::SetPressItem(ListItemBase* pItem)
{
    if (m_pItemPress == pItem)
        return;

    if (m_pItemPress)
    {
        m_pItemPress->SetPress(false);
        this->m_pListCtrlBase->InvalidateItem(m_pItemPress);
    }
    if (pItem)
    {
        pItem->SetPress(true);
        this->m_pListCtrlBase->InvalidateItem(pItem);
    }

    m_pItemPress = pItem;
}

void  ListCtrlMKMgrBase::SetFocusItem(ListItemBase* pItem)
{
    if (m_pItemFocus == pItem)
        return;

    if (m_pItemFocus)
        this->m_pListCtrlBase->InvalidateItem(m_pItemFocus);

    if (m_pItemFocus)
        m_pItemFocus->SetFocus(false);

    m_pItemFocus = pItem;

    if (m_pItemFocus)
        m_pItemFocus->SetFocus(true);

    // 更新foucs obj为m_pItemFocus下的对象
    if (m_pItemFocus)
    {
        IPanel* pPanel = m_pItemFocus->GetRootPanel();
        if (NULL == pPanel)
        {
            SetFocusObject(NULL);
        }
        else
        {
            Object* pFocusObj = GetFocusObject();
            if (pFocusObj)
            {
                if (!pPanel->GetImpl()->IsMyChild(pFocusObj, true))
                {
                    SetFocusObject(NULL);
                }
            }
        }
    }
    else
    {
        SetFocusObject(NULL);
    }

    if (m_pItemFocus)
        this->m_pListCtrlBase->InvalidateItem(m_pItemFocus);
}

void  ListCtrlMKMgrBase::SetHoverObject(Object* pNewHoverObj)
{
    _SetHoverObject(pNewHoverObj, this);
}
void  ListCtrlMKMgrBase::SetPressObject(Object* pNewPressObj)
{
    _SetPressObject(pNewPressObj, this);
}
void  ListCtrlMKMgrBase::SetFocusObject(Object* pObj)
{
    if (m_pObjFocus == pObj)  
        return;

    if (m_pObjFocus)
    {
        int nOldStateBits = m_pObjFocus->GetStateBit();
        m_pObjFocus->SetFocus(false);
        ::UISendMessage(m_pObjFocus, WM_KILLFOCUS, (WPARAM)pObj, NULL );
    }
    if (pObj)
    {
        int nOldStateBits = pObj->GetStateBit();
        pObj->SetFocus(true);
        ::UISendMessage(pObj, WM_SETFOCUS, (WPARAM)m_pObjFocus, NULL );
    }
    m_pObjFocus = pObj;
    return;
}
 
void  ListCtrlMKMgrBase::FireHoverItemChanged(ListItemBase* pOldHoverItem)
{
    ListItemBase* pHover = m_pItemHover;
    IListCtrlBase* pIListCtrlBase = m_pListCtrlBase->GetIListCtrlBase();

    // 通知 ctrl
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_HOVERITEMCHANGED;
    msg.wParam = (WPARAM)(pOldHoverItem?pOldHoverItem->GetIListItemBase():NULL);

    msg.lParam = (LPARAM)(pHover?pHover->GetIListItemBase():NULL);
    msg.pMsgFrom = pIListCtrlBase;

    // 先交给子类处理
    msg.pMsgTo = pIListCtrlBase;
    UISendMessage(&msg, 0, 0);

    // 再通知外部处理 
    msg.pMsgTo = NULL;
    msg.bHandled = FALSE;
    pIListCtrlBase->DoNotify(&msg);
}


//////////////////////////////////////////////////////////////////////////

BOOL  SingleSelListCtrlMKMgr::DoProcessMessage(UIMSG* pMsg)
{
    switch (pMsg->message)
    {
    case WM_LBUTTONDOWN:
        {
            OnLButtonDown(pMsg);
            m_pListCtrlBase->Refresh();
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
        {
            BOOL bHandled = __super::DoProcessMessage(pMsg);
            if (bHandled)
                return TRUE;
        
            bool bInterestMsg = true;
            OnKeyDown(pMsg, &bInterestMsg);
            if (bInterestMsg)
            {
                m_pListCtrlBase->Refresh();
                return TRUE;
            }
        }
        break;

    default:
        return __super::DoProcessMessage(pMsg);
        break;
    }
    return FALSE;
}

// 仅处理选项
void  SingleSelListCtrlMKMgr::OnLButtonDown(UIMSG* pMsg)
{
    __super::OnLButtonDown(pMsg);

    if (m_pItemPress)
    {
        // 只选这一个
        m_pListCtrlBase->SelectItem(m_pItemPress, false);
    }
    else
    {
        m_pListCtrlBase->ClearSelectItem(true);
    }
}

void  SingleSelListCtrlMKMgr::OnKeyDown(UIMSG* pMsg, bool* pbInterestMsg)
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

	case VK_SPACE:
		OnKeyDown_space(pMsg);
		break;

    default:
        if (pbInterestMsg)
            *pbInterestMsg = false;
        break;
    }
}

void  SingleSelListCtrlMKMgr::OnKeyDown_up(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstVisbleItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
        if (pFirstVisbleItem)
        {
            this->SetFocusItem(pFirstVisbleItem);
            this->m_pListCtrlBase->SelectItem(pFirstVisbleItem, false);
        }
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

void  SingleSelListCtrlMKMgr::OnKeyDown_down(UIMSG* pMsg)
{
    if (NULL == m_pItemFocus)
    {
        ListItemBase* pFirstVisbleItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
        if (pFirstVisbleItem)
        {
            this->SetFocusItem(pFirstVisbleItem);
            this->m_pListCtrlBase->SelectItem(pFirstVisbleItem, false);
        }
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

void  SingleSelListCtrlMKMgr::OnKeyDown_space(UIMSG* pMsg)
{
	if (NULL == m_pItemFocus)
	{
		ListItemBase* pFirstVisbleItem = m_pListCtrlBase->FindVisibleItemFrom(NULL);
		if (pFirstVisbleItem)
		{
			this->SetFocusItem(pFirstVisbleItem);
			this->m_pListCtrlBase->SelectItem(pFirstVisbleItem, false);
		}
	}
	else
	{
		this->m_pListCtrlBase->SelectItem(m_pItemFocus, false);
	}
}

//////////////////////////////////////////////////////////////////////////
