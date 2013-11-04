#include "stdafx.h"
#include "listitembase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemRootPanel\listitemrootpanel.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

ListItemBase::ListItemBase()
{
    m_pIListItemBase = NULL;
    m_nConfigWidth = m_nConfigHeight = 20;

    m_nLineIndex = 0;
    m_nTreeIndex = 0;
    m_nNeighbourIndex = 0;
    m_nDepth = 0;
  
    m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = NULL;
    m_pParent = m_pChild = NULL;

    m_rcParent.SetRectEmpty();

    m_lId = 0;
    m_lItemType = 0;
    m_pData = NULL;
    m_pCtrlData = NULL;
    m_nItemState = 0;
    m_nItemFlag = 0;
    m_pIconRender = NULL;
    m_pListCtrlBase = NULL;
    m_nFlagsDelayOp = 0;

    m_pPanelRoot = NULL;
}
ListItemBase::~ListItemBase()
{
    ListItemBase* pItem = m_pChild;
    while (pItem)
    {
        ListItemBase* pNext = pItem->GetNextItem();
        pItem->GetIListItemBase()->delete_this();

        pItem = pNext;
    }
    m_pChild = NULL;
    m_pParent = NULL;

    m_pPrev = m_pNext = m_pPrevSelection = m_pNextSelection = NULL;
    m_pData = NULL;
    m_pListCtrlBase = NULL;
    SAFE_RELEASE(m_pIconRender);
    SAFE_DELETE_Ixxx(m_pPanelRoot);
}

void  ListItemBase::SetIListCtrlBase(IListCtrlBase* p)
{
    if (p)
        m_pListCtrlBase = p->GetImpl();
    else
        m_pListCtrlBase = NULL;
}
IListCtrlBase*  ListItemBase::GetIListCtrlBase() 
{
    if (m_pListCtrlBase)
        return m_pListCtrlBase->GetIListCtrlBase();
    return NULL;
}

void  ListItemBase::AddChildFront(ListItemBase* p)
{
	if (!p)
		return;

	ListItemBase* pOldChild = this->GetChildItem();
	this->SetChildItem(p);
	p->SetParentItem(this);
	if (pOldChild)
	{
		pOldChild->SetPrevItem(p);
		p->SetNextItem(pOldChild);
	}

}
void  ListItemBase::AddChild(ListItemBase* p)
{
	if (!p)
		return;

	ListItemBase* pOldLast = this->GetLastChildItem();
	p->SetParentItem(this);
	if (pOldLast)
	{
		pOldLast->SetNextItem(p);
		p->SetPrevItem(pOldLast);
	}
	else
	{
		this->SetChildItem(p);
	}
}
void  ListItemBase::SetText(const TCHAR* szText)
{
    if (NULL == szText)
    {
        m_strText.clear();
        return;
    }
    m_strText = szText;
}
void  ListItemBase::SetToolTip(const TCHAR* szText)
{
    if (NULL == szText)
    {
        m_strToolTip.clear();
        return;
    }
    m_strToolTip = szText;
}

UINT  ListItemBase::GetItemState() 
{ 
    return m_nItemState; 
}
UINT  ListItemBase::GetItemDelayOp() 
{ 
    return m_nFlagsDelayOp; 
}
void  ListItemBase::AddItemDelayOp(int n)
{ 
    m_nFlagsDelayOp |= n;
}
void  ListItemBase::RemoveDelayOp(int n)
{ 
    m_nFlagsDelayOp &= ~n; 
}
void  ListItemBase::ClearDelayOp()
{ 
    m_nFlagsDelayOp = 0; 
}

bool  ListItemBase::IsMySelfVisible()
{
    if (m_nItemState & OSB_UNVISIBLE)
        return false;
    else
        return true;
}

bool  ListItemBase::IsVisible()
{
    if (false == IsMySelfVisible())
        return false;

    if (NULL == m_pParent)
        return true;

    if (m_pParent->IsCollapsed())
        return false;

    return m_pParent->IsVisible();
}

