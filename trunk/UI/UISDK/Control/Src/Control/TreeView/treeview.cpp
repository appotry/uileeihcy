#include "stdafx.h"
#include "treeview.h"
#include "UISDK\Control\Src\Control\TreeView\normaltreeitem.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"

namespace UI
{
TreeView::TreeView()
{
    m_pITreeView = NULL;
    m_pExpandIcon = NULL;
}
TreeView::~TreeView()
{
    SAFE_RELEASE(m_pExpandIcon);
}


HRESULT  TreeView::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(ITreeView, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pITreeView->SetLayoutDefaultHV();
    return S_OK;
}
void  TreeView::ResetAttribute()
{
    DO_PARENT_PROCESS(ITreeView, IListCtrlBase);
    SAFE_RELEASE(m_pExpandIcon);
}

void  TreeView::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication*  pUIApp = m_pITreeView->GetUIApplication();

    if (!pMapAttrib->HasAttrib(XML_TEXTRENDER_TYPE))
    {
        if (NULL == m_pITreeView->GetTextRender())
        {
            ITextRenderBase*  pTextRender = NULL;
            pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_THEME_LISTVIEW, m_pITreeView, &pTextRender);

            if (pTextRender)
            {
                UI::IMapAttribute*  pMapAttrib = NULL;
                UICreateIMapAttribute(&pMapAttrib);
                pTextRender->SetAttribute(pMapAttrib, NULL, true);
                SAFE_RELEASE(pMapAttrib);

                pTextRender->SetTextAlignment(DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_NOPREFIX);

                m_pITreeView->SetTextRender(pTextRender);
                SAFE_RELEASE(pTextRender);
            }
        }
    }

    DO_PARENT_PROCESS(ITreeView, IListCtrlBase);

    if (NULL == m_pITreeView->GetBkRender())
    {
        IRenderBase*  pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTBOX_BKGND, m_pITreeView, &pRender);

        if (pRender)
        {
            m_pITreeView->SetBkgndRender(pRender);
            CRegion4 r(2,2,2,2);
            m_pITreeView->SetBorderRegion(&r);
            SAFE_RELEASE(pRender);
        }
    }

    // 	if (NULL == m_pForegndRender)
    // 	{
    // 		m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_COLORLIST, this);
    // 		ColorListRender* p = dynamic_cast<ColorListRender*>(m_pForegndRender);
    // 
    // 		p->SetCount(8);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL,  RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER,   RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS,   RGB(51,153,255),true, 0,false);
    // 		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE, RGB(51,153,255),true, 0,false);
    // 	}
    if (NULL == m_pITreeView->GetForeRender())
    {
        IRenderBase*  pRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_ITEM, m_pITreeView, &pRender);

        if (pRender)
        {
            m_pITreeView->SetForegndRender(pRender);
            SAFE_RELEASE(pRender);
        }
    }

    pMapAttrib->GetAttr_RenderBase(XML_EXPANDICON_RENDER_TYPE_PREFIX, XML_RENDER_TYPE, true, pUIApp, m_pITreeView, &m_pExpandIcon);
    if (NULL == m_pExpandIcon)
    {
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_TREEVIEW_EXPANDCOLLAPSE_ICON, m_pITreeView, &m_pExpandIcon);
    }
    if (NULL == m_pITreeView->GetFocusRender())
    {
        IRenderBase*  pFocusRender = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_LISTVIEW_FOCUS, m_pITreeView, &pFocusRender);
        if (pFocusRender)
        {
            m_pITreeView->SetFocusRender(pFocusRender);
            SAFE_RELEASE(pFocusRender);
        }
    }


    IScrollBarManager*  pScrollBarMgr = m_pITreeView->GetIScrollBarMgr();
    if (pScrollBarMgr)
    {
        if (!pScrollBarMgr->GetHScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemHScrollBar*  p = NULL;
            ISystemHScrollBar::CreateInstance(pUIApp, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pITreeView->AddNcChild(p);
            p->SetID(XML_HSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
        }

        if (!pScrollBarMgr->GetVScrollBar() && pScrollBarMgr->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
        {
            ISystemVScrollBar*  p = NULL;
            ISystemVScrollBar::CreateInstance(pUIApp, &p);
            p->SetIScrollBarMgr(pScrollBarMgr);
            m_pITreeView->AddNcChild(p);
            p->SetID(XML_VSCROLLBAR_PRIFIX);

            p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
            p->SetVisible(false, false, false);
            pScrollBarMgr->SetVScrollWheel(20);
        }
    }
}

void TreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (DO_PARENT_PROCESS(ITreeView, IListCtrlBase))
        return;

    if (VK_LEFT == nChar)
    {
        IListItemBase* pItem = m_pITreeView->GetFocusItem();
        if (NULL == pItem)
            return;

        // 如果是自己是展开的，则收缩
        if (pItem->HasChild() && pItem->IsExpand())
        {
            m_pITreeView->CollapseItem(pItem, false);
        }
        // 否则选中父结点
        else if (NULL != pItem->GetParentItem())
        {
            m_pITreeView->SelectItem(pItem->GetParentItem(), false);
           m_pITreeView-> SetFocusItem(pItem->GetParentItem());
        }
        m_pITreeView->Refresh();;
    }
    else if (VK_RIGHT == nChar)
    {
        IListItemBase* pItem = m_pITreeView->GetFocusItem();
        if (NULL == pItem)
            return;

        // 如果是自己是收缩的，则展开
        if (pItem->HasChild() && pItem->IsCollapsed())
        {
            m_pITreeView->ExpandItem(pItem, false);
        }
        // 否则选中第一个子结点
        else if (NULL != pItem->GetChildItem())
        {
            m_pITreeView->SelectItem(pItem->GetChildItem(), false);
            m_pITreeView->SetFocusItem(pItem->GetChildItem());
        }
        m_pITreeView->Refresh();
    }
    else
    {
        SetMsgHandled(FALSE);  // 其它按钮由基类处理
    }
}


LRESULT  TreeView::OnGetExpandIconRender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IRenderBase** pp = (IRenderBase**)wParam;
    if (!pp)
        return 0;

    *pp = m_pExpandIcon;
    if (m_pExpandIcon)
        m_pExpandIcon->AddRef();

    return 1;
}

ITreeItem*  TreeView::InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{
    if (NULL == pItem)
        return NULL;

    INormalTreeItem*  pNewItem = NULL;
    INormalTreeItem::CreateInstance(m_pITreeView->GetUIApplication(), &pNewItem);

    if (pItem->mask & UITVIF_TEXT && NULL != pItem->pszText)
        pNewItem->SetText(pItem->pszText);
    if (pItem->mask & UITVIF_PARAM)
        pNewItem->SetData(pItem->pData);
    if (pItem->mask & UITVIF_IMAG)
        pNewItem->SetIconRender(pItem->pIconRender);

    bool bRet = m_pITreeView->InsertItem(pNewItem, pParent, pInsertAfter, nInsertFlags);

    if (false == bRet)
    {
        SAFE_DELETE_Ixxx(pNewItem);
        return NULL;
    }
   
    return pNewItem;
}

ITreeItem*  TreeView::InsertNormalItem(const TCHAR* szText, IListItemBase* pParent, IListItemBase* pAfter, int nInsertFlags)
{
    UITVITEM  item = {0};
    item.mask = UITVIF_TEXT;
    item.pszText = (TCHAR*)szText;

    return this->InsertNormalItem(&item, pParent, pAfter, nInsertFlags);
}

}