bool  ListItemBase::IsDisable()
{
    if (m_nItemState & OSB_DISABLE)
        return true;
    else
        return false;
}
bool  ListItemBase::IsHover()
{
    if (m_nItemState & OSB_HOVER)
        return true;
    else
        return false;
}
bool  ListItemBase::IsPress()
{
    if (m_nItemState & OSB_PRESS)
        return true;
    else
        return false;
}
bool  ListItemBase::IsFocus()
{
    if (m_nItemState & OSB_FOCUS)
        return true;
    else
        return false;
}
bool  ListItemBase::IsCollapsed()
{
    if (m_nItemState & OSB_COLLAPSED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsExpand()
{
    if (m_nItemState & OSB_COLLAPSED)
        return false;
    else
        return true;
}
bool  ListItemBase::IsSelected()
{
    if (m_nItemState & OSB_SELECTED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsChecked()
{
    if (m_nItemState & OSB_CHECKED)
        return true;
    else
        return false;
}
bool  ListItemBase::IsRadioChecked()
{
    if (m_nItemState & OSB_RADIOCHECKED)
        return true;
    else
        return false;
}

void  ListItemBase::SetPress(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_PRESS;
    else
        m_nItemState &= ~OSB_PRESS;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_PRESS;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetHover(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_HOVER;
    else
        m_nItemState &= ~OSB_HOVER;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetFocus(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_FOCUS;
    else
        m_nItemState &= ~OSB_FOCUS;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetExpand(bool b, bool bNotify)
{
    int nOld = m_nItemState;
    if (b)
        m_nItemState &= ~OSB_COLLAPSED;
    else
        m_nItemState |= OSB_COLLAPSED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_COLLAPSED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }

//     if (m_bExpand == b)
//         return;
// 
//     m_bExpand = b;
//     if (ListItemBase)
//     {
//         m_pCheckBtnExpandCollapse->SetCanRedraw(false);
//         m_pCheckBtnExpandCollapse->SetCheck(m_bExpand?BST_UNCHECKED:BST_CHECKED);
//         m_pCheckBtnExpandCollapse->SetCanRedraw(true);
//     }
}
void  ListItemBase::SetSelected(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_SELECTED;
    else
        m_nItemState &= ~OSB_SELECTED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

void  ListItemBase::SetDisable(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_DISABLE;
    else
        m_nItemState &= ~OSB_DISABLE;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_DISABLE;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetChecked(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_CHECKED;
    else
        m_nItemState &= ~OSB_CHECKED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_CHECKED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}
void  ListItemBase::SetRadioChecked(bool b, bool bNotify)
{
    UINT nOld = m_nItemState;
    if (b)
        m_nItemState |= OSB_RADIOCHECKED;
    else
        m_nItemState &= ~OSB_RADIOCHECKED;

    if (bNotify && m_nItemState != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_RADIOCHECKED;
        msg.pMsgTo = m_pIListItemBase;
        UISendMessage(&msg);
    }
}

ListItemBase*  ListItemBase::GetLastChildItem()
{
    ListItemBase* pChild = (ListItemBase*)m_pChild;
    if (NULL == pChild)
        return NULL;

    if (NULL == pChild->GetNextItem())
        return (ListItemBase*)pChild;

    while (pChild = pChild->GetNextItem())
    {
        if (NULL == pChild->GetNextItem())
            break;
    }
    return (ListItemBase*)pChild;
}

// 判断pChild是否是自己的子结点或者子孙结点
bool  ListItemBase::IsMyChildItem(ListItemBase* pChild, bool bTestGrandChildren)
{
    if (NULL == pChild)
        return false;

    if (bTestGrandChildren)
    {
        ListItemBase* pParent = pChild->GetParentItem();
        while (pParent)
        {
            if (pParent == this)
                return true;

            pParent = pParent->GetParentItem();
        }
    }
    else
    {
        return pChild->GetParentItem() == this;
    }

    return false;
}

// 按钮父->左->右的顺序获取遍历下一个结点
ListItemBase*  ListItemBase::GetNextTreeItem()
{
    ListItemBase* p = this;

    // 有子结点
    if (p->GetChildItem()) 
    {
        return p->GetChildItem();
    }

    // 没有子结点，获取下一个结点
    if (p->GetNextItem())
    {
        return p->GetNextItem();
    }

    // 也没有下一个子结点，获取父对象的下一个结点
    ListItemBase* pParent = p->GetParentItem();
    while (pParent)
    {
        ListItemBase* pNext = (ListItemBase*)pParent->GetNextItem();
        if (pNext)
            return pNext;

        pParent = pParent->GetParentItem();
    }

    return NULL;
}
// 按钮右->左->父的顺序获取遍历下一个结点
ListItemBase*  ListItemBase::GetPrevTreeItem()
{
    ListItemBase* p = (ListItemBase*)this;

    // 获取左侧结点的最后一个子结点
    if (p->GetPrevItem())
    {
        ListItemBase* pLeft = p->GetPrevItem();
        ListItemBase* pChild = NULL;
        while (pChild = pLeft->GetLastChildItem())
        {
            pLeft = pChild;
        }
        return pLeft;
    }

    // 有父结点
    if (p->GetParentItem()) 
    {
        return p->GetParentItem();
    }

    return NULL;
}

ListItemBase*  ListItemBase::GetNextVisibleItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetNextTreeItem())
    {
        if (pTreeItem->IsVisible())
            return pTreeItem;
    }

    return NULL;
}

ListItemBase*  ListItemBase::GetPrevVisibleItem()
{
    ListItemBase* pTreeItem = this;

    while (pTreeItem = pTreeItem->GetPrevTreeItem())
    {
        if (pTreeItem->IsVisible())
            return pTreeItem;
    }

    return NULL;
}
// int  ListItemBase::GetDepth()
// {
//     int  nDepth = 0;
//     
//     ListItemBase*  pParent = m_pParent;
//     while (pParent)
//     {
//         nDepth++;
//         pParent = pParent->GetParentItem();
//     }
//     return nDepth;
// }

IPanel*  ListItemBase::GetRootPanel() 
{ 
    return m_pPanelRoot;
}
// 创建RootPanel，为插入内部控件做准备
bool ListItemBase::CreateRootPanel()
{
    if (m_pPanelRoot)
        return true;

    if (NULL == m_pListCtrlBase)
    {
        UIASSERT(0);
        return false;
    }

    IUIApplication*  pUIApp = m_pListCtrlBase->GetIListCtrlBase()->GetUIApplication();

    IListItemRootPanel* p = NULL;
    IListItemRootPanel::CreateInstance(pUIApp, &p);
    if (NULL == p)
        return false;

    m_pPanelRoot = static_cast<IPanel*>(p);

    p->SetListCtrlItemPtr(m_pListCtrlBase->GetIListCtrlBase(), m_pIListItemBase);
    p->SetParentObjectDirect(m_pListCtrlBase->GetIListCtrlBase());   // 背景的绘制已由root panel完成，不再需要更多的父关系
    m_pPanelRoot->SetLayoutType(LAYOUT_TYPE_CANVAS);
    
    OnSize();

    return true;
}

LRESULT  ListItemBase::OnDelayOp(UINT, WPARAM, LPARAM)
{
    if (m_nFlagsDelayOp & DELAY_OP_SIZE_CHANGED)
    {
        OnSize();
        m_nFlagsDelayOp &= ~DELAY_OP_SIZE_CHANGED;
    }
    return 0;
}

void  ListItemBase::Draw(IRenderTarget* pRenderTarget, RenderContext* proc)
{
    if (m_nFlagsDelayOp)
    {
        UI::UISendMessage(m_pIListItemBase, UI_LISTITEM_MSG_DELAY_OP);
    }

    UISendMessage(m_pIListItemBase, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)proc);
}

void  ListItemBase::DrawItemInnerControl(IRenderTarget* pRenderTarget, RenderContext* proc)
{
    if (NULL == m_pPanelRoot || !proc)
        return;
	if (!m_pPanelRoot->GetChildObject())
		return;

    // 将DC偏移原点调整到RootPanel左上角

    POINT ptOldOffset = {0,0};
    pRenderTarget->GetViewportOrgEx(&ptOldOffset);
	
	RenderContext roc = *proc;
	roc.m_bUpdateClip = true;
	roc.DrawListItem(m_pIListItemBase);

	roc.m_ptOffset.x = ptOldOffset.x + m_rcParent.left;
	roc.m_ptOffset.y = ptOldOffset.y + m_rcParent.top;
	m_pPanelRoot->GetImpl()->DrawChildObject(pRenderTarget, roc);
	
	{
		bool bOld = proc->m_bUpdateClip;
		proc->m_bUpdateClip = true;
		proc->Update(pRenderTarget);  // 还原
		proc->m_bUpdateClip = bOld;
	}
}

void ListItemBase::SetParentRect(CRect* prc)
{
    m_rcParent.CopyRect(prc); 
    m_nFlagsDelayOp |= DELAY_OP_SIZE_CHANGED;  // 延迟处理，提高效率
}

void  ListItemBase::OnSize()
{
    BOOL bHandled = FALSE;
    UISendMessage(m_pIListItemBase, WM_SIZE, (WPARAM)0, MAKELPARAM(m_rcParent.Width(), m_rcParent.Height()), 0, 0, 0, &bHandled);
    if (bHandled)
        return;

    if (m_pPanelRoot)
    {
        m_pPanelRoot->SetObjectPos(&m_rcParent, SWP_NOREDRAW);
//         ILayout* pLayout = m_pPanelRoot->GetLayout();
//         if (pLayout)
//             pLayout->Arrange(NULL);
    }
}

void  ListItemBase::OnLButtonDown(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);
}
void  ListItemBase::OnLButtonUp(UINT nFlags, POINT point)
{
    if (m_pPanelRoot)
    {
        if (ProcessItemMKMessage(GetCurMsg()))
        {
            return;
        }
    }
}

void ListItemBase::Fire_ClickNotify()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_LCN_CLICK;
    msg.lParam  = (LPARAM)m_pListCtrlBase->GetPressItem();
    msg.pMsgFrom = m_pListCtrlBase->GetIMessage();
    m_pListCtrlBase->DoNotify(&msg);
}

void  ListItemBase::OnRButtonUp(UINT nFlags, POINT point)
{
    if (m_pPanelRoot)
        ProcessItemMKMessage(GetCurMsg());

    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_LCN_RCLICK;
    msg.wParam  = MAKEWPARAM(point.x, point.y);
    msg.lParam  = (LPARAM)m_pIListItemBase;
    msg.pMsgFrom = m_pListCtrlBase->GetIMessage();
    m_pListCtrlBase->DoNotify(&msg);
}

void  ListItemBase::OnLButtonDblClk(UINT nFlags, POINT point)
{
    if (m_pPanelRoot)
        ProcessItemMKMessage(GetCurMsg());

    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_LCN_DBCLICK;
    msg.wParam = MAKEWPARAM(point.x, point.y);
    msg.lParam = (LPARAM)m_pIListItemBase;
    msg.pMsgFrom = m_pListCtrlBase->GetIListCtrlBase();
    m_pListCtrlBase->GetIListCtrlBase()->DoNotify(&msg);
}

void  ListItemBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    if (!bReload)
    {
        const TCHAR* szText = pMapAttrib->GetAttr(XML_TOOLTIP, true);
        if (szText)
            m_strToolTip = szText;
    }

    pMapAttrib->GetAttr_RenderBase(XML_ICON_RENDER_PREFIX, XML_RENDER_TYPE, true,
        m_pListCtrlBase->GetUIApplication(), m_pListCtrlBase->GetIListCtrlBase(), &m_pIconRender);
}

void ListItemBase::SetIconRenderType(RENDER_TYPE eType)
{
    SAFE_RELEASE(m_pIconRender);

    // 为了保证自己在析构中释放的m_pIconRender是在自己的模块中创建的
    GetIListCtrlBase()->GetUIApplication()->CreateRenderBase(eType, GetIListCtrlBase(), &m_pIconRender);
}
void ListItemBase::SetIconRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pIconRender);
    m_pIconRender = p;

    if (m_pIconRender)
        m_pIconRender->AddRef();
}

void  ListItemBase::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    pSize->cx = m_nConfigWidth;
    pSize->cy = m_nConfigHeight;
//     ITextRenderBase* pTextRender = m_pListCtrlBase->GetIListCtrlBase()->GetTextRender();
//     if (pTextRender)
//     {
//         *pSize = pTextRender->GetDesiredSize(m_strText.c_str());
//     }
// 
//     int nTextLeftIndent = 0, nTextRightIndent = 0;
//     m_pListCtrlBase->GetTextIndent(&nTextLeftIndent, &nTextRightIndent);
//     pSize->cx += nTextLeftIndent + nTextRightIndent;
}


// 获取一个ITEM的渲染值
int  ListItemBase::GetItemRenderState()
{
	bool bSelected = IsSelected();
	bool bFocus = IsFocus();
    bool bPress = IsPress();
    bool bHover = IsHover();
    bool bCtrlFocus = m_pListCtrlBase->IsFocus();

	int nRenderState = 0;

    if (m_pListCtrlBase->GetIListCtrlBase()->TestStyleEx(LISTCTRLBASE_STYLE_POPUPLISTBOX))    // 以最后鼠标弹出来的那项作为被选中的项
    {
        if (bHover)
        {
            nRenderState = LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER;
        }
        else if(NULL == m_pListCtrlBase->GetHoverItem() && IsSelected())
        {
            nRenderState = LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER;
        }
        return nRenderState;
    }

	if (IsDisable())
	{
		nRenderState =  bSelected ?
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE :
					LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE ;
	}
	else if (bPress)
	{
		nRenderState = bSelected ? 
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS :
					LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS ;
	}
	else if (NULL == m_pListCtrlBase->GetPressItem() && bHover)
	{
		nRenderState = bSelected ?
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER :
					LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER ;
	}
	else
	{
		nRenderState =  bSelected ?
			(bCtrlFocus ? 
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL:
					LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS)
						:
					LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL ;
	}

	return nRenderState;
}

void  ListItemBase::SetIconFromFile(const TCHAR* szIconPath)
{
    SAFE_RELEASE(m_pIconRender);
    if (NULL == szIconPath)
        return;

    if (NULL == m_pListCtrlBase)
        return;

    IUIApplication*  pUIApplication = GetIListCtrlBase()->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, GetIListCtrlBase(), &m_pIconRender);
    IImageRender*  pImageForeRender = (IImageRender*)m_pIconRender->QueryInterface(UI::uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(GetRenderLibraryType(GetIListCtrlBase()), IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->LoadFromFile(szIconPath, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

}

void*  ListItemBase::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, IID_UI_IListItemBase))
        return m_pIListItemBase;

    SetMsgHandled(FALSE);
    return NULL;
}

BOOL  ListItemBase::ProcessItemMKMessage(UIMSG* pMSG)
{
    if (NULL == m_pListCtrlBase)
        return FALSE;

    return m_pListCtrlBase->ProcessItemMKMessage(pMSG, this);
}
}